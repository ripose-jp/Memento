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

#include <QClipboard>
#include <QCursor>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QSettings>
#include <QStyleFactory>
#include <QThreadPool>

#include "mpvadapter.h"
#include "../util/constants.h"
#include "../dict/dictionary.h"

/* Begin Constructor/Destructor */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_mediator(GlobalMediator::getGlobalMediator()),
      m_ui(new Ui::MainWindow),
      m_ankiClient(new AnkiClient(this)),
      m_maximized(false),
      m_definition(nullptr)
{
    m_ui->setupUi(this);

    /* Player Adapter */
    m_player = new MpvAdapter(m_ui->player, this);
    m_player->pause();
    m_ui->controls->setVolumeLimit(m_player->getMaxVolume());

    /* Subtitle List */
    m_mediator->setSubtitleList(m_ui->subtitleList);
    m_ui->subtitleList->hide();

    /* Anki */
    m_actionGroupAnkiProfile = new QActionGroup(this);
    updateAnkiProfileMenu();

    /* Options */
    m_optionsWindow = new OptionsWindow;
    m_optionsWindow->hide();

    /* About Window */
    m_aboutWindow = new AboutWindow;
    m_aboutWindow->hide();

    /* Initializers */
    initDefinitionWidget();
    initSubtitles();
    initMenuBar();
    initTheme();
    initWindow();
}

MainWindow::~MainWindow()
{
    disconnect();
    clearTracks();

    /* Widgets */
    delete m_ui;
    delete m_definition;
    delete m_optionsWindow;
    delete m_aboutWindow;

    /* Wrappers and Clients */
    delete m_player;
    delete m_ankiClient;
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
        m_mediator, &GlobalMediator::playerCursorHidden,
        this,       &MainWindow::hideControls
    );
    connect(
        m_mediator, &GlobalMediator::ankiSettingsChanged,
        this,       &MainWindow::updateAnkiProfileMenu
    );
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
    connect(
        m_mediator, &GlobalMediator::requestThemeRefresh, 
        this,       &MainWindow::deleteDefinitionWidget
    );
    connect(m_mediator, &GlobalMediator::playerTitleChanged, this, 
        [=] (const QString name) { setWindowTitle(name + " - Memento"); }
    );
    connect(m_mediator, &GlobalMediator::playerMouseMoved, this,
        [=] { if (isFullScreen()) m_ui->controls->show(); }
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

void MainWindow::initMenuBar()
{
    m_actionGroups.audio       = new QActionGroup(this);
    m_actionGroups.video       = new QActionGroup(this);
    m_actionGroups.subtitle    = new QActionGroup(this);
    m_actionGroups.subtitleTwo = new QActionGroup(this);

    m_ui->actionAudioNone->setActionGroup(m_actionGroups.audio);
    m_ui->actionVideoNone->setActionGroup(m_actionGroups.video);
    m_ui->actionSubtitleNone->setActionGroup(m_actionGroups.subtitle);
    m_ui->actionSubtitleTwoNone->setActionGroup(m_actionGroups.subtitleTwo);

    m_actionGroups.currentSubId = 0;
    m_actionGroups.currentSecSubId = 0;

    /* Signals */
        connect(
        m_mediator, &GlobalMediator::playerTracksChanged,
        this,       &MainWindow::setTracks
    );

    connect(
        m_actionGroups.audio, &QActionGroup::triggered, 
        this,                 &MainWindow::setAudioTrack
    );
    connect(
        m_actionGroups.video, &QActionGroup::triggered, 
        this,                 &MainWindow::setVideoTrack
    );
    connect(
        m_actionGroups.subtitle, &QActionGroup::triggered, 
        this,                    &MainWindow::setSubtitleTrack
    );
    connect(
        m_actionGroups.subtitleTwo, &QActionGroup::triggered, 
        this,                       &MainWindow::setSecondarySubtitleTrack
    );

    connect(
        m_mediator, &GlobalMediator::playerAudioTrackChanged,
        this,       &MainWindow::updateAudioAction
    );
    connect(
        m_mediator, &GlobalMediator::playerVideoTrackChanged,
        this,       &MainWindow::updateVideoAction
    );
    connect(
        m_mediator, &GlobalMediator::playerSubtitleTrackChanged,
        this,       &MainWindow::updateSubtitleAction
    );
    connect(
        m_mediator, &GlobalMediator::playerSecondSubtitleTrackChanged,
        this,       &MainWindow::updateSecondarySubtitleAction
    );

    connect(
        m_mediator, &GlobalMediator::playerAudioDisabled,
        this,       [=] { updateAudioAction(); }
    );
    connect(
        m_mediator, &GlobalMediator::playerVideoDisabled,
        this,       [=] { updateVideoAction(); }
    );
    connect(
        m_mediator, &GlobalMediator::playerSubtitlesDisabled,
        this,       [=] { updateSubtitleAction(); }
    );
    connect(
        m_mediator, &GlobalMediator::playerSecondSubtitlesDisabled,
        this,       [=] { updateSecondarySubtitleAction(); }
    );

    connect(
        m_ui->actionOptions, &QAction::triggered,
        m_optionsWindow,     &OptionsWindow::show
    );
    connect(
        m_ui->actionAbout, &QAction::triggered,
        m_aboutWindow,     &AboutWindow::show
    );
    connect(
        m_ui->actionOpen, &QAction::triggered,
        this,             &MainWindow::open
    );
    connect(
        m_ui->actionOpenUrl, &QAction::triggered,
        this,                &MainWindow::openUrl
    );
    connect(
        m_ui->actionUpdate, &QAction::triggered,
        this,               &NetworkUtils::checkForUpdates
    );
    connect(
        m_ui->actionAddSubtitle, &QAction::triggered,
        this,                    &MainWindow::openSubtitle
    );
}

void MainWindow::initSubtitles()
{
    m_subtitle.layoutPlayerOverlay = new QVBoxLayout(m_ui->player);
    m_subtitle.layoutPlayerOverlay->setSpacing(0);
    m_subtitle.layoutPlayerOverlay->setContentsMargins(QMargins(0, 0, 0, 0));
    m_subtitle.layoutPlayerOverlay->setMargin(0);
    m_subtitle.layoutPlayerOverlay->addStretch();

    m_subtitle.layoutSubtitle = new QHBoxLayout;
    m_subtitle.layoutSubtitle->setSpacing(0);
    m_subtitle.layoutSubtitle->setContentsMargins(QMargins(0, 0, 0, 0));
    m_subtitle.layoutSubtitle->setMargin(0);
    m_subtitle.layoutPlayerOverlay->addLayout(m_subtitle.layoutSubtitle);

    m_subtitle.subtitleWidget = new SubtitleWidget;
    m_subtitle.layoutSubtitle->addStretch();
    m_subtitle.layoutSubtitle->addWidget(m_subtitle.subtitleWidget);
    m_subtitle.layoutSubtitle->addStretch();

    m_subtitle.spacerWidget = new QWidget;
    m_subtitle.spacerWidget->setSizePolicy(
        QSizePolicy::Fixed, QSizePolicy::Fixed
    );
    m_subtitle.spacerWidget->setFixedWidth(0);
    m_subtitle.spacerWidget->setMouseTracking(true);
    m_subtitle.layoutPlayerOverlay->addWidget(m_subtitle.spacerWidget);

    repositionSubtitles();

    /* Signals */
    connect(
        m_mediator, &GlobalMediator::controlsHidden, 
        this,       &MainWindow::repositionSubtitles
    );
    connect(
        m_mediator, &GlobalMediator::controlsShown,
        this,       &MainWindow::repositionSubtitles
    );

    connect(
        m_ui->actionIncreaseSize, &QAction::triggered, this, 
        [=] { updateSubScale(0.01); }
    );
    connect(
        m_ui->actionDecreaseSize, &QAction::triggered, this,
        [=] { updateSubScale(-0.01); }
    );
    connect(
        m_ui->actionMoveUp, &QAction::triggered, this,
        [=] { moveSubtitles(0.01); }
    );
    connect(
        m_ui->actionMoveDown, &QAction::triggered, this,
        [=] { moveSubtitles(-0.01); }
    );
}

void MainWindow::initDefinitionWidget()
{
    connect(
        m_ui->splitterPlayerSubtitles, &QSplitter::splitterMoved,
        this,                          &MainWindow::deleteDefinitionWidget
    );
    connect(
        m_mediator, &GlobalMediator::termsChanged,
        this,       &MainWindow::setTerms
    );
    connect(
        m_mediator, &GlobalMediator::requestDefinitionDelete,
        this,       &MainWindow::deleteDefinitionWidget
    );
    connect(
        m_mediator, &GlobalMediator::playerSubtitleChanged,
        this,       &MainWindow::deleteDefinitionWidget
    );
    connect(
        m_mediator, &GlobalMediator::subtitleExpired,
        this,       &MainWindow::deleteDefinitionWidget
    );
    connect(
        m_mediator, &GlobalMediator::subtitleListShown,
        this,       &MainWindow::deleteDefinitionWidget
    );
    connect(
        m_mediator, &GlobalMediator::subtitleListHidden,
        this,       &MainWindow::deleteDefinitionWidget
    );
    connect(m_mediator, &GlobalMediator::playerPauseStateChanged, this, 
        [=] (const bool paused) { 
            if (!paused)
                deleteDefinitionWidget();
        }
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
        m_ui->menubar->setAutoFillBackground(true);
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
        m_ui->menubar->setAutoFillBackground(true);
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
        m_ui->menubar->setAutoFillBackground(false);
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

    /* Refresh the subtitle offset */
    m_subtitle.offsetPercent = settings.value(
            SETTINGS_INTERFACE_SUB_OFFSET,
            SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT
        ).toDouble();
    repositionSubtitles();

    settings.endGroup();
}

/* End initializers */
/* Begin Event Handlers */

void MainWindow::showEvent(QShowEvent *event)
{
    /* Load files opened with Memento */
    QStringList args = QApplication::arguments();
    if (args.size() > 1)
    {
        m_player->open(args[1]);
        for (size_t i = 2; i < args.size(); ++i)
        {
            m_player->open(args[i], true);
        }
        m_player->play();
    }

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

void MainWindow::resizeEvent(QResizeEvent *event)
{
    event->ignore();
    repositionSubtitles();
    deleteDefinitionWidget();
    QMainWindow::resizeEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    deleteDefinitionWidget();
    QMainWindow::mousePressEvent(event);
}

/* End Event Handlers */
/* Begin Subtitle Widget Helpers */

void MainWindow::repositionSubtitles()
{
    int height = m_ui->player->height() * m_subtitle.offsetPercent;
    if (isFullScreen() && m_ui->controls->isVisible())
    {
        height -= m_ui->controls->height();
        if (height < 0)
        {
            height = 0;
        }
    }
    m_subtitle.spacerWidget->setFixedHeight(height);
}

void MainWindow::updateSubScale(const double inc)
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    double scale = settings.value(
        SETTINGS_INTERFACE_SUB_SCALE, 
        SETTINGS_INTERFACE_SUB_SCALE_DEFAULT
    ).toDouble();
    scale += inc;
    if (scale <= 0.0)
    {
        scale = 0.0;
    }
    else if (scale >= 1.0)
    {
        scale = 1.0;
    }
    settings.setValue(SETTINGS_INTERFACE_SUB_SCALE, scale);
    Q_EMIT m_mediator->interfaceSettingsChanged();
}

void MainWindow::moveSubtitles(const double inc)
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    double offset = settings.value(
        SETTINGS_INTERFACE_SUB_OFFSET, 
        SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT
    ).toDouble();
    offset += inc;
    if (offset <= 0.0)
    {
        offset = 0.0;
    }
    else if (offset >= 1.0)
    {
        offset = 1.0;
    }
    settings.setValue(SETTINGS_INTERFACE_SUB_OFFSET, offset);
    Q_EMIT m_mediator->interfaceSettingsChanged();
}

/* End Definition Widget Helpers */
/* Begin Definition Widget Helpers */

void MainWindow::setTerms(const QList<Term *> *terms)
{
    deleteDefinitionWidget();
    
    m_definition = new DefinitionWidget(terms, m_ui->player);
    setDefinitionWidgetLocation();
    if (m_definition)
    {
        m_definition->show();
    }
}

void MainWindow::setDefinitionWidgetLocation()
{
    const QPoint mousePos = mapFromGlobal(QCursor::pos());

    int x = mousePos.x() - (m_definition->width() / 2);
    if (x < 0)
    {
        x = 0;
    }
    else if (x > m_ui->player->width() - m_definition->width())
    {
        x = m_ui->player->width() - m_definition->width();
    }
    
    int y = m_subtitle.subtitleWidget->pos().y() - m_definition->height();
    if (y < 0)
    {
        y = m_subtitle.subtitleWidget->pos().y() + 
            m_subtitle.subtitleWidget->height();
    }

    if (y + m_definition->height() > m_ui->player->height())
    {
        deleteDefinitionWidget();
    }
    else
    {
        m_definition->move(x, y);
    }
}

void MainWindow::deleteDefinitionWidget()
{
    if (m_definition)
    {
        m_definition->hide();
        m_definition->deleteLater();
    }
    m_definition = nullptr;
}

/* End Definition Widget Helpers */
/* Begin Fullscreen Helpers */

void MainWindow::setFullscreen(bool value)
{
    QApplication::processEvents();

    if (value)
    {
        m_maximized = isMaximized();
        showFullScreen();
        m_ui->menubar->setFixedHeight(0);

        /* Move the controls */
        m_ui->controls->hide();
        m_ui->centralwidget->layout()->removeWidget(m_ui->controls);
        m_ui->controls->raise();
        m_subtitle.layoutPlayerOverlay->addWidget(m_ui->controls);
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
            showMaximized();
        else
            showNormal();
    #endif
        m_ui->menubar->setMinimumHeight(0);
        m_ui->menubar->setMaximumHeight(QWIDGETSIZE_MAX);

        m_subtitle.layoutPlayerOverlay->removeWidget(m_ui->controls);
        m_ui->centralwidget->layout()->addWidget(m_ui->controls);
        m_ui->controls->show();
    }
}

inline bool MainWindow::isMouseOverPlayer()
{
    return !m_ui->controls->underMouse() && 
           !m_subtitle.subtitleWidget->underMouse() &&
           (
               m_definition == nullptr || 
               m_definition && 
               !m_definition->underMouse()
           );
}

void MainWindow::hideControls()
{
    if (isFullScreen() && isMouseOverPlayer())
    {
        m_ui->controls->hide();
        deleteDefinitionWidget();
    }
}

/* End Fullscreen Helpers */
/* Begin Track Helpers */

void MainWindow::clearTrack(QList<QAction *> &actions,
                            QMenu            *menu,
                            QActionGroup     *actionGroup,
                            QAction          *actionDisable)
{
    for (QAction *action : actions)
    {
        menu->removeAction(action);
        actionGroup->removeAction(action);
        delete action;
    }
    actions.clear();

    actionDisable->setChecked(true);
}

void MainWindow::clearTracks()
{
    clearTrack(
        m_actionGroups.audioActions, 
        m_ui->menuAudio, 
        m_actionGroups.audio,
        m_ui->actionAudioNone
    );
    clearTrack(
        m_actionGroups.videoActions,
        m_ui->menuVideo,
        m_actionGroups.video,
        m_ui->actionVideoNone
    );
    clearTrack(
        m_actionGroups.subtitleActions,
        m_ui->menuSubtitle,
        m_actionGroups.subtitle,
        m_ui->actionSubtitleNone
    );
    clearTrack(
        m_actionGroups.subtitleTwoActions,
        m_ui->menuSubtitleTwo,
        m_actionGroups.subtitleTwo,
        m_ui->actionSubtitleTwoNone
    );
    m_actionGroups.actionMap.clear();
    m_actionGroups.currentSubId = 0;
    m_actionGroups.currentSecSubId = 0;
}

void MainWindow::setTracks(QList<const Track *> tracks)
{
    clearTracks();

    for (const Track *track : tracks)
    {
        QAction *action = new QAction(this);
        action->setCheckable(true);

        QString actionText = "Track " + QString::number(track->id);
        if (!track->lang.isEmpty())
            actionText += " [" + track->lang + "]";
        if (!track->title.isEmpty())
            actionText += " - " + track->title;
        action->setText(actionText);

        switch (track->type)
        {
        case Track::Type::audio:
            m_ui->menuAudio->addAction(action);
            action->setActionGroup(m_actionGroups.audio);
            m_actionGroups.audioActions.append(action);
            break;

        case Track::Type::video:
            m_ui->menuVideo->addAction(action);
            action->setActionGroup(m_actionGroups.video);
            m_actionGroups.videoActions.append(action);
            break;

        case Track::Type::subtitle:
        {
            m_ui->menuSubtitle->addAction(action);
            action->setActionGroup(m_actionGroups.subtitle);
            m_actionGroups.subtitleActions.append(action);

            /* Secondary Subtitles */
            QAction *actionSubTwo = new QAction(actionText, this);
            actionSubTwo->setCheckable(true);
            actionSubTwo->setEnabled(!track->selected);
            m_ui->menuSubtitleTwo->addAction(actionSubTwo);
            actionSubTwo->setActionGroup(m_actionGroups.subtitleTwo);
            m_actionGroups.subtitleTwoActions.append(actionSubTwo);
            m_actionGroups.actionMap[actionSubTwo] = track->id;
            break;
        }
        }
        m_actionGroups.actionMap[action] = track->id;
        action->setChecked(track->selected);

        delete track;
    }
}

void MainWindow::setAudioTrack(QAction *action)
{
    if (!action->isChecked())
    {
        return;
    }

    if (action == m_ui->actionAudioNone)
    {
        m_player->disableAudio();
        return;
    }

    m_player->setAudioTrack(m_actionGroups.actionMap[action]);
}

void MainWindow::setVideoTrack(QAction *action)
{
    if (!action->isChecked())
    {
        return;
    }

    if (action == m_ui->actionVideoNone)
    {
        m_player->disableVideo();
        return;
    }

    m_player->setVideoTrack(m_actionGroups.actionMap[action]);
}

void MainWindow::setSubtitleTrack(QAction *action)
{
    if (!action->isChecked())
    {
        return;
    }

    if (action == m_ui->actionSubtitleNone)
    {
        m_actionGroups
            .subtitleTwoActions[m_actionGroups.currentSubId - 1]
                ->setEnabled(true);
        m_actionGroups.currentSubId = 0;
        m_player->disableSubtitles();
        return;
    }

    if (m_actionGroups.currentSubId)
    {
        m_actionGroups
            .subtitleTwoActions[m_actionGroups.currentSubId - 1]
                ->setEnabled(true);
    }
    m_actionGroups.currentSubId = m_actionGroups.actionMap[action];
    m_player->setSubtitleTrack(m_actionGroups.currentSubId);
    m_actionGroups
        .subtitleTwoActions[m_actionGroups.currentSubId - 1]
            ->setEnabled(false);
}

void MainWindow::setSecondarySubtitleTrack(QAction *action)
{
    if (!action->isChecked())
    {
        return;
    }

    if (action == m_ui->actionSubtitleTwoNone)
    {
        m_actionGroups
            .subtitleActions[m_actionGroups.currentSecSubId - 1]
                ->setEnabled(true);
        m_actionGroups.currentSecSubId = 0;
        m_player->disableSubtitleTwo();
        return;
    }

    if (m_actionGroups.currentSecSubId)
    {
        m_actionGroups
            .subtitleActions[m_actionGroups.currentSecSubId - 1]
                ->setEnabled(true);
    }
    m_actionGroups.currentSecSubId = m_actionGroups.actionMap[action];
    m_player->setSubtitleTwoTrack(m_actionGroups.currentSecSubId);
    m_actionGroups
        .subtitleActions[m_actionGroups.currentSecSubId - 1]->setEnabled(false);
}

void MainWindow::updateAudioAction(const int64_t id)
{
    if (!id)
    {
        m_ui->actionAudioNone->setChecked(true);
    }
    else if(!m_actionGroups.audioActions.isEmpty())
    {
        m_actionGroups.audioActions[id - 1]->setChecked(true); 
    }     
}

void MainWindow::updateVideoAction(const int64_t id)
{
    if (!id)
    {
        m_ui->actionVideoNone->setChecked(true);
    }
    else if(!m_actionGroups.videoActions.isEmpty())
    {
        m_actionGroups.videoActions[id - 1]->setChecked(true); 
    }   
}

void MainWindow::updateSubtitleAction(const int64_t id)
{
    if (!id)
    {
        m_ui->actionSubtitleNone->setChecked(true);
    }
    else if(!m_actionGroups.subtitleActions.isEmpty())
    {
        m_actionGroups.subtitleActions[id - 1]->setChecked(true);
    }

    if (m_actionGroups.currentSubId)
    {
        m_actionGroups
            .subtitleTwoActions[m_actionGroups.currentSubId - 1]
                ->setEnabled(true);
    }
    m_actionGroups.currentSubId = id;
    if (m_actionGroups.currentSubId)
    {
        m_actionGroups
            .subtitleTwoActions[m_actionGroups.currentSubId - 1]
                ->setEnabled(false); 
    }
}


void MainWindow::updateSecondarySubtitleAction(const int64_t id)
{
    if (!id)
    {
        m_ui->actionSubtitleTwoNone->setChecked(true);
    }
    else if(!m_actionGroups.subtitleTwoActions.isEmpty())
    {
        m_actionGroups.subtitleTwoActions[id - 1]->setChecked(true);
    }

    if (m_actionGroups.currentSecSubId)
    {
        m_actionGroups.
            subtitleActions[m_actionGroups.currentSecSubId - 1]
                ->setEnabled(true);
    }
    m_actionGroups.currentSecSubId = id;
    if (m_actionGroups.currentSecSubId)
    {
        m_actionGroups
            .subtitleActions[m_actionGroups.currentSecSubId - 1]
                ->setEnabled(false);
    }
}

/* End Track Helpers */
/* Begin Menu Bar Helpers */

void MainWindow::open()
{
    QList<QUrl> files = QFileDialog::getOpenFileUrls(0, "Open a video");
    if (!files.isEmpty())
    {
        m_player->stop();
        m_player->open(files);
        m_player->play();
    }
}

void MainWindow::openUrl()
{
    QInputDialog dialog(this, Qt::Dialog);
    dialog.setWindowTitle("Open Stream");
    dialog.setLabelText("Enter URL");
    dialog.setTextValue(QGuiApplication::clipboard()->text());
    dialog.setStyleSheet("QLabel { min-width: 500px; }");
    int     res = dialog.exec();
    QString url = dialog.textValue(); 

    if (res == QDialog::Accepted && !url.isEmpty())
    {
        m_player->stop();
        m_player->open(url);
        m_player->play();
    }
}

void MainWindow::openSubtitle()
{
    QString file = QFileDialog::getOpenFileName(0, "Open Subtitle");
    if (!file.isEmpty())
    {
        m_player->addSubtitle(file);
    }
}

void MainWindow::updateAnkiProfileMenu()
{
    /* Remove all profiles */
    QList<QAction *> actions = m_actionGroupAnkiProfile->actions();
    for (QAction *action : actions)
    {
        m_actionGroupAnkiProfile->removeAction(action);
        m_ui->menuAnkiProfile->removeAction(action);
        delete action;
    }

    /* Get profiles in alphabetical order */
    QStringList profiles = m_ankiClient->getProfiles();
    std::sort(profiles.begin(), profiles.end());

    /* Add new profiles */
    for (const QString &profile : profiles)
    {
        QAction *profileAction = new QAction(this);
        profileAction->setText(profile);
        profileAction->setCheckable(true);
        profileAction->setChecked(m_ankiClient->getProfile() == profile);
        m_ui->menuAnkiProfile->addAction(profileAction);
        profileAction->setActionGroup(m_actionGroupAnkiProfile);

        connect(profileAction, &QAction::triggered,
            [=] (const bool checked) {
                if (checked) 
                {
                    profileAction->blockSignals(true);
                    m_ankiClient->setProfile(profileAction->text());
                    m_ankiClient->writeChanges();
                    profileAction->blockSignals(false);
                }
            }
        );
    }

    /* Handle disabled Anki Integration */
    m_ui->menuAnkiProfile->menuAction()->setVisible(m_ankiClient->isEnabled());
}

/* End Menu Bar Helpers */
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
