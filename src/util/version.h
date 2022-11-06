////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 Ripose
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

#ifndef VERSION_H
#define VERSION_H

#define DEFINE_TO_STRING_HELPER(x)  (#x)
#define DEFINE_TO_STRING(x)         (DEFINE_TO_STRING_HELPER(x))

#ifdef CMAKE_VERSION
#define VERSION                         (QString(DEFINE_TO_STRING(CMAKE_VERSION)))
#else
#define VERSION                         (QString("Version Missing"))
#endif

#ifdef CMAKE_HASH
#define VERSION_HASH                    (QString(DEFINE_TO_STRING(CMAKE_HASH)))
#else
#define VERSION_HASH                    (QString(""))
#endif

#endif // VERSION_H