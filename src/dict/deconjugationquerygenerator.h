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

#ifndef DECONJUGATIONQUERYGENERATOR_H
#define DECONJUGATIONQUERYGENERATOR_H

#include "querygenerator.h"

#include <memory>

#include <mecab.h>

/**
 * Query generator that uses MeCab backed by ipadic to deconjugate text.
 */
class DeconjugationQueryGenerator final : public QueryGenerator
{
public:
    virtual ~DeconjugationQueryGenerator() = default;

    /**
     * Returns if the query generator is valid.
     * @return true if the generator is valid,
     * @return false otherwise.
     */
    [[nodiscard]]
    inline bool valid() const override
    {
        return true;
    }

    /**
     * Generates queries from a given text string.
     * @param text The text string to generate queries from.
     * @return The list of generated queries.
     */
    [[nodiscard]]
    std::vector<SearchQuery> generateQueries(
        const QString &text) const override;
};

#endif // DECONJUGATIONQUERYGENERATOR_H
