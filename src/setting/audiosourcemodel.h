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

#include "setting/audiosource.h"

/**
 * @brief A model of audio sources for interoperability with QML.
 */
class AudioSourceModel : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief Roles corresponding to each element in an AudioSource.
     */
    enum ItemRoles
    {
        NameRole = Qt::UserRole + 1,
        UrlRole,
        TypeRole,
        SkipHashRole
    };
    Q_ENUM(ItemRoles)

    explicit AudioSourceModel(QObject *parent = nullptr);
    virtual ~AudioSourceModel() = default;

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
    const QList<AudioSource> &items() const noexcept;

    /**
     * @brief Appends an audio source to the model at the end.
     *
     * @param item The item to append.
     */
    void appendItem(const AudioSource &item);

    /**
     * @brief Appends an item to the model.
     *
     * @param name The name of the audio source.
     * @param url The URL of the audio source
     * @param type The type of the audio source.
     * @param skipHash The MD5 skip hash of the audio source.
     */
    Q_INVOKABLE void appendItem(
        const QString &name,
        const QString &url,
        Setting::AudioSourceType type,
        const QString &skipHash);

    /**
     * @brief Removes an audio source at the index given.
     *
     * @param index The index of the audio source to remove.
     */
    Q_INVOKABLE void remove(int index);

    /**
     * @brief Moves an audio source from one location to another.
     *
     * @param from The index of the item to move.
     * @param to The index to move it to.
     */
    Q_INVOKABLE void move(int from, int to);

    /**
     * @brief Clears all rows in the model.
     */
    Q_INVOKABLE void clear();

private:
    QList<AudioSource> m_items;
};
