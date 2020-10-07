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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QString>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define ENV_VAR "APPDATA"
    #define CONFIG_PATH "\\memento"
    #define SLASH "\\"
#elif __linux__
    #define ENV_VAR "HOME"
    #define CONFIG_PATH "/.config/memento"
    #define SLASH "/"
#elif __APPLE__
    #if TARGET_OS_MAC
        #define ENV_VAR "HOME"
        #define CONFIG_PATH "/.config/memento"
        #define SLASH "/"
    #else
        #error "Apple OS type no supported"
    #endif
#else
    #error "OS not supported"
#endif

#define DICTIONARY_DIR "dict"
#define CACHE_DIR "dict"

class DirectoryUtils
{
public:
    static QString getConfigDir();
    static QString getDictionaryDir();
    static QString getCacheDir();
    
private:
    DirectoryUtils() {}
};

#endif // PARAMETERS_H