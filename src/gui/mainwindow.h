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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QActionGroup>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHash>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPair>
#include <QResizeEvent>
#include <QRunnable>
#include <QSpacerItem>
#include <QWheelEvent>

#include "../anki/ankiclient.h"
#include "../util/globalmediator.h"
#include "playeradapter.h"
#include "widgets/aboutwindow.h"
#include "widgets/definition/definitionwidget.h"
#include "widgets/settings/optionswindow.h"
#include "widgets/subtitlewidget.h"

namespace Ui
{
    class MainWindow;
}

/**
 * The MainWindow Memento.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public Q_SLOTS:
    /**
     * Set the fullscreen mode of the window.
     * Remembers if the windows was maximized or not.
     * @param value true if fullscreen, false otherwise
     */
    void setFullscreen(bool value);

protected:
    /**
     * Saves the the values of the MainWindow so they can be restored on the
     * next open. Quits the application.
     * @param event The close event.
     */
    void closeEvent(QCloseEvent *event) override;

    /**
     * Opens all media passed in as command line arguements.
     * Shows a message if no dictionaries are installed.
     * @param event The show event.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Captures all keypress events and sends them to the player.
     * @param event The event sent to the player.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * Used to close the current DefinitionWidget on a click elsewhere.
     * @param event The mouse event.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * Captures all wheel events and sends them to the player.
     * @param event The event to send to the player.
     */
    void wheelEvent(QWheelEvent *event) override;

    /**
     * Accepts drag events of type text/uri-list.
     * @param event The drag event.
     */
    void dragEnterEvent(QDragEnterEvent *event) override;

    /**
     * Sends dropped URIs over to the player to open.
     * @param event The drop event.
     */
    void dropEvent(QDropEvent *event) override;

    /**
     * Repositions the subtitle search and removes the DefinitionWidget.
     * @param event The resize event.
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * Emits a signal when the window focus is changed.
     * @param event The change event.
     */
    void changeEvent(QEvent *event) override;

private Q_SLOTS:
    /**
     * Opens a file picker plays the selected files.
     */
    void open();

    /**
     * Opens a dialogue and plays the inputted URL.
     */
    void openUrl();

    /**
     * Opens a file dialog and adds the subtitle file.
     */
    void openSubtitle();

    /**
     * Repositions the subtitles on player.
     */
    void repositionSubtitles();

    /**
     * Changes the subtitle scale. Saved in settings.
     * @param inc A value between -1.0 and 1.0 to increment the current scale
     *            by. If the final value is outside of 0.0 or 1.0, it is
     *            rounded.
     */
    void updateSubScale(const double inc);

    /**
     * Changes the subtitle position. Saved in settings.
     * @param inc A value between -1.0 and 1.0 to increment the position by. If
     *            the final value is outside of 0.0 or 1.0, it is rounded.
     */
    void moveSubtitles(const double inc);

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
     * Opens a new DefinitionWidget populated with the terms in the list.
     * @param terms A list of terms to display.
     */
    void setTerms(const QList<Term *> *terms);

    /**
     * Updates the values under Settings -> Anki Profile with up to date
     * information.
     */
    void updateAnkiProfileMenu();

    /**
     * Moves the DefinitionWidget to a position over the player relative to the
     * cursor.
     */
    void setDefinitionWidgetLocation();

    /**
     * Deletes the current DefinitionWidget if it exists.
     */
    void deleteDefinitionWidget();

    /**
     * Hides the player controls if the controls should be hidden.
     * Controls are only hidden if the player in fullscreened and the mouse is
     * not over the player controls or subtitle widget.
     */
    void hideControls();

    /**
     * Shows a critical QMessageBox. Used when an error has occurred.
     * This method is used to make sure that errors that occur on threads other
     * than the UI thread can show an error messages.
     * @param title The title of the window.
     * @param error The message to display.
     */
    void showErrorMessage(const QString title, const QString error) const;

    /**
     * Shows an information QMessageBox. Used when an error has occurred.
     * This method is used to make sure that errors that occur on threads other
     * than the UI thread can show an error messages.
     * @param title The title of the window.
     * @param error The message to display.
     */
    void showInfoMessage(const QString title, const QString error) const;

    /**
     * Updates the user interface Memento.
     * MainWindow is repsonsible for
     * - The global palette
     * - The splitter between the player and subtitle list
     * - The position of the subtitle search widget
     */
    void initTheme();

private:
    /**
     * Loads the last saved MainWindow settings. Currently size and maximized
     * state.
     */
    void initWindow();

    /**
     * Intializes the Audio, Video, and Subtitle track menus in the menu bar.
     */
    void initMenuBar();

    /**
     * Initializes the SubtitleWidget and its scaffolding.
     */
    void initSubtitles();

    /**
     * Intializes DefinitionWidget signals.
     */
    void initDefinitionWidget();

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

    /**
     * Returns if the cursor is over the player or not.
     * @return true if the cursor is not over the player controls or the
     *         DefinitonWidget, false otherwise.
     */
    inline bool isMouseOverPlayer();

    /* The object containing all of MainWindows widgets. Has ownership. */
    Ui::MainWindow *m_ui;

    /* A saved pointer to the GlobalMediator. Does not have ownership. */
    GlobalMediator *m_mediator;

    /* A saved pointer to the player adapter. Has ownership. */
    PlayerAdapter *m_player;

    /* A saved pointer to the current DefinitionWidget. Null if there is none.
     * Has ownership.
     */
    DefinitionWidget *m_definition;

    /* A saved pointer to the current AnkiClient. Has ownership. */
    AnkiClient *m_ankiClient;

    /* A saved pointer to the options window. Has ownership. */
    OptionsWindow *m_optionsWindow;

    /* A saved pointer to the AboutWindow. Has ownership. */
    AboutWindow *m_aboutWindow;

    /* Contains all the items needed for positioning items in fixed places over
     * over the player.
     */
    struct SubtitleUi
    {
        /* Top level layout over the player. Player has ownership */
        QVBoxLayout *layoutPlayerOverlay;

        /* Used for centering the subtitle widget horizontally. Player has
         * ownership.
         */
        QHBoxLayout *layoutSubtitle;

        /* Player has ownership. */
        SubtitleWidget *subtitleWidget;

        /* Used for spacing the SubtitleWidget vertically from the bottom.
         * A generic QWidget is used because spacers cannot be given a fixed
         * size.
         */
        QWidget *spacerWidget;

        /* A value from 0.0 to 1.0. Describes what percentage of the bottom of
         * the player's total height should be below the bottom of the
         * SubtitleWidget.
         */
        double offsetPercent;
    } m_subtitle;

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

    /* Saved value for determining if the MainWindow is maximized. Used for
     * restoring window state when leaving fullscreen.
     */
    bool m_maximized;
};

#endif // MAINWINDOW_H
