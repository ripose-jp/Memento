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

#include "markertokenizer.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>

QList<Anki::Token> Anki::tokenize(const QString &text)
{
    constexpr qsizetype MAX_ARG_DELIM_COUNT{1};
    constexpr qsizetype MAX_KEY_VALUE_DELIM_COUNT{1};

    QRegularExpression tokenMatcher("{.*?}");

    QList<Anki::Token> tokens;

    QRegularExpressionMatchIterator matchIt = tokenMatcher.globalMatch(text);
    while (matchIt.hasNext())
    {
        QRegularExpressionMatch match = matchIt.next();

        Token t;
        t.raw = match.captured();

        QString marker = t.raw.mid(1, t.raw.size() - 2);
        QStringList markerArgs = marker.split(':');
        if (markerArgs.isEmpty())
        {
            continue;
        }
        t.marker = markerArgs[0].trimmed();
        if (markerArgs.size() == 1)
        {
            tokens.emplaceBack(std::move(t));
            continue;
        }

        if (markerArgs.size() != MAX_ARG_DELIM_COUNT + 1)
        {
            continue;
        }
        QStringList args = markerArgs[1].split(',');
        for (const QString &arg : args)
        {
            QStringList kv = arg.split('=');
            if (kv.size() != MAX_KEY_VALUE_DELIM_COUNT + 1)
            {
                continue;
            }
            t.args.emplace(kv[0].trimmed(), kv[1].trimmed());
        }

        tokens.emplaceBack(std::move(t));
    }

    return tokens;
}
