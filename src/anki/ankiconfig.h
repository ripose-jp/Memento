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

struct AudioSource
{
    QString name;
    QString url;
    QString md5;

    AudioSource &operator=(const AudioSource &rhs)
    {
        name = rhs.name;
        url  = rhs.url;
        md5  = rhs.md5;

        return *this;
    }
};

struct AnkiConfig
{
    enum DuplicatePolicy
    {
        None = 0,
        DifferentDeck = 1,
        SameDeck = 2
    };

    enum FileType
    {
        jpg = 0,
        png = 1,
        webp = 2
    };

    QString         address;
    QString         port;
    DuplicatePolicy duplicatePolicy;
    FileType        screenshotType;
    AudioSource     audio;
    double          audioPadStart;
    double          audioPadEnd;
    QJsonArray      tags;

    QString         termDeck;
    QString         termModel;
    QJsonObject     termFields;

    QString         kanjiDeck;
    QString         kanjiModel;
    QJsonObject     kanjiFields;

    AnkiConfig() {}

    AnkiConfig &operator=(const AnkiConfig &rhs)
    {
        address         = rhs.address;
        port            = rhs.port;
        duplicatePolicy = rhs.duplicatePolicy;
        screenshotType  = rhs.screenshotType;
        audio           = rhs.audio;
        audioPadStart   = rhs.audioPadStart;
        audioPadEnd     = rhs.audioPadEnd;
        tags            = rhs.tags;

        termDeck        = rhs.termDeck;
        termModel       = rhs.termModel;
        termFields      = rhs.termFields;
        
        kanjiDeck       = rhs.kanjiDeck;
        kanjiModel      = rhs.kanjiModel;
        kanjiFields     = rhs.kanjiFields;
        
        return *this;
    }
};

#endif // ANKICONFIG_H