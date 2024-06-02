////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2024 Spacehamster
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

#ifndef DECONJUGATOR_H
#define DECONJUGATOR_H

#include <QString>
#include <QList>

enum class WordForm
{
    godanVerb,
    ichidanVerb,
    suruVerb,
    kuruVerb,
    irregularVerb,
    adjective,
    negative,
    past,
    te,
    conjunctive,
    volitional,
    passive,
    causative,
    imperative,
    potential,
    potentialPassive,
    conditional,
    imperativeNegative,
    zaru,
    zu,
    nu,
    neba,
    tari,
    shimau,
    chau,
    chimau,
    polite,
    tara,
    tai,
    nasai,
    sugiru,
    sou,
    e,
    ba,
    ki,
    toku,
    colloquialNegative,
    provisionalColloquialNegative,
    continuous,
    adverbial,
    noun,
    any,
    none,
};

/**
 * A struct that contains the results of a deconjugation
 */
struct ConjugationInfo
{
    /* Plain form of a word. */
    QString base;

    /* The original conjugated form. */
    QString conjugated;

    /* A list of conjugations that describe the relationship
     * between base and conjugated. */
    QList<WordForm> derivations;

    /* A human readable format of the derivations. */
    QString derivationDisplay;
};

/**
 * Attempts to deconjugate a word
 * @param query         The query to attempt to deconjugate
 * @param sentenceMode  If enabled, treats the query as a sentence and will
 *                      find potential words by trimming the query
 * @return A list of all the potential deconjugations found
 */
QList<ConjugationInfo> deconjugate(
    const QString query, bool sentenceMode = true);

#endif // DECONJUGATOR_H
