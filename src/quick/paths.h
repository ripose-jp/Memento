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

#include "util/utils.h"

/**
 * @brief QML singleton that makes DirectoryUtils paths available in QML.
 */
class Paths : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QString program
        READ program
        CONSTANT
    )

    Q_PROPERTY(
        QString ankiConfig
        READ ankiConfig
        CONSTANT
    )

    Q_PROPERTY(
        QString config
        READ config
        CONSTANT
    )

    Q_PROPERTY(
        QString dictionaryDb
        READ dictionaryDb
        CONSTANT
    )

    Q_PROPERTY(
        QString dictionaryResource
        READ dictionaryResource
        CONSTANT
    )

    Q_PROPERTY(
        QString mecabDictionary
        READ mecabDictionary
        CONSTANT
    )

    Q_PROPERTY(
        QString mpvInputConfig
        READ mpvInputConfig
        CONSTANT
    )

    Q_PROPERTY(
        QString historyDir
        READ historyDir
        CONSTANT
    )

public:
    Paths(QObject *parent = nullptr);

    /**
     * @brief Get the path to the program executable.
     *
     * @return Path to the program executable.
     */
    [[nodiscard]]
    const QString &program() const noexcept;

    /**
     * @brief Get the path to the Anki configuration file.
     *
     * @return Path to the Anki configuration file.
     */
    [[nodiscard]]
    const QString &ankiConfig() const noexcept;

    /**
     * @brief Get the path to the configuration directory.
     *
     * @return Path to the configuration directory.
     */
    [[nodiscard]]
    const QString &config() const noexcept;

    /**
     * @brief Get the path to the dictionary database.
     *
     * @return Path to the dictionary database.
     */
    [[nodiscard]]
    const QString &dictionaryDb() const noexcept;

    /**
     * @brief Get the path to the dictionary resource directory.
     *
     * @return Path to the dictionary resource directory.
     */
    [[nodiscard]]
    const QString &dictionaryResource() const noexcept;

    /**
     * @brief Get the path to the MeCab dictionary if it exists.
     *
     * @return The path to the MeCab dictionary if it exists, empty string
     * otherwise.
     */
    [[nodiscard]]
    const QString &mecabDictionary() const noexcept;

    /**
     * @brief Get the path to the mpv input.conf file.
     *
     * @return Path to the mpv input.conf file.
     */
    [[nodiscard]]
    const QString &mpvInputConfig() const noexcept;

    /**
     * @brief Get the path to the history directory.
     *
     * @return Path to the history directory.
     */
    [[nodiscard]]
    const QString &historyDir() const noexcept;

private:
    /* Path to the program executable */
    const QString m_program{DirectoryUtils::getProgramDirectory()};

    /* Path to the Anki configuration file */
    const QString m_ankiConfig{DirectoryUtils::getAnkiConfig()};

    /* Path to the configuration directory  */
    const QString m_config{DirectoryUtils::getConfigDir()};

    /* Path to the dictionary DB file */
    const QString m_dictionaryDb{DirectoryUtils::getDictionaryDb()};

    /* Path to the dictionary resource directory */
    const QString m_dictionaryResource{
        DirectoryUtils::getDictionaryResourceDir()
    };

    /* Path to the MeCab dictionary */
    const QString m_mecabDictionary{DirectoryUtils::getMecabDictionary()};

    /* Path to the mpv input.conf file */
    const QString m_mpvInputConfig{DirectoryUtils::getMpvInputConfig()};

    /* Path to the history directory */
    const QString m_historyDir{DirectoryUtils::getHistoryDir()};
};
