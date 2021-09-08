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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QMimeData>
#include <QSettings>

#include "../dict/dictionary.h"
#include "../util/constants.h"
#include "../util/iconfactory.h"
#include "mpvadapter.h"

/* Begin Constructor/Destructor */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_mediator(GlobalMediator::getGlobalMediator()),
      m_ui(new Ui::MainWindow),
      m_ankiClient(new AnkiClient(this)),
      m_maximized(false)
{
    m_ui->setupUi(this);

    /* Player Adapter */
    m_player = new MpvAdapter(m_ui->player, this);
    m_player->pause();

    /* Player Overlay */
    m_overlay = new PlayerOverlay(m_ui->player);

    /* Subtitle List */
    m_mediator->setSubtitleList(m_ui->subtitleList);
    m_ui->subtitleList->hide();

    /* Options */
    m_optionsWindow = new OptionsWindow;
    m_optionsWindow->hide();

    /* About Window */
    m_aboutWindow = new AboutWindow;
    m_aboutWindow->hide();

    /* Initializers */
    initTheme();
    initWindow();
}

MainWindow::~MainWindow()
{
    disconnect();

    /* Widgets */
    delete m_ui;
    m_optionsWindow->deleteLater();
    m_aboutWindow->deleteLater();
    m_overlay->deleteLater();

    /* Wrappers and Clients */
    m_player->deleteLater();
    m_ankiClient->deleteLater();
}

/* End Constructor/Destructor */
/* Begin initializers */

void MainWindow::initWindow()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP_WINDOW);

    restoreGeometry(settings.value(SETTINGS_GEOMETRY).toByteArray());
    if (settings.value(SETTINGS_MAXIMIZE, false).toBool())
    {
        QApplication::processEvents();
        showMaximized();
    }

    settings.endGroup();

    /* Signals */
    connect(
        m_mediator, &GlobalMediator::playerFullscreenChanged,
        this,       &MainWindow::setFullscreen
    );
    connect(
        m_mediator, &GlobalMediator::playerClosed,
        this,       &MainWindow::close
    );
    connect(
        m_mediator, &GlobalMediator::interfaceSettingsChanged,
        this,       &MainWindow::initTheme
    );
    connect(m_mediator, &GlobalMediator::playerTitleChanged, this,
        [=] (const QString name) { setWindowTitle(name + " - Memento"); }
    );

    /* Message boxes */
    connect(
        m_mediator, &GlobalMediator::showCritical,
        this,       &MainWindow::showErrorMessage
    );
    connect(
        m_mediator, &GlobalMediator::showInformation,
        this,       &MainWindow::showInfoMessage
    );
}

void MainWindow::initTheme()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);

    /* Set Palette */
    QPalette pal;
    Theme theme = (Theme)settings.value(
            SETTINGS_INTERFACE_THEME,
            (int)SETTINGS_INTERFACE_THEME_DEFAULT
        ).toInt();
    switch (theme)
    {
    case Theme::Light:
    {
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||\
        defined(__NT__) || __APPLE__
        QStyle *style = QStyleFactory::create("fusion");
        if (style)
            QApplication::setStyle(style);
    #endif
        QColor lightColor(240, 240, 240);
        QColor disabledColor(178, 179, 180);
        QColor white(255, 255, 255);
        QColor black(0, 0, 0);
        pal.setColor(QPalette::Window, lightColor);
        pal.setColor(QPalette::WindowText, black);
        pal.setColor(QPalette::Base, white);
        pal.setColor(QPalette::AlternateBase, lightColor);
        pal.setColor(QPalette::ToolTipBase, white);
        pal.setColor(QPalette::ToolTipText, black);
        pal.setColor(QPalette::Text, black);
        pal.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
        pal.setColor(QPalette::Button, lightColor);
        pal.setColor(QPalette::ButtonText, black);
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
        pal.setColor(QPalette::BrightText, Qt::red);
        pal.setColor(QPalette::Link, QColor(16, 50, 72));
        pal.setColor(QPalette::Highlight, QColor(61, 174, 233));
        pal.setColor(QPalette::HighlightedText, white);
        pal.setColor(
            QPalette::Disabled, QPalette::HighlightedText, disabledColor
        );
        break;
    }
    case Theme::Dark:
    {
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||\
        defined(__NT__) || __APPLE__
        QStyle *style = QStyleFactory::create("fusion");
        if (style)
            QApplication::setStyle(style);
    #endif
        /* Modified from
         * https://forum.qt.io/topic/101391/windows-10-dark-theme/5
         */
        QColor darkColor(45, 45, 45);
        QColor disabledColor(127, 127, 127);
        QColor white(255, 255, 255);
        pal.setColor(QPalette::Window, darkColor);
        pal.setColor(QPalette::WindowText, white);
        pal.setColor(QPalette::Base, QColor(18, 18, 18));
        pal.setColor(QPalette::AlternateBase, darkColor);
        pal.setColor(QPalette::ToolTipBase, white);
        pal.setColor(QPalette::ToolTipText, darkColor);
        pal.setColor(QPalette::Text, white);
        pal.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
        pal.setColor(QPalette::Button, darkColor);
        pal.setColor(QPalette::ButtonText, white);
        pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
        pal.setColor(QPalette::BrightText, Qt::red);
        pal.setColor(QPalette::Link, QColor(42, 130, 218));
        pal.setColor(QPalette::Highlight, QColor(31, 72, 94));
        pal.setColor(QPalette::HighlightedText, white);
        pal.setColor(
            QPalette::Disabled, QPalette::HighlightedText, disabledColor
        );
        break;
    }
    case Theme::System:
    default:
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) ||\
        defined(__NT__)
        QStyle *style = QStyleFactory::create("windowsvista");
        if (style)
            QApplication::setStyle(style);
    #elif __APPLE__
        QStyle *style = QStyleFactory::create("macintosh");
        if (style)
            QApplication::setStyle(style);
    #endif
        pal = QApplication::style()->standardPalette();
    }
    QApplication::setPalette(pal);

    IconFactory::create()->buildIcons();

    Q_EMIT m_mediator->requestThemeRefresh();

    /* Set QSplitter Stylesheet */
    bool customStylesEnabled = settings.value(
            SETTINGS_INTERFACE_STYLESHEETS,
            SETTINGS_INTERFACE_STYLESHEETS_DEFAULT
        ).toBool();
    if (customStylesEnabled)
    {
        m_ui->splitterPlayerSubtitles->setStyleSheet(
            settings.value(
                SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE,
                SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE_DEFAULT
            ).toString()
        );
    }
    else
    {
        m_ui->splitterPlayerSubtitles->setStyleSheet(
            SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE_DEFAULT
        );
    }

    settings.endGroup();
}

/* End initializers */
/* Begin Event Handlers */

void MainWindow::showEvent(QShowEvent *event)
{
    /* Check for installed dictionaries */
    if (m_mediator->getDictionary()->getDictionaries().isEmpty())
    {
        Q_EMIT m_mediator->showInformation(
            "No Dictionaries Installed",
            "No dictionaries are installed. For subtitle searching to work, "
            "please install a dictionary."
            "<br>"
            "Dictionaries can be found "
            "<a href='https://foosoft.net/projects/yomichan/#dictionaries'>"
                "here"
            "</a>."
            "<br>"
        #if __APPLE__
            "To install a dictionary, go to Memento -> Preferences -> "
            "Dictionaries."
        #else
            "To install a dictionary, go to Settings -> Options -> "
            "Dictionaries."
        #endif
        );
    }

    /* Load files opened with Memento */
    m_player->loadCommandLineArgs();
    m_player->play();

    QMainWindow::showEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP_WINDOW);

    settings.setValue(SETTINGS_GEOMETRY, saveGeometry());
    settings.setValue(
        SETTINGS_MAXIMIZE,
        isFullScreen() ? m_maximized : isMaximized()
    );

    QMainWindow::closeEvent(event);
    QApplication::quit();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    Q_EMIT m_mediator->keyPressed(event);

    QMainWindow::keyPressEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (!m_ui->subtitleList->underMouse())
    {
        Q_EMIT m_mediator->wheelMoved(event);
    }

    QMainWindow::wheelEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();

    QMainWindow::dragEnterEvent(event);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        m_player->open(event->mimeData()->urls());
        m_player->play();
    }
    QMainWindow::dropEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    Q_EMIT m_mediator->requestDefinitionDelete();
    QMainWindow::mousePressEvent(event);
}

void MainWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::ActivationChange)
    {
        Q_EMIT m_mediator->windowFocusChanged(isActiveWindow());
    }
#if __APPLE__
    else if (event->type() == QEvent::WindowStateChange)
    {
        bool paused = m_player->isPaused();
        m_player->pause();
        QApplication::processEvents();

        m_player->setFullscreen(isFullScreen());

        if (!paused)
        {
            m_player->play();
        }
    }
#endif
}

/* End Event Handlers */
/* Begin Fullscreen Helpers */

void MainWindow::setFullscreen(bool value)
{
    QApplication::processEvents();

    if (value)
    {
        m_maximized = isMaximized();
        showFullScreen();
    }
    else
    {
    #if __linux__
        showNormal();
        if (m_maximized)
        {
            /* Have call showNormal before showMaximized when leaving fullscreen
             * on Linux due to a bug with Qt.
             */
            showMaximized();
        }
    #else
        if (m_maximized)
        {
            showMaximized();
        }
        else
        {
            showNormal();
        }
    #endif
    }
}

/* End Fullscreen Helpers */
/* Begin Dialog Methods */

void MainWindow::showErrorMessage(const QString title,
                                  const QString error) const
{
    QMessageBox message;
    message.critical(0, title, error);
}

void MainWindow::showInfoMessage(const QString title,
                                 const QString error) const
{
    QMessageBox message;
    message.information(0, title, error);
}

/* End Dialog Methods */
