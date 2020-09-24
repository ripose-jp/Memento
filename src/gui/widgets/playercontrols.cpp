#include "playercontrols.h"
#include "ui_playercontrols.h"

PlayerControls::PlayerControls(QWidget *parent) : QWidget(parent), m_ui(new Ui::PlayerControls)
{
    m_ui->setupUi(this);
    
    connect(m_ui->m_sliderProgress, &QSlider::sliderMoved, this, &PlayerControls::sliderMoved);
    connect(m_ui->m_buttonPlay, &QToolButton::clicked, this, &PlayerControls::pauseResume);
    connect(m_ui->m_buttonSeekForward, &QToolButton::clicked, this, &PlayerControls::seekForward);
    connect(m_ui->m_buttonSeekBackward, &QToolButton::clicked, this, &PlayerControls::seekBackward);
}

void PlayerControls::setDuration(int value)
{
    m_ui->m_sliderProgress->setRange(0, value);
}

void PlayerControls::setPosition(int value)
{
    m_ui->m_sliderProgress->setValue(value);
}

void PlayerControls::setPaused(bool paused) {
    m_paused = paused;
    if(m_paused) {
        m_ui->m_buttonPlay->setIcon(QIcon::fromTheme(QString::fromUtf8(PLAY_ICON)));
    } else {
        m_ui->m_buttonPlay->setIcon(QIcon::fromTheme(QString::fromUtf8(PAUSE_ICON)));
    }
}

void PlayerControls::pauseResume()
{
    if(m_paused) {
        emit play();
    } else {
        emit pause();
    }
}

PlayerControls::~PlayerControls()
{
    delete m_ui;
}