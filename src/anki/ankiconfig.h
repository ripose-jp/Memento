////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#ifndef ANKICONFIG_H
#define ANKICONFIG_H

#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

struct AnkiConfig
{
    QString address;
    QString port;
    QJsonArray tags;
    QString deck;
    QString model;
    QJsonObject fields;

    AnkiConfig() {}

    AnkiConfig(const AnkiConfig &config)
    {
        address = config.address;
        port = config.port;
        tags = config.tags;
        deck = config.deck;
        model = config.model;
        fields = config.fields;
    }

    AnkiConfig &operator=(const AnkiConfig &rhs)
    {
        address = rhs.address;
        port = rhs.port;
        tags = rhs.tags;
        deck = rhs.deck;
        model = rhs.model;
        fields = rhs.fields;
        
        return *this;
    }
};

#endif // ANKICONFIG_H