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

#ifndef SEARCHQUERY_H
#define SEARCHQUERY_H

#include <QString>

/**
 * A pair to search for. The deconjugated string is used for querying the
 * database and the surface string is used for cloze generation.
 */
struct SearchQuery
{
    /* The deconjugated string */
    QString deconj;

    /* The raw conjugated string */
    QString surface;
};

#endif // SEARCHQUERY_H
