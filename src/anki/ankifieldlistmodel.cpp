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

#include "anki/ankifieldlistmodel.h"

#include <QJsonArray>
#include <QJsonObject>

AnkiFieldListModel::AnkiFieldListModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

int AnkiFieldListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_items.count();
}

QVariant AnkiFieldListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_items.count())
    {
        return QVariant();
    }

    const AnkiField &item = m_items[index.row()];
    switch (role)
    {
        case NameRole:
            return item.name;
        case ValueRole:
            return item.value;
    }

    return QVariant();
}

bool AnkiFieldListModel::setData(
    const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_items.count())
    {
        return false;
    }

    AnkiField &item = m_items[index.row()];
    switch (role)
    {
        case NameRole:
            item.name = value.toString();
            break;

        case ValueRole:
            item.value = value.toString();
            break;

        default:
            return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> AnkiFieldListModel::roleNames() const
{
    return QHash<int, QByteArray>{
        {NameRole, "name"},
        {ValueRole, "value"},
    };
}

const QList<AnkiField> &AnkiFieldListModel::items() const noexcept
{
    return m_items;
}

bool AnkiFieldListModel::setItems(const QJsonValue &value)
{
    switch (value.type())
    {
        case QJsonValue::Array:
            setItems(value.toArray());
            return true;

        case QJsonValue::Object:
            setItems(value.toObject());
            return true;

        default:
            return false;
    }
}

void AnkiFieldListModel::setItems(const QJsonArray &array)
{
    qsizetype EXPECTED_KV_ARRAY_SIZE = 2;
    qsizetype INDEX_NAME = 0;
    qsizetype INDEX_VALUE = 1;

    beginResetModel();
    m_items.clear();
    for (QJsonValueConstRef value : array)
    {
        if (!value.isArray())
        {
            continue;
        }
        QJsonArray kv = value.toArray();
        if (kv.size() != EXPECTED_KV_ARRAY_SIZE)
        {
            continue;
        }
        else if (!kv[INDEX_NAME].isString() || !kv[INDEX_VALUE].isString())
        {
            continue;
        }
        m_items.emplaceBack(AnkiField{
            .name = kv[INDEX_NAME].toString(),
            .value = kv[INDEX_VALUE].toString(),
        });
    }
    endResetModel();
}

void AnkiFieldListModel::setItems(const QJsonObject &object)
{
    beginResetModel();
    m_items.clear();
    for (QJsonObject::const_iterator it = std::cbegin(object);
        it != std::cend(object);
        ++it)
    {
        if (!it->isString())
        {
            continue;
        }
        m_items.emplaceBack(AnkiField{
            .name = it.key(),
            .value = it->toString(),
        });
    }
    endResetModel();
}

void AnkiFieldListModel::setItems(const QList<AnkiField> &items)
{
    beginResetModel();
    m_items = items;
    endResetModel();
}

void AnkiFieldListModel::setFields(const QStringList &fields)
{
    beginResetModel();
    m_items.clear();
    for (const QString &field : fields)
    {
        m_items.emplaceBack(AnkiField{
            .name = field,
            .value = {},
        });
    }
    endResetModel();
}

void AnkiFieldListModel::clear()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}

void AnkiFieldListModel::clearValues()
{
    beginResetModel();
    for (AnkiField &item : m_items)
    {
        item.value.clear();
    }
    endResetModel();
}

QJsonArray AnkiFieldListModel::toJsonArray() const
{
    QJsonArray array;
    for (const AnkiField &item : m_items)
    {
        QJsonArray kv;
        kv.append(item.name);
        kv.append(item.value);
        array.append(std::move(kv));
    }
    return array;
}
