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

struct Term;

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget *parent = 0);
    ~PlayerControls();

public Q_SLOTS:
    void setDuration(const double value);
    void setPaused(const bool paused);
    void setFullscreen(const bool value);
    void setVolumeLimit(const int64_t value);
    void setVolume(const int64_t value);
    void setPosition(const double value);

protected:
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    
private Q_SLOTS:
    void refreshIcons();
    void pauseResume();
    void toggleFullscreen();

private:
    Ui::PlayerControls *m_ui;
    
    bool   m_paused;
    bool   m_ignorePause;
    double m_duration;
    double m_startTime;
    double m_endTime;

    QString formatTime(int time);
};

#endif // PLAYERCONTROLS_H