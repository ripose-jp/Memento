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

#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>

#include "util/iconfactory.h"

namespace Ui
{
    class PlayerControls;
}

struct Term;

/**
 * Widget containing graphical controls for the player.
 */
class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget *parent = nullptr);
    ~PlayerControls();

public Q_SLOTS:
    /**
     * Updates the controls for the duration specified.
     * @param value The duration in seconds of the video.
     */
    void setDuration(const double value);

    /**
     * Updates the progress bar with the current timecode.
     * @param value The current time in seconds.
     */
    void setPosition(const double value);

    /**
     * Updates the controls to display the pause state.
     * @param paused true if paused, false otherwise.
     */
    void setPaused(const bool paused);

    /**
     * Updates the controls to display the fullscreen state.
     * @param value true if fullscreen, false otherwise.
     */
    void setFullscreen(const bool value);

    /**
     * Updates the controls to display the maximum volume the player supports.
     * @param value The maximum volume.
     */
    void setVolumeLimit(const int64_t value);

    /**
     * Updates the controls to display the current volume.
     * @param value The current volume.
     */
    void setVolume(const int64_t value);

protected:
    /**
     * Used for emitting the controlsHidden signal.
     * @param event The hide event, not used.
     */
    void hideEvent(QHideEvent *event) override;

    /**
     * Used for emitting the controlsShown signal.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    /**
     * Initializes the stylesheets and icons of the player.
     */
    void initTheme();

#ifdef OCR_SUPPORT
    /**
     * Initializes OCR related controls.
     */
    void initOCRSettings();
#endif // OCR_SUPPORT

    /**
     * Handles the clicking of the pause/play button.
     */
    void togglePause();

    /**
     * Handles the clicking of the fullscreen button.
     */
    void toggleFullscreen();

private:
    /* UI object containing all the widgets. */
    Ui::PlayerControls *m_ui;

    /* Current pause state of the player. */
    bool m_paused;

    /* Flag used to determine if the pause state should be ignored. Used for
     * slider signals.
     */
    bool m_ignorePause;

    /* The current duration of the media playing. */
    double m_duration;
};

#endif // PLAYERCONTROLS_H
