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
#include "mpvadapter.h"
#include "widgets/definitionwidget.h"
#include "widgets/ankisettings.h"
#include "../util/constants.h"
#include "../dict/dictionary.h"

#include <QCursor>
#include <QFileDialog>
#include <QMimeData>
#include <QDebug>
#include <QMessageBox>
#include <QThreadPool>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_mediator(GlobalMediator::createGlobalMedaitor()),
      m_ui(new Ui::MainWindow),
      m_maximized(false),
      m_manager(new QNetworkAccessManager),
      m_definition(nullptr)
{
    m_ui->setupUi(this);

    /* Video Action Groups */
    m_actionGroupAudio       = new QActionGroup(this);
    m_actionGroupVideo       = new QActionGroup(this);
    m_actionGroupSubtitle    = new QActionGroup(this);
    m_actionGroupSubtitleTwo = new QActionGroup(this);

    m_ui->actionAudioNone->setActionGroup(m_actionGroupAudio);
    m_ui->actionVideoNone->setActionGroup(m_actionGroupVideo);
    m_ui->actionSubtitleNone->setActionGroup(m_actionGroupSubtitle);
    m_ui->actionSubtitleTwoNone->setActionGroup(m_actionGroupSubtitleTwo);

    /* Player Adapter */
    m_player = new MpvAdapter(m_ui->mpv, this);
    m_player->pause();
    m_ui->controls->setVolumeLimit(m_player->getMaxVolume());

    /* Subtitle List */
    m_mediator->setSubtitleList(m_ui->listSubtitles);
    m_ui->listSubtitles->hide();

    /* Anki */
    m_ankiClient   = new AnkiClient(this);
    m_ankiSettings = new AnkiSettings;
    m_ankiSettings->hide();

    m_actionGroupAnkiProfile = new QActionGroup(this);
    updateAnkiProfileMenu();    

    /* Dictionary Settings */
    m_dictionarySettings = new DictionarySettings;
    m_dictionarySettings->hide();

    /* Toolbar Actions */
    connect(m_ui->actionAnki,        &QAction::triggered, m_ankiSettings,       &QWidget::show);
    connect(m_ui->actionOpen,        &QAction::triggered, this,                 &MainWindow::open);
    connect(m_ui->actionDict,        &QAction::triggered, m_dictionarySettings, &DictionarySettings::show);
    connect(m_ui->actionUpdate,      &QAction::triggered, this,                 &MainWindow::checkForUpdates);
    connect(m_ui->actionAddSubtitle, &QAction::triggered, this,
        [=] {
            QString file = QFileDialog::getOpenFileName(0, "Open Subtitle");
            if (!file.isEmpty())
            {
                m_player->addSubtitle(file);
                setTracks(m_player->getTracks());
            }
        }
    );

    /* State Changes */
    connect(m_mediator, &GlobalMediator::playerCursorHidden,      this, &MainWindow::hideControls);
    connect(m_mediator, &GlobalMediator::playerCursorHidden,      this, &MainWindow::hidePlayerCursor);
    connect(m_mediator, &GlobalMediator::ankiSettingsChanged,     this, &MainWindow::updateAnkiProfileMenu);
    connect(m_mediator, &GlobalMediator::playerFullscreenChanged, this, &MainWindow::setFullscreen);
    connect(m_mediator, &GlobalMediator::playerClosed,            this, &QApplication::quit);
    connect(m_mediator, &GlobalMediator::playerTitleChanged,      this, 
        [=] (const QString &name) { setWindowTitle(name + " - Memento"); }
    );
    connect(m_mediator, &GlobalMediator::controlsSubtitleListToggled,      this,
        [=] { 
            m_ui->listSubtitles->setVisible(!m_ui->listSubtitles->isVisible()); 
        }
    );

    /* Track Changes */
    connect(m_mediator, &GlobalMediator::playerTracksChanged, this, &MainWindow::setTracks);

    connect(m_mediator, &GlobalMediator::playerAudioTrackChanged, this,
        [=] (const int id) {
            if(!m_audioTracks.isEmpty())
                m_audioTracks[id - 1].first->setChecked(true); 
        }
    );
    connect(m_mediator, &GlobalMediator::playerVideoTrackChanged, this,
        [=] (const int id) {
        if(!m_videoTracks.isEmpty())
            m_videoTracks[id - 1].first->setChecked(true);
        }
    );
    connect(m_mediator, &GlobalMediator::playerSubtitleTrackChanged, this,
        [=] (const int id) {
            if(!m_subtitleTracks.isEmpty())
                m_subtitleTracks[id - 1].first->setChecked(true);
        }
    );
    connect(m_mediator, &GlobalMediator::playerSecondSubtitleTrackChanged, this,
        [=] (const int id) {
            if(!m_subtitleTwoTracks.isEmpty())
                m_subtitleTwoTracks[id - 1].first->setChecked(true);
        }
    );

    connect(m_mediator, &GlobalMediator::playerAudioDisabled, this,
        [=] { m_ui->actionAudioNone->setChecked(true); } 
    );
    connect(m_mediator, &GlobalMediator::playerVideoDisabled, this,
        [=] { m_ui->actionVideoNone->setChecked(true); }
    );
    connect(m_mediator, &GlobalMediator::playerSubtitlesDisabled, this,
        [=] { m_ui->actionSubtitleNone->setChecked(true); }
    );
    connect(m_mediator, &GlobalMediator::playerSecondSubtitlesDisabled, this,
        [=] { m_ui->actionSubtitleTwoNone->setChecked(true); }
    );

    connect(m_ui->actionAudioNone,       &QAction::triggered, m_player, &PlayerAdapter::disableAudio);
    connect(m_ui->actionVideoNone,       &QAction::triggered, m_player, &PlayerAdapter::disableVideo);
    connect(m_ui->actionSubtitleNone,    &QAction::triggered, m_player, &PlayerAdapter::disableSubtitles);
    connect(m_ui->actionSubtitleTwoNone, &QAction::triggered, m_player, &PlayerAdapter::disableSubtitleTwo);

    /* Definition Changes */
    connect(m_mediator, &GlobalMediator::termsChanged,            this, &MainWindow::setTerms);
    connect(m_mediator, &GlobalMediator::playerSubtitleChanged,   this, &MainWindow::deleteDefinitionWidget);
    connect(m_mediator, &GlobalMediator::subtitleExpired,         this, &MainWindow::deleteDefinitionWidget);
    connect(m_mediator, &GlobalMediator::playerPauseStateChanged, this, 
        [=] (const bool paused) { if (!paused) deleteDefinitionWidget(); }
    );

    /* Show message boxes */
    connect(m_mediator, &GlobalMediator::showCritical,    this, &MainWindow::showErrorMessage);
    connect(m_mediator, &GlobalMediator::showInformation, this, &MainWindow::showInfoMessage);
}

MainWindow::~MainWindow()
{
    clearTracks();
    delete m_ui;
    delete m_mediator;
    delete m_player;
    delete m_actionGroupAudio;
    delete m_actionGroupVideo;
    delete m_actionGroupSubtitle;
    delete m_actionGroupSubtitleTwo;
    delete m_definition;
    delete m_ankiClient;
    delete m_ankiSettings;
    delete m_dictionarySettings;
    delete m_actionGroupAnkiProfile;
    delete m_manager;
}

void MainWindow::showEvent(QShowEvent *event)
{
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
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    default:
    {
        Q_EMIT m_mediator->keyPressed(event);
    }
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (!m_ui->listSubtitles->underMouse())
    {
        Q_EMIT m_mediator->wheelMoved(event);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        m_player->open(event->mimeData()->urls());
        m_player->play();
    }   
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    event->ignore();
    deleteDefinitionWidget();
}

void MainWindow::setFullscreen(bool value)
{
    if (value)
    {
        m_maximized = isMaximized();
        showFullScreen();
        m_ui->menubar->hide();
        QApplication::processEvents();
        m_ui->controls->hide();
        m_ui->centralwidget->layout()->removeWidget(m_ui->controls);
        m_ui->controls->raise();
    }
    else
    {
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        if (m_maximized)
            showMaximized();
        else
            showNormal();
    #elif __linux__
        showNormal();
        if (m_maximized)
        {
            // Have call showNormal before showMaximized when leaving fullscreen
            // on Linux due to a bug with Qt
            showMaximized();
        }
    #endif
        
        m_ui->menubar->show();
        m_ui->controls->show();
        m_ui->centralwidget->layout()->addWidget(m_ui->controls);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isFullScreen() && m_ui->controls->isHidden())
    {
        m_ui->controls->show();
    }
    m_ui->mpv->setCursor(Qt::ArrowCursor);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->ignore();
    deleteDefinitionWidget();
}

void MainWindow::setTracks(QList<const Track *> tracks)
{
    clearTracks();

    for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
        const Track *track = *it;
        QString actionText = "Track " + QString::number(track->id);
        if (!track->lang.isEmpty())
            actionText += " [" + track->lang + "]";
        if (!track->title.isEmpty())
            actionText += " - " + track->title;

        QAction *action = new QAction(this);
        action->setText(actionText);
        action->setCheckable(true);
        QList<QPair<QAction *, const Track *>> *trackList;
        switch (track->type)
        {
        case Track::track_type::audio:
            m_ui->menuAudio->addAction(action);
            action->setActionGroup(m_actionGroupAudio);
            connect(action, &QAction::triggered, 
                [=] (const bool checked) { 
                    if (checked) m_player->setAudioTrack(track->id); 
                } );
            trackList = &m_audioTracks;
            break;
        case Track::track_type::video:
            m_ui->menuVideo->addAction(action);
            action->setActionGroup(m_actionGroupVideo);
            connect(action, &QAction::triggered, 
                [=] (const bool checked) { 
                    if (checked) m_player->setVideoTrack(track->id);
                } );
            trackList = &m_videoTracks;
            break;
        case Track::track_type::subtitle:
        {
            m_ui->menuSubtitle->addAction(action);
            action->setActionGroup(m_actionGroupSubtitle);
            connect(action, &QAction::triggered, 
                [=] (const bool checked) { 
                    if (checked) m_player->setSubtitleTrack(track->id); 
                } );
            trackList = &m_subtitleTracks;

            QAction *actionSubTwo = new QAction(this);
            actionSubTwo->setText(actionText);
            actionSubTwo->setCheckable(true);
            m_ui->menuSubtitleTwo->addAction(actionSubTwo);
            actionSubTwo->setActionGroup(m_actionGroupSubtitleTwo);
            connect(actionSubTwo, &QAction::triggered, 
                [=] (const bool checked) { 
                    if (checked) m_player->setSubtitleTwoTrack(track->id); 
                } );
            m_subtitleTwoTracks.push_back(
                QPair<QAction *, const Track *>(actionSubTwo, nullptr));
            break;
        }
        }
        action->setChecked(track->selected);
        trackList->push_back(QPair<QAction *, const Track *>(action, track));
    }
}

void MainWindow::setTerms(const QList<Term *> *terms)
{
    deleteDefinitionWidget();
    
    m_definition = new DefinitionWidget(terms, m_ankiClient, this);
    setDefinitionWidgetLocation();
    m_definition->show();
}

void MainWindow::updateAnkiProfileMenu()
{
    // Remove all profiles
    for (auto it = m_ankiProfiles.begin(); it != m_ankiProfiles.end(); ++it)
    {
        m_actionGroupAnkiProfile->removeAction(*it);
        m_ui->menuAnkiProfile->removeAction(*it);
        delete *it;
    }
    m_ankiProfiles.clear();

    // Get profiles in alphabetical order
    QStringList profiles = m_ankiClient->getProfiles();
    std::sort(profiles.begin(), profiles.end());

    // Add new profiles
    for (auto it = profiles.begin(); it != profiles.end(); ++it)
    {
        QAction *profileAction = new QAction(this);
        profileAction->setText(*it);
        profileAction->setCheckable(true);
        profileAction->setChecked(m_ankiClient->getProfile() == *it);
        m_ui->menuAnkiProfile->addAction(profileAction);
        profileAction->setActionGroup(m_actionGroupAnkiProfile);
        m_ankiProfiles.append(profileAction);

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

    // Handle disabled Anki Integration
    m_ui->menuAnkiProfile->menuAction()->setVisible(m_ankiClient->isEnabled());
}

void MainWindow::setDefinitionWidgetLocation()
{
    const QPoint mousePos = mapFromGlobal(QCursor::pos());

    int x = mousePos.x() - (m_definition->width() / 2);
    if (x < 0)
    {
        x = 0;
    }
    else if (x > width() - m_definition->width())
    {
        x = width() - m_definition->width();
    }

    int y = height() - m_definition->height() - m_ui->controls->height();
    m_definition->move(x, y);
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

inline bool MainWindow::isMouseOverPlayer()
{
    return !m_ui->controls->underMouse() && 
           (m_definition == nullptr || m_definition && !m_definition->underMouse());
}

void MainWindow::hideControls()
{
    if (isFullScreen() && isMouseOverPlayer())
    {
        m_ui->controls->hide();
        deleteDefinitionWidget();
    }
}

void MainWindow::hidePlayerCursor()
{
    if (isMouseOverPlayer()) 
    {
        m_ui->mpv->setCursor(Qt::BlankCursor);
    }
}

void MainWindow::clearTracks()
{
    clearTrack(m_audioTracks, m_ui->menuAudio, m_actionGroupAudio,
               m_ui->actionAudioNone);
    clearTrack(m_videoTracks, m_ui->menuVideo, m_actionGroupVideo,
               m_ui->actionVideoNone);
    clearTrack(m_subtitleTracks, m_ui->menuSubtitle, m_actionGroupSubtitle,
               m_ui->actionSubtitleNone);
    clearTrack(m_subtitleTwoTracks, m_ui->menuSubtitleTwo,
               m_actionGroupSubtitleTwo, m_ui->actionSubtitleTwoNone);
    
}

void MainWindow::clearTrack(
    QList<QPair<QAction *, const Track *>> &tracks,
    QMenu *menu, QActionGroup *actionGroup, QAction *actionDisable)
{
    for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
        menu->removeAction((*it).first);
        actionGroup->removeAction((*it).first);
        delete (*it).first;
        delete (*it).second;
    }
    tracks.clear();

    actionDisable->setChecked(true);
}

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

void MainWindow::checkForUpdates()
{
    m_manager->setTransferTimeout();
    QNetworkRequest request(GITHUB_API_LINK);
    QNetworkReply *reply = m_manager->get(request);
    connect(reply, &QNetworkReply::finished, [=] {
            if (reply->error())
            {
                Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
                    "Error", 
                    "Could not check for updates:\n" + reply->errorString()
                );
            }
            else
            {
                QJsonDocument replyDoc =
                    QJsonDocument::fromJson(reply->readAll());
                if (replyDoc.isNull() ||
                    !replyDoc.isArray() || 
                    replyDoc.array().isEmpty() ||
                    !replyDoc.array().first().isObject() ||
                    !replyDoc.array().first().toObject()["tag_name"].isString())
                {
                    Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
                        "Error", 
                        "Server did not send a valid reply.\n"
                        "Check manually <a href='" + GITHUB_RELEASES +
                        "'>here</a>"
                    );
                }
                else 
                {
                    QString tag = replyDoc.array()
                                          .first()
                                          .toObject()["tag_name"]
                                          .toString();
                    if (tag != VERSION)
                    {
                        Q_EMIT GlobalMediator::getGlobalMediator()->showInformation(
                            "Update Available",
                            "New version <a href='" + GITHUB_RELEASES + "'>" + 
                            tag + "</a> available"
                        );
                    }
                    else
                    {
                        Q_EMIT GlobalMediator::getGlobalMediator()->showInformation(
                            "Up to Date",
                            "Memento is up to date"
                        );
                    }
                }
            }
            reply->deleteLater();
    });
}

void MainWindow::showErrorMessage(const QString &title, 
                                  const QString &error) const
{
    QMessageBox message;
    message.critical(0, title, error);
}

void MainWindow::showInfoMessage(const QString &title, 
                                 const QString &error) const
{
    QMessageBox message;
    message.information(0, title, error);
}