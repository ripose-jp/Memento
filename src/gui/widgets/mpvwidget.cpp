#include "mpvwidget.h"

#include <stdexcept>
#include <QtGui/QOpenGLContext>
#include <QDebug>

static void wakeup(void *ctx)
{
    QMetaObject::invokeMethod((MpvWidget *)ctx, "on_mpv_events", Qt::QueuedConnection);
}

static void *get_proc_address(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx)
        return nullptr;
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

MpvWidget::MpvWidget(QWidget *parent) : QOpenGLWidget(parent), m_cursorTimer(new QTimer())
{
    mpv = mpv_create();
    if (!mpv)
        throw std::runtime_error("could not create mpv context");

    mpv_set_option_string(mpv, "terminal", "yes");
    mpv_set_option_string(mpv, "msg-level", "all=v");
    mpv_set_option_string(mpv, "keep-open", "yes");
    mpv_set_option_string(mpv, "config", "yes");

    // TODO: non-portable code
    QString path = getenv("HOME");
    path += CONFIG_PATH;
    mpv_set_option_string(mpv, "config-dir", path.toLatin1());

    if (mpv_initialize(mpv) < 0)
        throw std::runtime_error("could not initialize mpv context");

    // Request hw decoding, just for testing.
    mpv::qt::set_option_variant(mpv, "hwdec", "auto");

    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "pause", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "fullscreen", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "volume", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "track-list", MPV_FORMAT_NODE);

    mpv_observe_property(mpv, 0, "aid", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "vid", MPV_FORMAT_INT64);
    mpv_observe_property(mpv, 0, "sid", MPV_FORMAT_INT64);

    mpv_observe_property(mpv, 0, "aid", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "vid", MPV_FORMAT_FLAG);
    mpv_observe_property(mpv, 0, "sid", MPV_FORMAT_FLAG);

    mpv_observe_property(mpv, 0, "sub-text", MPV_FORMAT_STRING);

    mpv_set_wakeup_callback(mpv, wakeup, this);

    connect(m_cursorTimer, &QTimer::timeout, this, &MpvWidget::hideCursor);
    connect(m_cursorTimer, &QTimer::timeout, this, [=] { setCursor(Qt::BlankCursor); });
}

MpvWidget::~MpvWidget()
{
    makeCurrent();
    if (mpv_gl)
        mpv_render_context_free(mpv_gl);
    mpv_terminate_destroy(mpv);
    delete m_cursorTimer;
}

void MpvWidget::command(const QVariant &params)
{
    QVariant err = mpv::qt::command(mpv, params);
    if (mpv::qt::is_error(err))
        qDebug() << ERROR_STR << mpv::qt::get_error(err);
}

void MpvWidget::asyncCommand(const QVariant &args)
{
    mpv::qt::node_builder node(args);
    int err = mpv_command_node_async(mpv, ASYNC_COMMAND_REPLY, node.node());
    if (err < 0)
        qDebug() << ERROR_STR << err;
}

void MpvWidget::setProperty(const QString &name, const QVariant &value)
{
    int err = mpv::qt::set_property(mpv, name, value);
    if (err < 0)
        qDebug() << ERROR_STR << err;
}

QVariant MpvWidget::getProperty(const QString &name) const
{
    return mpv::qt::get_property_variant(mpv, name);
}

void MpvWidget::initializeGL()
{
    mpv_opengl_init_params gl_init_params{get_proc_address, nullptr, nullptr};
    mpv_render_param params[]{
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr}};

    if (mpv_render_context_create(&mpv_gl, mpv, params) < 0)
        throw std::runtime_error("failed to initialize mpv GL context");
    mpv_render_context_set_update_callback(mpv_gl, MpvWidget::on_update, reinterpret_cast<void *>(this));
}

void MpvWidget::paintGL()
{
    mpv_opengl_fbo mpfbo{static_cast<int>(defaultFramebufferObject()), width(), height(), 0};
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
                double time = *(double *)prop->data;
                Q_EMIT positionChanged(time);
            }
        }
        else if (strcmp(prop->name, "duration") == 0)
        {
            if (prop->format == MPV_FORMAT_DOUBLE)
            {
                double time = *(double *)prop->data;
                Q_EMIT durationChanged(time);
            }
        }
        else if (strcmp(prop->name, "pause") == 0)
        {
            if (prop->format == MPV_FORMAT_FLAG)
            {
                bool paused = *(int *)prop->data;
                Q_EMIT stateChanged(paused);
            }
        }
        else if (strcmp(prop->name, "fullscreen") == 0)
        {
            if (prop->format == MPV_FORMAT_FLAG)
            {
                bool full = *(int *)prop->data;
                Q_EMIT fullscreenChanged(full);
            }
        }
        else if (strcmp(prop->name, "volume") == 0)
        {
            if (prop->format == MPV_FORMAT_INT64)
            {
                int volume = *(int64_t *)prop->data;
                Q_EMIT volumeChanged(volume);
            }
        }
        else if (strcmp(prop->name, "track-list") == 0)
        {
            if (prop->format == MPV_FORMAT_NODE)
            {
                Q_EMIT tracklistChanged((mpv_node *)prop->data);
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
                int64_t id = *(int64_t *)prop->data;
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
                int64_t id = *(int64_t *)prop->data;
                Q_EMIT subtitleTrackChanged(id);
            }
            else if (prop->format == MPV_FORMAT_FLAG)
            {
                if (!*(int64_t *)prop->data) 
                    Q_EMIT subtitleDisabled();
            }
        }
        else if (strcmp(prop->name, "sub-text") == 0)
        {
            if (prop->format == MPV_FORMAT_STRING)
            {
                const char **subtitle = (const char **)prop->data;
                if (strcmp(*subtitle, ""))
                {
                    const int64_t end = getProperty("sub-end").toInt();
                    Q_EMIT subtitleChanged(subtitle, end);
                }
            }
        }
        break;
    }
    case MPV_EVENT_SHUTDOWN:
        Q_EMIT shutdown();
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
}

void MpvWidget::mouseReleaseEvent(QMouseEvent *event)
{
    command(QVariantList() << "keypress" << convertToMouseString(event));
}

void MpvWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    command(QVariantList() << "keypress" << convertToMouseString(event) + "_DBL");
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