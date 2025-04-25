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

#include <QJsonObject>

#include "anki/ankiconfig.h"
#include "anki/glossarybuilder.h"
#include "dict/expression.h"

namespace Anki
{
namespace Note
{

/**
 * Context containing information to build a term note.
 */
struct Context
{
    /* AnkiConnect compatible note object */
    QJsonObject ankiObject;

    /* A mapping of file to filenames */
    QList<GlossaryBuilder::FileInfo> fileMap;

    /**
     * Set the deck of this Anki object.
     * @param deck The name of the deck to use.
     */
    void setDeck(const QString &deck);

    /**
     * Set the model of this Anki object.
     * @param model The name of the model to use.
     */
    void setModel(const QString &model);

    /**
     * Set the tags of the Anki object.
     * @param tags
     */
    void setTags(const QJsonArray &tags);

    /**
     * Sets the duplicate policy of the Anki object.
     * @param policy The duplicate policy.
     */
    void setDuplicatePolicy(AnkiConfig::DuplicatePolicy policy);

    /**
     * Set the fields for this context.
     * @param fields The fields to use for this object.
     */
    void setFields(const QJsonValue &fields);
};

/**
 * Creates an AnkiConnect compatible note JSON object with corresponding media.
 * @param config The configuration to build the note to.
 * @param term   The term to make the object from.
 * @param media  true if screenshots and audio should be included in the
 *               object, false otherwise.
 * @return A Context containing an Anki compatible JSON object.
 */
[[nodiscard]]
Context build(const AnkiConfig &config, const Term &term, bool media);

/**
 * Creates an AnkiConnect compatible note JSON object with corresponding media.
 * @param config The configuration to build the note to.
 * @param kanji  The kanji to make the object from.
 * @param media  true if screenshots and audio should be included in the
 *               object, false otherwise.
 * @return A Context containing an Anki compatible JSON object.
 */
[[nodiscard]]
Context build(const AnkiConfig &config, const Kanji &kanji, bool media);

}
}
