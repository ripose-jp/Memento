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

#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QHash>
#include <QMouseEvent>
#include <QOpenGLWidget>
#include <QRegularExpression>
#include <QTimer>

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include "util/utils.h"

#if defined(Q_OS_MACOS)
#include <IOKit/pwr_mgt/IOPMLib.h>
#endif

class CursorTimer;

/**
 * Widget that displays the video output from mpv.
 */
class MpvWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    MpvWidget(QWidget *parent = nullptr);
    ~MpvWidget();

    /**
     * Get the mpv client context
     * @return mpv_handle for the MpvWidget
     */
    mpv_handle *getHandle() const { return m_mpv; }

    /**
     * Returns the recommend size of the widget.
     * Used as a hack to prevent the SubtitleList from eating up space when
     * shown.
     * @return The recommend size of the widget.
     */
    QSize sizeHint() const override { return QSize(480, 270); }

Q_SIGNALS:
    /**
     * Emitted whenever the duration of the current media changes.
     * @param value The length of the media in seconds.
     */
    void durationChanged(const double value) const;

    /**
     * Emitted whenever the current position of the current media changes.
     * @param value The current time in seconds.
     */
    void positionChanged(const double value) const;

    /**
     * Emitted when the maximum volume of mpv changes.
     * @param value The maximum possible volume.
     */
    void volumeMaxChanged(const int64_t value) const;

    /**
     * Emitted when the internal volume of mpv changes.
     * @param value The current volume.
     */
    void volumeChanged(const int64_t value) const;

    /**
     * Emitted when the pause state is changed.
     * @param paused true when mpv is paused, false otherwise.
     */
    void pauseChanged(const bool paused) const;

    /**
     * Emitted when the fullscreen state is changed.
     * @param full true if mpv is fullscreen, false otherwise.
     */
    void fullscreenChanged(const bool full) const;

    /**
     * Emitted when the number of tracks changes.
     * @param node The mpv_node containing information on all the tracks.
     */
    void tracklistChanged(const mpv_node *node) const;

    /**
     * Emitted when the chapters change.
     * @param chapters A list of the starting time in seconds of each chapter.
     */
    void chaptersChanged(const QList<double> &chapters) const;

    /**
     * Emitted when the title of the current media file changes.
     * @param name The title of the current media file, path if there is no
     *             title.
     */
    void titleChanged(QString name) const;

    /**
     * Emitted when the current file being played changes.
     * @param path The path of the current file being played.
     */
    void fileChanged(QString path) const;

    /**
     * Emitted when the current video track changes to a different id.
     * @param id The id of the current video track starting at 1.
     */
    void videoTrackChanged(const int64_t id) const;

    /**
     * Emitted when the current audio track changes to a different id.
     * @param id The id of the current audio track starting at 1.
     */
    void audioTrackChanged(const int64_t id) const;

    /**
     * Emitted when the current subtitle track changes to a different id.
     * @param id The id of the current subtitle track starting at 1.
     */
    void subtitleTrackChanged(const int64_t id) const;

    /**
     * Emitted when the current secondary subtitle track changes to a different
     * id.
     * @param id The id of the current secondary subtitle track starting at 1.
     */
    void subtitleTwoTrackChanged(const int64_t id) const;

    /**
     * Emitted when the audio track is disabled.
     */
    void audioDisabled() const;

    /**
     * Emitted when the video track is disabled.
     */
    void videoDisabled() const;

    /**
     * Emitted when the subtitle track is disabled.
     */
    void subtitleDisabled() const;

    /**
     * Emitted when the secondary subtitle track is disabled.
     */
    void subtitleTwoDisabled();

    /**
     * Emitted when the current subtitle changes.
     * @param subtitle The text of the current subtitle.
     * @param start    The start time of the current subtitle in seconds.
     * @param end      The end time of the current subtitle in seconds.
     * @param delay    The signed delay of the current subtitle in seconds.
     */
    void subtitleChanged(const QString &subtitle,
                         const double start,
                         const double end,
                         const double delay) const;

    /**
     * Emitted when the current subtitle changes. Unflitered by regex.
     * @param subtitle The text of the current subtitle.
     * @param start    The start time of the current subtitle in seconds.
     * @param end      The end time of the current subtitle in seconds.
     * @param delay    The signed delay of the current subtitle in seconds.
     */
    void subtitleChangedRaw(const QString &subtitle,
                            const double start,
                            const double end,
                            const double delay) const;

    /**
     * Emitted when the current secondary subtitle changes.
     * @param subtitle The text of the current secondary subtitle.
     * @param start    The start time of the current secondary subtitle in
     *                 seconds.
     * @param end      The end time of the current secondary subtitle in
     *                 seconds.
     * @param delay    The signed delay of the current subtitle in seconds.
     */
    void subtitleChangedSecondary(const QString &subtitle,
                                  const double start,
                                  const double end,
                                  const double delay) const;

    /**
     * Emitted when the subtitle delay is changed.
     * @param delay The signed delay in seconds.
     */
    void subDelayChanged(const double delay) const;

    /**
     * Emitted when the secondary subtitle delay is changed.
     * @param delay The signed delay in seconds.
     */
    void secSubDelayChanged(const double delay) const;

    /**
     * Emitted when MpvWidget hides the cursor.
     */
    void cursorHidden() const;

    /**
     * Emitted when the mouse is moved across the MpvWidget.
     * @param event The mouse move event.
     */
    void mouseMoved(const QMouseEvent *event) const;

    /**
     * Emitted when a new file is loaded.
     * @param width  The width of the media.
     * @param height The height of the media.
     */
    void newFileLoaded(int64_t width, int64_t height) const;

    /**
     * Emitted when mpv is shutdown.
     */
    void shutdown() const;

protected:
    /**
     * Called once before the first call to paintGL().
     * Initializes mpv's OpenGL render context.
     */
    void initializeGL() override;

    /**
     * Paints the current mpv frame.
     */
    void paintGL() override;

    /**
     * Called when the widget is resized.
     * @param width The new width of the widget. Not accurate on HiDPI displays.
     * @param height The new width of the widget. Not accurate on HiDPI
     *        displays.
     */
    void resizeGL(int width, int height) override;

    /**
     * Handles events that can only done on first show.
     * @param event The show event.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Called when the mouse is moved.
     * @param event The mouse move event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * Called when the mouse is released.
     * Used for passing mouse events to mpv.
     * @param event The mouse release event.
     */
    void mouseReleaseEvent(QMouseEvent *event) override;

    /**
     * Called when the widget is double clicked.
     * Used for passing mouse events to mpv.
     * @param event The double click event.
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private Q_SLOTS:
    /**
     * Handles updates that come about due to a screen change.
     * @param screen The new screen.
     */
    void screenChanged(QScreen *screen);

    /**
     * Processes all the events in the event queue.
     */
    void onMpvEvents();

    /**
     * Reports a frameswap to mpv for better timing.
     */
    void reportFrameSwap();

    /**
     * Updates the current output if it needs to be updated.
     */
    void maybeUpdate();

    /**
     * Initializes the subtitle regex filter from the saved settings value.
     */
    void initSubtitleRegex();

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    /**
     * Initializes screen dimensions with regards to DPI scaling.
     */
    void initDimensions();
#endif

    /**
     * Initializes the cursor timer.
     */
    void initTimer();

    /**
     * Initializes mpv options from the command line.
     */
    void initOptions();

    /**
     * Shows the cursor over this widget.
     */
    void showCursor();

    /**
     * Hides the cursor over this widget.
     */
    void hideCursor();

private:
    /**
     * Initializes screen signals. This needs to be called after the widget is
     * shown.
     */
    void initScreenSignals();

    /**
     * Initializes m_propertyMap with all the event handling functions.
     */
    void initPropertyMap();

    /**
     * Handles an mpv_event if there is an associated event handler.
     * @param event The event to handle.
     */
    void handleMpvEvent(mpv_event *event);

    /**
     * Allows the screen to dim.
     * Can be undone with a call to preventScreenDimming().
     */
    void allowScreenDimming();

    /**
     * Prevents the screen from dimming.
     * Can be undone with a call to allowScreenDimming().
     */
    void preventScreenDimming();

    /**
     * Checks if a property exists.
     * @return true if the property exists, false otherwise.
     */
    bool propertyExists(const char *prop);

    /* The mpv context */
    mpv_handle *m_mpv = nullptr;

    /* The mpv render context */
    mpv_render_context *mpv_gl;

    /* Keeps track if this widget has already been shown once before or not */
    bool m_firstShow = true;

    /* The DPI ratio. */
    qreal m_devicePixelRatio = 1.0;

    /* Override default device pixel ratio */
    bool m_overrideDevicePixelRatio = false;

    /* The height of the player adjusted for DPI. */
    int m_height;

    /* The width of the player adjusted for DPI. */
    int m_width;

    /* Maps mpv properties to the appropriate event handling function */
    QHash<QString, std::function<void(mpv_event_property *)>> m_propertyMap;

    /* Timer responsible for showing and hiding the cursor */
    CursorTimer *m_cursorTimer = nullptr;

    /* Regular expression used to filter subtitles */
    QRegularExpression m_regex;

    /* true if secondary-sub-delay is supported, false otherwise */
    bool m_secSubDelaySupported = false;

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    /* The DBus cookie. Used for allowing the screen to dim again. */
    uint32_t dbus_cookie;
#endif

#if defined(Q_OS_MACOS)
    /* Assertion ID for preventing sleep on macOS */
    IOPMAssertionID m_sleepAssertID;

    /* True if the AssertionID is currently valid. */
    bool m_sleepAssertIDValid;
#endif
};

#endif // MPVWIDGET_H
