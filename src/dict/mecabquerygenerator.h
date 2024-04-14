////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2024 Ripose
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

#ifndef MECABQUERYGENERATOR_H
#define MECABQUERYGENERATOR_H

#include "querygenerator.h"

#include <memory>

#include <mecab.h>

/**
 * Query generator that uses MeCab backed by ipadic to deconjugate text.
 */
class MeCabQueryGenerator final : public QueryGenerator
{
public:
    MeCabQueryGenerator();
    virtual ~MeCabQueryGenerator() = default;

    /**
     * Returns if the query generator is valid.
     * @return true if the generator is valid,
     * @return false otherwise.
     */
    [[nodiscard]]
    inline bool valid() const override
    {
        return m_tagger != nullptr;
    }

    /**
     * Generates queries from a given text string.
     * @param text The text string to generate queries from.
     * @return The list of generated queries.
     */
    [[nodiscard]]
    std::vector<SearchQuery> generateQueries(
        const QString &text) const override;

private:
    /**
     * A special SearchPair that contains additional information needed by
     * MeCab.
     */
    struct MeCabQuery : public SearchQuery
    {
        /* The surface string without whitespace */
        QString surfaceClean;
    };

    /**
     * Recursively generates queries and surface strings from a node.
     * @param node The node to start at. Usually the next node after the BOS
     *             node. Is nullptr safe.
     * @return A list of conjugated string and surface (raw) strings.
     */
    [[nodiscard]]
    static std::vector<MeCabQuery> generateQueriesHelper(
        const MeCab::Node *node);

    /**
     * Gets the deconjugated word from a MeCab node.
     * @param node The node to get the deconjugation from.
     * @return The deconjugated word, * if there was an error.
     */
    [[nodiscard]]
    static inline QString extractDeconjugation(const MeCab::Node *node);

    /**
     * Gets the surface string including whitespace from a MeCab node.
     * @param node The MeCab node to get the surface from.
     * @return The surface string including whitespace.
     */
    [[nodiscard]]
    static inline QString extractSurface(const MeCab::Node *node);

    /**
     * Gets the surface string without whitespace from a MeCab node.
     * @param node The MeCab node to get the surface from.
     * @return The surface string without whitespace.
     */
    [[nodiscard]]
    static inline QString extractCleanSurface(const MeCab::Node *node);

    /* The object used for interacting with MeCab */
    std::unique_ptr<MeCab::Tagger> m_tagger{nullptr};
};

#endif // MECABQUERYGENERATOR_H
