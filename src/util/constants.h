////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define VERSION         (QString("v0.4.1-alpha"))
#define GITHUB_API_LINK (QString("https://api.github.com/repos/ripose-jp/memento/releases"))
#define GITHUB_RELEASES (QString("https://github.com/ripose-jp/memento/releases"))

#define SETTINGS_DICTIONARIES       "dictionaries"

#define SETTINGS_SEARCH             "search"
#define SETTINGS_SEARCH_LIMIT       "limit"
#define SETTINGS_SEARCH_METHOD      "method"
#define SETTINGS_SEARCH_DELAY       "delay"
#define SETTINGS_SEARCH_MODIFIER    "modifier"

#define MODIFIER_ALT                "Alt"
#define MODIFIER_CTRL               "Control"
#define MODIFIER_SHIFT              "Shift"
#define MODIFIER_SUPER              "Super"

#define SEARCH_METHOD_HOVER         "Hover"
#define SEARCH_METHOD_MODIFIER      "Modifier"

#define DEFAULT_LIMIT               30
#define DEFAULT_METHOD              SEARCH_METHOD_HOVER
#define DEFAULT_DELAY               250
#define DEFAULT_MODIFIER            MODIFIER_SHIFT

#endif // CONSTANTS_H