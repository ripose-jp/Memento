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
#include <QWindow>

#if defined(Q_OS_WIN)
#include <qpa/qplatformwindow_p.h>
#endif

#include "dict/dictionary.h"
#include "gui/widgets/common/sliderjumpstyle.h"
#include "player/mpvadapter.h"
#include "util/constants.h"
#include "util/iconfactory.h"

/* Begin Constructor/Destructor */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_ui(new Ui::MainWindow),
      m_mediator(GlobalMediator::getGlobalMediator()),
      m_ankiClient(new AnkiClient(this)),
      m_maximized(false),
      m_firstShow(true)
{
    m_ui->setupUi(this);

#if defined(Q_OS_MACOS)
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
    m_ui->subtitleList->setWindowTitle(
        "Memento - " + m_ui->subtitleList->windowTitle()
    );
    m_ui->subtitleList->hide();
    m_ui->searchWidget->setWindowTitle(
        "Memento - " + m_ui->searchWidget->windowTitle()
    );
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

    /* Reparent the widgets so they get deleted */
    m_ui->subtitleList->setParent(this);
    m_ui->searchWidget->setParent(this);

    /* Widgets */
    delete m_ui;
    m_optionsWindow->deleteLater();
    m_aboutWindow->deleteLater();
    m_overlay->deleteLater();

    /* Wrappers and Clients */
    m_player->deleteLater();
    m_ankiClient->deleteLater();
#if defined(Q_OS_MACOS)
    delete m_cocoaHandler;
#endif
}

/* End Constructor/Destructor */
/* Begin initializers */

void MainWindow::initWindow()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Window::GROUP);

    restoreGeometry(settings.value(Constants::Settings::Window::GEOMETRY).toByteArray());
    m_maximized = settings.value(Constants::Settings::Window::MAXIMIZE, false).toBool();
    QApplication::processEvents();
    if (m_maximized)
    {
        showMaximized();
    }
    else
    {
        showNormal();
    }

    settings.endGroup();

    /* Search Widget Signals */
    connect(
        m_mediator, &GlobalMediator::requestSearchVisibility,
        m_ui->searchWidget, &SearchWidget::setVisible,
        Qt::QueuedConnection
    );
    connect(
        m_ui->searchWidget, &SearchWidget::widgetHidden,
        m_mediator, &GlobalMediator::searchWidgetHidden,
        Qt::QueuedConnection
    );
    connect(
        m_ui->searchWidget, &SearchWidget::widgetShown,
        m_mediator, &GlobalMediator::searchWidgetShown,
        Qt::QueuedConnection
    );
    connect(
        m_mediator, &GlobalMediator::searchWidgetRequest,
        m_ui->searchWidget, &SearchWidget::setSearch,
        Qt::QueuedConnection
    );

    /* Subtitle List Signals */
    connect(
        m_mediator, &GlobalMediator::requestSubtitleListVisibility,
        m_ui->subtitleList, &SubtitleListWidget::setVisible,
        Qt::QueuedConnection
    );
    connect(
        m_ui->subtitleList, &SubtitleListWidget::widgetShown,
        m_mediator, &GlobalMediator::subtitleListShown,
        Qt::QueuedConnection
    );
    connect(
        m_ui->subtitleList, &SubtitleListWidget::widgetHidden,
        m_mediator, &GlobalMediator::subtitleListHidden,
        Qt::QueuedConnection
    );

    /* MainWindow Signals */
    connect(
        m_mediator, &GlobalMediator::playerFullscreenChanged,
        this, &MainWindow::setFullscreen,
        Qt::QueuedConnection
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
        this, &MainWindow::initTheme,
        Qt::QueuedConnection
    );
    connect(
        m_mediator, &GlobalMediator::controlsSubtitleListToggled,
        this, &MainWindow::toggleSubtitleListVisibility,
        Qt::QueuedConnection
    );
    connect(
        m_mediator, &GlobalMediator::requestSearchVisibility,
        this, &MainWindow::updateSearchSubListSplitter,
        Qt::QueuedConnection
    );
    connect(
        m_mediator, &GlobalMediator::requestSubtitleListVisibility,
        this, &MainWindow::updateSearchSubListSplitter,
        Qt::QueuedConnection
    );
    connect(
        m_mediator, &GlobalMediator::playerTitleChanged, this,
        [this] (const QString &name) { setWindowTitle(name + " - Memento"); },
        Qt::QueuedConnection
    );

    /* Show Windows */
    connect(
        m_mediator, &GlobalMediator::showCritical,
        this, &MainWindow::showErrorMessage,
        Qt::QueuedConnection
    );
    connect(
        m_mediator, &GlobalMediator::showInformation,
        this, &MainWindow::showInfoMessage,
        Qt::QueuedConnection
    );
    connect(
        m_mediator, &GlobalMediator::menuShowOptions,
        this, &MainWindow::showOptions,
        Qt::QueuedConnection
    );
    connect(
        m_mediator, &GlobalMediator::menuShowAbout,
        this, &MainWindow::showAbout,
        Qt::QueuedConnection
    );
}

void MainWindow::initTheme()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);

    /* Set Palette */
    QPalette pal{QApplication::style()->standardPalette()};
    Constants::Theme theme = static_cast<Constants::Theme>(settings.value(
            Constants::Settings::Interface::THEME,
            static_cast<int>(Constants::Settings::Interface::THEME_DEFAULT)
        ).toInt());
    switch (theme)
    {
    case Constants::Theme::Light:
    {
#if defined(Q_OS_WIN)
        QStyle *style = QStyleFactory::create("fusion");
        if (style)
        {
            QApplication::setStyle(style);
        }
#elif defined(Q_OS_MACOS)
        QStyle *style = QStyleFactory::create("fusion");
        if (style)
        {
            QApplication::setStyle(style);
        }
        QApplication *app = (QApplication *)QApplication::instance();
        app->setStyleSheet("");
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
    case Constants::Theme::Dark:
    {
#if defined(Q_OS_WIN)
        QStyle *style = QStyleFactory::create("fusion");
        if (style)
        {
            QApplication::setStyle(style);
        }
#elif defined(Q_OS_MACOS)
        QStyle *style = QStyleFactory::create("fusion");
        if (style)
        {
            QApplication::setStyle(style);
        }
        QApplication *app = (QApplication *)QApplication::instance();
        app->setStyleSheet("");
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
    case Constants::Theme::System:
    default:
#if defined(Q_OS_WIN)
        QStyle *style = QStyleFactory::create("windowsvista");
        if (style)
        {
            QApplication::setStyle(style);
        }
#elif defined(Q_OS_MACOS)
        QStyle *style = QStyleFactory::create("macos");
        if (style)
        {
            QApplication::setStyle(style);
        }
        QApplication *app = (QApplication *)QApplication::instance();
        app->setStyleSheet(
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

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    IconFactory::recreate(
        settings.value(
            Constants::Settings::Interface::SYSTEM_ICONS,
            Constants::Settings::Interface::SYSTEM_ICONS_DEFAULT
        ).toBool()
    );
#else
    IconFactory::create()->buildIcons();
#endif

    Q_EMIT m_mediator->requestThemeRefresh();

    /* Set QSplitter Stylesheet */
    bool customStylesEnabled = settings.value(
            Constants::Settings::Interface::STYLESHEETS,
            Constants::Settings::Interface::STYLESHEETS_DEFAULT
        ).toBool();
    if (customStylesEnabled)
    {
        m_ui->splitterPlayerSubtitles->setStyleSheet(
            settings.value(
                Constants::Settings::Interface::Style::SPLITTER,
                Constants::Settings::Interface::Style::SPLITTER_DEFAULT
            ).toString()
        );
    }
    else
    {
        m_ui->splitterPlayerSubtitles->setStyleSheet(
            Constants::Settings::Interface::Style::SPLITTER_DEFAULT
        );
    }

    /* Reparent Search Widget */
    bool vis = m_ui->searchWidget->isVisibleTo(m_ui->splitterSearchList);
    m_ui->searchWidget->setParent(nullptr);
    bool differentWindow = settings.value(
        Constants::Settings::Interface::Subtitle::SEARCH_WINDOW,
        Constants::Settings::Interface::Subtitle::SEARCH_WINDOW_DEFAULT
    ).toBool();
    if (!differentWindow)
    {
        m_ui->splitterSearchList->addWidget(m_ui->searchWidget);
    }
    m_ui->searchWidget->setVisible(vis);

    /* Reparent Subtitle List Widget */
    vis = m_ui->subtitleList->isVisibleTo(m_ui->splitterSearchList);
    m_ui->subtitleList->setParent(nullptr);
    differentWindow = settings.value(
        Constants::Settings::Interface::Subtitle::LIST_WINDOW,
        Constants::Settings::Interface::Subtitle::LIST_WINDOW_DEFAULT
    ).toBool();
    if (!differentWindow)
    {
        m_ui->splitterSearchList->addWidget(m_ui->subtitleList);
    }
    m_ui->subtitleList->setVisible(vis);
    updateSearchSubListSplitter();

#if defined(Q_OS_WIN)
    /* Enable or disable the 1px border on Windows */
    setHasBorderInFullScreen(
        settings.value(
            Constants::Settings::Interface::Subtitle::MENUBAR_FULLSCREEN,
            Constants::Settings::Interface::Subtitle::MENUBAR_FULLSCREEN_DEFAULT
        ).toBool()
    );
#endif

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

#if defined(Q_OS_WIN)
    /* This is needed because of the show menu bar option */
    initTheme();
#endif

    /* Check for installed dictionaries */
    if (m_mediator->getDictionary()->getDictionaries().isEmpty())
    {
        QMessageBox::information(0,
            "No Dictionaries Installed",
            "No dictionaries are installed. For subtitle searching to work, "
            "please install a dictionary."
            "<br>"
            "Dictionaries can be found "
            "<a href='https://github.com/ripose-jp/Memento?tab=readme-ov-file#dictionaries'>"
                "here"
            "</a>."
            "<br>"
#if defined(Q_OS_MACOS)
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

    /* Update the title in case something is already playing */
    QString title = m_player->getTitle();
    if (!title.isEmpty())
    {
        setWindowTitle(title + " - Memento");
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Window::GROUP);

    settings.setValue(Constants::Settings::Window::GEOMETRY, saveGeometry());
    settings.setValue(
        Constants::Settings::Window::MAXIMIZE,
        isFullScreen() ? m_maximized : isMaximized()
    );

    QMainWindow::closeEvent(event);
    QApplication::quit();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    event->ignore();
    switch (event->key())
    {
    case Qt::Key::Key_Delete:
        Q_EMIT m_mediator->windowOSCStateCycled();
        event->accept();
        break;
    default:
        Q_EMIT m_mediator->keyPressed(event);
        break;
    }

    /* If nothing in the application wanted the keypress, send it to the
     * player */
    if (!event->isAccepted())
    {
        m_player->keyPressed(event);
    }

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
        if (m_player->open(QUrl::toStringList(event->mimeData()->urls())))
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
#if defined(Q_OS_MACOS)
    else if (event->type() == QEvent::WindowStateChange)
    {
        m_player->setFullscreen(isFullScreen());
    }
#endif
}

/* End Event Handlers */
/* Begin Window Helpers */

#if defined(Q_OS_WIN)
void MainWindow::setHasBorderInFullScreen(bool value)
{
    if (window() == nullptr)
    {
        return;
    }
    else if (window()->windowHandle() == nullptr)
    {
        return;
    }
    QNativeInterface::Private::QWindowsWindow *windowsWindow = window()
        ->windowHandle()
        ->nativeInterface<QNativeInterface::Private::QWindowsWindow>();
    if (windowsWindow == nullptr)
    {
        return;
    }
    windowsWindow->setHasBorderInFullScreen(value);
}
#endif

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
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
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
    settings.beginGroup(Constants::Settings::Behavior::GROUP);
    bool autoFit = settings.value(
            Constants::Settings::Behavior::AUTOFIT,
            Constants::Settings::Behavior::AUTOFIT_DEFAULT
        ).toBool();
    unsigned int percent = settings.value(
            Constants::Settings::Behavior::AUTOFIT_PERCENT,
            Constants::Settings::Behavior::AUTOFIT_PERCENT_DEFAULT
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
        (m_ui->subtitleList->parent() != nullptr &&
        m_ui->subtitleList->isVisibleTo(m_ui->splitterSearchList)) ||
        (m_ui->searchWidget->parent() != nullptr &&
        m_ui->searchWidget->isVisibleTo(m_ui->splitterSearchList))
    );
}

/* End Window Helpers */
/* Begin Show Methods */

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


void MainWindow::showOptions() const
{
    m_optionsWindow->show();
    m_optionsWindow->activateWindow();
    m_optionsWindow->raise();
}

void MainWindow::showAbout() const
{
    m_aboutWindow->show();
    m_aboutWindow->activateWindow();
    m_aboutWindow->raise();
}

/* End Show Methods */
#if defined(Q_OS_MACOS)
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
