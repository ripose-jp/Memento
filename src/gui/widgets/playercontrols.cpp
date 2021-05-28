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

#include "playercontrols.h"
#include "ui_playercontrols.h"

#include "sliderjumpstyle.h"
#include "../../util/globalmediator.h"
#include "../playeradapter.h"

#define SECONDS_IN_MINUTE   60
#define SECONDS_IN_HOUR     3600
#define FILL_SPACES         2
#define BASE_TEN            10
#define FILL_CHAR           '0'

PlayerControls::PlayerControls(QWidget *parent) 
    : QWidget(parent), 
      m_ui(new Ui::PlayerControls),
      m_ignorePause(false),
      m_paused(true)
{
    m_ui->setupUi(this);
    
#if __APPLE__
    m_ui->layoutButtons->setSpacing(2);
#endif

    refreshTheme();
    setCursor(Qt::ArrowCursor);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Signals */
    connect(m_ui->sliderProgress, &QSlider::sliderPressed, this, 
        [=] {
            m_ignorePause = !m_paused;
            Q_EMIT mediator->controlsPause();
        }
    );
    connect(m_ui->sliderProgress, &QSlider::sliderReleased, this, 
        [=] {
            if (m_ignorePause)
            {
                m_ignorePause = false;
                Q_EMIT mediator->controlsPlay();
            }
        }
    );
    connect(m_ui->sliderProgress,      &QSlider::valueChanged, mediator, &GlobalMediator::controlsPositionChanged, Qt::QueuedConnection);
    connect(m_ui->sliderVolume,        &QSlider::valueChanged, mediator, &GlobalMediator::controlsVolumeChanged);

    connect(m_ui->buttonPlay,          &QToolButton::clicked, this,      &PlayerControls::pauseResume);
    connect(m_ui->buttonSeekForward,   &QToolButton::clicked, mediator,  &GlobalMediator::controlsSeekForward);
    connect(m_ui->buttonSeekBackward,  &QToolButton::clicked, mediator,  &GlobalMediator::controlsSeekBackward);
    connect(m_ui->buttonSkipForward,   &QToolButton::clicked, mediator,  &GlobalMediator::controlsSkipForward);
    connect(m_ui->buttonSkipBackward,  &QToolButton::clicked, mediator,  &GlobalMediator::controlsSkipBackward);
    connect(m_ui->buttonFullscreen,    &QToolButton::clicked, this,      &PlayerControls::toggleFullscreen);
    connect(m_ui->buttonToggleSubList, &QToolButton::clicked, mediator,  &GlobalMediator::controlsSubtitleListToggled);

    /* Slots */
    connect(mediator, &GlobalMediator::playerPauseStateChanged, this, &PlayerControls::setPaused);
    connect(mediator, &GlobalMediator::playerFullscreenChanged, this, &PlayerControls::setFullscreen);
    connect(mediator, &GlobalMediator::playerVolumeChanged,     this, &PlayerControls::setVolume);
    connect(mediator, &GlobalMediator::playerDurationChanged,   this, &PlayerControls::setDuration);
    connect(mediator, &GlobalMediator::playerPositionChanged,   this, &PlayerControls::setPosition);
    connect(mediator, &GlobalMediator::requestThemeRefresh,     this, &PlayerControls::refreshTheme);
}

PlayerControls::~PlayerControls()
{
    disconnect();
    delete m_ui;
}

void PlayerControls::refreshTheme()
{
    IconFactory *factory = IconFactory::create();
    m_ui->buttonPlay->         setIcon(factory->getIcon(m_paused ? IconFactory::Icon::play : IconFactory::Icon::pause));
    m_ui->buttonFullscreen->   setIcon(factory->getIcon(IconFactory::Icon::fullscreen));
    m_ui->buttonSeekBackward-> setIcon(factory->getIcon(IconFactory::Icon::seek_backward));
    m_ui->buttonSeekForward->  setIcon(factory->getIcon(IconFactory::Icon::seek_forward));
    m_ui->buttonSkipBackward-> setIcon(factory->getIcon(IconFactory::Icon::skip_backward));
    m_ui->buttonSkipForward->  setIcon(factory->getIcon(IconFactory::Icon::skip_forward));
    m_ui->buttonToggleSubList->setIcon(factory->getIcon(IconFactory::Icon::hamburger));

    m_ui->sliderProgress->setStyle(new SliderJumpStyle(QApplication::style()));
    m_ui->sliderVolume->setStyle(new SliderJumpStyle(QApplication::style()));
}

void PlayerControls::setDuration(const double value)
{
    setPosition(0);
    m_duration = value;
    m_startTime = -1;
    m_endTime = -1;
    m_ui->sliderProgress->setRange(0, value);
    m_ui->labelTotal->setText(formatTime(value));
}

void PlayerControls::setPosition(const double value)
{
    if (value > m_duration)
        return;
    
    m_ui->sliderProgress->blockSignals(true);
    m_ui->sliderProgress->setValue(value);
    m_ui->sliderProgress->blockSignals(false);
    m_ui->labelCurrent->setText(formatTime(value));
}

void PlayerControls::setPaused(const bool paused)
{
    m_paused = paused;
    m_ui->buttonPlay->setIcon(IconFactory::create()->getIcon(m_paused ? IconFactory::Icon::play : IconFactory::Icon::pause));
}

void PlayerControls::pauseResume()
{
    if (m_paused)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->controlsPlay();
    }
    else
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->controlsPause();
    }
}

void PlayerControls::setFullscreen(const bool value)
{
    if (value)
    {
        m_ui->buttonFullscreen->setIcon(IconFactory::create()->getIcon(IconFactory::Icon::restore));
    }
    else
    {
        m_ui->buttonFullscreen->setIcon(IconFactory::create()->getIcon(IconFactory::Icon::fullscreen));
    }
}

void PlayerControls::toggleFullscreen()
{
    bool fullscreen = GlobalMediator::getGlobalMediator()->getPlayerAdapter()->isFullScreen();
    Q_EMIT GlobalMediator::getGlobalMediator()->controlsFullscreenChanged(!fullscreen);
}

void PlayerControls::setVolumeLimit(const int64_t value)
{
    m_ui->sliderVolume->setRange(0, value);
}

void PlayerControls::setVolume(const int64_t value)
{
    m_ui->sliderVolume->blockSignals(true);
    m_ui->sliderVolume->setValue(value);
    m_ui->sliderVolume->blockSignals(false);
    QString volume = QString::number(value) + "%";
    m_ui->labelVolume->setText(volume);
}

QString PlayerControls::formatTime(const int time)
{
    int hours   = time / SECONDS_IN_HOUR;
    int minutes = (time % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
    int seconds = time % SECONDS_IN_MINUTE;

    QString formatted = QString("%1:%2")
        .arg(minutes, FILL_SPACES, BASE_TEN, QChar(FILL_CHAR))
        .arg(seconds, FILL_SPACES, BASE_TEN, QChar(FILL_CHAR));
    if (hours)
    {
        formatted.prepend(QString("%1:").arg(hours));
    }

    return formatted;
}

void PlayerControls::hideEvent(QHideEvent *event)
{
    Q_EMIT GlobalMediator::getGlobalMediator()->controlsHidden();
}

void PlayerControls::showEvent(QShowEvent *event)
{
    Q_EMIT GlobalMediator::getGlobalMediator()->controlsShown();
}