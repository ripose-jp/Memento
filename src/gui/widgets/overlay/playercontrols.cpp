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

#include <QSettings>

#include "gui/widgets/common/sliderjumpstyle.h"
#include "player/playeradapter.h"
#include "util/constants.h"

/* Begin Constructor/Destructor */

PlayerControls::PlayerControls(QPointer<Context> context, QWidget *parent) :
    QWidget(parent),
    m_ui(std::make_unique<Ui::PlayerControls>()),
    m_context(std::move(context))
{
    m_ui->setupUi(this);

    m_ui->sliderProgress->initialize(m_context);

#if defined(Q_OS_MACOS)
    m_ui->layoutButtons->setSpacing(2);
    m_ui->sliderVolume->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Minimum
    );
#endif

    initTheme();
    setCursor(Qt::ArrowCursor);

#ifdef OCR_SUPPORT
    initOCRSettings();
#else
    m_ui->buttonToggleOCR->hide();
#endif

    /* Signals */
    connect(
        m_ui->sliderProgress, &QSlider::sliderPressed, this,
        [this]
        {
            m_ignorePause = !m_paused;
            emit m_context->controlsPause();
        }
    );
    connect(
        m_ui->sliderProgress, &QSlider::sliderReleased, this,
        [this]
        {
            if (m_ignorePause)
            {
                m_ignorePause = false;
                emit m_context->controlsPlay();
            }
        }
    );
    connect(
        m_ui->sliderProgress, &QSlider::valueChanged,
        m_context, &Context::controlsPositionChanged,
        Qt::QueuedConnection
    );
    connect(
        m_ui->sliderVolume, &QSlider::valueChanged,
        m_context, &Context::controlsVolumeChanged
    );

    connect(
        m_ui->buttonPlay, &QToolButton::clicked,
        this, &PlayerControls::togglePause,
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonSeekForward, &QToolButton::clicked,
        m_context, &Context::controlsSeekForward,
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonSeekBackward, &QToolButton::clicked,
        m_context, &Context::controlsSeekBackward,
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonSkipForward, &QToolButton::clicked,
        m_context, &Context::controlsSkipForward,
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonSkipBackward, &QToolButton::clicked,
        m_context, &Context::controlsSkipBackward,
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonFullscreen, &QToolButton::clicked,
        this, &PlayerControls::toggleFullscreen,
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonToggleSubList, &QToolButton::clicked,
        m_context, &Context::controlsSubtitleListToggled,
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonToggleOCR, &QToolButton::clicked,
        m_context, &Context::controlsOCRToggled,
        Qt::QueuedConnection
    );

    /* Slots */
    connect(
        m_context, &Context::playerPauseStateChanged,
        this, &PlayerControls::setPaused,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerFullscreenChanged,
        this, &PlayerControls::setFullscreen,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerVolumeChanged,
        this, &PlayerControls::setVolume
    );
    connect(
        m_context, &Context::playerMaxVolumeChanged,
        this, &PlayerControls::setVolumeLimit
    );
    connect(
        m_context, &Context::playerDurationChanged,
        this, &PlayerControls::setDuration
    );
    connect(
        m_context, &Context::playerPositionChanged,
        this, &PlayerControls::setPosition,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::requestThemeRefresh,
        this, &PlayerControls::initTheme,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerChaptersChanged,
        m_ui->sliderProgress, &ProgressSlider::setChapters
    );

#ifdef OCR_SUPPORT
    connect(
        m_context, &Context::ocrSettingsChanged,
        this, &PlayerControls::initOCRSettings
    );
#endif // OCR_SUPPORT
}

PlayerControls::~PlayerControls()
{
    disconnect();
}

/* End Constructor/Destructor */
/* Begin Initializers */

void PlayerControls::initTheme()
{
    IconFactory *factory = IconFactory::create();
    m_ui->buttonPlay->setIcon(
        factory->getIcon(
            m_paused ? IconFactory::Icon::play : IconFactory::Icon::pause
        )
    );
    m_ui->buttonPlay->setAutoRaise(true);

    m_ui->buttonFullscreen->setIcon(
        factory->getIcon(IconFactory::Icon::fullscreen)
    );
    m_ui->buttonFullscreen->setAutoRaise(true);

    m_ui->buttonSeekBackward->setIcon(
        factory->getIcon(IconFactory::Icon::seek_backward)
    );
    m_ui->buttonSeekBackward->setAutoRaise(true);

    m_ui->buttonSeekForward->setIcon(
        factory->getIcon(IconFactory::Icon::seek_forward)
    );
    m_ui->buttonSeekForward->setAutoRaise(true);

    m_ui->buttonSkipBackward->setIcon(
        factory->getIcon(IconFactory::Icon::skip_backward)
    );
    m_ui->buttonSkipBackward->setAutoRaise(true);

    m_ui->buttonSkipForward->setIcon(
        factory->getIcon(IconFactory::Icon::skip_forward)
    );
    m_ui->buttonSkipForward->setAutoRaise(true);

    m_ui->buttonToggleSubList->setIcon(
        factory->getIcon(IconFactory::Icon::hamburger)
    );
    m_ui->buttonToggleSubList->setAutoRaise(true);

    m_ui->buttonToggleOCR->setIcon(
        factory->getIcon(IconFactory::Icon::eye)
    );
    m_ui->buttonToggleOCR->setAutoRaise(true);
}

#ifdef OCR_SUPPORT
void PlayerControls::initOCRSettings()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::OCR::GROUP);

    bool enabled = settings.value(
            Constants::Settings::OCR::ENABLED,
            Constants::Settings::OCR::ENABLED_DEFAULT
        ).toBool();
    m_ui->buttonToggleOCR->setVisible(enabled);

    settings.endGroup();
}
#endif // OCR_SUPPORT

/* End Initializers */
/* Begin Event Handlers */

void PlayerControls::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    emit m_context->controlsHidden();
}

void PlayerControls::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    emit m_context->controlsShown();
}

/* End Event Handlers */
/* Begin Slots */

void PlayerControls::setDuration(const double value)
{
    setPosition(0);
    m_duration = value;
    m_ui->sliderProgress->setRange(0, value);
    m_ui->labelTotal->setText(ProgressSlider::formatTime(value));
}

void PlayerControls::setPosition(const double value)
{
    if (value > m_duration)
        return;

    m_ui->sliderProgress->blockSignals(true);
    m_ui->sliderProgress->setValue(value);
    m_ui->sliderProgress->blockSignals(false);
    m_ui->labelCurrent->setText(ProgressSlider::formatTime(value));
}

void PlayerControls::setPaused(const bool paused)
{
    m_paused = paused;
    m_ui->buttonPlay->setIcon(
        IconFactory::create()->getIcon(
            m_paused ? IconFactory::Icon::play : IconFactory::Icon::pause
        )
    );
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

void PlayerControls::setFullscreen(const bool value)
{
    if (value)
    {
        m_ui->buttonFullscreen->setIcon(
            IconFactory::create()->getIcon(IconFactory::Icon::restore)
        );
    }
    else
    {
        m_ui->buttonFullscreen->setIcon(
            IconFactory::create()->getIcon(IconFactory::Icon::fullscreen)
        );
    }
}

/* End Slots */
/* Begin Button Implementations */

void PlayerControls::togglePause()
{
    if (m_paused)
    {
        emit m_context->controlsPlay();
    }
    else
    {
        emit m_context->controlsPause();
    }
}


void PlayerControls::toggleFullscreen()
{
    bool fullscreen = m_context->getPlayerAdapter()->isFullscreen();
    emit m_context->controlsFullscreenChanged(!fullscreen);
}

/* End Button Implementations */
