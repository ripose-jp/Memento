////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
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

#pragma once

#include <QString>

#include "setting/keys.h"

/**
 * @brief Defines the parameters of an audio source.
 */
struct AudioSource
{
    /* The name of the audio source */
    QString name;

    /* The URL of the audio source */
    QString url;

    /* The type of the audio source */
    Setting::AudioSourceType type;

    /* The MD5 skip hash of the audio source */
    QString skipHash;
};
