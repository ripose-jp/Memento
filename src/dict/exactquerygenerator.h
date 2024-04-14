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

#ifndef EXACTQUERYGENERATOR_H
#define EXACTQUERYGENERATOR_H

#include "querygenerator.h"

class ExactQueryGenerator final : public QueryGenerator
{
public:
    virtual ~ExactQueryGenerator() = default;

    /**
     * ExactQueryGenerator is always valid.
     * @return Always returns true.
     */
    [[nodiscard]]
    inline bool valid() const override
    {
        return true;
    }

    /**
     * Generates a list of queries from the given text.
     * For a query like 昨日すき焼きを食べました, return results are like
     * 昨日すき焼きを食べました
     * 昨日すき焼きを食べまし
     * 昨日すき焼きを食べま
     * 昨日すき焼きを食べ
     * 昨日すき焼きを食
     * 昨日すき焼きを
     * 昨日すき焼き
     * 昨日すき焼
     * 昨日すき
     * 昨日す
     * 昨日
     * 昨
     * @param text The text to turn into queries
     * @return The list of generated queries
     */
    [[nodiscard]]
    std::vector<SearchQuery> generateQueries(
        const QString &text) const override;
};

#endif // EXACTQUERYGENERATOR_H
