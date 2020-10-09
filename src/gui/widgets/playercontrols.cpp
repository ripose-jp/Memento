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

#define SECONDS_IN_MINUTE 60
#define SECONDS_IN_HOUR 3600
#define FILL_SPACES 2
#define BASE_TEN 10
#define FILL_CHAR '0'

PlayerControls::PlayerControls(QWidget *parent) : QWidget(parent), 
                                                  m_ui(new Ui::PlayerControls)
{
    m_ui->setupUi(this);

    m_iconFactory = IconFactory::create(this);
    setIcons();
    m_ui->m_buttonStop->hide(); // stop button may be removed entirely

    m_ui->m_sliderProgress->setStyle(
        new SliderJumpStyle(m_ui->m_sliderProgress->style()));
    m_ui->m_sliderVolume->setStyle(
        new SliderJumpStyle(m_ui->m_sliderVolume->style()));

    connect(m_ui->m_sliderProgress, &QSlider::sliderPressed,
        this, &PlayerControls::pause);
    connect(m_ui->m_sliderProgress, &QSlider::sliderReleased,
        this, &PlayerControls::play);
    connect(m_ui->m_sliderProgress, &QSlider::valueChanged,
        this, &PlayerControls::sliderMoved, Qt::QueuedConnection);

    connect(m_ui->m_sliderVolume, &QSlider::valueChanged,
        this, &PlayerControls::volumeSliderMoved);

    connect(m_ui->m_buttonPlay, &QToolButton::clicked,
        this, &PlayerControls::pauseResume);
    connect(m_ui->m_buttonSeekForward, &QToolButton::clicked,
        this, &PlayerControls::seekForward);
    connect(m_ui->m_buttonSeekBackward, &QToolButton::clicked,
        this, &PlayerControls::seekBackward);
    connect(m_ui->m_buttonSkipForward, &QToolButton::clicked,
        this, &PlayerControls::skipForward);
    connect(m_ui->m_buttonSkipBackward, &QToolButton::clicked,
        this, &PlayerControls::skipBackward);
    connect(m_ui->m_buttonStop, &QToolButton::clicked,
        this, &PlayerControls::stop);
    connect(m_ui->m_buttonFullscreen, &QToolButton::clicked,
        this, &PlayerControls::toggleFullscreen);

    connect(m_ui->m_subtitle, &SubtitleWidget::entriesChanged,
        this, &PlayerControls::entriesChanged);
    connect(m_ui->m_subtitle, &SubtitleWidget::textChanged,
        this, &PlayerControls::hideDefinition);
    connect(this, &PlayerControls::definitionHidden,
        m_ui->m_subtitle, &SubtitleWidget::deselectText);
}

PlayerControls::~PlayerControls()
{
    delete m_ui;
    delete m_iconFactory;
}

void PlayerControls::setIcons()
{
    m_ui->m_buttonPlay->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::play));
    m_ui->m_buttonStop->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::stop));

    m_ui->m_buttonFullscreen->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::fullscreen));

    m_ui->m_buttonSeekBackward->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::seek_backward));
    m_ui->m_buttonSeekForward->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::seek_forward));

    m_ui->m_buttonSkipBackward->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::skip_backward));
    m_ui->m_buttonSkipForward->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::skip_forward));
}

void PlayerControls::setDuration(const int value)
{
    setPosition(0);
    m_duration = value;
    m_endtime = -1;
    m_ui->m_sliderProgress->setRange(0, value);
    m_ui->m_labelTotal->setText(formatTime(value));
}

void PlayerControls::setPosition(const int value)
{
    if (value > m_duration)
        return;
    
    m_ui->m_sliderProgress->blockSignals(true);
    m_ui->m_sliderProgress->setValue(value);
    m_ui->m_sliderProgress->blockSignals(false);
    m_ui->m_labelCurrent->setText(formatTime(value));

    if (value > m_endtime)
        m_ui->m_subtitle->updateText("");
}

void PlayerControls::setPaused(const bool paused)
{
    m_paused = paused;
    if (m_paused)
    {
        m_ui->m_buttonPlay->setIcon(
            m_iconFactory->getIcon(IconFactory::Icon::play));
    }
    else
    {
        m_ui->m_buttonPlay->setIcon(
            m_iconFactory->getIcon(IconFactory::Icon::pause));
    }
}

void PlayerControls::pauseResume()
{
    if (m_paused)
    {
        Q_EMIT play();
    }
    else
    {
        Q_EMIT pause();
    }
}

void PlayerControls::setFullscreen(const bool value)
{
    m_fullscreen = value;
    if (m_fullscreen)
    {
        m_ui->m_buttonFullscreen->setIcon(
            m_iconFactory->getIcon(IconFactory::Icon::restore));
    }
    else
    {
        m_ui->m_buttonFullscreen->setIcon(
            m_iconFactory->getIcon(IconFactory::Icon::fullscreen));
    }
}

void PlayerControls::toggleFullscreen()
{
    Q_EMIT fullscreenChanged(!m_fullscreen);
}

void PlayerControls::setVolumeLimit(const int value)
{
    m_ui->m_sliderVolume->setRange(0, value);
}

void PlayerControls::setVolume(const int value)
{
    m_ui->m_sliderVolume->blockSignals(true);
    m_ui->m_sliderVolume->setValue(value);
    m_ui->m_sliderVolume->blockSignals(false);
    QString volume = QString::number(value) + "%";
    m_ui->m_labelVolume->setText(volume);
}

void PlayerControls::setSubtitle(const QString &subtitle, const int end)
{
    m_ui->m_subtitle->updateText(subtitle);
    m_endtime = end;
}

QString PlayerControls::formatTime(const int time)
{
    int hours = time / SECONDS_IN_HOUR;
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

void PlayerControls::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
    if (!m_ui->m_subtitle->underMouse())
    {
        m_ui->m_subtitle->deselectText();
        Q_EMIT hideDefinition();
    }
}