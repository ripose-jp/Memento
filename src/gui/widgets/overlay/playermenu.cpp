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

#include <QActionGroup>
#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGuiApplication>
#include <QInputDialog>
#include <QList>
#include <QProxyStyle>
#include <QSettings>

#include "anki/ankiclient.h"
#include "state/context.h"
#include "util/constants.h"
#include "util/utils.h"

/* Begin MenuBar Proxy Style */

/**
 * A proxy style preventing the menubar from grabbing focus when Alt is pressed.
 */
class MenuStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;

    int styleHint(
        StyleHint hint,
        const QStyleOption *opt,
        const QWidget *widget,
        QStyleHintReturn *returnData) const
    {
        if (hint == QStyle::SH_MenuBar_AltKeyNavigation)
        {
            return 0;
        }
        return QProxyStyle::styleHint(hint, opt, widget, returnData);
    }
};

/* End MenuBar Proxy Style */
/* Begin Constructor/Destructor */

PlayerMenu::PlayerMenu(Context *context, QWidget *parent) :
    QWidget(parent),
    m_ui(std::make_unique<Ui::PlayerMenu>()),
    m_context(std::move(context))
{
    m_ui->setupUi(this);

    updateSubtitlePauseAction();

    /* Anki */
    m_actionGroupAnkiProfile = new QActionGroup(this);
    updateAnkiProfileMenu();

    m_actionGroups.audio = new QActionGroup(this);
    m_actionGroups.subtitle = new QActionGroup(this);
    m_actionGroups.subtitleTwo = new QActionGroup(this);

    m_ui->actionAudioNone->setActionGroup(m_actionGroups.audio);
    m_ui->actionSubtitleNone->setActionGroup(m_actionGroups.subtitle);
    m_ui->actionSubtitleTwoNone->setActionGroup(m_actionGroups.subtitleTwo);

    loadRecentFiles();

#ifdef OCR_SUPPORT
    initOCRSettings();
#else
    m_ui->actionOCRMode->setVisible(false);
#endif

    /* Open Signals */
    connect(
        m_ui->actionOpen, &QAction::triggered,
        this, &PlayerMenu::openFile,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionOpenUrl, &QAction::triggered,
        this, &PlayerMenu::openUrl,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionAddSubtitle, &QAction::triggered,
        this, &PlayerMenu::openSubtitle,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionUpdate, &QAction::triggered,
        this, [this] { NetworkUtils::checkForUpdates(m_context); },
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionClearRecents, &QAction::triggered,
        this, &PlayerMenu::clearRecentFiles,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::playerFileChanged,
        this, &PlayerMenu::addRecentFile,
        Qt::QueuedConnection
    );

    /* Track Signals */
    connect(
        m_context, &Context::playerTracksChanged,
        this, &PlayerMenu::setTracks
    );

    connect(
        m_ui->actionAudioNone, &QAction::toggled, this,
        [this] (bool checked) { if (checked) setAudioTrack(0); }
    );
    connect(
        m_ui->actionSubtitleNone, &QAction::toggled, this,
        [this] (bool checked) { if (checked) setSubtitleTrack(0); }
    );
    connect(
        m_ui->actionSubtitleTwoNone, &QAction::toggled, this,
        [this] (bool checked) { if (checked) setSecondarySubtitleTrack(0); }
    );

    connect(
        m_context, &Context::playerAudioTrackChanged,
        this, &PlayerMenu::updateAudioAction
    );
    connect(
        m_context, &Context::playerSubtitleTrackChanged,
        this, &PlayerMenu::updateSubtitleAction
    );
    connect(
        m_context, &Context::playerSecondSubtitleTrackChanged,
        this, &PlayerMenu::updateSecondarySubtitleAction
    );

    connect(
        m_context, &Context::playerAudioDisabled,
        this, [this] { updateAudioAction(); }
    );
    connect(
        m_context, &Context::playerSubtitlesDisabled,
        this, [this] { updateSubtitleAction(); }
    );
    connect(
        m_context, &Context::playerSecondSubtitlesDisabled,
        this, [this] { updateSecondarySubtitleAction(); }
    );

    connect(
        m_context, &Context::playerFileLoaded, this,
        [this]
        {
            QList<Track> tracks = m_context->getPlayerAdapter()->getTracks();
            setTracks(tracks);
        }
    );

    /* Option Signals */
    connect(
        m_ui->actionSubtitlePause, &QAction::toggled,
        this, &PlayerMenu::applySubtitlePauseSetting,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionShowSearch, &QAction::triggered,
        this, &PlayerMenu::handleToggleSearch,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionOCRMode, &QAction::triggered,
        m_context, &Context::menuEnterOCRMode,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionShowSubtitleList, &QAction::triggered,
        this, &PlayerMenu::handleToggleSubtitleList,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionOptions, &QAction::triggered,
        m_context, &Context::menuShowOptions,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionAbout, &QAction::triggered,
        m_context, &Context::menuShowAbout,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionOpenConfig, &QAction::triggered,
        this, &PlayerMenu::openConfigFolder,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::behaviorSettingsChanged,
        this, &PlayerMenu::updateSubtitlePauseAction,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::ankiSettingsChanged,
        this, &PlayerMenu::updateAnkiProfileMenu,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionSubVis, &QAction::toggled,
        m_context, &Context::menuSubtitleVisibilityToggled,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionIncreaseSize, &QAction::triggered,
        m_context, &Context::menuSubtitleSizeIncrease,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionDecreaseSize, &QAction::triggered,
        m_context, &Context::menuSubtitleSizeDecrease,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionMoveUp, &QAction::triggered,
        m_context, &Context::menuSubtitlesMoveUp,
        Qt::QueuedConnection
    );
    connect(
        m_ui->actionMoveDown, &QAction::triggered,
        m_context, &Context::menuSubtitlesMoveDown,
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::searchWidgetHidden,
        m_ui->actionShowSearch,
        [this] { m_ui->actionShowSearch->setChecked(false); },
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::searchWidgetShown,
        m_ui->actionShowSearch,
        [this] { m_ui->actionShowSearch->setChecked(true); },
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::subtitleListHidden,
        m_ui->actionShowSearch,
        [this] { m_ui->actionShowSubtitleList->setChecked(false); },
        Qt::QueuedConnection
    );
    connect(
        m_context, &Context::subtitleListShown,
        m_ui->actionShowSearch,
        [this] { m_ui->actionShowSubtitleList->setChecked(true); },
        Qt::QueuedConnection
    );

    /* aboutToHide Signals */
    QList<QMenu *> menus = m_ui->menubar->findChildren<QMenu *>();
    for (const QMenu *menu : menus)
    {
        connect(
            menu, &QMenu::aboutToHide, this, &PlayerMenu::aboutToHide,
            Qt::QueuedConnection
        );
    }

#ifdef OCR_SUPPORT
    connect(
        m_context, &Context::ocrSettingsChanged,
        this, &PlayerMenu::initOCRSettings
    );
#endif // OCR_SUPPORT
}

PlayerMenu::~PlayerMenu()
{
    clearTracks();
    saveRecentFiles();
}

/* End Constructor/Destructor */
/* Begin Initializers */

#ifdef OCR_SUPPORT
void PlayerMenu::initOCRSettings()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::OCR::GROUP);

    bool enabled = settings.value(
        Constants::Settings::OCR::ENABLED,
        Constants::Settings::OCR::ENABLED_DEFAULT
    ).toBool();
    m_ui->actionOCRMode->setVisible(enabled);

    settings.endGroup();
}
#endif // OCR_SUPPORT

/* End Initializers */
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
    QList<QMenu *> menus = m_ui->menubar->findChildren<QMenu *>();
    for (QMenu *menu : menus)
    {
        menu->close();
    }
}

void PlayerMenu::showMenu()
{
    setMinimumHeight(0);
    setMaximumHeight(QWIDGETSIZE_MAX);
}

/* End Status Methods */
/* Begin Recent File Methods */

void PlayerMenu::loadRecentFiles()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Recent::GROUP);

    m_recentFiles = settings.value(
        Constants::Settings::Recent::FILES
    ).toStringList();

    settings.endGroup();

    refreshRecentFileMenu();
}

void PlayerMenu::saveRecentFiles()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Recent::GROUP);

    settings.setValue(Constants::Settings::Recent::FILES, m_recentFiles);

    settings.endGroup();
}

void PlayerMenu::addRecentFile(const QString &path)
{
    constexpr qsizetype MAX_RECENT_FILES = 10;

    m_recentFiles.removeAll(path);
    m_recentFiles.prepend(path);
    if (m_recentFiles.size() > MAX_RECENT_FILES)
    {
        m_recentFiles.pop_back();
    }

    refreshRecentFileMenu();
}

void PlayerMenu::clearRecentFiles()
{
    m_recentFiles.clear();
    refreshRecentFileMenu();
}

void PlayerMenu::refreshRecentFileMenu()
{
    m_ui->menuRecentFiles->removeAction(m_ui->actionClearRecents);
    m_ui->menuRecentFiles->clear();

    for (const QString &path : m_recentFiles)
    {
        QAction *actionRecentFile = m_ui->menuRecentFiles->addAction(path);
        connect(
            actionRecentFile, &QAction::triggered,
            this, [this, path] { m_context->getPlayerAdapter()->open(path); },
            Qt::QueuedConnection
        );
    }

    m_ui->menuRecentFiles->addSeparator();
    m_ui->menuRecentFiles->addAction(m_ui->actionClearRecents);

    m_ui->menuRecentFiles->setEnabled(!m_recentFiles.isEmpty());
}

/* End Recent File Methods */
/* Begin Track Methods */

void PlayerMenu::clearTrack(
    QList<QAction *> &actions,
    QMenu *menu,
    QActionGroup *actionGroup,
    QAction *actionDisable)
{
    for (QAction *action : actions)
    {
        menu->removeAction(action);
        actionGroup->removeAction(action);
        action->deleteLater();
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

#define MAX_LENGTH 45

QAction *PlayerMenu::createTrackAction(const Track &track) const
{
    QAction *action = new QAction;
    action->setCheckable(true);

    QString actionText = "Track " + QString::number(track.id);
    if (!track.lang.isEmpty())
    {
        actionText += " [" + track.lang + "]";
    }
    if (!track.title.isEmpty())
    {
        actionText += " - " + track.title;
    }
    if (actionText.length() > MAX_LENGTH)
    {
        actionText = actionText.left(MAX_LENGTH);
        actionText += "...";
    }
    action->setText(actionText);

    return action;
}

#undef MAX_LENGTH

void PlayerMenu::setTracks(const QList<Track> &tracks)
{
    clearTracks();

    m_actionGroups.audio->blockSignals(true);
    m_actionGroups.subtitle->blockSignals(true);
    m_actionGroups.subtitleTwo->blockSignals(true);

    for (const Track &track : tracks)
    {
        QAction *action = createTrackAction(track);
        const int64_t id = track.id;
        switch (track.type)
        {
        case Track::Type::audio:
            m_ui->menuAudio->addAction(action);
            action->setActionGroup(m_actionGroups.audio);
            m_actionGroups.audioActions.append(action);

            if (track.selected)
            {
                action->setChecked(true);
                m_ui->actionAudioNone->setChecked(false);
            }

            connect(action, &QAction::toggled, action,
                [this, id] (bool checked)
                {
                    if (checked)
                    {
                        setAudioTrack(id);
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

            if (track.selected && track.mainSelection == 0)
            {
                action->setChecked(true);
                m_ui->actionSubtitleNone->setChecked(false);
                actionSubTwo->setEnabled(false);
            }
            else if (track.selected && track.mainSelection == 1)
            {
                actionSubTwo->setChecked(true);
                m_ui->actionSubtitleTwoNone->setChecked(false);
                action->setEnabled(false);
            }

            connect(action, &QAction::toggled, action,
                [this, id] (bool checked)
                {
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
                [this, id] (bool checked)
                {
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

        case Track::Type::video:
        default:
            action->deleteLater();
            break;
        }
    }

    m_actionGroups.audio->blockSignals(false);
    m_actionGroups.subtitle->blockSignals(false);
    m_actionGroups.subtitleTwo->blockSignals(false);
}

void PlayerMenu::setAudioTrack(const int id)
{
    if (id)
    {
        m_context->getPlayerAdapter()->setAudioTrack(id);
    }
    else
    {
        m_context->getPlayerAdapter()->disableAudio();
    }
}

void PlayerMenu::setSubtitleTrack(const int id)
{
    if (id)
    {
        m_context->getPlayerAdapter()->setSubtitleTrack(id);
    }
    else
    {
        m_context->getPlayerAdapter()->disableSubtitles();
    }
}

void PlayerMenu::setSecondarySubtitleTrack(const int id)
{
    if (id)
    {
        m_context->getPlayerAdapter()->setSubtitleTwoTrack(id);
    }
    else
    {
        m_context->getPlayerAdapter()->disableSubtitleTwo();
    }
}

void PlayerMenu::updateAudioAction(const int id)
{
    if (!id)
    {
        m_ui->actionAudioNone->setChecked(true);
    }
    else if (id <= m_actionGroups.audioActions.size())
    {
        m_actionGroups.audioActions[id - 1]->setChecked(true);
    }
}

void PlayerMenu::updateSubtitleAction(const int id)
{
    if (!id)
    {
        m_ui->actionSubtitleNone->setChecked(true);
    }
    else if (id <= m_actionGroups.subtitleActions.size())
    {
        m_actionGroups.subtitleActions[id - 1]->setChecked(true);
    }
}

void PlayerMenu::updateSecondarySubtitleAction(const int id)
{
    if (!id)
    {
        m_ui->actionSubtitleTwoNone->setChecked(true);
    }
    else if (id <= m_actionGroups.subtitleTwoActions.size())
    {
        m_actionGroups.subtitleTwoActions[id - 1]->setChecked(true);
    }
}

/* End Track Methods */
/* Begin Media Openers */

QString PlayerMenu::getOpenFilePath() const
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Behavior::GROUP);

    Constants::FileOpenDirectory type =
        static_cast<Constants::FileOpenDirectory>(
            settings.value(
                Constants::Settings::Behavior::FILE_OPEN_DIR,
                static_cast<int>(
                    Constants::Settings::Behavior::FILE_OPEN_DIR_DEFAULT
                )
            ).toInt()
        );
    if (type == Constants::FileOpenDirectory::Custom)
    {
        return settings.value(
            Constants::Settings::Behavior::FILE_OPEN_CUSTOM,
            Constants::Settings::Behavior::FILE_OPEN_CUSTOM_DEFAULT
        ).toString();
    }
    return DirectoryUtils::getFileOpenDirectory(type);
}

void PlayerMenu::openFile()
{
    QStringList files = QFileDialog::getOpenFileNames(
        window(), "Open File(s)", getOpenFilePath()
    );
    if (!files.isEmpty())
    {
        PlayerAdapter *player = m_context->getPlayerAdapter();
        player->stop();
        player->open(files);
        player->play();
    }
}

void PlayerMenu::openUrl()
{
    QInputDialog dialog(window(), Qt::Dialog);
    dialog.setWindowTitle("Open Stream");
    dialog.setLabelText("Enter URL");
    dialog.setTextValue(QGuiApplication::clipboard()->text());
    dialog.setStyleSheet("QLabel { min-width: 500px; }");
    int     res = dialog.exec();
    QString url = dialog.textValue();

    if (res == QDialog::Accepted && !url.isEmpty())
    {
        PlayerAdapter *player = m_context->getPlayerAdapter();
        player->stop();
        player->open(url);
        player->play();
    }
}

void PlayerMenu::openSubtitle()
{
    QString file = QFileDialog::getOpenFileName(
        window(), "Open Subtitle", getOpenFilePath(),
        "Subtitle Files (*.ass *.srt *.vtt *.utf *.utf8 *.utf-8 *.idx *.sub "
        "*.rt *.ssa *.mks *.sup *.scc *.smi *.lrc *.pgs);;All Files (*)"
    );
    if (!file.isEmpty())
    {
        m_context->getPlayerAdapter()->addSubtitle(file);
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
    settings.beginGroup(Constants::Settings::Behavior::GROUP);
    const bool newSubtitlePauseSetting = settings.value(
            Constants::Settings::Behavior::SUBTITLE_PAUSE,
            Constants::Settings::Behavior::SUBTITLE_PAUSE_DEFAULT
        ).toBool();
    settings.endGroup();

    m_ui->actionSubtitlePause->setChecked(newSubtitlePauseSetting);
}

void PlayerMenu::applySubtitlePauseSetting()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Behavior::GROUP);
    settings.setValue(
        Constants::Settings::Behavior::SUBTITLE_PAUSE,
        m_ui->actionSubtitlePause->isChecked()
    );
    settings.endGroup();

    emit m_context->behaviorSettingsChanged();
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
    AnkiClient *anki = m_context->getAnkiClient();
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
    emit m_context->requestSearchVisibility(
        m_ui->actionShowSearch->isChecked()
    );
}

void PlayerMenu::handleToggleSubtitleList()
{
    emit m_context->requestSubtitleListVisibility(
        m_ui->actionShowSubtitleList->isChecked()
    );
}

/* End Widget Handlers */
