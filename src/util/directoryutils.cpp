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

#include "directoryutils.h"

#include <QFile>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #include <windows.h>
    #define CONFIG_PATH "config"
#elif __linux__
    #define BASE_DIR getenv("HOME")
    #define CONFIG_PATH ".config/memento/"
#elif __APPLE__
    #if TARGET_OS_MAC
        #define BASE_DIR getenv("HOME")
        #define CONFIG_PATH ".config/memento/"
    #else
        #error "Apple OS type no supported"
    #endif
#else
    #error "OS not supported"
#endif

#define DICTIONARY_DIR "dict"

QString DirectoryUtils::getProgramDirectory()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    WCHAR buf[MAX_PATH];
    GetModuleFileNameW(NULL, buf, MAX_PATH);
    QString path = QString::fromWCharArray(buf);
    return path.left(path.lastIndexOf(SLASH) + 1);
#elif __linux__
    return BASE_DIR;
#endif
}

QString DirectoryUtils::getConfigDir()
{
    QString path = getProgramDirectory();
    path += CONFIG_PATH;
    path += SLASH;
    return path;
}

QString DirectoryUtils::getGlobalConfigDir()
{
    return getConfigDir();
}

QString DirectoryUtils::getDictionaryDir()
{
    return getConfigDir() + DICTIONARY_DIR + SLASH;
}

QString DirectoryUtils::getDictionaryDB()
{
    return getDictionaryDir() + DICT_DB_FILE;
}

QString DirectoryUtils::getMpvInputConfig()
{
    return getConfigDir() + MPV_INPUT_CONF_FILE;
}