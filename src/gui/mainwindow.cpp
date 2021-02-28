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
#include "../dict/builder/dictionarybuilder.h"
#include "../util/constants.h"

#include <QCursor>
#include <QFileDialog>
#include <QMimeData>
#include <QDebug>
#include <QMessageBox>
#include <QThreadPool>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_ui(new Ui::MainWindow), m_maximized(false),
      m_manager(new QNetworkAccessManager)
{
    m_ui->setupUi(this);

    // Video Action Groups
    m_actionGroupAudio = new QActionGroup(this);
    m_ui->m_actionAudioNone->setActionGroup(m_actionGroupAudio);
    m_actionGroupVideo = new QActionGroup(this);
    m_ui->m_actionVideoNone->setActionGroup(m_actionGroupVideo);
    m_actionGroupSubtitle = new QActionGroup(this);
    m_ui->m_actionSubtitleNone->setActionGroup(m_actionGroupSubtitle);
    m_actionGroupSubtitleTwo = new QActionGroup(this);
    m_ui->m_actionSubtitleTwoNone->setActionGroup(m_actionGroupSubtitleTwo);

    // Player behaviors
    m_player = new MpvAdapter(m_ui->m_mpv, this);
    m_player->pause();

    // Anki
    m_ankiClient = new AnkiClient(this, m_player);
    m_ankiSettings = new AnkiSettings(m_ankiClient);
    m_ankiSettings->hide();

    // Definitions
    m_definition = new DefinitionWidget(m_ankiClient, m_ui->m_mpv);
    m_ui->m_controls->setVolumeLimit(m_player->getMaxVolume());

    m_actionGroupAnkiProfile = new QActionGroup(this);
    updateAnkiProfileMenu();

    // Toolbar Actions
    connect(m_ui->m_actionOpen, &QAction::triggered, this, &MainWindow::open);
    connect(m_ui->m_actionAddSubtitle, &QAction::triggered, [=] {
        QString file = QFileDialog::getOpenFileName(0, "Open Subtitle");
        if (!file.isEmpty())
        {
            m_player->addSubtitle(file);
            setTracks(m_player->getTracks());
        }
    });
    connect(m_ui->m_actionAnki, &QAction::triggered,
        m_ankiSettings, &QWidget::show);
    connect(m_ui->m_actionUpdateJMDict, &QAction::triggered,
        this, &MainWindow::updateJMDict);
    connect(m_ui->m_actionUpdate, &QAction::triggered,
        this, &MainWindow::checkForUpdates);

    // Buttons
    connect(m_ui->m_controls, &PlayerControls::play,
        m_player, &PlayerAdapter::play);
    connect(m_ui->m_controls, &PlayerControls::pause,
        m_player, &PlayerAdapter::pause);
    connect(m_ui->m_controls, &PlayerControls::stop,
        m_player, &PlayerAdapter::stop);
    connect(m_ui->m_controls, &PlayerControls::skipForward,
        m_player, &PlayerAdapter::skipForward);
    connect(m_ui->m_controls, &PlayerControls::skipBackward,
        m_player, &PlayerAdapter::skipBackward);
    connect(m_ui->m_controls, &PlayerControls::seekForward,
        m_player, &PlayerAdapter::seekForward);
    connect(m_ui->m_controls, &PlayerControls::seekBackward,
        m_player, &PlayerAdapter::seekBackward);
    connect(m_ui->m_controls, &PlayerControls::fullscreenChanged,
        m_player, &PlayerAdapter::setFullscreen);

    // Slider
    connect(m_ui->m_controls, &PlayerControls::volumeSliderMoved,
        m_player, &PlayerAdapter::setVolume);
    connect(m_ui->m_controls, &PlayerControls::sliderMoved,
        m_player, &PlayerAdapter::seek);
    connect(m_player, &PlayerAdapter::durationChanged,
        m_ui->m_controls, &PlayerControls::setDuration);
    connect(m_player, &PlayerAdapter::positionChanged,
        m_ui->m_controls, &PlayerControls::setPosition);

    // State changes
    connect(m_player, &PlayerAdapter::subtitleChanged,
        m_ui->m_controls, &PlayerControls::setSubtitle);
    connect(m_player, &PlayerAdapter::stateChanged,
        m_ui->m_controls, &PlayerControls::setPaused);
    connect(m_player, &PlayerAdapter::fullscreenChanged,
        this, &MainWindow::setFullscreen);
    connect(m_player, &PlayerAdapter::fullscreenChanged,
        m_ui->m_controls, &PlayerControls::setFullscreen);
    connect(m_player, &PlayerAdapter::volumeChanged,
        m_ui->m_controls, &PlayerControls::setVolume);
    connect(m_player, &PlayerAdapter::hideCursor,
        this, &MainWindow::hideControls);
    connect(m_player, &PlayerAdapter::hideCursor,
        this, &MainWindow::hidePlayerCursor);
    connect(m_player, &PlayerAdapter::close, this, &QApplication::quit);
    connect(m_player, &PlayerAdapter::titleChanged, 
        [=] (const QString &name) { setWindowTitle(name + " - Memento"); });
    connect(m_ankiClient, &AnkiClient::settingsChanged,
        this, &MainWindow::updateAnkiProfileMenu);

    // Key/Mouse presses
    connect(this, &MainWindow::keyPressed,
        m_player, &PlayerAdapter::keyPressed);
    connect(this, &MainWindow::wheelMoved,
        m_player, &PlayerAdapter::mouseWheelMoved);

    // Track changes
    connect(m_player, &PlayerAdapter::tracksChanged,
        this, &MainWindow::setTracks);

    connect(m_player, &PlayerAdapter::audioTrackChanged, 
            [=] (const int id) {
                if(!m_audioTracks.isEmpty())
                    m_audioTracks[id - 1].first->setChecked(true); 
            } );
    connect(m_player, &PlayerAdapter::videoTrackChanged,
            [=] (const int id) {
                if(!m_videoTracks.isEmpty())
                    m_videoTracks[id - 1].first->setChecked(true);
            } );
    connect(m_player, &PlayerAdapter::subtitleTrackChanged,
            [=] (const int id) {
                if(!m_subtitleTracks.isEmpty())
                    m_subtitleTracks[id - 1].first->setChecked(true);
            } );
    connect(m_player, &PlayerAdapter::subtitleTwoTrackChanged,
            [=] (const int id) {
                if(!m_subtitleTwoTracks.isEmpty())
                    m_subtitleTwoTracks[id - 1].first->setChecked(true);
            } );

    connect(m_player, &PlayerAdapter::audioDisabled,
        [=] { m_ui->m_actionAudioNone->setChecked(true); } );
    connect(m_player, &PlayerAdapter::videoDisabled,
        [=] { m_ui->m_actionVideoNone->setChecked(true); } );
    connect(m_player, &PlayerAdapter::subtitleDisabled,
        [=] { m_ui->m_actionSubtitleNone->setChecked(true); } );
    connect(m_player, &PlayerAdapter::subtitleTwoDisabled,
        [=] { m_ui->m_actionSubtitleTwoNone->setChecked(true); } );

    connect(m_ui->m_actionAudioNone, &QAction::triggered,
        m_player, &PlayerAdapter::disableAudio);
    connect(m_ui->m_actionVideoNone, &QAction::triggered,
        m_player, &PlayerAdapter::disableVideo);
    connect(m_ui->m_actionSubtitleNone, &QAction::triggered,
        m_player, &PlayerAdapter::disableSubtitles);
    connect(m_ui->m_actionSubtitleTwoNone, &QAction::triggered,
        m_player, &PlayerAdapter::disableSubtitleTwo);

    // Definition changes
    connect(m_ui->m_controls, &PlayerControls::entriesChanged,
        m_definition, &DefinitionWidget::setEntries);
    connect(m_ui->m_controls, &PlayerControls::entriesChanged,
        this, &MainWindow::setDefinitionWidgetLocation);
    connect(m_ui->m_controls, &PlayerControls::hideDefinition,
        m_definition, &DefinitionWidget::hide);
    connect(m_definition, &DefinitionWidget::definitionHidden,
        m_ui->m_controls, &PlayerControls::definitionHidden);
    connect(this, &MainWindow::jmDictUpdated,
        m_ui->m_controls, &PlayerControls::jmDictUpdated);

    // Thread message box signals
    connect(this, &MainWindow::threadError, 
        this, &MainWindow::showErrorMessage);
    connect(this, &MainWindow::threadInfo, 
        this, &MainWindow::showInfoMessage);
}

MainWindow::~MainWindow()
{
    clearTracks();
    delete m_ui;
    delete m_player;
    delete m_actionGroupAudio;
    delete m_actionGroupVideo;
    delete m_actionGroupSubtitle;
    delete m_actionGroupSubtitleTwo;
    delete m_definition;
    delete m_ankiClient;
    delete m_ankiSettings;
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
        Q_EMIT keyPressed(event);
    }
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    Q_EMIT wheelMoved(event);
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

void MainWindow::setFullscreen(bool value)
{
    if (value)
    {
        m_maximized = isMaximized();
        showFullScreen();
        m_ui->m_menubar->hide();
        QApplication::processEvents();
        m_ui->m_controls->hide();
        m_ui->m_centralwidget->layout()->removeWidget(m_ui->m_controls);
        m_ui->m_controls->raise();
    }
    else
    {
        showNormal();
        if (m_maximized)
        {
            // Have call showNormal before showMaximized when leaving fullscreen
            // on Linux due to a bug with Qt
            showMaximized();
        }
        m_ui->m_menubar->show();
        m_ui->m_controls->show();
        m_ui->m_centralwidget->layout()->addWidget(m_ui->m_controls);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isFullScreen() && m_ui->m_controls->isHidden())
    {
        m_ui->m_controls->show();
    }
    m_ui->m_mpv->setCursor(Qt::ArrowCursor);
}

void MainWindow::setTracks(QList<const PlayerAdapter::Track *> tracks)
{
    clearTracks();

    for (auto it = tracks.begin(); it != tracks.end(); ++it)
    {
        const PlayerAdapter::Track *track = *it;
        QString actionText = "Track " + QString::number(track->id);
        if (!track->lang.isEmpty())
            actionText += " [" + track->lang + "]";
        if (!track->title.isEmpty())
            actionText += " - " + track->title;

        QAction *action = new QAction(this);
        action->setText(actionText);
        action->setCheckable(true);
        QList<QPair<QAction *, const PlayerAdapter::Track *>> *trackList;
        switch (track->type)
        {
        case PlayerAdapter::Track::track_type::audio:
            m_ui->m_menuAudio->addAction(action);
            action->setActionGroup(m_actionGroupAudio);
            connect(action, &QAction::triggered, 
                [=] (const bool checked) { 
                    if (checked) m_player->setAudioTrack(track->id); 
                } );
            trackList = &m_audioTracks;
            break;
        case PlayerAdapter::Track::track_type::video:
            m_ui->m_menuVideo->addAction(action);
            action->setActionGroup(m_actionGroupVideo);
            connect(action, &QAction::triggered, 
                [=] (const bool checked) { 
                    if (checked) m_player->setVideoTrack(track->id);
                } );
            trackList = &m_videoTracks;
            break;
        case PlayerAdapter::Track::track_type::subtitle:
        {
            m_ui->m_menuSubtitle->addAction(action);
            action->setActionGroup(m_actionGroupSubtitle);
            connect(action, &QAction::triggered, 
                [=] (const bool checked) { 
                    if (checked) m_player->setSubtitleTrack(track->id); 
                } );
            trackList = &m_subtitleTracks;

            QAction *actionSubTwo = new QAction(this);
            actionSubTwo->setText(actionText);
            actionSubTwo->setCheckable(true);
            m_ui->m_menuSubtitleTwo->addAction(actionSubTwo);
            actionSubTwo->setActionGroup(m_actionGroupSubtitleTwo);
            connect(actionSubTwo, &QAction::triggered, 
                [=] (const bool checked) { 
                    if (checked) m_player->setSubtitleTwoTrack(track->id); 
                } );
            m_subtitleTwoTracks.push_back(
                QPair<QAction *, const PlayerAdapter::Track *>
                    (actionSubTwo, nullptr));
            break;
        }
        }
        action->setChecked(track->selected);
        trackList->push_back(
            QPair<QAction *, const PlayerAdapter::Track *>(action, track));
    }
}

void MainWindow::updateAnkiProfileMenu()
{
    // Remove all profiles
    for (auto it = m_ankiProfiles.begin(); it != m_ankiProfiles.end(); ++it)
    {
        m_actionGroupAnkiProfile->removeAction(*it);
        m_ui->m_menuAnkiProfile->removeAction(*it);
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
        m_ui->m_menuAnkiProfile->addAction(profileAction);
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
    m_ui->m_menuAnkiProfile->
        menuAction()->setVisible(m_ankiClient->isEnabled());
}

void MainWindow::setDefinitionWidgetLocation()
{
    const QPoint mousePos = mapFromGlobal(QCursor::pos());

    int x = mousePos.x() - (m_definition->width() / 2);
    if (x < 0)
    {
        x = 0;
    }
    else if (x > m_ui->m_mpv->width() - m_definition->width())
    {
        x = m_ui->m_mpv->width() - m_definition->width();
    }

    int y = m_ui->m_mpv->height() - m_definition->height();
    y = isFullScreen() ? y - m_ui->m_controls->height() : y;

    m_definition->move(x, y);
}

void MainWindow::hideControls()
{
    if (isFullScreen() && !m_ui->m_controls->underMouse() && 
        !m_definition->underMouse())
    {
        m_ui->m_controls->hide();
    }
}

void MainWindow::hidePlayerCursor()
{
    if (!m_ui->m_controls->underMouse() && !m_definition->underMouse()) 
    {
        m_ui->m_mpv->setCursor(Qt::BlankCursor);
    }
}

void MainWindow::clearTracks()
{
    clearTrack(m_audioTracks, m_ui->m_menuAudio, m_actionGroupAudio,
               m_ui->m_actionAudioNone);
    clearTrack(m_videoTracks, m_ui->m_menuVideo, m_actionGroupVideo,
               m_ui->m_actionVideoNone);
    clearTrack(m_subtitleTracks, m_ui->m_menuSubtitle, m_actionGroupSubtitle,
               m_ui->m_actionSubtitleNone);
    clearTrack(m_subtitleTwoTracks, m_ui->m_menuSubtitleTwo,
               m_actionGroupSubtitleTwo, m_ui->m_actionSubtitleTwoNone);
    
}

void MainWindow::clearTrack(
    QList<QPair<QAction *, const PlayerAdapter::Track *>> &tracks,
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

void MainWindow::updateJMDict()
{
    QString jmdictFile = 
        QFileDialog::getOpenFileName(0, "Open the JMDict file");
    if (!jmdictFile.isEmpty())
    {
        JMDictUpdaterThread *thread = new JMDictUpdaterThread(this, jmdictFile);
        QThreadPool::globalInstance()->start(thread);
    }
    
}

void MainWindow::JMDictUpdaterThread::run()
{
    QString error = QString::fromStdString(DictionaryBuilder::buildDictionary(
        m_path.toStdString(), 
        (DirectoryUtils::getDictionaryDir() + JMDICT_DB_FILE).toStdString()
    ));
    if (error.isEmpty())
    {
        Q_EMIT m_parent->jmDictUpdated();
    }
    else
    {
        Q_EMIT m_parent->threadError("Error Updating JMDict", error);
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
                Q_EMIT threadError(
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
                    !replyDoc.array().last().isObject() ||
                    !replyDoc.array().last().toObject()["tag_name"].isString())
                {
                    Q_EMIT threadError(
                        "Error", 
                        "Server did not send a valid reply.\n"
                        "Check manually <a href='" + GITHUB_RELEASES +
                        "'>here</a>"
                    );
                }
                else 
                {
                    QString tag = replyDoc.array()
                                           .last()
                                           .toObject()["tag_name"]
                                           .toString();
                    if (tag != VERSION)
                    {
                        Q_EMIT threadInfo(
                            "Update Available",
                            "New version <a href='" + GITHUB_RELEASES + "'>" + 
                            tag + "</a> available"
                        );
                    }
                    else
                    {
                        Q_EMIT threadInfo(
                            "Up to Date",
                            "Memento is up to date"
                        );
                    }
                }
            }
            delete reply;
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