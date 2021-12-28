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
#include <QScreen>
#include <QSettings>
#include <QStyleFactory>

#include "../dict/dictionary.h"
#include "../util/constants.h"
#include "../util/iconfactory.h"
#include "mpvadapter.h"
#include "widgets/common/sliderjumpstyle.h"

/* Begin Constructor/Destructor */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_mediator(GlobalMediator::getGlobalMediator()),
      m_ui(new Ui::MainWindow),
      m_ankiClient(new AnkiClient(this)),
      m_firstShow(true),
      m_maximized(false)
{
    m_ui->setupUi(this);

#if __APPLE__
    m_oldUpdatesEnabled = updatesEnabled();
    m_cocoaHandler = new CocoaEventHandler(this);
#endif

    /* Player Adapter */
    m_player = new MpvAdapter(m_ui->player, this);
    m_player->pause();

    /* Player Overlay */
    m_overlay = new PlayerOverlay(m_ui->player);

    /* Utility Widgets */
    m_mediator->setSubtitleList(m_ui->subtitleList);
    m_ui->subtitleList->hide();
    m_ui->searchWidget->hide();
    m_ui->splitterSearchList->hide();

    /* Options */
    m_optionsWindow = new OptionsWindow;
    m_optionsWindow->hide();

    /* About Window */
    m_aboutWindow = new AboutWindow;
    m_aboutWindow->hide();

    /* Splitter */
    m_ui->splitterPlayerSubtitles->setStretchFactor(0, 1);
    m_ui->splitterPlayerSubtitles->setStretchFactor(1, 0);

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
#if __APPLE__
    delete m_cocoaHandler;
#endif
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
        this, &MainWindow::setFullscreen
    );
    connect(
        m_mediator, &GlobalMediator::requestSearchVisibility,
        m_ui->searchWidget, &SearchWidget::setVisible
    );
    connect(
        m_mediator, &GlobalMediator::requestSubtitleListVisibility,
        m_ui->subtitleList, &SubtitleListWidget::setVisible
    );
    connect(
        m_mediator, &GlobalMediator::playerFileLoaded,
        this, &MainWindow::autoFitMedia
    );
    connect(
        m_mediator, &GlobalMediator::playerClosed,
        this, &MainWindow::close
    );
    connect(
        m_mediator, &GlobalMediator::interfaceSettingsChanged,
        this, &MainWindow::initTheme
    );
    connect(
        m_mediator, &GlobalMediator::controlsSubtitleListToggled,
        this, &MainWindow::toggleSubtitleListVisibility
    );
    connect(
        m_mediator, &GlobalMediator::requestSearchVisibility,
        this, &MainWindow::updateSearchSubListSplitter
    );
    connect(
        m_mediator, &GlobalMediator::requestSubtitleListVisibility,
        this, &MainWindow::updateSearchSubListSplitter
    );
    connect(m_mediator, &GlobalMediator::playerTitleChanged, this,
        [=] (const QString name) { setWindowTitle(name + " - Memento"); }
    );

    /* Message boxes */
    connect(
        m_mediator, &GlobalMediator::showCritical,
        this, &MainWindow::showErrorMessage
    );
    connect(
        m_mediator, &GlobalMediator::showInformation,
        this, &MainWindow::showInfoMessage
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
    #if __APPLE__
        setStyleSheet("");
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
    #if __APPLE__
        setStyleSheet("");
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
        setStyleSheet(
            "QToolButton {"
                "border: 0px;"
                "border-radius: 4px;"
                "background-color: palette(window);"
            "}"
            "QToolButton:hover:!pressed"
            "{"
                "border: 1px solid palette(highlight);"
            "}"
        );
    #endif
        pal = QApplication::style()->standardPalette();
    }
    QApplication::setPalette(pal);
    QApplication::setStyle(new SliderJumpStyle(QApplication::style()));

#if __linux__
    IconFactory::recreate(
        settings.value(
            SETTINGS_INTERFACE_SYSTEM_ICONS,
            SETTINGS_INTERFACE_SYSTEM_ICONS_DEFAULT
        ).toBool()
    );
#else
    IconFactory::create()->buildIcons();
#endif

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
    QMainWindow::showEvent(event);

    if (!m_firstShow)
    {
        return;
    }
    m_firstShow = false;

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
            "To install a dictionary, go to Memento → Preferences → "
            "Dictionaries."
        #else
            "To install a dictionary, go to Settings → Options → Dictionaries."
        #endif
        );
    }

    /* Load files opened with Memento */
    m_player->loadCommandLineArgs();
    m_player->play();
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
    if (!m_ui->subtitleList->underMouse() && !m_overlay->underMouse())
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
        if (m_player->open(event->mimeData()->urls()))
        {
            m_player->play();
        }
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
        m_player->setFullscreen(isFullScreen());
    }
#endif
}

/* End Event Handlers */
/* Begin Window Helpers */

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

/* https://github.com/u8sand/Baka-MPlayer/blob/4009cd02ef70b365105a00de6b5dc375f76c02ef/src/bakacommands.cpp#L425 */
void MainWindow::autoFitMedia(int width, int height)
{
    if (width == 0 || height == 0)
    {
        return;
    }

    QSettings settings;
    settings.beginGroup(SETTINGS_BEHAVIOR);
    bool autoFit = settings.value(
            SETTINGS_BEHAVIOR_AUTOFIT, SETTINGS_BEHAVIOR_AUTOFIT_DEFAULT
        ).toBool();
    unsigned int percent = settings.value(
            SETTINGS_BEHAVIOR_AUTOFIT_PERCENT,
            SETTINGS_BEHAVIOR_AUTOFIT_PERCENT_DEFAULT
        ).toUInt();
    settings.endGroup();

    if (window()->isFullScreen() || window()->isMaximized() || !autoFit)
    {
        return;
    }

    QSize vG = QSize(width, height);                               // video geometry
    QRect mG = m_ui->player->geometry();                           // mpv geometry
    QRect wfG = window()->frameGeometry();                         // frame geometry of window (window geometry + window frame)
    QRect wG = window()->geometry();                               // window geometry
    QScreen *sAt = QGuiApplication::screenAt(wfG.center());
    if (sAt == nullptr)
    {
        return;
    }
    QRect aG = sAt->availableGeometry();   // available geometry of the screen we're in--(geometry not including the taskbar)

    double a; // aspect ratio
    double w; // width of vid we want
    double h; // height of vid we want

    // obtain natural video aspect ratio
    a = double(vG.width()) / vG.height(); // use display width and height for aspect ratio

    // calculate resulting display:
    if(percent == 0) // fit to window
    {
        // set our current mpv frame dimensions
        w = mG.width();
        h = mG.height();

        double cmp = w/h - a; // comparison
        double eps = 0.01;  // epsilon (deal with rounding errors) we consider -eps < 0 < eps ==> 0

        if(cmp > eps) // too wide
        {
            w = h * a; // calculate width based on the correct height
        }
        else if(cmp < -eps) // too long
        {
            h = w / a; // calculate height based on the correct width
        }
    }
    else // fit into desired dimensions
    {
        double scale = percent / 100.0; // get scale

        w = vG.width() * scale;  // get scaled width
        h = vG.height() * scale; // get scaled height
    }

    double dW = w + (wfG.width() - mG.width());   // calculate display width of the window
    double dH = h + (wfG.height() - mG.height()); // calculate display height of the window

    if(dW > aG.width()) // if the width is bigger than the available area
    {
        dW = aG.width(); // set the width equal to the available area
        w = dW - (wfG.width() - mG.width());    // calculate the width
        h = w / a;                              // calculate height
        dH = h + (wfG.height() - mG.height());  // calculate new display height
    }
    if(dH > aG.height()) // if the height is bigger than the available area
    {
        dH = aG.height(); // set the height equal to the available area
        h = dH - (wfG.height() - mG.height()); // calculate the height
        w = h * a;                             // calculate the width accordingly
        dW = w + (wfG.width() - mG.width());   // calculate new display width
    }

    // get the centered rectangle we want
    QRect rect = QStyle::alignedRect(
        Qt::LeftToRight, Qt::AlignCenter,
        QSize(dW, dH),
        percent == 0 ? wfG : aG // center in window (autofit) or on our screen
    );

    // adjust the rect to compensate for the frame
    rect.setLeft(rect.left() + (wG.left() - wfG.left()));
    rect.setTop(rect.top() + (wG.top() - wfG.top()));
    rect.setRight(rect.right() - (wfG.right() - wG.right()));
    rect.setBottom(rect.bottom() - (wfG.bottom() - wG.bottom()));

    // finally set the geometry of the window
    window()->setGeometry(rect);
}

void MainWindow::toggleSubtitleListVisibility()
{
    bool vis = m_ui->subtitleList->isVisibleTo(m_ui->splitterSearchList);
    Q_EMIT m_mediator->requestSubtitleListVisibility(!vis);
}

void MainWindow::updateSearchSubListSplitter()
{
    QApplication::processEvents();
    m_ui->splitterSearchList->setVisible(
        m_ui->subtitleList->isVisibleTo(m_ui->splitterSearchList) ||
        m_ui->searchWidget->isVisibleTo(m_ui->splitterSearchList)
    );
}

/* End Window Helpers */
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
#if __APPLE__
/* Begin Cocoa Handlers */

void MainWindow::beforeTransition()
{
    m_oldUpdatesEnabled = updatesEnabled();
    setUpdatesEnabled(false);

    m_oldPause = m_player->isPaused();
    m_player->pause();
}

void MainWindow::afterTransition()
{
    setUpdatesEnabled(m_oldUpdatesEnabled);
    if (!m_oldPause)
    {
        m_player->play();
    }
}

/* End Cocoa Handlers */
#endif
