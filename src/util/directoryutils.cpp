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

#include "util/directoryutils.h"

#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>

QString DirectoryUtils::getProgramDirectory()
{
    return QDir::toNativeSeparators(
        QCoreApplication::applicationDirPath()
    ) + QDir::separator();
}

QString DirectoryUtils::getConfigDir()
{
    QString path = QDir::toNativeSeparators(
        QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
    );
    path.chop(sizeof("memento") - 1);
    if (path.isEmpty() || !path.endsWith(QDir::separator()))
    {
        path += QDir::separator();
    }
    return path;
}

QString DirectoryUtils::getMecabDictionary()
{
#if defined(Q_OS_WIN)
    return getProgramDirectory() + "dic\\";
#elif defined(MEMENTO_APPBUNDLE)
    return getProgramDirectory() + "/../Frameworks/mecab/dic/";
#else
    return "";
#endif
}

QString DirectoryUtils::getDictionaryDb()
{
    constexpr const char *DICT_DB_FILE = "dictionaries.sqlite";
    return getConfigDir() + DICT_DB_FILE;
}

QString DirectoryUtils::getAnkiConfig()
{
    constexpr const char *ANKI_CONFIG_FILE = "anki_connect.json";
    return getConfigDir() + ANKI_CONFIG_FILE;
}

QString DirectoryUtils::getMpvInputConfig()
{
    constexpr const char *MPV_INPUT_CONF_FILE = "input.conf";
    return getConfigDir() + MPV_INPUT_CONF_FILE;
}

QString DirectoryUtils::getDictionaryResourceDir()
{
    constexpr const char *RESOURCE_DIR = "res";
    return getConfigDir() + RESOURCE_DIR + QDir::separator();
}
