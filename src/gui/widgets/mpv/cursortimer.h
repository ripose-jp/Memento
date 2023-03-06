////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 Ripose
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

#ifndef CURSORTIMER_H
#define CURSORTIMER_H

#include <QObject>

#include <QTimer>

struct mpv_event_property;
struct mpv_handle;

/**
 * Interface class describing the functionality of a cursor timer.
 */
class CursorTimer : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

Q_SIGNALS:
    /**
     * Emitted when the cursor should be shown.
     */
    void showCursor() const;

    /**
     * Emitted when the cursor should be hidden.
     */
    void hideCursor() const;

public Q_SLOTS:
    /**
     * Starts the timer.
     */
    virtual void start() = 0;

    /**
     * Stops the timer.
     */
    virtual void stop() = 0;

    /**
     * Forces the timer into the timeout state.
     */
    virtual void forceTimeout() = 0;
};

/**
 * Class that synchronizes behavior with the Memento OSC.
 */
class OSCTimer : public CursorTimer
{
    Q_OBJECT

public:
    OSCTimer(QObject *parent = nullptr);
    ~OSCTimer();

public Q_SLOTS:
    void start() override;

    void stop() override;

    void forceTimeout() override;

private:
    /* The timer backing this timer */
    QTimer m_timer;
};

/**
 * Class that synchronizes behavior with mpv.
 */
class MpvTimer : public CursorTimer
{
    Q_OBJECT

public:
    /**
     * Constructs a new MpvTimer.
     * @param mpv The mpv handle this timer is connected to.
     * @param parent The parent that created this timer.
     */
    MpvTimer(mpv_handle *mpv, QObject *parent = nullptr);
    ~MpvTimer();

public Q_SLOTS:
    void start() override;

    void stop() override;

    void forceTimeout() override;

private Q_SLOTS:
    /**
     * Handles the backend timer timing out.
     */
    void handleTimeout() const;

    /**
     * Handles mpv events.
     */
    void handleEvents();

private:
    /* The potentially states cursor-autohide can take */
    enum class TimerState
    {
        /* Cursor duration is decided by the currently set time. */
        Number,

        /* The cursor is always visible. */
        Always,

        /* The cursor is never visible. */
        Never,
    };

    /**
     * Handles an mpv property change event.
     * @param event The mpv property that changed.
     */
    void handlePropertyChange(mpv_event_property *prop);

    /**
     * Handles a cursor-autohide state change.
     * @param state The new autohide state.
     */
    void handleStateChange(TimerState state);

    /**
     * Handles a cursor-autohide-fs-only state change.
     * @param fsOnly The new autohide-fs-only state change.
     */
    void handleFullscreenOnlyChange(bool fsOnly);

    void handleFullscreenChange(bool fullscreen);

    /**
     * Returns if the mpv instance is fullscreen.
     * @return true if the mpv instance is fullscreen, false otherwise.
     */
    bool isFullscreen() const;

    /**
     * Returns if logic should be ignored in favor of always showing the cursor.
     * @return If true, keep the cursor visible, otherwise use logic to
     * determine whether or not to show the cursor.
     */
    inline bool shouldIgnore() const;

    /* The timer backing this timer */
    QTimer m_timer;

    /* The mpv_handle for this timer */
    mpv_handle *m_handle;

    /* The current state of cursor-autohide */
    TimerState m_state = TimerState::Number;

    /* Indicates if mpv is currently fullscreen */
    bool m_fullscreen = false;

    /* Indicates if the timer respects cursor-autohide in fullscreen only */
    bool m_fullscreenOnly = false;
};

#endif // CURSORTIMER_H
