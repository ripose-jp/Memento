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

#ifndef TERMWIDGET_H
#define TERMWIDGET_H

#include <QWidget>

#include <memory>

#include <QLabel>
#include <QMenu>
#include <QMutex>
#include <QShortcut>

#include <qcoro/qcorotask.h>

#include "dict/expression.h"
#include "gui/widgets/common/flowlayout.h"
#include "gui/widgets/definition/definitionstate.h"
#include "state/context.h"

namespace Ui
{
    class TermWidget;
}

/**
 * Widget used for displaying Term information.
 */
class TermWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor for TermWidget.
     * @param context The application context.
     * @param term    The term to display.
     * @param state   The state of the parent of this widget.
     * @param parent  The parent of this widget.
     */
    TermWidget(
        Context *context,
        std::shared_ptr<const Term> term,
        const DefinitionState &state,
        QWidget *parent = nullptr);
    virtual ~TermWidget();

    /**
     * Sets this widget to be addable.
     * @param expression true if the expression is addable, false otherwise
     * @param reading    true if the reading is addable, false otherwise
     */
    void setAddable(bool expression, bool reading);

    /**
     * Deletes this term once all outstanding network requests are finished.
     */
    void deleteWhenReady();

public Q_SLOTS:
    /**
     * Plays the audio from the first available source.
     */
    void playAudio();

Q_SIGNALS:
    /**
     * Emitted when a kanji is searched by the user and found.
     * @param kanji The found kanji.
     */
    void kanjiSearched(std::shared_ptr<const Kanji> kanji);

    /**
     * Emitted when a recursive search is performed.
     * @param terms The list of terms that was searched. Can be nullptr.
     * @param kanji The kanji that was searched. Can be nullptr.
     */
    void contentSearched(SharedTermList terms, SharedKanji kanji);

    /**
     * Emitted when all audio sources are finished loading.
     */
    void audioSourcesLoaded() const;

    /**
     * A signal emitted when this widget is safe to delete.
     * This is used to prevent terms from not being added to Anki while network
     * requests are pending.
     */
    void safeToDelete() const;

private Q_SLOTS:
    /**
     * Toggles the visibility of the glossary.
     */
    void toggleGlossaryVisibility();

    /**
     * Toggles whether a word is added with kanji or kana.
     */
    void toggleExpressionKanji();

    /**
     * Adds the term belonging to this widget to Anki.
     */
    void addNote();

    /**
     * Adds the term belonging to this widget to Anki.
     * @param src The audio source to add.
     * @return An awaitable task.
     */
    QCoro::Task<void> addNote(const DefinitionState::AudioSource &src);

    /**
     * Opens an Anki window searching the current configured term card deck for
     * the expression of the current term.
     * @return An awaitable task.
     */
    QCoro::Task<void> searchAnki();

    /**
     * Plays audio for the term from the audio source.
     * @param src The audio source to play.
     */
    void playAudio(const DefinitionState::AudioSource &src);

    /**
     * Opens up a context menu with every playable audio source.
     * @param pos The position (relative to the audio button) to open the
     *            context menu.
     */
    void showPlayableAudioSources(const QPoint &pos);

    /**
     * Opens up a context menu with every addable audio source.
     * @param pos The position (relative to the add button) to open the
     *            context menu.
     */
    void showAddableAudioSources(const QPoint &pos);

    /**
     * Searches a clicked kanji to see if there is an entry.
     * @param ch The kanji to search.
     */
    void searchKanji(const QString &ch);

private:
    /**
     * Puts term information into the UI.
     * @param term            The term to populate the UI with.
     */
    void initUi(const Term &term);

    /**
     * Initializes a new term to add to Anki without audio information.
     * @return A term without audio source information.
     */
    [[nodiscard]]
    std::unique_ptr<Term> initAnkiTerm() const;

    /**
     * Loads all remote audio sources.
     */
    void loadAudioSources();

    /**
     * Populates an audio source menu that calls the handler with the specific
     * audio source.
     * @param menu    The menu to populate.
     * @param handler The handler to call.
     */
    void populateAudioSourceMenu(
        QMenu *menu,
        std::function<void(const DefinitionState::AudioSource &)> handler);

    /**
     * Returns the first valid file-type audio source.
     * @return A pointer to the first file-type audio source. nullptr if all are
     *         invalid;
     */
    DefinitionState::AudioSource *getFirstAudioSource();

    /* UI object containing all the widgets */
    std::unique_ptr<Ui::TermWidget> m_ui;

    /* The application context */
    Context *m_context = nullptr;

    /* The term this widget is displaying. */
    std::shared_ptr<const Term> m_term = nullptr;

    /* The state of the parent definition widget */
    const DefinitionState &m_state;

    /* This term is a copy m_term that is eventually passed to Anki to add.
     * If this term is not nullptr, TermWidget is not safe to delete without
     * losing this term. This is necessary because of the asynchronous nature
     * of JSON audio sources.
     */
    std::unique_ptr<Term> m_ankiTerm = nullptr;

    /* True if the TermWidget is safe to delete right now, false otherwise */
    bool m_safeToDelete = true;

    /* The list of current audio sources */
    std::vector<DefinitionState::AudioSource> m_sources;

    /* Protects concurrent accesses to the m_sources list */
    QMutex m_lockSources;

    /* The number of json audio sources that haven't been parsed */
    int m_jsonSources = 0;

    /* Lock JSON sources */
    QMutex m_lockJsonSources;

    /* Label used for displaying deconjugation information */
    QLabel *m_labelDeconj = nullptr;

    /* Layout used for holding term tags. */
    FlowLayout *m_layoutTermTags = nullptr;

    /* Layout used for holding frequency tags. */
    FlowLayout *m_layoutFreqTags = nullptr;

    /* Layout used hold pitch tags and pitches. */
    QVBoxLayout *m_layoutPitches = nullptr;

    /* Layout used for holding glossary entries. */
    QVBoxLayout *m_layoutGlossary = nullptr;

    /* The context menu for the add button */
    QMenu *m_menuAdd = nullptr;

    /* The context menu for the sound button */
    QMenu *m_menuAudio = nullptr;

    /* Shortcut to add an Anki note */
    QShortcut *m_shortcutAddCard = nullptr;

    /* true if the {expression} marker should use kana, false for kanji */
    bool m_readingAsExp = false;
};

#endif // TERMWIDGET_H
