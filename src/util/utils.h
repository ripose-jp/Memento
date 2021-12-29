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

#ifndef UTILS_H
#define UTILS_H

#include <QFile>
#include <QString>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define SLASH "\\"
#elif __linux__
    #define SLASH "/"
#elif __APPLE__
    #define SLASH "/"
#else
    #error "OS not supported"
#endif

/* Dictionary database file name. */
#define DICT_DB_FILE        "dictionaries.sqlite"

/* mpv input configuration file name. */
#define MPV_INPUT_CONF_FILE "input.conf"

/**
 * Utilities for getting important program directories and files.
 */
class DirectoryUtils
{
public:
    /**
     * Gets the directory the exectuable is in.
     * @return Path to the program directory.
     */
    static QString getProgramDirectory();

    /**
     * Gets the config directory.
     * @return Path to the config directory.
     */
    static QString getConfigDir();

    /**
     * Gets the dictionary directory if it exists.
     * @return Path to the dictionary directory if it exists, empty string
     *         otherwise.
     */
    static QString getDictionaryDir();

    /**
     * Gets the dictionary database.
     * @return Path to the dictionary directory.
     */
    static QString getDictionaryDB();

    /**
     * Gets the path to the mpv input.conf.
     * @return Path to mpv's input.conf.
     */
    static QString getMpvInputConfig();

private:
    DirectoryUtils() {}
};

/**
 * Utilities for dealing with files.
 */
class FileUtils
{
public:
    /**
     * Calculate the MD5 hash of the file at the given path.
     * @param path The path to the file.
     * @return An MD5 hash, empty string on error.
     */
    static QString calculateMd5(const QString &path);

    /**
     * Calculate the MD5 hash of the file at the given path.
     * @param file The file to calculate the hash of.
     * @return An MD5 hash, empty string on error.
     */
    static QString calculateMd5(QFile *file);

private:
    FileUtils() {}
};

/**
 * Utilities for getting information from a network.
 */
class NetworkUtils
{
public:
    /**
     * Checks if there is a new version of Memento. Displays a dialog with a
     * link to a new version, one saying Memento is up to date, or an error.
     */
    static void checkForUpdates();
};

/**
 * Utilities for building images.
 */
class GraphicUtils
{
public:
    /**
     * Generates a pitch graph SVG.
     * @param moraSize The number of mora.
     * @param pos      The pitch position number.
     * @param fill     The fill color of the graph. Hex code formatted.
     * @param stroke   The stroke color of the graph. Hex code formatted.
     * @return The string representation of the SVG.
     */
    static QString generatePitchGraph(const int      moraSize,
                                      const uint8_t  pos,
                                      const QString &fill,
                                      const QString &stroke);

private:
    GraphicUtils() {}
};

class CharacterUtils
{
public:
    /**
     * Determines if a character is kanji.
     * @param ch The character to check.
     * @return true if the character is kanji, false otherwise.
     */
    static bool isKanji(const QString &ch);

private:
    CharacterUtils() {}
};

#endif // UTILS_H