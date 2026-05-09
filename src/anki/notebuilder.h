////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Ripose
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

#pragma once

#include <QJsonObject>

#include "anki/ankiprofile.h"
#include "anki/glossarybuilder.h"
#include "dict/data/kanji.h"
#include "dict/data/term.h"

class Context;

namespace Anki
{
namespace Note
{

/**
 * @brief Context containing information to build a term note.
 */
struct Context
{
    /**
     * @brief Set the deck of this Anki object.
     *
     * @param deck The name of the deck to use.
     */
    void setDeck(const QString &deck);

    /**
     * @brief Set the model of this Anki object.
     *
     * @param model The name of the model to use.
     */
    void setModel(const QString &model);

    /**
     * @brief Set the tags of the Anki object.
     *
     * @param tags
     */
    void setTags(const QStringList &tags);

    /**
     * @brief Set the duplicate policy of the Anki object.
     *
     * @param policy The duplicate policy.
     */
    void setDuplicatePolicy(Anki::DuplicatePolicy policy);

    /**
     * @brief Set the fields for this context.
     *
     * @param fields The fields to use for this object.
     */
    void setFields(const QJsonValue &fields);

    /* AnkiConnect compatible note object */
    QJsonObject ankiObject;

    /* A mapping of file to filenames */
    QList<GlossaryBuilder::FileInfo> fileMap;
};

/**
 * @brief Create an AnkiConnect compatible note JSON object with corresponding
 * media.
 *
 * @param context The context for the program.
 * @param profile The profile to build the note to.
 * @param term The term to make the object from.
 * @param media true if screenshots and audio should be included in the object,
 * false otherwise.
 * @return A Context containing an Anki compatible JSON object.
 */
[[nodiscard]]
Anki::Note::Context build(
    const ::Context &context,
    const AnkiProfile &profile,
    const Term &term,
    bool media);

/**
 * @brief Create an AnkiConnect compatible note JSON object with corresponding
 * media.
 *
 * @param context The context for the program.
 * @param profile The profile to build the note to.
 * @param kanji The kanji to make the object from.
 * @param media true if screenshots and audio should be included in the object,
 * false otherwise.
 * @return A Context containing an Anki compatible JSON object.
 */
[[nodiscard]]
Anki::Note::Context build(
    const ::Context &context,
    const AnkiProfile &profile,
    const Kanji &kanji,
    bool media);
}
}
