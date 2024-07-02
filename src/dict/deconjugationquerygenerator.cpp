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

#include "deconjugationquerygenerator.h"
#include "deconjugator.h"

#include <QList>

#include "util/utils.h"

/* Begin Query Generator */

static QString convertWordformToRule(WordForm wordform)
{
    switch (wordform)
    {
        case WordForm::godanVerb:
            return "v5";
        case WordForm::ichidanVerb:
            return "v1";
        case WordForm::kuruVerb:
            return "vk";
        case WordForm::suruVerb:
            return "vs";
        case WordForm::adjective:
            return "adj-i";
        default:
            return "";
    }
}

std::vector<SearchQuery> DeconjugationQueryGenerator::generateQueries(
    const QString &text) const
{
    if (text.isEmpty())
    {
        return {};
    }

    QList<ConjugationInfo> deconjQueries = deconjugate(text);
    std::vector<SearchQuery> result;
    for (ConjugationInfo &info : deconjQueries)
    {
        QString rule = convertWordformToRule(info.derivations[0]);
        auto duplicateIt = std::find_if(
            result.begin(),
            result.end(),
            [&] (const SearchQuery &o)
            {
                if (o.deconj == info.base)
                {
                    return o.ruleFilter.contains(rule) ||
                        o.conjugationExplanation == info.derivationDisplay;
                }
                return false;
            }
        );
        if (duplicateIt != result.end())
        {
            duplicateIt->ruleFilter.insert(std::move(rule));
        }
        else
        {
            result.emplace_back(SearchQuery{
                SearchQuery::Source::deconj,
                info.base,
                info.conjugated,
                { rule },
                info.derivationDisplay,
            });
        }

    }
    return result;
}

/* End Query Generator */
