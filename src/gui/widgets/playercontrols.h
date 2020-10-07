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

#include "../../util/iconfactory.h"

namespace Ui
{
    class PlayerControls;
}

struct Entry;

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget *parent = 0);
    ~PlayerControls();

public Q_SLOTS:
    void setDuration(const int value);
    void setPaused(const bool paused);
    void setFullscreen(const bool value);
    void setVolumeLimit(const int value);
    void setVolume(const int value);
    void setPosition(const int value);
    void setSubtitle(const QString &subtitle, const int end);

Q_SIGNALS:
    void play();
    void pause();
    void stop();
    void seekForward();
    void seekBackward();
    void skipForward();
    void skipBackward();
    void sliderMoved(const int value);
    void volumeSliderMoved(const int value);
    void fullscreenChanged(const bool value);
    void entryChanged(const Entry *entry);
    void definitionHidden();
    void hideDefinition();

private Q_SLOTS:
    void pauseResume();
    void toggleFullscreen();

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::PlayerControls *m_ui;
    bool m_paused;
    bool m_fullscreen;
    int m_endtime;
    IconFactory *m_iconFactory;

    QString formatTime(int time);
    void setIcons();
};

#endif // PLAYERCONTROLS_H