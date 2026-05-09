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

#include "subtitle/subtitlelistmodel.h"

#include <QRegularExpression>

#include "state/context.h"

SubtitleListModel::SubtitleListModel(Context *context, QObject *parent) :
    QAbstractListModel(parent),
    m_context(context)
{

}

QItemSelectionModel *SubtitleListModel::selectionModel() const noexcept
{
    return m_selectionModel;
}

int SubtitleListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_items.size();
}

QVariant SubtitleListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_items.size()))
    {
        return QVariant();
    }

    const SubtitleEntry &item = m_items[index.row()];
    switch (role)
    {
        case TextRole:
            return item.text;
        case StartRole:
            return item.start;
        case EndRole:
            return item.end;
    }

    return QVariant();
}

bool SubtitleListModel::setData(
    const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_items.size()))
    {
        return false;
    }

    SubtitleEntry &item = m_items[index.row()];
    switch (role)
    {
        case TextRole:
        {
            QString text = value.toString();
            if (item.text == text)
            {
                return true;
            }
            item.text = std::move(text);
            break;
        }

        case StartRole:
        {
            double start = value.toDouble();
            if (item.start == start)
            {
                return true;
            }
            item.start = start;
            break;
        }

        case EndRole:
        {
            double end = value.toDouble();
            if (item.end == end)
            {
                return true;
            }
            item.end = end;
            break;
        }

        default:
            return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> SubtitleListModel::roleNames() const
{
    return QHash<int, QByteArray>{
        {TextRole, "text"},
        {StartRole, "start"},
        {EndRole, "end"},
    };
}

qsizetype SubtitleListModel::addSubtitle(
    const QString &text, double start, double end)
{
    constexpr double TIME_DELTA = 0.0001;

    if (m_blockAdds)
    {
        return -1;
    }

    auto itemIt = std::lower_bound(
        std::begin(m_items),
        std::end(m_items),
        start - TIME_DELTA,
        [] (const SubtitleEntry &entry, double start) -> bool
        {
            return entry.start < start;
        }
    );
    while (itemIt != std::end(m_items) && itemIt->start <= start + TIME_DELTA)
    {
        if (itemIt->text == text)
        {
            return std::distance(std::begin(m_items), itemIt);
        }
        itemIt = std::next(itemIt);
    }

    std::ptrdiff_t index = std::distance(std::begin(m_items), itemIt);
    beginInsertRows(QModelIndex(), index, index);
    m_items.insert(
        itemIt,
        SubtitleEntry{
            .text = text,
            .start = start,
            .end = end,
        }
    );
    endInsertRows();
    return index;
}

void SubtitleListModel::selectPosition(double position)
{
    constexpr double TIME_DELTA = 0.0001;

    m_selectionModel->clear();

    position += TIME_DELTA;
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        SubtitleEntry &item = m_items[i];

        bool shouldSelect = item.start < position && position < item.end;
        if (shouldSelect)
        {
            m_selectionModel->select(
                createIndex(i, 0), QItemSelectionModel::Select
            );
        }
    }

    QModelIndexList selectedIndices = m_selectionModel->selectedRows();
    if (selectedIndices.size() > 0)
    {
        m_selectionModel->setCurrentIndex(
            selectedIndices.front(), QItemSelectionModel::NoUpdate
        );
        emit positionSelected(
            selectedIndices.front().row(), selectedIndices.back().row()
        );
    }
}

QList<int> SubtitleListModel::find(QString str, bool ignoreWhitespace) const
{
    static const QRegularExpression REGEX_REMOVE_WHITESPACE("\\s*");

    if (ignoreWhitespace)
    {
        str.remove(REGEX_REMOVE_WHITESPACE);
    }
    if (str.isEmpty())
    {
        return {};
    }

    QRegularExpression removeRegex(m_context->settings()->searchRemoveRegex());

    QList<int> results;
    for (size_t i = 0; i < m_items.size(); ++i)
    {
        QString subtitleText = m_items[i].text;
        subtitleText.remove(removeRegex);
        if (ignoreWhitespace)
        {
            subtitleText.remove(REGEX_REMOVE_WHITESPACE);
        }
        if (subtitleText.contains(str, Qt::CaseInsensitive))
        {
            results.emplaceBack(i);
        }
    }
    return results;
}

void SubtitleListModel::clear()
{
    m_selectionModel->clear();
    m_blockAdds = false;
    beginResetModel();
    m_items.clear();
    endResetModel();
}

void SubtitleListModel::setItems(std::vector<SubtitleEntry> &&items)
{
    m_selectionModel->clear();
    m_blockAdds = true;
    beginResetModel();
    m_items = std::move(items);
    endResetModel();
}

const std::vector<SubtitleEntry> &SubtitleListModel::items() const noexcept
{
    return m_items;
}
