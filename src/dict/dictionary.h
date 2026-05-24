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

#include "dict/databasemanager.h"

/**
 * @brief A common class to handle dictionary database interaction.
 */
class Dictionary : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool modifyingDatabase
        READ modifyingDatabase
        NOTIFY modifyingDatabaseChanged
    )

public:
    Dictionary(QObject *parent = nullptr);
    virtual ~Dictionary();

    /**
     * @brief Create the static database instance.
     */
    static void createDatabaseInstance();

    /**
     * @brief Destroy the static database instance.
     */
    static void destroyDatabaseInstance();

    /**
     * @brief Get if the database is being modified.
     *
     * @return true if the database is being modified,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool modifyingDatabase() const noexcept;

protected slots:
    /**
     * @brief Set if the database is being modified.
     *
     * @param value true if the database is being modified, false otherwise.
     */
    void setModifyingDatabase(bool value);

signals:
    /**
     * @brief Emitted when the database modification state changes.
     *
     * @param value true if the database is being modified, false otherwise.
     */
    void modifyingDatabaseChanged(bool value);

protected:
    /* The shared database instance of Dictionaries */
    static inline DatabaseManager *m_db{nullptr};

    /* True if modifying the database, false otherwise */
    bool m_modifyingDatabase{false};
};
