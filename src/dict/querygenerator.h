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

#ifndef QUERYGENERATOR_H
#define QUERYGENERATOR_H

#include <vector>

#include "searchquery.h"

/**
 * Interface class for generating search queries from text.
 */
class QueryGenerator
{
public:
    QueryGenerator() = default;
    virtual ~QueryGenerator() = default;

    /**
     * Returns if the generator is valid.
     * @return true if the generator is valid,
     * @return false otherwise
     */
    [[nodiscard]]
    virtual bool valid() const = 0;

    /**
     * Returns if the generator is invalid
     * @return true if the generator is invalid,
     * @return false otherwise
     */
    [[nodiscard]]
    inline bool operator!() const
    {
        return !valid();
    }

    /**
     * Generate a list of queries from a string of text.
     * @param text The text to extract queries from.
     * @return A list of search queries.
     */
    [[nodiscard]]
    virtual std::vector<SearchQuery> generateQueries(
        const QString &text) const = 0;
};

#endif // QUERYGENERATOR_H
