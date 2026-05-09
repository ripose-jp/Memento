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

#include <QFile>
#include <QString>

/**
 * @brief Utilities for dealing with files.
 */
namespace FileUtils
{

/**
 * @brief Calculate the MD5 hash of the file at the given path.
 *
 * @param path The path to the file.
 * @return An MD5 hash, empty string on error.
 */
[[nodiscard]]
QString calculateMd5(const QString &path);

/**
 * @brief Calculate the MD5 hash of the file at the given path.
 *
 * @param file The file to calculate the hash of.
 * @return An MD5 hash, empty string on error.
 */
[[nodiscard]]
QString calculateMd5(QFile *file);

/**
 * @brief Conevert a file path into base64.
 *
 * @param path The path of the file to encode.
 * @return The base64 encoding of the given file, empty if failed.
 */
[[nodiscard]]
QString toBase64(const QString &path);

/**
 * @brief Convert a file into base64.
 *
 * @param file The file to encode.
 * @return The base64 encoding of the given file, empty if failed.
 */
[[nodiscard]]
QString toBase64(QFile *file);

};
