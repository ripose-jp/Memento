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

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QJsonArray>
#include <QList>
#include <QMetaType>
#include <QSet>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QVariant>

/**
 * Struct holding all the data that makes up a tag.
 */
struct Tag
{
    /* The dictionary the tag comes from */
    QString dictionary;

    /* The name of the tag. */
    QString name;

    /* Category that the tag belongs to. */
    QString category;

    /* Further detail on the tag. */
    QString notes;

    /* Order of the tag. Used for ordering. */
    int order;

    /* Score of the tag. Used for ordering. */
    int score;

    bool operator==(const Tag &lhs) const
    {
        return name == lhs.name &&
               category == lhs.category &&
               notes == lhs.notes &&
               order == lhs.order &&
               score == lhs.score;
    }
};

/**
 * Struct containing all the information of a frequency tag.
 */
struct Frequency
{
    /* The name of the frequency dictionary. */
    QString dictionary;

    /* Frequency of the expression/kanji/etc. */
    QString freq;
};

/**
 * Struct containing all the information of a pitch accent entry.
 */
struct Pitch
{
    /* The name of the dictionary this pitch accent belongs to. */
    QString dictionary;

    /* A list of all the mora that appear in the expression. */
    QStringList mora;

    /* The pitch "position". See https://i.imgur.com/oEjxhWU.png */
    QList<uint8_t> position;
};

/**
 * Struct containing all the information making up a single definition.
 */
struct TermDefinition
{
    /* The name of the dictionary this entry comes from. */
    QString dictionary;

    /* A list of the tags associated with this entry. */
    QList<Tag> tags;

    /* A list of the rules associated with this entry. */
    QSet<QString> rules;

    /* A list of glossary entries for this definition. */
    QJsonArray glossary;

    /* Score of this definition.
     *  Used for ordering. More common entries have a larger score.
     */
    int score;
};

/**
 * A parent struct of Term and Kanji that contains fields common between the
 * two.
 */
struct CommonExpFields
{
    /* The title of the expression this came from. */
    QString title;

    /* The complete sentence this term was found in. */
    QString sentence;

    /* The start time of the subtitle */
    double startTime;

    /* The end time of the subtitle */
    double endTime;

    /* The current secondary subtitle */
    QString sentence2;

    /* The start time of the selected context */
    double startTimeContext;

    /* The end time of the selected context */
    double endTimeContext;

    /* The currently selected context */
    QString context;

    /* The current secondary context */
    QString context2;

    /* The current text in the user's clipboard */
    QString clipboard;

    /* The raw term as it was matched by Memento. */
    QString clozeBody;

    /* Everything in the sentence before the cloze body */
    QString clozePrefix;

    /* Everything in the sentence after the cloze body */
    QString clozeSuffix;

    /* A list of frequencies */
    QList<Frequency> frequencies;
};

/**
 * A struct containing all the information that makes up one term.
 */
struct Term : public CommonExpFields
{
    /* The expression of the term (includes kanji if it exists). */
    QString expression;

    /* The reading of a term. Usually empty if the term includes no kanji. */
    QString reading;

    /* The conjugation explaination of a term. Usually empty if the term was not conjugated. */
    QString conjugationExplanation;

    /* true if the reading should be used as the expression, false otherwise */
    bool readingAsExpression = false;

    /* The list of tags applicable to this term. */
    QList<Tag> tags;

    /* The list of pitches for this term. */
    QList<Pitch> pitches;

    /* The list of definitions for this term. */
    QList<TermDefinition> definitions;

    /* The score of this term.
     * Larger values generally mean this term is more common.
     */
    int score = 0;

    /* The name of the audio source */
    QString audioSrcName;

    /* The URL of the selected audio source */
    QString audioURL;

    /* The MD5 audio skip hash for the audio source */
    QString audioSkipHash;
};

/**
 * Struct containing all the information that makes up a single kanji
 * definition entry.
 */
struct KanjiDefinition
{
    /* The name of the dictionary the definition comes from. */
    QString dictionary;

    /* The onyomi (Chinese) readings of the kanji. */
    QStringList onyomi;

    /* The kunyomi (Japanese) readings of the kanji. */
    QStringList kunyomi;

    /* A list of definitions for this kanji. */
    QStringList glossary;

    /* The list of tags belonging to this definition. */
    QList<Tag> tags;

    /* Statistics for this definition.
     * The notes field of the Tag contains the description.
     * The string is the corresponding value.
     */
    QList<QPair<Tag, QString>> stats;

    /* Classifications for this definition.
     * The notes field of the Tag contains the description.
     * The string is the corresponding value.
     */
    QList<QPair<Tag, QString>> clas;

    /* Codepoints for this definition.
     * The notes field of the Tag contains the description.
     * The string is the corresponding value.
     */
    QList<QPair<Tag, QString>> code;

    /* Dictionary Indices for this definition.
     * The notes field of the Tag contains the description.
     * The string is the corresponding value.
     */
    QList<QPair<Tag, QString>> index;
};

/**
 * A struct containing everything that makes up a kanji entry.
 */
struct Kanji : public CommonExpFields
{
    /* The kanji. */
    QString character;

    /* A list of definitions belonging to the kanji. */
    QList<KanjiDefinition> definitions;
};

using SharedTerm = QSharedPointer<Term>;
using SharedTermList = QSharedPointer<QList<SharedTerm>>;
using SharedKanji = QSharedPointer<Kanji>;

#endif // EXPRESSION_H
