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

#include "exactquerygenerator.h"

std::vector<SearchQuery> ExactQueryGenerator::generateQueries(
    const QString &text) const
{
    std::vector<SearchQuery> queries;

    QString query = text;
    while (!query.isEmpty())
    {
        SearchQuery sq;
        sq.deconj = query;
        sq.surface = query;
        sq.source = SearchQuery::Source::exact;
        queries.emplace_back(std::move(sq));
        query.chop(1);
    }

    return queries;
}
