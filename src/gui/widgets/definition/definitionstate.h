////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2023 Ripose
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

#ifndef DEFINITIONSTATE_H
#define DEFINITIONSTATE_H

#include <vector>

#include "util/constants.h"

/**
 * Contains the state of a definition object that needs to be shared with its
 * children.
 */
struct DefinitionState
{
    /**
     * Internal representation of a user defined audio source.
     */
    struct AudioSource
    {
        /* The type of the audio source */
        Constants::AudioSourceType type;

        /* The name of the audio source. */
        QString name;

        /* The raw URL (may contain {expression}/{reading}) of the audio source. */
        QString url;

        /* The MD5 hash to skip if the audio from the source matches. */
        QString md5;

        /* A subarray of audio sources for this source. */
        std::vector<AudioSource> audioSources;
    };

    /* The set of audio sources */
    std::vector<AudioSource> sources;

    /* The number of audio sources in sources that are JSON sources */
    int jsonSourceCount;

    /* The maximum number of results that can appear in a search */
    int resultLimit;

    /* The style of glossary to display */
    Constants::GlossaryStyle glossaryStyle;

    /* Whether or not to auto-play the first result's audio */
    bool autoPlayAudio;

    /* The modifier key used for executing sub-searches */
    Qt::KeyboardModifier searchModifier;
};

#endif // DEFINITIONSTATE_H
