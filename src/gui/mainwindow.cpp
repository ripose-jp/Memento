#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mpvadapter.h"
#include "iconfactory.h"

#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);

    m_player = new MpvAdapter(m_ui->m_mpv, this);

    m_ui->m_controls->setVolumeLimit(m_player->getMaxVolume());

    // Toolbar Actions
    connect(m_ui->m_actionOpen, &QAction::triggered, m_player, &PlayerAdapter::open);

    // Buttons
    connect(m_ui->m_controls, &PlayerControls::play, m_player, &PlayerAdapter::play);
    connect(m_ui->m_controls, &PlayerControls::pause, m_player, &PlayerAdapter::pause);
    connect(m_ui->m_controls, &PlayerControls::seekForward, m_player, &PlayerAdapter::seekForward);
    connect(m_ui->m_controls, &PlayerControls::seekBackward, m_player, &PlayerAdapter::seekBackward);
    connect(m_ui->m_controls, &PlayerControls::fullscreenChanged, m_player, &PlayerAdapter::setFullscreen);

    // Slider
    connect(m_ui->m_controls, &PlayerControls::volumeSliderMoved, m_player, &PlayerAdapter::setVolume);
    connect(m_ui->m_controls, &PlayerControls::sliderMoved, m_player, &PlayerAdapter::seek);
    connect(m_player, &PlayerAdapter::durationChanged, m_ui->m_controls, &PlayerControls::setDuration);
    connect(m_player, &PlayerAdapter::positionChanged, m_ui->m_controls, &PlayerControls::setPosition);

    // State changes
    connect(m_player, &PlayerAdapter::stateChanged, m_ui->m_controls, &PlayerControls::setPaused);
    connect(m_player, &PlayerAdapter::fullscreenChanged, this, &MainWindow::setFullscreen);
    connect(m_player, &PlayerAdapter::fullscreenChanged, m_ui->m_controls, &PlayerControls::setFullscreen);
    connect(m_player, &PlayerAdapter::volumeChanged, m_ui->m_controls, &PlayerControls::setVolume);
    connect(m_player, &PlayerAdapter::hideCursor, [=] { if(isFullScreen()) m_ui->m_controls->hide(); } );
    connect(m_player, &PlayerAdapter::close, this, &QApplication::quit);

    // Key presses
    connect(this, &MainWindow::keyPressed, m_player, &PlayerAdapter::keyPressed);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        default: {
            Q_EMIT keyPressed(event);
        };
    }
}

void MainWindow::setFullscreen(bool value)
{
    if (value) {
        showFullScreen();
        m_ui->m_menubar->hide();
        QApplication::processEvents();
        m_ui->m_controls->hide();
        m_ui->m_centralwidget->layout()->removeWidget(m_ui->m_controls);
    } else {
        showNormal();
        m_ui->m_menubar->show();
        m_ui->m_controls->show();
        m_ui->m_centralwidget->layout()->addWidget(m_ui->m_controls);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isFullScreen() && m_ui->m_controls->isHidden()) {
        m_ui->m_controls->show();
    }
}

MainWindow::~MainWindow() {
    delete m_ui;
    delete m_player;
}