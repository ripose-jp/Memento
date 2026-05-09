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

#include "dict/dictionaryinfomodel.h"

DictionaryInfoModel::DictionaryInfoModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

int DictionaryInfoModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_items.count();
}

QVariant DictionaryInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count())
    {
        return QVariant();
    }

    const DictionaryInfo *item = m_items[index.row()];
    switch (role)
    {
        case IdRole:
            return QVariant::fromValue(item->id());
        case NameRole:
            return item->name();
        case EnabledRole:
            return item->enabled();
    }

    return QVariant();
}

bool DictionaryInfoModel::setData(
    const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_items.count())
    {
        return false;
    }

    DictionaryInfo *item = m_items[index.row()];
    switch (role)
    {
        case IdRole:
            item->setId(value.toLongLong());
            break;

        case NameRole:
            item->setName(value.toString());
            break;

        case EnabledRole:
            item->setEnabled(value.toBool());
            break;

        default:
            return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> DictionaryInfoModel::roleNames() const
{
    return QHash<int, QByteArray>{
        {IdRole, "id"},
        {NameRole, "name"},
        {EnabledRole, "enabled"},
    };
}

const QList<DictionaryInfo *> &DictionaryInfoModel::items() const noexcept
{
    return m_items;
}

void DictionaryInfoModel::appendItem(DictionaryInfo *item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    item->setParent(this);
    m_items.emplaceBack(item);
    endInsertRows();
}

void DictionaryInfoModel::setItems(QList<DictionaryInfo *> &&items)
{
    beginResetModel();
    qDeleteAll(m_items);
    m_items = std::move(items);
    endResetModel();
}

void DictionaryInfoModel::remove(int index)
{
    if (index < 0 || index >= m_items.count())
    {
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    m_items[index]->deleteLater();
    m_items.removeAt(index);
    endRemoveRows();
}

bool DictionaryInfoModel::move(int from, int to)
{
    if (from == to ||
        from < 0 || from >= m_items.count() ||
        to < 0 || to >= m_items.count())
    {
        return false;
    }

    /* Destination is the index where the row will be BEFORE the move */
    int destination = (to > from) ? to + 1 : to;
    if (beginMoveRows(QModelIndex(), from, from, QModelIndex(), destination))
    {
        m_items.move(from, to);
        endMoveRows();
    }
    else
    {
        return false;
    }

    return true;
}

void DictionaryInfoModel::clear()
{
    beginResetModel();
    qDeleteAll(m_items);
    m_items.clear();
    endResetModel();
}
