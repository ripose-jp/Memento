////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#include "playermenu.h"
#include "ui_playermenu.h"

#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGuiApplication>
#include <QInputDialog>
#include <QList>
#include <QSettings>

#include "../../../anki/ankiclient.h"
#include "../../../util/constants.h"
#include "../../../util/globalmediator.h"
#include "../../../util/utils.h"
#include "../../playeradapter.h"

/* Begin Constructor/Destructor */

PlayerMenu::PlayerMenu(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::PlayerMenu),
      m_player(GlobalMediator::getGlobalMediator()->getPlayerAdapter())
{
    m_ui->setupUi(this);

    updateSubtitlePauseAction();

    /* Anki */
    m_actionGroupAnkiProfile = new QActionGroup(this);
    updateAnkiProfileMenu();

    m_actionGroups.audio       = new QActionGroup(this);
    m_actionGroups.video       = new QActionGroup(this);
    m_actionGroups.subtitle    = new QActionGroup(this);
    m_actionGroups.subtitleTwo = new QActionGroup(this);

    m_ui->actionAudioNone->setActionGroup(m_actionGroups.audio);
    m_ui->actionVideoNone->setActionGroup(m_actionGroups.video);
    m_ui->actionSubtitleNone->setActionGroup(m_actionGroups.subtitle);
    m_ui->actionSubtitleTwoNone->setActionGroup(m_actionGroups.subtitleTwo);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Open Signals */
    connect(
        m_ui->actionOpen, &QAction::triggered,
        this, &PlayerMenu::openFile
    );
    connect(
        m_ui->actionOpenUrl, &QAction::triggered,
        this, &PlayerMenu::openUrl
    );
    connect(
        m_ui->actionAddSubtitle, &QAction::triggered,
        this, &PlayerMenu::openSubtitle
    );
    connect(
        m_ui->actionUpdate, &QAction::triggered,
        this, &NetworkUtils::checkForUpdates
    );

    /* Track Signals */
        connect(
        mediator, &GlobalMediator::playerTracksChanged,
        this,       &PlayerMenu::setTracks
    );

    connect(
        m_ui->actionAudioNone, &QAction::toggled, this,
        [=] (bool checked) { if (checked) setAudioTrack(0); }
    );
    connect(
        m_ui->actionVideoNone, &QAction::toggled, this,
        [=] (bool checked) { if (checked) setVideoTrack(0); }
    );
    connect(
        m_ui->actionSubtitleNone, &QAction::toggled, this,
        [=] (bool checked) { if (checked) setSubtitleTrack(0); }
    );
    connect(
        m_ui->actionSubtitleTwoNone, &QAction::toggled, this,
        [=] (bool checked) { if (checked) setSecondarySubtitleTrack(0); }
    );

    connect(
        mediator, &GlobalMediator::playerAudioTrackChanged,
        this, &PlayerMenu::updateAudioAction
    );
    connect(
        mediator, &GlobalMediator::playerVideoTrackChanged,
        this, &PlayerMenu::updateVideoAction
    );
    connect(
        mediator, &GlobalMediator::playerSubtitleTrackChanged,
        this, &PlayerMenu::updateSubtitleAction
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitleTrackChanged,
        this, &PlayerMenu::updateSecondarySubtitleAction
    );

    connect(
        mediator, &GlobalMediator::playerAudioDisabled,
        this, [=] { updateAudioAction(); }
    );
    connect(
        mediator, &GlobalMediator::playerVideoDisabled,
        this, [=] { updateVideoAction(); }
    );
    connect(
        mediator, &GlobalMediator::playerSubtitlesDisabled,
        this, [=] { updateSubtitleAction(); }
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitlesDisabled,
        this, [=] { updateSecondarySubtitleAction(); }
    );

    connect(
        mediator, &GlobalMediator::playerFileLoaded,
        this, [=] {
            QList<const Track *> tracks = m_player->getTracks();
            setTracks(tracks);
            for (const Track *track : tracks)
            {
                delete track;
            }
        }
    );

    /* Option Signals */
    connect(
        m_ui->actionSubtitlePause, &QAction::toggled,
        this, &PlayerMenu::applySubtitlePauseSetting
    );
    connect(
        m_ui->actionShowSearch, &QAction::triggered,
        this, &PlayerMenu::handleToggleSearch
    );
    connect(
        m_ui->actionOptions, &QAction::triggered,
        mediator, &GlobalMediator::menuShowOptions
    );
    connect(
        m_ui->actionAbout, &QAction::triggered,
        mediator, &GlobalMediator::menuShowAbout
    );
    connect(
        m_ui->actionOpenConfig, &QAction::triggered,
        this, &PlayerMenu::openConfigFolder
    );
    connect(
        mediator, &GlobalMediator::behaviorSettingsChanged,
        this, &PlayerMenu::updateSubtitlePauseAction
    );
    connect(
        mediator, &GlobalMediator::ankiSettingsChanged,
        this, &PlayerMenu::updateAnkiProfileMenu
    );
    connect(
        m_ui->actionSubVis, &QAction::toggled,
        mediator, &GlobalMediator::menuSubtitleVisibilityToggled
    );
    connect(
        m_ui->actionIncreaseSize, &QAction::triggered,
        mediator, &GlobalMediator::menuSubtitleSizeIncrease
    );
    connect(
        m_ui->actionDecreaseSize, &QAction::triggered,
        mediator, &GlobalMediator::menuSubtitleSizeDecrease
    );
    connect(
        m_ui->actionMoveUp, &QAction::triggered,
        mediator, &GlobalMediator::menuSubtitlesMoveUp
    );
    connect(
        m_ui->actionMoveDown, &QAction::triggered,
        mediator, &GlobalMediator::menuSubtitlesMoveDown
    );
    connect(
        mediator, &GlobalMediator::searchWidgetHidden,
        m_ui->actionShowSearch,
        [=] { m_ui->actionShowSearch->setChecked(false); }
    );
    connect(
        mediator, &GlobalMediator::searchWidgetShown,
        m_ui->actionShowSearch,
        [=] { m_ui->actionShowSearch->setChecked(true); }
    );

    /* aboutToHide Signals */
    QList<QMenu *> menus = m_ui->menubar->findChildren<QMenu *>();
    for (const QMenu *menu : menus)
    {
        connect(menu, &QMenu::aboutToHide, this, &PlayerMenu::aboutToHide);
    }
}

PlayerMenu::~PlayerMenu()
{
    clearTracks();
    delete m_ui;
}

/* End Constructor/Destructor */
/* Begin Status Methods */

bool PlayerMenu::menuOpen() const
{
    QList<QMenu *> menus = m_ui->menubar->findChildren<QMenu *>();
    for (const QMenu *menu : menus)
    {
        if (menu->isVisible())
        {
            return true;
        }
    }
    return false;
}

bool PlayerMenu::menuVisible() const
{
    return height() != 0;
}

void PlayerMenu::hideMenu()
{
    setFixedHeight(0);
}

void PlayerMenu::showMenu()
{
    setMinimumHeight(0);
    setMaximumHeight(QWIDGETSIZE_MAX);
}

/* End Status Methods */
/* Begin Track Methods */

void PlayerMenu::clearTrack(QList<QAction *> &actions,
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

    actionGroup->removeAction(actionDisable);
    bool signalsBlocked = actionDisable->signalsBlocked();
    actionDisable->blockSignals(true);
    actionDisable->setChecked(true);
    actionDisable->blockSignals(signalsBlocked);
    actionGroup->addAction(actionDisable);
}

void PlayerMenu::clearTracks()
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
}

QAction *PlayerMenu::createTrackAction(const Track *track) const
{
    QAction *action = new QAction;
    action->setCheckable(true);

    QString actionText = "Track " + QString::number(track->id);
    if (!track->lang.isEmpty())
    {
        actionText += " [" + track->lang + "]";
    }
    if (!track->title.isEmpty())
    {
        actionText += " - " + track->title;
    }
    action->setText(actionText);

    return action;
}

void PlayerMenu::setTracks(const QList<const Track *> &tracks)
{
    clearTracks();

    m_actionGroups.audio->blockSignals(true);
    m_actionGroups.video->blockSignals(true);
    m_actionGroups.subtitle->blockSignals(true);
    m_actionGroups.subtitleTwo->blockSignals(true);

    for (const Track *track : tracks)
    {
        QAction *action = createTrackAction(track);
        const int64_t id = track->id;
        switch (track->type)
        {
        case Track::Type::audio:
            m_ui->menuAudio->addAction(action);
            action->setActionGroup(m_actionGroups.audio);
            m_actionGroups.audioActions.append(action);

            if (track->selected)
            {
                action->setChecked(true);
                m_ui->actionAudioNone->setChecked(false);
            }

            connect(action, &QAction::toggled, action,
                [=] (bool checked) {
                    if (checked)
                    {
                        setAudioTrack(id);
                    }
                }
            );
            break;

        case Track::Type::video:
            m_ui->menuVideo->addAction(action);
            action->setActionGroup(m_actionGroups.video);
            m_actionGroups.videoActions.append(action);

            if (track->selected)
            {
                action->setChecked(true);
                m_ui->actionVideoNone->setChecked(false);
            }

            connect(action, &QAction::toggled, action,
                [=] (bool checked) {
                    if (checked)
                    {
                        setVideoTrack(id);
                    }
                }
            );
            break;

        case Track::Type::subtitle:
        {
            m_ui->menuSubtitle->addAction(action);
            action->setActionGroup(m_actionGroups.subtitle);
            m_actionGroups.subtitleActions.append(action);

            /* Secondary Subtitles */
            QAction *actionSubTwo = createTrackAction(track);
            m_ui->menuSubtitleTwo->addAction(actionSubTwo);
            actionSubTwo->setActionGroup(m_actionGroups.subtitleTwo);
            m_actionGroups.subtitleTwoActions.append(actionSubTwo);

            if (track->selected && track->mainSelection == 0)
            {
                action->setChecked(true);
                m_ui->actionSubtitleNone->setChecked(false);
                actionSubTwo->setEnabled(false);
            }
            else if (track->selected && track->mainSelection == 1)
            {
                actionSubTwo->setChecked(true);
                m_ui->actionSubtitleTwoNone->setChecked(false);
                action->setEnabled(false);
            }

            connect(action, &QAction::toggled, action,
                [=] (bool checked) {
                    if (checked)
                    {
                        setSubtitleTrack(id);
                        m_actionGroups.subtitleTwoActions[id - 1]
                            ->setEnabled(false);
                    }
                    else
                    {
                        m_actionGroups.subtitleTwoActions[id - 1]
                            ->setEnabled(true);
                    }
                }
            );

            connect(actionSubTwo, &QAction::toggled, actionSubTwo,
                [=] (bool checked) {
                    if (checked)
                    {
                        setSecondarySubtitleTrack(id);
                        m_actionGroups.subtitleActions[id - 1]
                            ->setEnabled(false);
                    }
                    else
                    {
                        m_actionGroups.subtitleActions[id - 1]
                            ->setEnabled(true);
                    }
                }
            );
            break;
        }
        }
    }

    m_actionGroups.audio->blockSignals(false);
    m_actionGroups.video->blockSignals(false);
    m_actionGroups.subtitle->blockSignals(false);
    m_actionGroups.subtitleTwo->blockSignals(false);
}

void PlayerMenu::setAudioTrack(const uint64_t id)
{
    if (id)
    {
        m_player->setAudioTrack(id);
    }
    else
    {
        m_player->disableAudio();
    }
}

void PlayerMenu::setVideoTrack(const uint64_t id)
{
    if (id)
    {
        m_player->setVideoTrack(id);
    }
    else
    {
        m_player->disableVideo();
    }
}

void PlayerMenu::setSubtitleTrack(const uint64_t id)
{
    if (id)
    {
        m_player->setSubtitleTrack(id);
    }
    else
    {
        m_player->disableSubtitles();
    }
}

void PlayerMenu::setSecondarySubtitleTrack(const uint64_t id)
{
    if (id)
    {
        m_player->setSubtitleTwoTrack(id);
    }
    else
    {
        m_player->disableSubtitleTwo();
    }
}

void PlayerMenu::updateAudioAction(const uint64_t id)
{
    if (!id)
    {
        m_ui->actionAudioNone->setChecked(true);
    }
    else if(id <= m_actionGroups.audioActions.size())
    {
        m_actionGroups.audioActions[id - 1]->setChecked(true);
    }
}

void PlayerMenu::updateVideoAction(const uint64_t id)
{
    if (!id)
    {
        m_ui->actionVideoNone->setChecked(true);
    }
    else if(id <= m_actionGroups.videoActions.size())
    {
        m_actionGroups.videoActions[id - 1]->setChecked(true);
    }
}

void PlayerMenu::updateSubtitleAction(const uint64_t id)
{
    if (!id)
    {
        m_ui->actionSubtitleNone->setChecked(true);
    }
    else if(id <= m_actionGroups.subtitleActions.size())
    {
        m_actionGroups.subtitleActions[id - 1]->setChecked(true);
    }
}

void PlayerMenu::updateSecondarySubtitleAction(const uint64_t id)
{
    if (!id)
    {
        m_ui->actionSubtitleTwoNone->setChecked(true);
    }
    else if(id <= m_actionGroups.subtitleTwoActions.size())
    {
        m_actionGroups.subtitleTwoActions[id - 1]->setChecked(true);
    }
}

/* End Track Methods */
/* Begin Media Openers */

void PlayerMenu::openFile()
{
    QList<QUrl> files = QFileDialog::getOpenFileUrls(0, "Open a video");
    if (!files.isEmpty())
    {
        m_player->stop();
        m_player->open(files);
        m_player->play();
    }
}

void PlayerMenu::openUrl()
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

void PlayerMenu::openSubtitle()
{
    QString file = QFileDialog::getOpenFileName(0, "Open Subtitle");
    if (!file.isEmpty())
    {
        m_player->addSubtitle(file);
    }
}

/* End Media Openers */
/* Begin Config Actions */

void PlayerMenu::openConfigFolder() const
{
    QDesktopServices::openUrl(
        QUrl::fromLocalFile(DirectoryUtils::getConfigDir())
    );
}

/* End Config Actions */
/* Begin Playback Actions */

void PlayerMenu::updateSubtitlePauseAction()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_BEHAVIOR);
    const bool newSubtitlePauseSetting = settings.value(
            SETTINGS_BEHAVIOR_SUBTITLE_PAUSE,
            SETTINGS_BEHAVIOR_SUBTITLE_PAUSE_DEFAULT
        ).toBool();
    settings.endGroup();

    m_ui->actionSubtitlePause->setChecked(newSubtitlePauseSetting);
}

void PlayerMenu::applySubtitlePauseSetting()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_BEHAVIOR);
    settings.setValue(
        SETTINGS_BEHAVIOR_SUBTITLE_PAUSE,
        m_ui->actionSubtitlePause->isChecked()
    );
    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->behaviorSettingsChanged();
}
/* End Playback Actions */
/* Begin Anki Profile Handler */

void PlayerMenu::updateAnkiProfileMenu()
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
    AnkiClient *anki = GlobalMediator::getGlobalMediator()->getAnkiClient();
    QStringList profiles = anki->getProfiles();
    std::sort(profiles.begin(), profiles.end());

    /* Add new profiles */
    for (const QString &profile : profiles)
    {
        QAction *profileAction = new QAction(this);
        profileAction->setText(profile);
        profileAction->setCheckable(true);
        profileAction->setChecked(anki->getProfile() == profile);
        m_ui->menuAnkiProfile->addAction(profileAction);
        profileAction->setActionGroup(m_actionGroupAnkiProfile);

        connect(profileAction, &QAction::triggered, profileAction,
            [=] (const bool checked) {
                if (checked)
                {
                    profileAction->blockSignals(true);
                    anki->setProfile(profileAction->text());
                    anki->writeChanges();
                    profileAction->blockSignals(false);
                }
            }
        );
    }

    /* Handle disabled Anki Integration */
    m_ui->menuAnkiProfile->menuAction()->setVisible(anki->isEnabled());
}

/* End Anki Profile Handler */
/* Begin Widget Handlers */

void PlayerMenu::handleToggleSearch()
{
    Q_EMIT GlobalMediator::getGlobalMediator()->
        requestSearchVisibility(m_ui->actionShowSearch->isChecked());
}

/* End Widget Handlers */
