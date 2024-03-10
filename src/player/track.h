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

#ifndef TRACK_H
#define TRACK_H

#include <QString>

/**
 * Struct containing track information.
 */
struct Track
{
    /**
     * The type of track.
     */
    enum Type
    {
        audio,
        video,
        subtitle
    };

    /* Type of track. */
    Type type;

    /* Id of the track start at 1. Unique within its type. */
    int64_t id;

    /* Track ID as used in the source file. Not always available. */
    int64_t srcId;

    /* Track title as it is stored in the file. Not always available. */
    QString title;

    /* Track language as identified by the file. Not always available. */
    QString lang;

    /* true if the track has album art. */
    bool albumart;

    /* true if the track is a default track. */
    bool def;

    /* true if the track is currently selected. */
    bool selected;

    /* 0 if this primary track, 1 if the secondary track */
    int64_t mainSelection;

    /* true if the track is from an external file. */
    bool external;

    /* The filename if the track is from an external file, unavailable
     * otherwise.
     */
    QString externalFilename;

    /* The codec name used by this track, for example h264. Unavailable in some
     * rare cases.
     */
    QString codec;
};

#endif // TRACK_H
