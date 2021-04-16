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

#include "../../util/globalmediator.h"

#include <stdexcept>
#include <QtGui/QOpenGLContext>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #include <winbase.h>
#elif __linux__
    #include <QtDBus>
#else
    #error "OS not supported"
#endif

#define ASYNC_COMMAND_REPLY 20
#define ERROR_STR "MPV threw error code: "
#define TIMEOUT 1000

static void wakeup(void *ctx)
{
    QMetaObject::invokeMethod(
        (MpvWidget *)ctx, "on_mpv_events", Qt::QueuedConnection);
}

static void *get_proc_address(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

MpvWidget::MpvWidget(QWidget *parent)
    : QOpenGLWidget(parent), m_cursorTimer(new QTimer(this))
{
    m_cursorTimer->setSingleShot(true);

    mpv = mpv_create();
    if (!mpv)
    {
        qDebug() << "Could not create mpv context";
        exit(EXIT_FAILURE);
    }

    mpv_set_option_string(mpv, "terminal", "yes");
    mpv_set_option_string(mpv, "msg-level", "all=v");
    mpv_set_option_string(mpv, "keep-open", "yes");
    mpv_set_option_string(mpv, "config", "yes");
    QByteArray configDir = DirectoryUtils::getConfigDir().toUtf8();
    mpv_set_option_string(mpv, "config-dir", configDir);
    mpv_set_option_string(mpv, "input-default-bindings", "yes");
    QByteArray inputFile = DirectoryUtils::getMpvInputConfig().toUtf8();
    mpv_set_option_string(mpv, "input-conf", inputFile);

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "fullscreen", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "volume", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "media-title", MPV_FORMAT_STRING);
    mpv_observe_property(mpv, 0, "path", MPV_FORMAT_STRING);

    mpv_observe_property(mpv, 0, "aid", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "vid", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "sid", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "secondary-sid", MPV_FORMAT_INT64);

    mpv_observe_property(mpv, 0, "aid", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "vid", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "sid", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "secondary-sid", MPV_FORMAT_FLAG);

    mpv_observe_property(mpv, 0, "sub-text", MPV_FORMAT_STRING);

    mpv_set_wakeup_callback(mpv, wakeup, this);

    connect(m_cursorTimer, &QTimer::timeout, this, &MpvWidget::hideCursor);
    connect(m_cursorTimer, &QTimer::timeout, this, 
        [=] {
            setCursor(Qt::BlankCursor);
        }
    );
    connect(GlobalMediator::getGlobalMediator(), &GlobalMediator::definitionsHidden, this, 
        [=] {
            m_cursorTimer->start(TIMEOUT);
        }
    );
}

MpvWidget::~MpvWidget()
{
    makeCurrent();
    if (mpv_gl)
        mpv_render_context_free(mpv_gl);
    mpv_terminate_destroy(mpv);
    delete m_cursorTimer;
}

void MpvWidget::initializeGL()
{
    mpv_opengl_init_params gl_init_params{get_proc_address, nullptr, nullptr};
    mpv_render_param params[]{
        {MPV_RENDER_PARAM_API_TYPE, 
            const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    if (mpv_render_context_create(&mpv_gl, mpv, params) < 0)
        throw std::runtime_error("failed to initialize mpv GL context");
    mpv_render_context_set_update_callback(mpv_gl, MpvWidget::on_update,
                                           reinterpret_cast<void *>(this));
}

void MpvWidget::paintGL()
{
    qreal ratio = QApplication::desktop()->devicePixelRatioF();
    mpv_opengl_fbo mpfbo{
        static_cast<int>(defaultFramebufferObject()),
        (int) (width() * ratio),
        (int) (height() * ratio),
        0
    };
    int flip_y{1};

    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr}};
    // See render_gl.h on what OpenGL environment mpv expects, and
    // other API details.
    mpv_render_context_render(mpv_gl, params);
}

void MpvWidget::on_mpv_events()
{
    // Process all events, until the event queue is empty.
    while (mpv)
    {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
        {
            break;
        }
        handle_mpv_event(event);
    }
}

void MpvWidget::handle_mpv_event(mpv_event *event)
{
    switch (event->event_id)
    {
    case MPV_EVENT_PROPERTY_CHANGE:
    {
        mpv_event_property *prop = (mpv_event_property *)event->data;
        if (strcmp(prop->name, "time-pos") == 0)
        {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                double time = *(double *) prop->data;
                Q_EMIT positionChanged(time);
            }
        }
        else if (strcmp(prop->name, "duration") == 0)
        {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                double time = *(double *) prop->data;
                Q_EMIT durationChanged(time);
            }
        }
        else if (strcmp(prop->name, "sub-text") == 0)
        {
            if (prop->format == MPV_FORMAT_STRING)
            {
                const char **subtitle = (const char **) prop->data;
                if (strcmp(*subtitle, ""))
                {
                    double delay;
                    mpv_get_property(mpv, "sub-delay", MPV_FORMAT_DOUBLE, &delay);
                    double start;
                    mpv_get_property(mpv, "sub-start", MPV_FORMAT_DOUBLE, &start);
                    double end;
                    mpv_get_property(mpv, "sub-end", MPV_FORMAT_DOUBLE, &end);

                    Q_EMIT subtitleChanged(*subtitle, start, end, delay);
                }
            }
        }
        else if (strcmp(prop->name, "pause") == 0)
        {
            if (prop->format == MPV_FORMAT_FLAG)
            {
                bool paused = *(int *) prop->data;

                // Keep the computer from going to sleep
            #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
                    SetThreadExecutionState(paused ? ES_CONTINUOUS : 
                                                     ES_CONTINUOUS | 
                                                     ES_SYSTEM_REQUIRED | 
                                                     ES_AWAYMODE_REQUIRED);
            #elif __linux__
                    QDBusInterface screenSaver("org.freedesktop.ScreenSaver",
                                               "/org/freedesktop/ScreenSaver");
                    if (paused)
                    {
                        screenSaver.call("UnInhibit", dbus_cookie);
                    }
                    else
                    {
                        QDBusMessage reply =
                            screenSaver.call("Inhibit",
                                             "ripose.memento",
                                             "Playing a video");
                        
                        if (QDBusMessage::ReplyMessage)
                        {
                            if (reply.arguments().size() == 1 &&
                                reply.arguments().first().isValid())
                            {
                                dbus_cookie = 
                                    reply.arguments().first().toUInt();
                            }
                            else
                            {
                                qDebug() << "org.freedesktop.ScreenSaver reply "
                                            "is invalid";
                            }
                        }
                        else if (QDBusMessage::ErrorMessage)
                        {
                            qDebug() << "Error from org.freedesktop.ScreenSaver" 
                                     << reply.errorMessage();
                        }
                        else
                        {
                            qDebug() << "Unknown reply from "
                                        "org.freedesktop.ScreenSaver";
                        }     
                    }
            #else
                #error "OS not supported"
            #endif
                
                Q_EMIT stateChanged(paused);
            }
        }
        else if (strcmp(prop->name, "aid") == 0)
        {
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
        }
        else if (strcmp(prop->name, "vid") == 0)
        {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int64_t id = *(int64_t *) prop->data;
                Q_EMIT videoTrackChanged(id);
            }
            else if (prop->format == MPV_FORMAT_FLAG)
            {
                if (!*(int64_t *)prop->data) 
                    Q_EMIT videoDisabled();
            }
        }
        else if (strcmp(prop->name, "sid") == 0)
        {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int64_t id = *(int64_t *) prop->data;
                Q_EMIT subtitleTrackChanged(id);
            }
            else if (prop->format == MPV_FORMAT_FLAG)
            {
                if (!*(int64_t *)prop->data) 
                    Q_EMIT subtitleDisabled();
            }
        }
        else if (strcmp(prop->name, "secondary-sid") == 0)
        {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int64_t id = *(int64_t *) prop->data;
                Q_EMIT subtitleTwoTrackChanged(id);
            }
            else if (prop->format == MPV_FORMAT_FLAG)
            {
                if (!*(int64_t *)prop->data) 
                    Q_EMIT subtitleTwoDisabled();
            }
        }
        else if (strcmp(prop->name, "fullscreen") == 0)
        {
            if (prop->format == MPV_FORMAT_FLAG)
            {
                setCursor(Qt::BlankCursor);
                bool full = *(int *) prop->data;
                Q_EMIT fullscreenChanged(full);
            }
        }
        else if (strcmp(prop->name, "volume") == 0)
        {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int volume = *(int64_t *) prop->data;
                Q_EMIT volumeChanged(volume);
            }
        }
        else if (strcmp(prop->name, "media-title") == 0)
        {
            if (prop->format == MPV_FORMAT_STRING)
            {
                const char **name = (const char **) prop->data;
                Q_EMIT titleChanged(*name);
            }
        }
        else if (strcmp(prop->name, "path") == 0)
        {
            if (prop->format == MPV_FORMAT_STRING)
            {
                const char **path = (const char **) prop->data;
                Q_EMIT fileChanged(*path);
            }
        }
        break;
    }
    case MPV_EVENT_FILE_LOADED:
    {
        mpv_node node;
        mpv_get_property(mpv, "track-list", MPV_FORMAT_NODE, &node);
        Q_EMIT tracklistChanged(&node);
        mpv_free_node_contents(&node);
        break;
    }
    case MPV_EVENT_SHUTDOWN:
        Q_EMIT shutdown();
        break;
    default:;
        // Ignore uninteresting or unknown events.
    }
}

// Make Qt invoke mpv_render_context_render() to draw a new/updated video frame.
void MpvWidget::maybeUpdate()
{
    // If the Qt window is not visible, Qt's update() will just skip rendering.
    // This confuses mpv's render API, and may lead to small occasional
    // freezes due to video rendering timing out.
    // Handle this by manually redrawing.
    // Note: Qt doesn't seem to provide a way to query whether update() will
    //       be skipped, and the following code still fails when e.g. switching
    //       to a different workspace with a reparenting window manager.
    if (window()->isMinimized())
    {
        makeCurrent();
        paintGL();
        context()->swapBuffers(context()->surface());
        doneCurrent();
    }
    else
    {
        update();
    }
}

void MpvWidget::on_update(void *ctx)
{
    QMetaObject::invokeMethod((MpvWidget *)ctx, "maybeUpdate");
}

void MpvWidget::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
    if (cursor().shape() == Qt::BlankCursor)
    {
        setCursor(Qt::ArrowCursor);
    }
    m_cursorTimer->start(TIMEOUT);
    Q_EMIT mouseMoved(event);
}

void MpvWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QByteArray release = convertToMouseString(event).toUtf8();
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
    QByteArray press = (convertToMouseString(event) + "_DBL").toUtf8();
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

QString MpvWidget::convertToMouseString(const QMouseEvent *event) const
{
    switch (event->button())
    {
    case Qt::LeftButton:
        return "MBTN_LEFT";
    case Qt::RightButton:
        return "MBTN_RIGHT";
    case Qt::BackButton:
        return "MBTN_BACK";
    case Qt::ForwardButton:
        return "MBTN_FORWARD";
    }
    return "";
}