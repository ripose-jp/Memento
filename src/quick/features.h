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

/**
 * @brief QML object containing all information about compiled features.
 */
class Features : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        int versionMajor
        READ versionMajor
        CONSTANT
    )

    Q_PROPERTY(
        int versionMinor
        READ versionMinor
        CONSTANT
    )

    Q_PROPERTY(
        int versionPatch
        READ versionPatch
        CONSTANT
    )

    Q_PROPERTY(
        QString version
        READ version
        CONSTANT
    )

    Q_PROPERTY(
        QString versionHash
        READ versionHash
        CONSTANT
    )

    Q_PROPERTY(
        bool windows
        READ windows
        CONSTANT
    )

    Q_PROPERTY(
        bool unix
        READ unix
        CONSTANT
    )

    Q_PROPERTY(
        bool macos
        READ macos
        CONSTANT
    )

    Q_PROPERTY(
        Platform platform
        READ platform
        CONSTANT
    )

    Q_PROPERTY(
        bool mecab
        READ mecab
        CONSTANT
    )

    Q_PROPERTY(
        bool ocr
        READ ocr
        CONSTANT
    )

    Q_PROPERTY(
        bool qApplication
        READ qApplication
        CONSTANT
    )

public:
    explicit Features(QObject *parent = nullptr);
    virtual ~Features();

    /**
     * @brief Enum describing all supported OSes.
     */
    enum Platform
    {
        Unknown = 0,
        Windows = 1,
        Unix = 2,
        MacOs = 3,
    };
    Q_ENUM(Platform)

    /**
     * @brief Get the major version of the software.
     *
     * @return The major version of the software.
     */
    [[nodiscard]]
    int versionMajor() const noexcept;

    /**
     * @brief Get the minor version of the software.
     *
     * @return The minor version of the software.
     */
    [[nodiscard]]
    int versionMinor() const noexcept;

    /**
     * @brief Get the patch version of the software.
     *
     * @return The patch version of the software.
     */
    [[nodiscard]]
    int versionPatch() const noexcept;

    /**
     * @brief Get the version string of the software.
     *
     * @return The version string.
     */
    [[nodiscard]]
    QString version() const;

    /**
     * @brief Get the Git hash of the software.
     * Empty if MEMENTO_RELEASE_BUILD=ON.
     *
     * @return The Git hash of the software.
     */
    [[nodiscard]]
    QString versionHash() const;

    /**
     * @brief Get if this is running on Windows.
     *
     * @return true if running on Windows,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool windows() const noexcept;

    /**
     * @brief Get if this is running on Unix, excluding macOS.
     *
     * @return true if running on Unix,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool unix() const noexcept;

    /**
     * @brief Get if this is running on macOS.
     *
     * @return true if running on macOS platform,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool macos() const noexcept;

    /**
     * @brief Get the exact OS this is running on.
     *
     * @return The OS this is running on.
     */
    [[nodiscard]]
    Platform platform() const noexcept;

    /**
     * @brief Get if Memento was compiled with MeCab support.
     *
     * @return true if compiled with mecab support,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool mecab() const noexcept;

    /**
     * @brief Get if Memento was compiled with OCR support.
     *
     * @return true if compiled with OCR support,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool ocr() const noexcept;

    /**
     * @brief Get if Memento was compiled as a QApplication instead of
     * QGuiApplication.
     *
     * @return true if compiled as a QApplication,
     * @return false if compiled as a QGuiApplication.
     */
    [[nodiscard]]
    bool qApplication() const noexcept;
};
