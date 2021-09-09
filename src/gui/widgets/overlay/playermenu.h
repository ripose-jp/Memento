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

#ifndef PLAYERMENU_H
#define PLAYERMENU_H

#include <QWidget>

class QActionGroup;
class QAction;
class QMenu;

class PlayerAdapter;
struct Track;

namespace Ui
{
    class PlayerMenu;
}

/**
 * Class encapsulating the menubar. Has the added benefit of working with
 * autoFillBackground().
 */
class PlayerMenu : public QWidget
{
    Q_OBJECT

public:
    PlayerMenu(QWidget *parent = nullptr);
    ~PlayerMenu();

    /**
     * Returns if a menu in the menubar is currently open.
     * @return true if a menu is open, false otherwise.
     */
    bool menuOpen() const;

    /**
     * Returns if the menu is currently visible.
     * @return true if the menu has a height larger than 0, false otherwise.
     */
    bool menuVisible() const;

public Q_SLOTS:
    /**
     * Hides the menu without disabling key shortcuts.
     */
    void hideMenu();

    /**
     * Shows the menu without disabling key shortcuts.
     */
    void showMenu();

Q_SIGNALS:
    /**
     * Emitted when a menu is hidden.
     */
    void aboutToHide() const;

private Q_SLOTS:
    /**
     * Updates the menu with the currently available tracks.
     * @param tracks A list of the currently available tracks.
     */
    void setTracks(QList<const Track *> tracks);

    /**
     * Changes audio track to the one specified.
     * @param id The id of the audio track.
     */
    void setAudioTrack(const uint64_t id);

    /**
     * Changes video track to the one specified.
     * @param id The id of the video track.
     */
    void setVideoTrack(const uint64_t id);

    /**
     * Changes subtitle track to the one specified.
     * @param id The id of the subtitle track.
     */
    void setSubtitleTrack(const uint64_t id);

    /**
     * Changes secondary subtitle track to the one specified.
     * @param id The id of the secondary subtitle track.
     */
    void setSecondarySubtitleTrack(const uint64_t id);

    /**
     * Checks the audio QAction corresponding to the id.
     * @param id The id of the audio track. 0 is none.
     */
    void updateAudioAction(const uint64_t id = 0);

     /**
     * Checks the video QAction corresponding to the id.
     * @param id The id of the video track. 0 is none.
     */
    void updateVideoAction(const uint64_t id = 0);

    /**
     * Checks the subtitle QAction corresponding to the id.
     * @param id The id of the subtitle track. 0 is none.
     */
    void updateSubtitleAction(const uint64_t id = 0);

    /**
     * Checks the secondary subtitle QAction corresponding to the id.
     * @param id The id of the secondary subtitle track. 0 is none.
     */
    void updateSecondarySubtitleAction(const uint64_t id = 0);

    /**
     * Opens a file picker plays the selected files.
     */
    void openFile();

    /**
     * Opens a dialogue and plays the inputted URL.
     */
    void openUrl();

    /**
     * Opens a file dialog and adds the subtitle file.
     */
    void openSubtitle();

    /**
     * Opens the configuration directory in the native file browser.
     */
    void openConfigFolder() const;

    /**
     * Updates the values under Settings -> Anki Profile with up to date
     * information.
     */
    void updateAnkiProfileMenu();

private:
    /**
     * Clears all the tracks in the Audio, Video, and Subtitle menus.
     */
    void clearTracks();

    /**
     * Clears one set of tracks. Helper method for clearTracks().
     * @param actions       List of actions to remove.
     * @param menu          The menu the actions are currently in.
     * @param actionGroup   The action group the actions are members of.
     * @param actionDisable The disable actions. (e.g. m_ui->actionAudioNone)
     */
    void clearTrack(QList<QAction *> &actions,
                    QMenu            *menu,
                    QActionGroup     *actionGroup,
                    QAction          *actionDisable);

    /**
     * Creates an action for a track. Helper method to setTracks().
     * @param track The track to create an action for.
     * @return A QAction corresponding to the track. Belongs to the caller.
     */
    QAction *createTrackAction(const Track *track) const;

    Ui::PlayerMenu *m_ui;

    /* Contains all the information about menu items corresponding to tracks */
    struct ActionGroups
    {
        /* Action group for the tracks in the "Audio" menu */
        QActionGroup *audio;

        /* Action group for the tracks in the "Video" menu */
        QActionGroup *video;

        /* Action group for the tracks in the "Subtitle" menu */
        QActionGroup *subtitle;

        /* Action group for the tracks in the "Secondary Subtitle" menu */
        QActionGroup *subtitleTwo;

        /* List of audio QActions (exclusing "None") */
        QList<QAction *> audioActions;

        /* List of video QActions (exclusing "None") */
        QList<QAction *> videoActions;

        /* List of subtitle QActions (exclusing "None") */
        QList<QAction *> subtitleActions;

        /* List of secondary subtitle QActions (exclusing "None") */
        QList<QAction *> subtitleTwoActions;
    } m_actionGroups;

    /* Action group for Anki Integration profiles. */
    QActionGroup *m_actionGroupAnkiProfile;

    /* A saved pointer to the player */
    PlayerAdapter *m_player;
};

#endif // PLAYERMENU_H
