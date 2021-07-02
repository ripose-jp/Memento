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

#include <QString>
#include <QStringList>
#include <QVariant>

/**
 * Struct holding all the data that makes up a tag.
 */
struct Tag
{
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

    bool operator==(const Tag &lhs)
    {
        return name == lhs.name && 
               category == lhs.category &&
               notes == lhs.notes &&
               order == lhs.order && 
               score == lhs.score;
    }
} typedef Tag;

/**
 * Struct containing all the information of a frequency tag.
 */
struct Frequency
{
    /* The name of the frequency dictionary. */
    QString dictionary;

    /* Frequency of the expression/kanji/etc. */
    uint64_t freq;
} typedef Frequency;

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
} typedef Pitch;

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
    QList<Tag>  rules;

    /* A list of glossary entries for this definition. */
    QStringList glossary;

    /* Score of this definition.
     *  Used for ordering. More common entries have a larger score.
     */
    int score;
} typedef TermDefinition;

/**
 * A struct containg all the information that makes up one term.
 */
struct Term
{
    Term() : score(0) {}

    /* The expression of the term (includes kanji if it exists). */
    QString expression;

    /* The reading of a term. Usually empty if the term includes no kanji. */
    QString reading;

    /* The list of tags applicable to this term. */
    QList<Tag> tags;

    /* The list of frequencies for this term. */
    QList<Frequency> frequencies;

    /* The list of pitches for this term. */
    QList<Pitch> pitches;

    /* The list of definitions for this term. */
    QList<TermDefinition> definitions;

    /* The score of this term.
     * Larger values generally mean this term is more common.
     */
    int score;

    /* Values below here will be set outside of DatabaseManager. */

    /* The complete sentence this term was found in. */
    QString sentence;

    /* The raw term as it was matched by Memento. */
    QString clozeBody;

    /* Everything in the sentence before the cloze body. */
    QString clozePrefix;

    /* Everything in the sentence after the cloze body. */
    QString clozeSuffix;
} typedef Term;


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
} typedef KanjiDefinition;

/**
 * A struct containing everything that makes up a kanji entry.
 */
struct Kanji
{
    /* The kanji. */
    QString character;

    /* A list of definitions belonging to the kanji. */
    QList<KanjiDefinition> definitions;

    /* A list of frequencies corresponding to the kanji. */
    QList<Frequency>       frequencies;

    /* Values below here will be set outside of DatabaseManager. */

    /* The complete sentence this kanji was found in. */
    QString sentence;

    /* The raw term as it was matched by Memento. Not the raw kanji. */
    QString clozeBody;

    /* Everything in the sentence before the cloze body. */
    QString clozePrefix;

    /* Everything in the sentence after the cloze body. */
    QString clozeSuffix;
} typedef Kanji;

#endif // EXPRESSION_H