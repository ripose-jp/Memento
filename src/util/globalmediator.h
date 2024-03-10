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

#ifndef GLOBALMEDIATOR_H
#define GLOBALMEDIATOR_H

#include <QObject>

#include <QSharedPointer>

#include "player/track.h"

class AnkiClient;
class AudioPlayer;
class Dictionary;
class PlayerAdapter;
class QWidget;
class SubtitleListWidget;

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;

struct Term;
typedef QSharedPointer<Term> SharedTerm;
typedef QSharedPointer<QList<SharedTerm>> SharedTermList;

struct Kanji;
typedef QSharedPointer<Kanji> SharedKanji;

/**
 * A mediator that other objects can tap in to in order to send and receive
 * signals without having to know about eachother. Also contains pointers
 * to several shared objects.
 */
class GlobalMediator : public QObject
{
    Q_OBJECT

public:
    ~GlobalMediator() {}

    /**
     * Unconditionally creates a new GlobalMediator that can be accessed by
     * getGlobalMediator. This should only be called once, probably in main.
     * @return The created GlobalMediator.
     */
    static GlobalMediator *createGlobalMediator();

    /**
     * Gets the shared GlobalMediator.
     * @return The GlobalMediator, nullptr if it doesn't exist.
     */
    static GlobalMediator *getGlobalMediator();

    /**
     * Gets the AnkiClient object used for interacting with Anki.
     * @return The AnkiClient object, nullptr if it doesn't exist.
     */
    AnkiClient *getAnkiClient() const;

    /**
     * Gets the AudioPlayer object used for playing audio files from URLs.
     * @return The AudioPlayer object, nullptr if it doesn't exist.
     */
    AudioPlayer *getAudioPlayer() const;

    /**
     * Gets the shared Dictionary object for accessing the dictionary database.
     * @return The Dictionary object, nullptr if it doesn't exist.
     */
    Dictionary *getDictionary() const;

    /**
     * Gets the shared PlayerAdapter object for accessing the player.
     * @return The PlayerAdapter object, nullptr if it doesn't exist.
     */
    PlayerAdapter *getPlayerAdapter() const;

    /**
     * Gets the PlayerWidget for accessing general information about the widget.
     * DO NOT CAST THIS.
     * @return The PlayerWidget object, nullptr if it doesn't exist.
     */
    QWidget *getPlayerWidget() const;

    /**
     * Gets the SubtitleListWidget for accessing general information it has.
     * @return The SubtitleListWidget object, nullptr if it doesn't exist.
     */
    SubtitleListWidget *getSubtitleListWidget() const;

    /**
     * Sets the shared AnkiClient. Does not take ownership.
     * @param client The shared AnkiClient.
     * @return The shared GlobalMediator, nullptr if it doesn't exist.
     */
    GlobalMediator *setAnkiClient(AnkiClient *client);

    /**
     * Sets the shared AudioPlayer. Does not take ownership.
     * @param audioPlayer The shared AudioPlayer.
     * @return The shared GlobalMediator, nullptr if it doesn't exist.
     */
    GlobalMediator *setAudioPlayer(AudioPlayer *audioPlayer);

    /**
     * Sets the shared Dictionary. Does not take ownership.
     * @param dictionary The shared Dictionary.
     * @return The shared GlobalMediator, nullptr if it doesn't exist.
     */
    GlobalMediator *setDictionary(Dictionary *dictionary);

    /**
     * Sets the shared PlayerAdapter. Does not take ownership.
     * @param player The shared PlayerAdapter.
     * @return The shared GlobalMediator, nullptr if it doesn't exist.
     */
    GlobalMediator *setPlayerAdapter(PlayerAdapter *player);

    /**
     * Sets the shared Player widget. Does not take ownership.
     * @param widget The shared widget displaying the player.
     * @return The shared GlobalMediator, nullptr if it doesn't exist.
     */
    GlobalMediator *setPlayerWidget (QWidget *widget);

    /**
     * Sets the shared subtitle list. Does not take ownership.
     * @param player The shared subtitle list.
     * @return The shared GlobalMediator, nullptr if it doesn't exist.
     */
    GlobalMediator *setSubtitleList(SubtitleListWidget *subList);

Q_SIGNALS:
    /* Begin Dialog Boxes */

    /**
     * Shows an information dialog box.
     * @param title   The title of the dialog box.
     * @param content The content of the dialog box.
     */
    void showInformation(QString title, QString content) const;

    /**
     * Shows a critical dialog box.
     * @param title   The title of the dialog box.
     * @param content The content of the dialog box.
     */
    void showCritical(QString title, QString content) const;

    /* End Dialog Boxes */
    /* Begin Interrupts */

    /**
     * Emitted when MainWindow detects a keypress.
     * @param event The key press event.
     */
    void keyPressed(QKeyEvent *event) const;

    /**
     * Emitted when MainWindow detects a mouse wheel movement.
     * @param event The mouse wheel event.
     */
    void wheelMoved(const QWheelEvent *event) const;

    /**
     * Emitted when the window focus changes.
     * @param inFocus true if the window is in focus, false otherwise.
     */
    void windowFocusChanged(bool inFocus) const;

    /* End Interrupts */
    /* Begin Player State Changes */

    /**
     * Emitted when the player tracks change.
     * @param tracks A list of player tracks. All tracks are deleted when
     *               callees return.
     */
    void playerTracksChanged(const QList<const Track *> &tracks) const;

    /**
     * Emitted when the player chapters change.
     * @param chapters A list of the chapter start times in seconds.
     */
    void playerChaptersChanged(QList<double> chapters) const;

    /**
     * Emitted when the current audio track is changed.
     * @param id The id of the audio track.
     */
    void playerAudioTrackChanged(const int64_t id) const;

    /**
     * Emitted when the current video track is changed.
     * @param id The id of the video track.
     */
    void playerVideoTrackChanged(const int64_t id) const;

    /**
     * Emitted when the current subtitle track is changed.
     * @param id The id of the subtitle track.
     */
    void playerSubtitleTrackChanged(const int64_t id) const;

    /**
     * Emitted when the current secondary subtitle track is changed.
     * @param id The id of the secondary subtitle track.
     */
    void playerSecondSubtitleTrackChanged(const int64_t id) const;

    /**
     * Emitted when the player's audio is disabled.
     */
    void playerAudioDisabled() const;

    /**
     * Emitted when the player's video is disabled.
     */
    void playerVideoDisabled() const;

    /**
     * Emitted when the player's subtitles are disabled.
     */
    void playerSubtitlesDisabled() const;

    /**
     * Emitted when the player's secondary subtitle are disabled.
     */
    void playerSecondSubtitlesDisabled() const;

    /**
     * Emitted when the current subtitle changes.
     * @param subtitle The text of the subtitle.
     * @param start    The start time in seconds of the subtitle.
     * @param end      The end time in seconds of the subtitle.
     * @param delay    The delay in seconds of the subtitle.
     */
    void playerSubtitleChanged(const QString &subtitle,
                               const double start,
                               const double end,
                               const double delay) const;

    /**
     * Emitted when the current subtitle changes. This signal is unfiltered by
     * regex.
     * @param subtitle The text of the subtitle.
     * @param start    The start time in seconds of the subtitle.
     * @param end      The end time in seconds of the subtitle.
     * @param delay    The delay in seconds of the subtitle.
     */
    void playerSubtitleChangedRaw(const QString &subtitle,
                                  const double start,
                                  const double end,
                                  const double delay) const;

    /**
     * Emitted when the current secondary subtitle changes.
     * @param subtitle The text of the secondary subtitle.
     * @param start    The start time in seconds of the subtitle.
     * @param end      The end time in seconds of the subtitle.
     * @param delay    The delay in seconds of the subtitle.
     */
    void playerSecSubtitleChanged(const QString &subtitle,
                                  const double start,
                                  const double end,
                                  const double delay) const;

    /**
     * Emitted when the subtitle delay changes.
     * @param delay The subtitle delay in seconds.
     */
    void playerSubDelayChanged(const double delay) const;

    /**
     * Emitted when the secondary subtitle delay changes.
     * @param delay The secondary subtitle delay in seconds.
     */
    void playerSecSubDelayChanged(const double delay) const;

    /**
     * Emitted when the duration of the media being played changes.
     * @param value The duration in seconds.
     */
    void playerDurationChanged(const double value) const;

    /**
     * Emitted when the current position of the video is changed.
     * @param value The current position in seconds.
     */
    void playerPositionChanged(const double value) const;

    /**
     * Emitted when the player is paused/unpaused.
     * @param paused true if paused, false otherwise.
     */
    void playerPauseStateChanged(const bool paused) const;

    /**
     * Emitted when the fullscreen state of the player is changed.
     * @param full true if fullscreen, false otherwise.
     */
    void playerFullscreenChanged(const bool full) const;

    /**
     * Emitted when the maximum volume of the player changes.
     * @param value The maximum volume of the player.
     */
    void playerMaxVolumeChanged(const int64_t value) const;

    /**
     * Emitted when the volume of the player changes.
     * @param value The current volume of the player.
     */
    void playerVolumeChanged(const int64_t value) const;

    /**
     * Emitted when the title of the media changes.
     * @param title The title of the currently playing media, filename if none
     *              exists.
     */
    void playerTitleChanged(QString title) const;

    /**
     * Emitted when the file being played changes.
     * @param path The path of the file.
     */
    void playerFileChanged(QString path) const;

    /**
     * Emitted when the mouse is moved over the player.
     */
    void playerMouseMoved() const;

    /**
     * Emitted when the player loads a new file.
     * @param width  The width of the file.
     * @param height The height of the file.
     */
    void playerFileLoaded(int width, int height) const;

    /**
     * Emitted when the player terminates.
     */
    void playerClosed() const;

    /**
     * Emitted when the widget housing the player is resized.
     */
    void playerResized() const;

    /* End Player State Changes */
    /* Begin Settings Signals */

    /**
     * Emitted when Anki integration settings are changed.
     */
    void ankiSettingsChanged() const;

    /**
     * Emitted when Audio source settings are changed.
     */
    void audioSourceSettingsChanged() const;

    /**
     * Emitted when behavior settings are changed.
     */
    void behaviorSettingsChanged() const;

    /**
     * Emitted when search settings are changed.
     */
    void searchSettingsChanged() const;

    /**
     * Emitted when interface settings are changed.
     */
    void interfaceSettingsChanged() const;

    /**
     * Emitted when OCR settings are changed.
     */
    void ocrSettingsChanged() const;

    /* End Settings Signals */
    /* Begin Menu Signals */

    /**
     * Emitted when subtitle visibility should be toggled.
     * @param visible true if subtitles should be visible, false otherwise.
     */
    void menuSubtitleVisibilityToggled(bool visible) const;

    /**
     * Emitted when the action for increasing the size of the subtitles is
     * triggered.
     */
    void menuSubtitleSizeIncrease() const;

    /**
     * Emitted when the action for decreasing the size of the subtitles is
     * triggered.
     */
    void menuSubtitleSizeDecrease() const;

    /**
     * Emitted when the action for moving the subtitles up is triggered.
     */
    void menuSubtitlesMoveUp() const;

    /**
     * Emitted when the action for moving the subtitles down is triggered.
     */
    void menuSubtitlesMoveDown() const;

    /**
     * Emitted when the action for entering OCR mode is triggered.
     */
    void menuEnterOCRMode() const;

    /**
     * Emitted when the menu action for showing options is triggered.
     */
    void menuShowOptions() const;

    /**
     * Emitted when the menu action for showing the about page is triggered.
     */
    void menuShowAbout() const;

    /* End Menu Signals */
    /* Begin Player Control Signals */

    /**
     * Emitted when the play button is pressed.
     */
    void controlsPlay() const;

    /**
     * Emitted when the pause button is pressed.
     */
    void controlsPause() const;

    /**
     * Emitted when the seek forward button is pressed.
     */
    void controlsSeekForward() const;

    /**
     * Emitted when the seek backward button is pressed.
     */
    void controlsSeekBackward() const;

    /**
     * Emitted when the skip forward button is pressed.
     */
    void controlsSkipForward() const;

    /**
     * Emitted when the skip backward button is pressed.
     */
    void controlsSkipBackward() const;

    /**
     * Emitted when the controls are hidden.
     */
    void controlsHidden() const;

    /**
     * Emitted when the controls are shown.
     */
    void controlsShown() const;

    /**
     * Emitted when the subtitle list visibility button is pressed
     */
    void controlsSubtitleListToggled() const;

    /**
     * Emitted when the OCR button is pressed.
     */
    void controlsOCRToggled() const;

    /**
     * Emitted when the position of the control slider is changed.
     * @param value The position, in seconds, of the slider.
     */
    void controlsPositionChanged(const int value) const;

    /**
     * Emitted when the volume slider is changed.
     * @param value The value of the slider.
     */
    void controlsVolumeChanged(const int value) const;

    /**
     * Emitted when the fullscreen button is pressed.
     * @param value true if fullscreen, false otherwise.
     */
    void controlsFullscreenChanged(const bool value) const;

    /* End Player Control Signals */
    /* Begin Window Signals */

    /**
     * Emitted when the OSC state is cycled.
     */
    void windowOSCStateCycled() const;

    /* End Window Signals */
    /* Begin Subtitle Widget Signals */

    /**
     * Emitted when the list of terms changes.
     * @param terms The list of terms if found, nullptr otherwise.
     * @param kanji The kanji if found, nullptr otherwise.
     */
    void termsChanged(SharedTermList terms, SharedKanji kanji) const;

    /**
     * Emitted when the subtitle changes or passes its end time.
     */
    void subtitleExpired() const;

    /**
     * Emitted when the subtitle search is hidden.
     */
    void subtitleHidden() const;

    /* End Subtitle Widget Signals */
    /* Begin Definition Signals */

    /**
     * Emitted when the definition widget is hidden.
     */
    void definitionsHidden() const;

    /**
     * Emitted when the definition widget is shown.
     */
    void definitionsShown() const;

    /* End Definition Signals */
    /* Begin Subtitle List Signals */

    /**
     * Emitted when the subtitle list is hidden.
     */
    void subtitleListHidden() const;

    /**
     * Emitted when the subtitle list is shown.
     */
    void subtitleListShown() const;

    /* End Subtitle List Signals */
    /* Begin Search Widget Signals */

    /**
     * Emitted when the search widget is hidden.
     */
    void searchWidgetHidden() const;

    /**
     * Emitted when the search widget is shown.
     */
    void searchWidgetShown() const;

    /**
     * Emitted when it is requested for the search widget to execute a query.
     * @param query The text to search.
     */
    void searchWidgetRequest(const QString &query) const;

    /* End Search Widget Signals */
    /* Begin Dictionary Signals */

    /**
     * Emitted when a dictionary is added or removed.
     */
    void dictionariesChanged() const;

    /**
     * Emitted when the order of dictionaries is changed.
     */
    void dictionaryOrderChanged() const;

    /* End Dictionary Signals */
    /* Begin Request Changes */

    /**
     * Requests that player subtitle visibility change. May be ignored.
     * @param value true to show subtitles, false otherwise.
     */
    void requestSubtitleVisibility(const bool value) const;

    /**
     * Requests subtitle widget visibility be changed. May be ignored.
     * @param value true to show subtitles, false otherwise.
     */
    void requestSubtitleWidgetVisibility(const bool value) const;

    /**
     * Requests that the subtitle list visibility be changed.
     * @param value true to show the subtitle list, false to hide it.
     */
    void requestSubtitleListVisibility(const bool value) const;

    /**
     * Requests that the search widget visibility be changed.
     * @param value true to show the search widget, false to hide.
     */
    void requestSearchVisibility(const bool value) const;

    /**
     * Requests that the definiton widget be deleted.
     */
    void requestDefinitionDelete() const;

    /**
     * Requests a global theme refresh.
     */
    void requestThemeRefresh() const;

    /* End Request Changes */

private:
    /* The saved GlobalMediator. */
    inline static GlobalMediator *m_mediator = nullptr;

    /* Mediator does not take ownership */
    Dictionary         *m_dictionary;
    AnkiClient         *m_ankiClient;
    PlayerAdapter      *m_player;
    QWidget            *m_playerWidget;
    SubtitleListWidget *m_subList;
    AudioPlayer        *m_audioPlayer;

    GlobalMediator(QObject *parent = nullptr);
};

#endif // GLOBALMEDIATOR_H
