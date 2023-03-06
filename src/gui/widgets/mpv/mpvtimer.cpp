#include "cursortimer.h"

#include <QMetaObject>
#include <QSettings>

#include <mpv/client.h>

#include "util/constants.h"

/* Begin C Functions */

static void wakeup(void *ctx)
{
    QMetaObject::invokeMethod(
        (MpvTimer *)ctx, "handleEvents", Qt::QueuedConnection
    );
}

/* End C Functions */
/* Begin Constructor/Destructor */

MpvTimer::MpvTimer(mpv_handle *mpv, QObject *parent) : CursorTimer(parent)
{
    m_handle = mpv_create_client(mpv, "mpvtimer");

    /* Get the timeout duration */
    {
        int64_t time = 0;
        mpv_get_property(m_handle, "cursor-autohide", MPV_FORMAT_INT64, &time);
        m_timer.setInterval(time);
    }

    /* Get the current state */
    {
        const char *state =
            mpv_get_property_string(m_handle, "cursor-autohide");

        if (strcmp(state, "no") == 0)
        {
            m_state = TimerState::Never;
        }
        else if (strcmp(state, "always") == 0)
        {
            m_state = TimerState::Always;
        }
        else
        {
            m_state = TimerState::Number;
        }
        mpv_free((void *)state);
    }

    /* Get the value of cursor-autohide-fs-only */
    {
        int fsOnly;
        mpv_get_property(
            m_handle, "cursor-autohide-fs-only", MPV_FORMAT_FLAG, &fsOnly
        );
        m_fullscreenOnly = !!fsOnly;
    }

    /* Get the value of fullscreen */
    {
        int fullscreen;
        mpv_get_property(m_handle, "fullscreen", MPV_FORMAT_FLAG, &fullscreen);
        m_fullscreen = !!fullscreen;
    }

    /* Observe the necessary properties */
    mpv_observe_property(
        m_handle, 0, "cursor-autohide", MPV_FORMAT_INT64);
    mpv_observe_property(
        m_handle, 0, "cursor-autohide", MPV_FORMAT_STRING);
    mpv_observe_property(
        m_handle, 0, "cursor-autohide-fs-only", MPV_FORMAT_FLAG);
    mpv_observe_property(
        m_handle, 0, "fullscreen", MPV_FORMAT_FLAG);

    mpv_set_wakeup_callback(m_handle, wakeup, this);

    /* Connect the timeout signals */
    connect(&m_timer, &QTimer::timeout, this, &MpvTimer::handleTimeout);
}

MpvTimer::~MpvTimer()
{
    disconnect();
    mpv_destroy(m_handle);
}

/* End Constructor/Destructor */
/* Begin Public Control Methods */

void MpvTimer::start()
{
    if (shouldIgnore())
    {
        Q_EMIT showCursor();
        return;
    }

    switch (m_state)
    {
    case TimerState::Number:
        m_timer.start();
        Q_EMIT showCursor();
        break;

    case TimerState::Always:
        Q_EMIT hideCursor();
        break;

    case TimerState::Never:
        Q_EMIT showCursor();
        break;
    }
}

void MpvTimer::stop()
{
    m_timer.stop();
}

void MpvTimer::forceTimeout()
{
    m_timer.stop();
    handleTimeout();
}

/* End Public Control Methods */
/* Begin Timer Event Handlers */

void MpvTimer::handleTimeout() const
{
    if (shouldIgnore())
    {
        Q_EMIT showCursor();
        return;
    }

    switch (m_state)
    {
    case TimerState::Number:
        Q_EMIT hideCursor();
        break;

    case TimerState::Always:
    case TimerState::Never:
        break;
    }
}

void MpvTimer::handleStateChange(TimerState state)
{
    m_state = state;

    if (shouldIgnore())
    {
        Q_EMIT showCursor();
        return;
    }

    switch (m_state)
    {
    case TimerState::Number:
        m_timer.start();
        break;

    case TimerState::Always:
        m_timer.stop();
        Q_EMIT hideCursor();
        break;

    case TimerState::Never:
        m_timer.stop();
        Q_EMIT showCursor();
        break;
    }
}

void MpvTimer::handleFullscreenOnlyChange(bool fsOnly)
{
    m_fullscreenOnly = fsOnly;
    handleStateChange(m_state);
}

void MpvTimer::handleFullscreenChange(bool fullscreen)
{
    m_fullscreen = fullscreen;
    handleStateChange(m_state);
}

/* End Timer Event Handlers */
/* Begin mpv Event Handlers */

void MpvTimer::handleEvents()
{
    /* Process all events, until the event queue is empty. */
    while (m_handle)
    {
        mpv_event *event = mpv_wait_event(m_handle, 0);
        switch (event->event_id)
        {
        case MPV_EVENT_PROPERTY_CHANGE:
            handlePropertyChange((mpv_event_property *)event->data);
            break;

        case MPV_EVENT_NONE:
            return;

        default:
            break;
        }
    }
}

void MpvTimer::handlePropertyChange(mpv_event_property *prop)
{
    if (strcmp(prop->name, "cursor-autohide") == 0)
    {
        switch (prop->format)
        {
        case MPV_FORMAT_INT64:
            m_timer.setInterval(*(int64_t *)prop->data);
            handleStateChange(TimerState::Number);
            break;

        case MPV_FORMAT_STRING:
        {
            const char *state = *(const char **)prop->data;
            if (strcmp(state, "always") == 0)
            {
                handleStateChange(TimerState::Always);
            }
            else if (strcmp(state, "no") == 0)
            {
                handleStateChange(TimerState::Never);
            }
            break;
        }

        default:
            break;
        }
    }
    else if (strcmp(prop->name, "cursor-autohide-fs-only") == 0)
    {
        switch (prop->format)
        {
        case MPV_FORMAT_FLAG:
            handleFullscreenOnlyChange(!!*(int *)prop->data);
            break;

        default:
            break;
        }
    }
    else if (strcmp(prop->name, "fullscreen") == 0)
    {
        switch (prop->format)
        {
        case MPV_FORMAT_FLAG:
            handleFullscreenChange(!!*(int *)prop->data);
            break;

        default:
            break;
        }
    }
}

/* End mpv Event Handlers */
/* Begin Getters */

inline bool MpvTimer::shouldIgnore() const
{
    return m_fullscreenOnly && !m_fullscreen;
}

/* End Getters */
