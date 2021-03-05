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
#define DOUBLE_DELTA 0.05

PlayerControls::PlayerControls(QWidget *parent) : QWidget(parent), 
                                                  m_ui(new Ui::PlayerControls)
{
    m_ui->setupUi(this);

    m_iconFactory = IconFactory::create(this);
    setIcons();
    m_ui->buttonStop->hide(); // stop button may be removed entirely

    m_ui->sliderProgress->setStyle(
        new SliderJumpStyle(m_ui->sliderProgress->style()));
    m_ui->sliderVolume->setStyle(
        new SliderJumpStyle(m_ui->sliderVolume->style()));

    connect(m_ui->sliderProgress, &QSlider::sliderPressed, [=] {
        blockSignals(true);
        Q_EMIT pause();
        blockSignals(false);
    });
    connect(m_ui->sliderProgress, &QSlider::sliderReleased, [=] {
        if (!m_paused)
        {
            Q_EMIT play();
        }
    });
    connect(m_ui->sliderProgress, &QSlider::valueChanged,
        this, &PlayerControls::sliderMoved, Qt::QueuedConnection);

    connect(m_ui->sliderVolume, &QSlider::valueChanged,
        this, &PlayerControls::volumeSliderMoved);

    connect(m_ui->buttonPlay, &QToolButton::clicked,
        this, &PlayerControls::pauseResume);
    connect(m_ui->buttonSeekForward, &QToolButton::clicked,
        this, &PlayerControls::seekForward);
    connect(m_ui->buttonSeekBackward, &QToolButton::clicked,
        this, &PlayerControls::seekBackward);
    connect(m_ui->buttonSkipForward, &QToolButton::clicked,
        this, &PlayerControls::skipForward);
    connect(m_ui->buttonSkipBackward, &QToolButton::clicked,
        this, &PlayerControls::skipBackward);
    connect(m_ui->buttonStop, &QToolButton::clicked,
        this, &PlayerControls::stop);
    connect(m_ui->buttonFullscreen, &QToolButton::clicked,
        this, &PlayerControls::toggleFullscreen);
    connect(m_ui->buttonToggleSubList, &QToolButton::clicked,
        this, &PlayerControls::subtitleListToggled);

    connect(m_ui->subtitle, &SubtitleWidget::entriesChanged,
        this, &PlayerControls::entriesChanged);
    connect(m_ui->subtitle, &SubtitleWidget::textChanged,
        this, &PlayerControls::hideDefinition);
    connect(this, &PlayerControls::definitionHidden,
        m_ui->subtitle, &SubtitleWidget::deselectText);
    connect(this, &PlayerControls::jmDictUpdated,
        m_ui->subtitle, &SubtitleWidget::jmDictUpdated);
}

PlayerControls::~PlayerControls()
{
    delete m_ui;
    delete m_iconFactory;
}

void PlayerControls::setIcons()
{
    m_ui->buttonPlay->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::play));
    m_ui->buttonStop->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::stop));

    m_ui->buttonFullscreen->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::fullscreen));

    m_ui->buttonSeekBackward->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::seek_backward));
    m_ui->buttonSeekForward->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::seek_forward));

    m_ui->buttonSkipBackward->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::skip_backward));
    m_ui->buttonSkipForward->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::skip_forward));

    m_ui->buttonToggleSubList->setIcon(
        m_iconFactory->getIcon(IconFactory::Icon::hamburger));
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

    if (value < m_startTime - DOUBLE_DELTA || value > m_endTime + DOUBLE_DELTA)
        m_ui->subtitle->updateText("");
}

void PlayerControls::setPaused(const bool paused)
{
    m_paused = paused;
    m_ui->buttonPlay->setIcon(
        m_iconFactory->getIcon(
            m_paused ? IconFactory::Icon::play : IconFactory::Icon::pause));
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
        m_ui->buttonFullscreen->setIcon(
            m_iconFactory->getIcon(IconFactory::Icon::restore));
    }
    else
    {
        m_ui->buttonFullscreen->setIcon(
            m_iconFactory->getIcon(IconFactory::Icon::fullscreen));
    }
}

void PlayerControls::toggleFullscreen()
{
    Q_EMIT fullscreenChanged(!m_fullscreen);
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

void PlayerControls::setSubtitle(const QString &subtitle,
                                 const double start, 
                                 const double end,
                                 const double delay)
{
    m_ui->subtitle->updateText(subtitle);
    m_startTime = start + delay;
    m_endTime = end + delay;
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
    if (!m_ui->subtitle->underMouse())
    {
        m_ui->subtitle->deselectText();
        Q_EMIT hideDefinition();
    }
}