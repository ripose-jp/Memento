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

#include <QAbstractListModel>

#include <QList>

#include "dict/data/data.h"

/**
 * @brief A model of dictionaries for interoperability with QML.
 */
class DictionaryInfoModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief Roles corresponding to each element in an DictionaryItem.
     */
    enum ItemRoles
    {
        IdRole = Qt::UserRole + 1,
        NameRole,
        EnabledRole,
    };

    explicit DictionaryInfoModel(QObject *parent = nullptr);
    virtual ~DictionaryInfoModel() = default;

    /**
     * @brief Returns the number of rows under the parent.
     *
     * @param parent The parent to get the number of rows under.
     * @return The number of rows below parent.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Gets an element from the model.
     *
     * @param index The index of the row.
     * @param role The role to get from that row.
     * @return The requested role. Empty QVariant on error.
     */
    QVariant data(
        const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Sets the data role at the given index.
     *
     * @param index The index/row to set.
     * @param value The value to set.
     * @param role The data role to update.
     * @return true if the data was successfully set,
     * @return false otherwise.
     */
    bool setData(
        const QModelIndex &index,
        const QVariant &value,
        int role = Qt::EditRole) override;

    /**
     * @brief The name of each role supported by this model.
     *
     * @return A hashtable of roles and names.
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Gets the underlying container.
     *
     * @return The underlying dictionary item container.
     */
    const QList<DictionaryInfo *> &items() const noexcept;

    /**
     * @brief Appends an to the model at the end.
     *
     * @param item The item to append.
     */
    void appendItem(DictionaryInfo *item);

    /**
     * @brief Set the items in this model. Overwrites existing and takes
     * ownership.
     *
     * @param items The items to set.
     */
    void setItems(QList<DictionaryInfo *> &&items);

    /**
     * @brief Removes an dictionary item at the index given.
     *
     * @param index The index of the dictionary item to remove.
     */
    void remove(int index);

    /**
     * @brief Moves an dictionary item from one location to another.
     *
     * @param from The index of the item to move.
     * @param to The index to move it to.
     * @return true if the move occurred,
     * @return false if nothing happened.
     */
    bool move(int from, int to);

    /**
     * @brief Clears all rows in the model.
     */
    void clear();

private:
    QList<DictionaryInfo *> m_items;
};
