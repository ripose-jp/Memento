#include "playercontrols.h"
#include "ui_playercontrols.h"
#include "../iconfactory.h"
#include "sliderjumpstyle.h"

PlayerControls::PlayerControls(QWidget *parent) : QWidget(parent), m_ui(new Ui::PlayerControls)
{
    m_ui->setupUi(this);

    // Make QSliders jump to the position clicked
    m_ui->m_sliderProgress->setStyle(new SliderJumpStyle(m_ui->m_sliderProgress->style()));
    m_ui->m_sliderVolume->setStyle(new SliderJumpStyle(m_ui->m_sliderVolume->style()));
    
    connect(m_ui->m_sliderProgress, &QSlider::sliderPressed, this, &PlayerControls::pause);
    connect(m_ui->m_sliderProgress, &QSlider::sliderReleased, this, &PlayerControls::play);
    connect(m_ui->m_sliderProgress, &QSlider::valueChanged, this, &PlayerControls::sliderMoved, Qt::QueuedConnection);

    connect(m_ui->m_sliderVolume, &QSlider::valueChanged, this, &PlayerControls::volumeSliderMoved);
    connect(m_ui->m_buttonPlay, &QToolButton::clicked, this, &PlayerControls::pauseResume);
    connect(m_ui->m_buttonSeekForward, &QToolButton::clicked, this, &PlayerControls::seekForward);
    connect(m_ui->m_buttonSeekBackward, &QToolButton::clicked, this, &PlayerControls::seekBackward);
    connect(m_ui->m_buttonFullscreen, &QToolButton::clicked, this, &PlayerControls::toggleFullscreen);
}

void PlayerControls::setDuration(const int value)
{
    setPosition(0);
    m_ui->m_sliderProgress->setRange(0, value);
    m_ui->m_labelTotal->setText(formatTime(value));
}

void PlayerControls::setPosition(const int value)
{
    m_ui->m_sliderProgress->blockSignals(true);
    m_ui->m_sliderProgress->setValue(value);
    m_ui->m_sliderProgress->blockSignals(false);
    m_ui->m_labelCurrent->setText(formatTime(value));
}

void PlayerControls::setPaused(const bool paused) {
    m_paused = paused;
    if (m_paused) {
        m_ui->m_buttonPlay->setIcon(IconFactory::getIcon(IconFactory::Icon::play, this));
    } else {
        m_ui->m_buttonPlay->setIcon(IconFactory::getIcon(IconFactory::Icon::pause, this));
    }
}

void PlayerControls::pauseResume()
{
    if (m_paused) {
        Q_EMIT play();
    } else {
        Q_EMIT pause();
    }
}

void PlayerControls::setFullscreen(const bool value)
{
    m_fullscreen = value;
    if (m_fullscreen) {
        m_ui->m_buttonFullscreen->setIcon(IconFactory::getIcon(IconFactory::Icon::restore, this));
    } else {
        m_ui->m_buttonFullscreen->setIcon(IconFactory::getIcon(IconFactory::Icon::fullscreen, this));
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
    m_ui->m_sliderVolume->setValue(value);
    QString volume = QString::number(value) + "%";
    m_ui->m_labelVolume->setText(volume);
}

QString PlayerControls::formatTime(const int time)
{
    int hours = time / SECONDS_IN_HOUR;
    int minutes = (time % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
    int seconds = time % SECONDS_IN_MINUTE;
    
    QString formatted = QString("%1:%2").arg(minutes, FILL_SPACES, BASE_TEN, QChar(FILL_CHAR))
                                        .arg(seconds, FILL_SPACES, BASE_TEN, QChar(FILL_CHAR));
    if (hours) {
        formatted.prepend(QString("%1:").arg(hours));
    }

    return formatted;
}

PlayerControls::~PlayerControls()
{
    delete m_ui;
}