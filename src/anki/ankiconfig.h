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

#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QStringList>

/**
 * Internal representation of a user defined audio source.
 */
struct AudioSource
{
    /* The name of the audio source. */
    QString name;

    /* The raw URL (may contain {expression}/{reading}) of the audio source. */
    QString url;

    /* The MD5 hash to skip if the audio from the source matches. */
    QString md5;

    AudioSource &operator=(const AudioSource &rhs)
    {
        name = rhs.name;
        url  = rhs.url;
        md5  = rhs.md5;

        return *this;
    }
};

/**
 * Representation of a single Anki Integration configuration profile.
 */
struct AnkiConfig
{
    /**
     * The duplicate different duplicate policies available.
     */
    enum DuplicatePolicy
    {
        /* Duplicates cannot be across all decks. */
        None = 0,

        /* Duplicates cannot be in the same deck, but can be in different decks.
         */
        DifferentDeck = 1,

        /* Duplicates can be in the same deck. */
        SameDeck = 2
    };

    /**
     * The file type to save images as.
     */
    enum FileType
    {
        jpg = 0,
        png = 1,
        webp = 2
    };

    /* The address of the AnkiConnect server. */
    QString address;

    /* The port of the AnkiConnect server. */
    QString port;

    /* The duplicate policy of this profile. */
    DuplicatePolicy duplicatePolicy;

    /* The file type to save screenshots as. */
    FileType screenshotType;

    /* The audio source for {audio} markers. */
    AudioSource audio;

    /* The amount of padding to add (in seconds) to the start of an audio clip.
     * Used for the {audio-media} marker.
     */
    double audioPadStart;

    /* The amount of padding to add (in seconds) to the end of an audio clip.
     * Used for the {audio-media} marker.
     */
    double audioPadEnd;

    /* true if audio should be normalized, false otherwise */
    bool audioNormalize;

    /* The dB value audio should be normalized to */
    double audioDb;

    /* A list of tags to add to Anki notes. */
    QJsonArray tags;

    /* The name of the deck to add term cards to. */
    QString termDeck;

    /* The name of the model to create term cards with. */
    QString termModel;

    /* A JSON object mapping the field names to their raw, user-defined values
     * for term cards.
     */
    QJsonObject termFields;

    /* The name of the deck to add kanji cards to. */
    QString kanjiDeck;

    /* The name of the model to create kanji cards with. */
    QString kanjiModel;

    /* A JSON object mapping the field names to their raw, user-defined values
     * for kanji cards.
     */
    QJsonObject kanjiFields;

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
        audioNormalize  = rhs.audioNormalize;
        audioDb         = rhs.audioDb;
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