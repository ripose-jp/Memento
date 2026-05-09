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

#include <vector>

#include <QItemSelectionModel>

#include "subtitle/subtitleentry.h"

class Context;

/**
 * @brief A model of subtitles.
 */
class SubtitleListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(
        QItemSelectionModel *selectionModel
        READ selectionModel
        CONSTANT
    )

public:
    enum ItemRoles
    {
        TextRole = Qt::UserRole + 1,
        StartRole,
        EndRole,
    };
    Q_ENUM(ItemRoles)

    /**
     * @brief Construct a SubtitleListModel.
     *
     * @param context The application context.
     * @param parent The parent of this object.
     */
    explicit SubtitleListModel(Context *context, QObject *parent = nullptr);
    virtual ~SubtitleListModel() = default;

    /**
     * @brief Get the selection model.
     *
     * @return The item selection model.
     */
    [[nodiscard]]
    QItemSelectionModel *selectionModel() const noexcept;


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
     * @brief Add a subtitle to the model if it doesn't already exist.
     *
     * @param text Text of the subtitle.
     * @param start Start time of the subtitle.
     * @param end End time of the subtitle.
     * @return The row index of the subtitle.
     */
    Q_INVOKABLE qsizetype addSubtitle(
        const QString &text, double start, double end);

    /**
     * @brief Select all items a time position overlaps with and deselects all
     * subtitles that don't overlap.
     *
     * @param position The position to test.
     */
    Q_INVOKABLE void selectPosition(double position);

    /**
     * @brief Get a list of rows that contain a search string.
     *
     * @param str The string to search for.
     * @param ignoreWhitespace true if results should match across whitespace,
     * false otherwise.
     * @return A list of rows containing the search string.
     */
    [[nodiscard]]
    Q_INVOKABLE QList<int> find(
        QString str, bool ignoreWhitespace = false) const;

    /**
     * @brief Clear out all elements in the model.
     */
    Q_INVOKABLE void clear();

    /**
     * @brief Set the items of the model and block adds.
     *
     * @param entries The entries to set.
     */
    void setItems(std::vector<SubtitleEntry> &&entries);

    /**
     * @brief Get the backing list of items.
     *
     * @return The backing list of items.
     */
    [[nodiscard]]
    const std::vector<SubtitleEntry> &items() const noexcept;

signals:
    /**
     * @brief Emitted when a new position is selected.
     *
     * @param start The starting row selected (inclusive).
     * @param end The ending row selected (inclusive).
     */
    void positionSelected(int start, int end);

private:
    /* The application context */
    Context *m_context{nullptr};

    /* The selection model */
    QItemSelectionModel *m_selectionModel{new QItemSelectionModel(this, this)};

    /* true to block addSubtitle() calls, false to accept */
    bool m_blockAdds{false};

    /* List of items sorted by start time */
    std::vector<SubtitleEntry> m_items;
};
