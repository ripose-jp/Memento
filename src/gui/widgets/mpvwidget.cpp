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
#include <QDesktopWidget>
#include <QMessageBox>
#include <QOpenGLContext>
#include <QSettings>

#if defined(Q_OS_WIN)
#include <winbase.h>
#elif defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
#include <qpa/qplatformnativeinterface.h>
#include <QtDBus>
#include <QX11Info>
#endif

#include "../../util/constants.h"
#include "../../util/globalmediator.h"

/* Timeout before the cursor disappears */
#define CURSOR_TIMEOUT 700

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
        return nullptr;
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
      m_height(height() * QApplication::desktop()->devicePixelRatioF()),
      m_width(width() * QApplication::desktop()->devicePixelRatioF())
{
    /* Run initialization tasks */
#if defined(Q_OS_MACOS)
    m_sleepAssertIDValid = false;
#endif
    m_cursorTimer.setSingleShot(true);
    initPropertyMap();
    initSubtitleRegex();
    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    mediator->setPlayerWidget(this);

    /* Setup mpv */
    mpv = mpv_create();
    if (!mpv)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
            "Could not start mpv",
            "MpvWidget: Could not create mpv context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }

    mpv_set_option_string(mpv, "terminal",               "yes");
    mpv_set_option_string(mpv, "keep-open",              "yes");
    mpv_set_option_string(mpv, "hwdec",                  "auto-safe");
    mpv_set_option_string(mpv, "config",                 "yes");
    mpv_set_option_string(mpv, "input-default-bindings", "yes");
    mpv_set_option_string(mpv, "screenshot-directory",   "~~desktop/");
    mpv_set_option_string(mpv, "ytdl",                   "yes");

    QByteArray configDir = DirectoryUtils::getConfigDir().toUtf8();
    mpv_set_option_string(mpv, "config-dir", configDir);

    QByteArray inputFile = DirectoryUtils::getMpvInputConfig().toUtf8();
    mpv_set_option_string(mpv, "input-conf", inputFile);

    if (mpv_initialize(mpv) < 0)
    {
        QMessageBox::critical(
            nullptr,
            "Could not start mpv",
            "MpvWidget: Failed to initialize mpv context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }

    mpv_observe_property(mpv, 0, "chapter-list",       MPV_FORMAT_NODE);
    mpv_observe_property(mpv, 0, "duration",           MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "fullscreen",         MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "media-title",        MPV_FORMAT_STRING);
    mpv_observe_property(mpv, 0, "path",               MPV_FORMAT_STRING);
    mpv_observe_property(mpv, 0, "pause",              MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "time-pos",           MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "track-list/count",   MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "volume",             MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "volume-max",         MPV_FORMAT_INT64);

    mpv_observe_property(mpv, 0, "aid",                MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "aid",                MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "secondary-sid",      MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "secondary-sid",      MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "sid",                MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "sid",                MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "vid",                MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "vid",                MPV_FORMAT_INT64);

    mpv_observe_property(mpv, 0, "secondary-sub-text", MPV_FORMAT_STRING);
    mpv_observe_property(mpv, 0, "sub-delay",          MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "sub-text",           MPV_FORMAT_STRING);

    mpv_set_wakeup_callback(mpv, wakeup, this);

    /* Signals */
    connect(
        this, &QOpenGLWidget::frameSwapped,
        this, &MpvWidget::reportFrameSwap
    );
    connect(&m_cursorTimer, &QTimer::timeout, this, &MpvWidget::hideCursor);
    connect(
        mediator, &GlobalMediator::definitionsHidden,
        this, &MpvWidget::resetTimer
    );
    connect(
        mediator, &GlobalMediator::requestPlayerTimerReset,
        this, &MpvWidget::resetTimer
    );
    connect(
        mediator, &GlobalMediator::searchSettingsChanged,
        this, &MpvWidget::initSubtitleRegex
    );
}

MpvWidget::~MpvWidget()
{
    disconnect();
    makeCurrent();
    if (mpv_gl)
        mpv_render_context_free(mpv_gl);
    mpv_terminate_destroy(mpv);
}

/* End Constructor/Destructor */
/* Begin Initialization Functions */

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
            for (size_t i = 0; i < arr->num; ++i)
            {
                if (arr->values[i].format != MPV_FORMAT_NODE_MAP)
                {
                    continue;
                }
                mpv_node_list *map = arr->values[i].u.list;
                for (size_t j = 0; j < map->num; ++j)
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
                setCursor(Qt::BlankCursor);
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
        [=] (mpv_event_property *prop) {
            mpv_node node;
            mpv_get_property(mpv, "track-list", MPV_FORMAT_NODE, &node);
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

    m_propertyMap["sub-delay"] =
        [=] (mpv_event_property *prop) {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                Q_EMIT subDelayChanged(*(double *)prop->data);
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
                        mpv, "sub-delay", MPV_FORMAT_DOUBLE, &delay
                    );
                    mpv_get_property(
                        mpv, "sub-start", MPV_FORMAT_DOUBLE, &start
                    );
                    mpv_get_property(
                        mpv, "sub-end",   MPV_FORMAT_DOUBLE, &end
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
                        mpv, "secondary-sub-start", MPV_FORMAT_DOUBLE, &start
                    );
                    mpv_get_property(
                        mpv, "secondary-sub-end", MPV_FORMAT_DOUBLE, &end
                    );
                    mpv_get_property(
                        mpv, "sub-delay", MPV_FORMAT_DOUBLE, &delay
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
    settings.beginGroup(SETTINGS_SEARCH);
    m_regex.setPattern(
        settings.value(
            SETTINGS_SEARCH_REMOVE_REGEX,
            DEFAULT_REMOVE_REGEX
        ).toString()
    );
    settings.endGroup();
}

/* End Initialization Functions */
/* Begin OpenGL Functions */

void MpvWidget::initializeGL()
{
    mpv_opengl_init_params gl_init_params{
        .get_proc_address = get_proc_address,
        .get_proc_address_ctx = nullptr,
#if MPV_CLIENT_API_VERSION < MPV_MAKE_VERSION(2, 0)
        .extra_exts = nullptr,
#endif
    };
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
    if (QGuiApplication::platformName().contains("xcb"))
    {
        params[2].type = MPV_RENDER_PARAM_X11_DISPLAY;
        params[2].data = QX11Info::display();
    }
    else if (QGuiApplication::platformName().contains("wayland"))
    {
        QPlatformNativeInterface *native =
            QGuiApplication::platformNativeInterface();
        params[2].type = MPV_RENDER_PARAM_WL_DISPLAY;
        params[2].data = native->nativeResourceForWindow("display", nullptr);
    }
#endif

    if (mpv_render_context_create(&mpv_gl, mpv, params) < 0)
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
    qreal ratio = QApplication::desktop()->devicePixelRatioF();
    m_width = width * ratio;
    m_height = height * ratio;
    Q_EMIT GlobalMediator::getGlobalMediator()->playerResized();
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
        mpv_get_property(mpv, "video-params/w", MPV_FORMAT_INT64, &w);
        int64_t h = 0;
        mpv_get_property(mpv, "video-params/h", MPV_FORMAT_INT64, &h);
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
    while (mpv)
    {
        mpv_event *event = mpv_wait_event(mpv, 0);
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

    if (cursor().shape() == Qt::BlankCursor)
    {
        setCursor(Qt::ArrowCursor);
    }
    m_cursorTimer.start(CURSOR_TIMEOUT);
    Q_EMIT mouseMoved(event);
}

void MpvWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseReleaseEvent(event);
    event->ignore();

    QByteArray release = mouseButtonStringToString(event->button()).toUtf8();
    const char *args[3] = {
        "keypress",
        release,
        NULL
    };
    if (mpv_command_async(mpv, -1, args) < 0)
    {
        qDebug() << "Could not send click event to mpv";
    }
}

void MpvWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseDoubleClickEvent(event);
    event->ignore();

    QByteArray press =
        (mouseButtonStringToString(event->button(), true)).toUtf8();
    const char *args[3] = {
        "keypress",
        press,
        NULL
    };
    if (mpv_command_async(mpv, -1, args) < 0)
    {
        qDebug() << "Could not send double click event to mpv";
    }
}

void MpvWidget::resetTimer()
{
    try
    {
        m_cursorTimer.start(CURSOR_TIMEOUT);
    }
    catch (std::bad_alloc) {} // Find somebody else who cares
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
        ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_AWAYMODE_REQUIRED
    );
#elif defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    QDBusInterface screenSaver(
        "org.freedesktop.ScreenSaver", "/org/freedesktop/ScreenSaver"
    );
    QDBusMessage reply = screenSaver.call(
        "Inhibit", "ripose.memento", "Playing a video"
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

QString MpvWidget::mouseButtonStringToString(const Qt::MouseButton button,
                                             const bool doubleClick) const
{
    QString str;

    switch (button)
    {
    case Qt::LeftButton:
        str += "MBTN_LEFT";
        break;
    case Qt::RightButton:
        str += "MBTN_RIGHT";
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

/* End Helper Functions */
