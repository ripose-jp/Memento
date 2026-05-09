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

#include <QObject>

namespace Anki
{
Q_NAMESPACE

/**
 * @brief Describes different modes for detecting duplicates.
 */
enum DuplicatePolicy
{
    /* Duplicates cannot be in any deck */
    DuplicatePolicyNone = 0,

    /* Duplicates cannot be in the same deck */
    DuplicatePolicyDifferentDeck = 1,

    /* Duplicates are allowed */
    DuplicatePolicySameDeck = 2,
};
Q_ENUM_NS(DuplicatePolicy)

/**
 * @brief The format to save images in.
 */
enum FileType
{
    FileTypeJpeg = 0,
    FileTypePng = 1,
    FileTypeWebp = 2,
};
Q_ENUM_NS(FileType)

namespace Keys
{

/* Global Settings */

constexpr const char *ENABLED = "enabled";
constexpr bool ENABLED_DEFAULT = false;

constexpr const char *PROFILES = "profiles";

constexpr const char *SET_PROFILE = "setProfile";

/* Profile Settings */

constexpr const char *NAME = "name";
constexpr const char *NAME_DEFAULT = "Default";

constexpr const char *HOSTNAME = "host";
constexpr const char *HOSTNAME_DEFAULT = "localhost";

constexpr const char *PORT = "port";
constexpr const char *PORT_DEFAULT = "8765";

constexpr const char *DUPLICATE_POLICY = "duplicate";
constexpr Anki::DuplicatePolicy DUPLICATE_POLICY_DEFAULT =
    Anki::DuplicatePolicyDifferentDeck;

constexpr const char *SCREENSHOT = "screenshot";
constexpr Anki::FileType SCREENSHOT_DEFAULT = Anki::FileTypeJpeg;

constexpr const char *NEWLINE_REPLACER = "newline-replace";
constexpr const char *NEWLINE_REPLACER_DEFAULT = "<br>";

constexpr const char *AUDIO_PAD_START = "audio-pad-start";
constexpr double AUDIO_PAD_START_DEFAULT = 0.0;

constexpr const char *AUDIO_PAD_END = "audio-pad-end";
constexpr double AUDIO_PAD_END_DEFAULT = 0.0;

constexpr const char *AUDIO_NORMALIZE = "audio-normalize";
constexpr bool AUDIO_NORMALIZE_DEFAULT = false;

constexpr const char *AUDIO_DB = "audio-db";
constexpr double AUDIO_DB_DEFAULT = -20.0;

constexpr const char *TERM = "term";

constexpr const char *KANJI = "kanji";

constexpr const char *TAGS = "tags";
static const QStringList TAGS_DEFAULT{{"memento"}};

constexpr const char *DECK = "deck";

constexpr const char *MODEL = "model";

constexpr const char *FIELDS = "fields";

constexpr const char *EXCLUDE_GLOSSARIES = "ex-glos";

} // namespace Keys
} // namespace Anki
