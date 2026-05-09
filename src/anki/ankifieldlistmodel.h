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

#include <QJsonValue>
#include <QList>

#include "anki/ankifield.h"

/**
 * @brief A model of audio sources for interoperability with QML.
 */
class AnkiFieldListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief Roles corresponding to each element in an AudioSource.
     */
    enum ItemRoles
    {
        NameRole = Qt::UserRole + 1,
        ValueRole,
    };
    Q_ENUM(ItemRoles)

    explicit AnkiFieldListModel(QObject *parent = nullptr);
    virtual ~AnkiFieldListModel() = default;

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
     * @return The underlying audio sources container.
     */
    const QList<AnkiField> &items() const noexcept;

    /**
     * @brief Set the items of this model from a JSON value.
     * The JSON value can be an array of arrays where each subarray has two
     * elements of the form ["name", "value"].
     * The JSON value can be an object where the key is the field name and the
     * value is the field value.
     *
     * @param value The JSON value to set.
     * @return true if the model was updated,
     * @return false if the value couldn't be used.
     */
    bool setItems(const QJsonValue &value);

    /**
     * @brief Set the items of this model from a JSON array of arrays where each
     * subarray has two elements of the form ["name", "value"].
     *
     * @param value The JSON array to set.
     */
    void setItems(const QJsonArray &array);

    /**
     * @brief Set the items of this model from a JSON object where the key is
     * the field name and the value is the field value.
     *
     * @param value The JSON object to set.
     */
    void setItems(const QJsonObject &object);

    /**
     * @brief Set the items of this model from a list of AnkiField.
     *
     * @param items The AnkiFields to set.
     */
    void setItems(const QList<AnkiField> &items);

    /**
     * @brief Set the items of this model from a list of field names.
     *
     * @param fields The list of field names.
     */
    Q_INVOKABLE void setFields(const QStringList &fields);

    /**
     * @brief Clears all rows in the model.
     */
    Q_INVOKABLE void clear();

    /**
     * @brief Clear the value column in the model.
     */
    Q_INVOKABLE void clearValues();

    /**
     * @brief Get the field list as a JSON array of arrays where each element is
     * of the form ["name", "value"].
     *
     * @return The JSON array.
     */
    [[nodiscard]]
    QJsonArray toJsonArray() const;

private:
    /* The ordered backing list of items */
    QList<AnkiField> m_items;
};
