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

#include "setting/audiosourcemodel.h"

AudioSourceModel::AudioSourceModel(QObject *parent) : QAbstractListModel(parent)
{

}

int AudioSourceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_items.count();
}

QVariant AudioSourceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count())
    {
        return QVariant();
    }

    const AudioSource &item = m_items[index.row()];
    switch (role)
    {
        case NameRole:
            return item.name;
        case UrlRole:
            return item.url;
        case TypeRole:
            return item.type;
        case SkipHashRole:
            return item.skipHash;
    }

    return QVariant();
}

bool AudioSourceModel::setData(
    const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_items.count())
    {
        return false;
    }

    AudioSource &item = m_items[index.row()];
    switch (role)
    {
        case NameRole:
            item.name = value.toString();
            break;

        case UrlRole:
            item.url = value.toString();
            break;

        case TypeRole:
            item.type = static_cast<Setting::AudioSourceType>(value.toInt());
            break;

        case SkipHashRole:
            item.skipHash = value.toString();
            break;

        default:
            return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> AudioSourceModel::roleNames() const
{
    return QHash<int, QByteArray>{
        {NameRole, "name"},
        {UrlRole, "url"},
        {TypeRole, "type"},
        {SkipHashRole, "skipHash"},
    };
}

const QList<AudioSource> &AudioSourceModel::items() const noexcept
{
    return m_items;
}

void AudioSourceModel::appendItem(const AudioSource &item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_items.emplaceBack(item);
    endInsertRows();
}

void AudioSourceModel::appendItem(
    const QString &name,
    const QString &url,
    Setting::AudioSourceType type,
    const QString &skipHash)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count());
    m_items.emplaceBack(AudioSource{
        .name = name,
        .url = url,
        .type = type,
        .skipHash = skipHash
    });
    endInsertRows();
}

void AudioSourceModel::remove(int index)
{
    if (index < 0 || index >= m_items.count())
    {
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    m_items.removeAt(index);
    endRemoveRows();
}

void AudioSourceModel::move(int from, int to)
{
    if (from == to ||
        from < 0 || from >= m_items.count() ||
        to < 0 || to >= m_items.count())
    {
        return;
    }

    /* Destination is the index where the row will be BEFORE the move */
    int destination = (to > from) ? to + 1 : to;
    if (beginMoveRows(QModelIndex(), from, from, QModelIndex(), destination))
    {
        m_items.move(from, to);
        endMoveRows();
    }
}

void AudioSourceModel::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}
