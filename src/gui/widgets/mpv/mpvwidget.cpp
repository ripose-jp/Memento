////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include "mpvwidget.h"

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QOpenGLContext>
#include <QScreen>
#include <QSettings>
#include <QWindow>

#if defined(Q_OS_WIN)
#ifndef NOMINMAX
#define NOMINMAX 1
#endif // NOMINMAX
#include <Windows.h>
#include <winbase.h>
#elif defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
#include <QtDBus>
#endif

#include "cursortimer.h"

#include "util/constants.h"
#include "util/globalmediator.h"

/* Begin C functions */

static void wakeup(void *ctx)
{
    QMetaObject::invokeMethod(
        (MpvWidget *)ctx, "onMpvEvents", Qt::QueuedConnection
    );
}

static void *get_proc_address(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
    {
        return nullptr;
    }
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

static void onUpdate(void *ctx)
{
    QMetaObject::invokeMethod((MpvWidget *)ctx, "maybeUpdate");
}

/* End C functions */
/* Begin Constructor/Destructor */

MpvWidget::MpvWidget(QWidget *parent)
    : QOpenGLWidget(parent),
      m_height(height() * parent->screen()->devicePixelRatio()),
      m_width(width() * parent->screen()->devicePixelRatio())
{
    /* Set the mediator player widget */
    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    mediator->setPlayerWidget(this);

    /* Initialize mpv */
    m_mpv = mpv_create();
    if (!m_mpv)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
            "Could not start mpv",
            "MpvWidget: Could not create mpv context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }

    /* Signals */
    connect(
        this, &QOpenGLWidget::frameSwapped,
        this, &MpvWidget::reportFrameSwap
    );
    connect(
        mediator, &GlobalMediator::searchSettingsChanged,
        this, &MpvWidget::initSubtitleRegex,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::behaviorSettingsChanged,
        this, &MpvWidget::initTimer,
        Qt::QueuedConnection
    );
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    connect(
        mediator, &GlobalMediator::interfaceSettingsChanged,
        this, &MpvWidget::initDimensions,
        Qt::QueuedConnection
    );
#endif

    /* Run initialization tasks */
#if defined(Q_OS_MACOS)
    m_sleepAssertIDValid = false;
#endif
    initPropertyMap();
    initSubtitleRegex();
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    initDimensions();
#endif
}

MpvWidget::~MpvWidget()
{
    disconnect();
    makeCurrent();
    delete m_cursorTimer;
    if (mpv_gl)
    {
        mpv_render_context_free(mpv_gl);
    }
    mpv_terminate_destroy(m_mpv);
}

/* End Constructor/Destructor */
/* Begin Initialization Functions */

void MpvWidget::initScreenSignals()
{
    connect(
        window()->windowHandle(), &QWindow::screenChanged,
        this, &MpvWidget::screenChanged
    );
    screenChanged(screen());
}

void MpvWidget::initPropertyMap()
{
    m_propertyMap["chapter-list"] =
        [=] (mpv_event_property *prop)
        {
            if (prop->format != MPV_FORMAT_NODE)
            {
                return;
            }
            mpv_node *node = (mpv_node *)prop->data;

            if (node->format != MPV_FORMAT_NODE_ARRAY)
            {
                return;
            }
            mpv_node_list *arr = node->u.list;

            QList<double> chapters;
            for (int i = 0; i < arr->num; ++i)
            {
                if (arr->values[i].format != MPV_FORMAT_NODE_MAP)
                {
                    continue;
                }
                mpv_node_list *map = arr->values[i].u.list;
                for (int j = 0; j < map->num; ++j)
                {
                    if (map->values[j].format == MPV_FORMAT_DOUBLE &&
                        strcmp(map->keys[j], "time") == 0)
                    {
                        chapters << map->values[j].u.double_;
                    }
                }
            }

            Q_EMIT chaptersChanged(chapters);
        };

    m_propertyMap["duration"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                double time = *(double *)prop->data;
                Q_EMIT durationChanged(time);
            }
        };

    m_propertyMap["fullscreen"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_FLAG)
            {
                m_cursorTimer->forceTimeout();
                bool full = *(int *)prop->data;
                Q_EMIT fullscreenChanged(full);
            }
        };

    m_propertyMap["media-title"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_STRING)
            {
                const char **name = (const char **)prop->data;
                Q_EMIT titleChanged(*name);
            }
        };

    m_propertyMap["path"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_STRING)
            {
                const char **path = (const char **)prop->data;
                Q_EMIT fileChanged(*path);
            }
        };

    m_propertyMap["pause"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_FLAG)
            {
                bool paused = *(int *)prop->data;
                if (paused)
                {
                    allowScreenDimming();
                }
                else
                {
                    preventScreenDimming();
                }
                Q_EMIT pauseChanged(paused);
            }
        };

    m_propertyMap["time-pos"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                double time = *(double *)prop->data;
                Q_EMIT positionChanged(time);
            }
        };

    m_propertyMap["track-list/count"] =
        [=] (mpv_event_property *) {
            mpv_node node;
            mpv_get_property(m_mpv, "track-list", MPV_FORMAT_NODE, &node);
            Q_EMIT tracklistChanged(&node);
            mpv_free_node_contents(&node);
        };

    m_propertyMap["volume"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int volume = *(int64_t *)prop->data;
                Q_EMIT volumeChanged(volume);
            }
        };

    m_propertyMap["volume-max"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int volume = *(int64_t *)prop->data;
                Q_EMIT volumeMaxChanged(volume);
            }
        };

    m_propertyMap["aid"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int64_t id = *(int64_t *)prop->data;
                Q_EMIT audioTrackChanged(id);
            }
            else if (prop->format == MPV_FORMAT_FLAG)
            {
                if (!*(int64_t *)prop->data)
                    Q_EMIT audioDisabled();
            }
        };

    m_propertyMap["secondary-sid"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int64_t id = *(int64_t *)prop->data;
                Q_EMIT subtitleTwoTrackChanged(id);
            }
            else if (prop->format == MPV_FORMAT_FLAG)
            {
                if (!*(int64_t *)prop->data)
                    Q_EMIT subtitleTwoDisabled();
            }
        };

    m_propertyMap["sid"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int64_t id = *(int64_t *)prop->data;
                Q_EMIT subtitleTrackChanged(id);
            }
            else if (prop->format == MPV_FORMAT_FLAG)
            {
                if (!*(int64_t *)prop->data)
                    Q_EMIT subtitleDisabled();
            }
        };

    m_propertyMap["vid"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int64_t id = *(int64_t *)prop->data;
                Q_EMIT videoTrackChanged(id);
            }
            else if (prop->format == MPV_FORMAT_FLAG)
            {
                if (!*(int64_t *)prop->data)
                    Q_EMIT videoDisabled();
            }
        };

    m_propertyMap["secondary-sub-delay"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                m_secSubDelaySupported = true;
                Q_EMIT secSubDelayChanged(*(double *)prop->data);
            }
        };

    m_propertyMap["sub-delay"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                Q_EMIT subDelayChanged(*(double *)prop->data);
                if (!m_secSubDelaySupported)
                {
                    Q_EMIT secSubDelayChanged(*(double *)prop->data);
                }
            }
        };

    m_propertyMap["sub-text"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_STRING)
            {
                QString subtitle = *(const char **)prop->data;
                subtitle.remove(m_regex);
                if (!subtitle.isEmpty())
                {
                    double delay, start, end;

                    mpv_get_property(
                        m_mpv, "sub-delay", MPV_FORMAT_DOUBLE, &delay
                    );
                    mpv_get_property(
                        m_mpv, "sub-start", MPV_FORMAT_DOUBLE, &start
                    );
                    mpv_get_property(
                        m_mpv, "sub-end",   MPV_FORMAT_DOUBLE, &end
                    );

                    Q_EMIT subtitleChanged(subtitle, start, end, delay);
                    Q_EMIT subtitleChangedRaw(
                        *(const char **)prop->data, start, end, delay
                    );
                }
            }
        };

    m_propertyMap["secondary-sub-text"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_STRING)
            {
                const char **subtitle = (const char **)prop->data;
                if (strcmp(*subtitle, ""))
                {
                    double start, end, delay;

                    mpv_get_property(
                        m_mpv, "secondary-sub-start", MPV_FORMAT_DOUBLE, &start
                    );
                    mpv_get_property(
                        m_mpv, "secondary-sub-end", MPV_FORMAT_DOUBLE, &end
                    );
                    mpv_get_property(
                        m_mpv, "sub-delay", MPV_FORMAT_DOUBLE, &delay
                    );

                    Q_EMIT subtitleChangedSecondary(
                        *subtitle, start, end, delay
                    );
                }
            }
        };
}

void MpvWidget::initSubtitleRegex()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Search::GROUP);
    m_regex.setPattern(
        settings.value(
            Constants::Settings::Search::REMOVE_REGEX,
            Constants::Settings::Search::REMOVE_REGEX_DEFAULT
        ).toString()
    );
    settings.endGroup();
}

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
void MpvWidget::initDimensions()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);

    m_overrideDevicePixelRatio = settings.value(
        Constants::Settings::Interface::DPI_SCALE_OVERRIDE,
        Constants::Settings::Interface::DPI_SCALE_OVERRIDE_DEFAULT
    ).toBool();
    if (m_overrideDevicePixelRatio)
    {
        m_devicePixelRatio = settings.value(
            Constants::Settings::Interface::DPI_SCALE_FACTOR,
            Constants::Settings::Interface::DPI_SCALE_FACTOR_DEFAULT
        ).toDouble();
    }
    else
    {
        m_devicePixelRatio = screen()->devicePixelRatio();
    }

    m_height = height() * m_devicePixelRatio;
    m_width = width() * m_devicePixelRatio;
}
#endif

void MpvWidget::initTimer()
{
    {
        QSettings settings;
        settings.beginGroup(Constants::Settings::Behavior::GROUP);
        bool useOSCTimer = settings.value(
            Constants::Settings::Behavior::OSC_CURSOR_HIDE,
            Constants::Settings::Behavior::OSC_CURSOR_HIDE_DEFAULT
        ).toBool();
        settings.endGroup();


        delete m_cursorTimer;
        if (useOSCTimer)
        {
            m_cursorTimer = new OSCTimer(this);
        }
        else
        {
            m_cursorTimer = new MpvTimer(m_mpv, this);
        }
    }

    connect(
        m_cursorTimer, &CursorTimer::showCursor,
        this, &MpvWidget::showCursor,
        Qt::DirectConnection
    );
    connect(
        m_cursorTimer, &CursorTimer::hideCursor,
        this, &MpvWidget::hideCursor,
        Qt::DirectConnection
    );
    connect(
        GlobalMediator::getGlobalMediator(), &GlobalMediator::definitionsHidden,
        m_cursorTimer, &CursorTimer::start,
        Qt::QueuedConnection
    );

    m_cursorTimer->forceTimeout();
}

void MpvWidget::initOptions()
{
    /* Hardcoded options */
    mpv_set_option_string(m_mpv, "config",                 "yes");
    mpv_set_option_string(m_mpv, "input-default-bindings", "yes");
    mpv_set_option_string(m_mpv, "keep-open",              "yes");
    mpv_set_option_string(m_mpv, "screenshot-directory",   "~~desktop/");
    mpv_set_option_string(m_mpv, "terminal",               "yes");
    mpv_set_option_string(m_mpv, "vo",                     "libmpv");
    mpv_set_option_string(m_mpv, "ytdl",                   "yes");

    /* Commandline options */
    QStringList args = QApplication::arguments();
    for (qsizetype i = 0; i < args.size(); ++i)
    {
        /* Skip non-options */
        if (!args[i].startsWith("--"))
        {
            continue;
        }

        /* Skip per file options */
        if (args[i] == "--{")
        {
            qsizetype depth = 1;
            while (depth > 0)
            {
                ++i;
                if (i >= args.size())
                {
                    break;
                }
                else if (args[i] == "--{")
                {
                    ++depth;
                }
                else if (args[i] == "--}")
                {
                    --depth;
                }
            }
            continue;
        }

        QString arg = args[i].last(args[i].size() - 2);
        if (arg.startsWith("no-"))
        {
            arg = arg.last(arg.size() - 3)
                .append("=no");
        }

        qsizetype sepIndex = arg.indexOf('=');
        if (sepIndex == -1)
        {
            continue;
        }

        QByteArray option = arg.first(sepIndex).toUtf8();
        QByteArray value = arg.mid(sepIndex + 1).toUtf8();
        mpv_set_option_string(m_mpv, option, value);
    }
}

/* End Initialization Functions */
/* Begin OpenGL Functions */

void MpvWidget::initializeGL()
{
    /* Initialize the mpv render context */
    mpv_opengl_init_params gl_init_params;
    gl_init_params.get_proc_address = get_proc_address;
    gl_init_params.get_proc_address_ctx = nullptr;
#if MPV_CLIENT_API_VERSION < MPV_MAKE_VERSION(2, 0)
    gl_init_params.extra_exts = nullptr;
#endif
    mpv_render_param params[]{
        {
            MPV_RENDER_PARAM_API_TYPE,
            const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)
        },
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID,            nullptr},
        {MPV_RENDER_PARAM_INVALID,            nullptr}
    };

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    if (auto x11App = qApp->nativeInterface<QNativeInterface::QX11Application>())
    {
        params[2].type = MPV_RENDER_PARAM_X11_DISPLAY;
        params[2].data = x11App->display();
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    else if (auto wlApp = qApp->nativeInterface<QNativeInterface::QWaylandApplication>())
    {
        params[2].type = MPV_RENDER_PARAM_WL_DISPLAY;
        params[2].data = wlApp->display();
    }
#endif // QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#endif // defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)

    if (mpv_render_context_create(&mpv_gl, m_mpv, params) < 0)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
            "Could not start mpv",
            "MpvWidget: Failed to initialize mpv GL context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }
    mpv_render_context_set_update_callback(
        mpv_gl, onUpdate, reinterpret_cast<void *>(this)
    );

    /* Initialize the mpv context */
    initOptions();

    QByteArray configDir = DirectoryUtils::getConfigDir().toUtf8();
    mpv_set_option_string(m_mpv, "config-dir", configDir);

    QByteArray inputFile = DirectoryUtils::getMpvInputConfig().toUtf8();
    mpv_set_option_string(m_mpv, "input-conf", inputFile);

    if (mpv_initialize(m_mpv) < 0)
    {
        QMessageBox::critical(
            nullptr,
            "Could not start mpv",
            "MpvWidget: Failed to initialize mpv context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }

    mpv_observe_property(m_mpv, 0, "chapter-list",        MPV_FORMAT_NODE);
    mpv_observe_property(m_mpv, 0, "duration",            MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "fullscreen",          MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "media-title",         MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "path",                MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "pause",               MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "time-pos",            MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "track-list/count",    MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "volume",              MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "volume-max",          MPV_FORMAT_INT64);

    mpv_observe_property(m_mpv, 0, "aid",                 MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "aid",                 MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "secondary-sid",       MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "secondary-sid",       MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "sid",                 MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "sid",                 MPV_FORMAT_INT64);
    mpv_observe_property(m_mpv, 0, "vid",                 MPV_FORMAT_FLAG);
    mpv_observe_property(m_mpv, 0, "vid",                 MPV_FORMAT_INT64);

    mpv_observe_property(m_mpv, 0, "secondary-sub-text",  MPV_FORMAT_STRING);
    mpv_observe_property(m_mpv, 0, "secondary-sub-delay", MPV_FORMAT_DOUBLE); // IMPORTANT: Put this before sub-delay
    mpv_observe_property(m_mpv, 0, "sub-delay",           MPV_FORMAT_DOUBLE);
    mpv_observe_property(m_mpv, 0, "sub-text",            MPV_FORMAT_STRING);

    mpv_set_wakeup_callback(m_mpv, wakeup, this);

    initTimer();
}

void MpvWidget::paintGL()
{
    mpv_opengl_fbo mpfbo{
        static_cast<int>(defaultFramebufferObject()),
        m_width,
        m_height,
        0
    };
    int flip_y{1};

    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_FLIP_Y,     &flip_y},
        {MPV_RENDER_PARAM_INVALID,    nullptr}
    };
    // See render_gl.h on what OpenGL environment mpv expects, and
    // other API details.
    mpv_render_context_render(mpv_gl, params);
}

void MpvWidget::resizeGL(int width, int height)
{
    m_width = width * m_devicePixelRatio;
    m_height = height * m_devicePixelRatio;
    Q_EMIT GlobalMediator::getGlobalMediator()->playerResized();
}

void MpvWidget::screenChanged(QScreen *screen)
{
    if (!m_overrideDevicePixelRatio)
    {
        m_devicePixelRatio = screen ? screen->devicePixelRatio() : 1.0;
        resizeGL(width(), height());
    }
}

void MpvWidget::reportFrameSwap()
{
    if (mpv_gl)
    {
        mpv_render_context_report_swap(mpv_gl);
    }
}

// Make Qt invoke mpv_render_context_render() to draw a new/updated video frame.
void MpvWidget::maybeUpdate()
{
    /* If the Qt window is not visible, Qt's update() will just skip rendering.
     * This confuses mpv's render API, and may lead to small occasional
     * freezes due to video rendering timing out.
     * Handle this by manually redrawing.
     * Note: Qt doesn't seem to provide a way to query whether update() will
     *       be skipped, and the following code still fails when e.g. switching
     *       to a different workspace with a reparenting window manager.
     */
    if (window()->isMinimized() || !updatesEnabled())
    {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        reportFrameSwap();
        doneCurrent();
    }
    else
    {
        update();
    }
}

/* End OpenGL Functions */
/* Begin Event Handlers */

void MpvWidget::showEvent(QShowEvent *event)
{
    QOpenGLWidget::showEvent(event);
    if (!m_firstShow)
    {
        return;
    }
    m_firstShow = false;
    initScreenSignals();
}

void MpvWidget::handleMpvEvent(mpv_event *event)
{
    switch (event->event_id)
    {
    case MPV_EVENT_PROPERTY_CHANGE:
    {
        mpv_event_property *prop = (mpv_event_property *)event->data;
        auto func = m_propertyMap.find(prop->name);
        if (func != m_propertyMap.end())
        {
            (*func)(prop);
        }
        break;
    }
    case MPV_EVENT_FILE_LOADED:
    {
        int64_t w = 0;
        mpv_get_property(m_mpv, "video-params/w", MPV_FORMAT_INT64, &w);
        int64_t h = 0;
        mpv_get_property(m_mpv, "video-params/h", MPV_FORMAT_INT64, &h);
        Q_EMIT newFileLoaded(w, h);
        break;
    }
    case MPV_EVENT_SHUTDOWN:
        Q_EMIT shutdown();
        break;
    default:;
        // Ignore uninteresting or unknown events.
    }
}

void MpvWidget::onMpvEvents()
{
    // Process all events, until the event queue is empty.
    while (m_mpv)
    {
        mpv_event *event = mpv_wait_event(m_mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
        {
            break;
        }
        handleMpvEvent(event);
    }
}

void MpvWidget::mouseMoveEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseMoveEvent(event);
    event->ignore();

    m_cursorTimer->start();
    Q_EMIT mouseMoved(event);
}

/**
 * Converts the current mouse event to an mpv button command string.
 * @param button      The mouse button clicked.
 * @param doubleClick True if this is a double click.
 */
static inline QByteArray mouseButtonToString(
    const Qt::MouseButton button,
    const bool doubleClick)
{
    QByteArray str;
    Qt::KeyboardModifiers mods = QGuiApplication::keyboardModifiers();
    if (mods & Qt::ShiftModifier)
    {
        str += "Shift+";
    }
    if (mods & Qt::ControlModifier)
    {
        str += "Ctrl+";
    }
    if (mods & Qt::AltModifier)
    {
        str += "Alt+";
    }
    if (mods & Qt::MetaModifier)
    {
        str += "Meta+";
    }

    switch (button)
    {
    case Qt::LeftButton:
        str += "MBTN_LEFT";
        break;
    case Qt::RightButton:
        str += "MBTN_RIGHT";
        break;
    case Qt::MiddleButton:
        str += "MBTN_MID";
        break;
    case Qt::BackButton:
        str += "MBTN_BACK";
        break;
    case Qt::ForwardButton:
        str += "MBTN_FORWARD";
        break;
    default:
        return "";
    }

    if (doubleClick)
    {
        str += "_DBL";
    }

    return str;
}

void MpvWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseReleaseEvent(event);
    event->ignore();

    QByteArray release = mouseButtonToString(event->button(), false);
    const char *args[3] = {
        "keypress",
        release,
        NULL
    };
    if (mpv_command_async(m_mpv, -1, args) < 0)
    {
        qDebug() << "Could not send click event to mpv";
    }
}

void MpvWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseDoubleClickEvent(event);
    event->ignore();

    QByteArray press = mouseButtonToString(event->button(), true);
    const char *args[3] = {
        "keypress",
        press,
        NULL
    };
    if (mpv_command_async(m_mpv, -1, args) < 0)
    {
        qDebug() << "Could not send double click event to mpv";
    }
}

void MpvWidget::hideCursor()
{
    setCursor(Qt::BlankCursor);
    /* A call to update here is necessary because of how mpv handles updating
     * this widget with maybeUpdate().
     */
    update();
    Q_EMIT cursorHidden();
}

void MpvWidget::showCursor()
{
    setCursor(Qt::CursorShape::ArrowCursor);
}

/* End Event Handlers */
/* Begin Helper Functions */

void MpvWidget::allowScreenDimming()
{
#if defined(Q_OS_WIN)
    SetThreadExecutionState(ES_CONTINUOUS);
#elif defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    QDBusInterface screenSaver(
        "org.freedesktop.ScreenSaver", "/org/freedesktop/ScreenSaver"
    );
    screenSaver.call("UnInhibit", dbus_cookie);
#elif defined(Q_OS_MACOS)
    if (!m_sleepAssertIDValid)
    {
        return;
    }

    IOReturn success = IOPMAssertionRelease(m_sleepAssertID);
    if (success == kIOReturnSuccess)
    {
        m_sleepAssertIDValid = false;
    }
    else
    {
        qDebug() << "Could not uninhibit sleep!";
        qDebug() << "Error code:" << success;
    }
#endif
}

void MpvWidget::preventScreenDimming()
{
#if defined(Q_OS_WIN)
    SetThreadExecutionState(
        ES_CONTINUOUS | ES_DISPLAY_REQUIRED
    );
#elif defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    QDBusInterface screenSaver(
        "org.freedesktop.ScreenSaver", "/org/freedesktop/ScreenSaver"
    );
    QDBusMessage reply = screenSaver.call(
        "Inhibit", "io.github.ripose-jp.memento", "Playing a video"
    );
    switch(reply.type())
    {
    case QDBusMessage::ReplyMessage:
        if (reply.arguments().size() == 1 &&
            reply.arguments().first().isValid())
        {
            dbus_cookie = reply.arguments().first().toUInt();
        }
        else
        {
            qDebug() << "org.freedesktop.ScreenSaver reply is invalid";
        }
        break;
    case QDBusMessage::ErrorMessage:
        qDebug() << "Error from org.freedesktop.ScreenSaver"
                 << reply.errorMessage();
        break;
    default:
        qDebug() << "Unknown reply from org.freedesktop.ScreenSaver";
        break;
    }
#elif defined(Q_OS_MACOS)
    CFStringRef reasonForActivity =
        CFSTR("Memento: Media is playing, preventing sleep...");

    IOReturn success = IOPMAssertionCreateWithName(
        kIOPMAssertionTypeNoDisplaySleep,
        kIOPMAssertionLevelOn,
        reasonForActivity,
        &m_sleepAssertID
    );

    if (success == kIOReturnSuccess)
    {
        m_sleepAssertIDValid = true;
    }
    else
    {
        qDebug() << "Could not inhibit sleep!";
        qDebug() << "Error code:" << success;
    }
#endif
}

/* End Helper Functions */
