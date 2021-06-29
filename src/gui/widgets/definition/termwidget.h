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

#include "../../../anki/ankiclient.h"
#include "../../../dict/expression.h"
#include "../common/flowlayout.h"

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
     * @param term    The term to display. Does not take ownership.
     * @param sources A list of audio sources. Does not take ownership.
     * @param parent  The parent of this widget.
     */
    TermWidget(const Term               *term, 
               const QList<AudioSource> *sources,
               QWidget                  *parent = nullptr);
    ~TermWidget();

    /**
     * Sets this widget to be addable.
     * @param value If true, Anki add button is visible. If false Anki search
     *              button is visible.
     */
    void setAddable(bool value);

Q_SIGNALS:
    /**
     * Emitted when a kanji is searched by the user and found.
     * @param kanji The found kanji. Recipient takes ownership.
     */
    void kanjiSearched(const Kanji *kanji);

private Q_SLOTS:
    /**
     * Adds the term belonging to this widget to Anki.
     */
    void addNote();

    /**
     * Opens an Anki window searching the current configured term card deck for
     * the expression of the current term.
     */
    void searchAnki();

    /**
     * Plays audio for the term from the audio source.
     * @param url  The url of the audio.
     * @param hash The md5 skip hash of audio to be skipped.
     */
    void playAudio(QString url, const QString &hash);

    /**
     * Opens up a context menu with every audio source.
     * @param pos The position (relative to the audio button) to open the
     *            context menu.
     */
    void showAudioSources(const QPoint &pos);

    /**
     * Searches a clicked kanji to see if there is an entry.
     * @param ch The kanji to search.
     */
    void searchKanji(const QString &ch);

private:
    /**
     * Puts term information into the UI.
     * @param term The term to populate the UI with.
     */
    void initUi(const Term &term);

    /**
     * Creates a Jisho link from an expression.
     * @param exp The expression to create a Jisho link for.
     * @return An HTML formatted Jisho link for the expression.
     */
    inline QString generateJishoLink(const QString &exp);

    /**
     * Describes if a character is Kanji or not.
     * @param ch The character to check.
     * @return True if it is a kanji, false otherwise.
     */
    inline bool isKanji(const QString &ch);

    /* UI object containing all the widgets. */
    Ui::TermWidget *m_ui;

    /* The term this widget is displaying. */
    const Term *m_term;

    /* Saved pointer to the global AnkiClient. */
    AnkiClient *m_client;

    /* A saved pointer to the list of audio sources. */
    const QList<AudioSource> *m_sources;

    /* Layout used for holding term tags. */
    FlowLayout  *m_layoutTermTags;

    /* Layout used for holding frequency tags. */
    FlowLayout  *m_layoutFreqTags;

    /* Layout used hold pitch tags and pitches. */
    QVBoxLayout *m_layoutPitches;

    /* Layout used for holding glossary entries. */
    QVBoxLayout *m_layoutGlossary;
};

#endif // TERMWIDGET_H