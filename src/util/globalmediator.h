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

class Dictionary;
class PlayerAdapter;
class AnkiClient;
class SubtitleListWidget;

class QKeyEvent;
class QWheelEvent;
class QMouseEvent;

struct Track;
struct Term;

class GlobalMediator : public QObject
{
    Q_OBJECT

public:
    ~GlobalMediator() {}

    static GlobalMediator *createGlobalMedaitor();
    static GlobalMediator *getGlobalMediator();

    Dictionary         *getDictionary()         const;
    PlayerAdapter      *getPlayerAdapter()      const;
    AnkiClient         *getAnkiClient()         const;
    SubtitleListWidget *getSubtitleListWidget() const;

    /* Mediator does not take ownership */
    GlobalMediator *setDictionary   (Dictionary         *dictionary);
    GlobalMediator *setPlayerAdapter(PlayerAdapter      *player);
    GlobalMediator *setAnkiClient   (AnkiClient         *client);
    GlobalMediator *setSubtitleList (SubtitleListWidget *subList);

Q_SIGNALS:
    /* Message Box Signals */
    void showInformation(QString title, QString content) const;
    void showCritical   (QString title, QString content) const;

    /* Interrupts */
    void keyPressed(const QKeyEvent   *event) const;
    void wheelMoved(const QWheelEvent *event) const;

    /* Player State Changes */
    void playerTracksChanged(QList<const Track *> tracks) const;

    void playerAudioTrackChanged         (const int64_t id) const;
    void playerVideoTrackChanged         (const int64_t id) const;
    void playerSubtitleTrackChanged      (const int64_t id) const;
    void playerSecondSubtitleTrackChanged(const int64_t id) const;

    void playerAudioDisabled()           const;
    void playerVideoDisabled()           const;
    void playerSubtitlesDisabled()       const;
    void playerSecondSubtitlesDisabled() const;

    void playerSubtitleChanged  (QString        subtitle, 
                                 const double   start, 
                                 const double   end, 
                                 const double   delay)  const;
    void playerDurationChanged  (const double   value)  const;
    void playerPositionChanged  (const double   value)  const;
    void playerPauseStateChanged(const bool     paused) const;
    void playerFullscreenChanged(const bool     full)   const;
    void playerVolumeChanged    (const int64_t  value)  const;
    void playerTitleChanged     (QString        title)  const;
    void playerFileChanged      (QString        path)   const;

    void playerCursorHidden()                 const;
    void playerMouseMoved(QMouseEvent *event) const;
    void playerClosed()                       const;

    /* Settings Signals */
    void ankiSettingsChanged()      const;
    void searchSettingsChanged()    const;
    void interfaceSettingsChanged() const;

    /* Player Control Signals */
    void controlsPlay()         const;
    void controlsPause()        const;
    void controlsSeekForward()  const;
    void controlsSeekBackward() const;
    void controlsSkipForward()  const;
    void controlsSkipBackward() const;

    void controlsSubtitleListToggled() const;

    void controlsPositionChanged  (const int  value) const;
    void controlsVolumeChanged    (const int  value) const;
    void controlsFullscreenChanged(const bool value) const;

    /* Subtitle Widget Signals */
    void termsChanged(const QList<Term *> *terms) const;
    void subtitleExpired() const;

    /* Definition Signals */
    void definitionsShown()        const;
    void definitionsHidden()       const;

    /* Dictionary Signals */
    void dictionaryAdded() const;

    /* Subtitle List Widget */
    void subtitleListHidden();
    void subtitleListShown();

    /* Request Changes */
    void requestDefinitionDelete() const;
    void requestFullscreenResize() const;
    void requestThemeRefresh()     const;

private:
    inline static GlobalMediator *m_mediator = nullptr;

    /* Mediator does not take ownership */
    Dictionary         *m_dictionary;
    AnkiClient         *m_ankiClient;
    PlayerAdapter      *m_player;
    SubtitleListWidget *m_subList;

    GlobalMediator(QObject *parent = nullptr);
};

#endif // GLOBALMEDIATOR_H