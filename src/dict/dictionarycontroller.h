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

#include "dict/dictionary.h"

#include "dict/dictionaryinfomodel.h"
#include "setting/settings.h"

/**
 * @brief Handles adding, removing, enabling, and ordering dictionaries in the
 * database.
 */
class DictionaryController : public Dictionary
{
    Q_OBJECT

    Q_PROPERTY(
        DictionaryInfoModel *dictionaries
        READ dictionaries
        CONSTANT
    )

    Q_PROPERTY(
        QString lastError
        READ lastError
        NOTIFY lastErrorChanged
    )

public:
    /**
     * @brief Create a dictionary controller.
     *
     * @param settings The application settings.
     * @param parent The parent of this object.
     */
    DictionaryController(Settings *settings, QObject *parent = nullptr);
    virtual ~DictionaryController();

    /**
     * @brief Add a dictionary to the database.
     *
     * @param path The path to the dictionary to add.
     */
    Q_INVOKABLE void addDictionary(QString path);

    /**
     * @brief Remove a dictionary from the database.
     *
     * @param id The ID of the dictionary to remove.
     * @return An awaitable task.
     */
    Q_INVOKABLE void removeDictionary(int64_t id);

    /**
     * @brief Reorders dictionary priority.
     *
     * @param from The index of the dictionary to move.
     * @param to The index to move the dictionary to.
     */
    Q_INVOKABLE void moveDictionary(int from, int to);

    /**
     * @brief Get the dictionaries in the database.
     *
     * @return The dictionaries in the database.
     */
    [[nodiscard]]
    DictionaryInfoModel *dictionaries() const noexcept;

    /**
     * @brief Get the last error from this instance.
     *
     * @return The last error from this dictionary controller.
     */
    [[nodiscard]]
    const QString &lastError() const noexcept;

signals:
    /**
     * @brief Emitted when a new error is set.
     *
     * @param error The last error set.
     */
    void lastErrorChanged(const QString &error);

private slots:
    /**
     * @brief Refreshes the dictionaries model.
     */
    void updateDictionaries();

    /**
     * @brief Updates the order map based on the ordering of dictionaries.
     */
    void updateOrder();

    /**
     * @brief Updates the order in settings.
     */
    void writeOrder();

    /**
     * @brief Updates the enabled status of the dictionaries in the database.
     *
     * @param info The dictionary info that changed.
     * @return An awaitable task.
     */
    void handleEnabledChanged(const DictionaryInfo *info);

private:
    /**
     * @brief Set the last error string.
     *
     * @param error The error string to set.
     */
    void setLastError(const QString &error);

    /* The application settings */
    Settings *m_settings{nullptr};

    /* The model for dictionaries */
    DictionaryInfoModel *m_dictionaries{new DictionaryInfoModel(this)};

    /* A mapping of dictionary ID to order. Lower numbers is higher order. */
    QHash<int64_t, qsizetype> m_order;

    /* The last error from this instance */
    QString m_lastError;
};

