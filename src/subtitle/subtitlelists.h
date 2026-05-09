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

#include "subtitle/subtitlelistmodel.h"

/**
 * @brief Object holding the current lists for the subtitle tracks.
 */
class SubtitleLists : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        SubtitleListModel *primary
        READ primary
        NOTIFY primaryChanged
    )

    Q_PROPERTY(
        SubtitleListModel *secondary
        READ secondary
        NOTIFY secondaryChanged
    )

public:
    SubtitleLists(QObject *parent = nullptr);
    virtual ~SubtitleLists() = default;

    /**
     * @brief Get the primary subtitle list model.
     *
     * @return The primary subtitle list model, nullptr if no subtitle is
     * selected.
     */
    [[nodiscard]]
    SubtitleListModel *primary() const noexcept;

    /**
     * @brief Set the primary subtitle list model.
     *
     * @param value The primary subtitle list model.
     */
    void setPrimary(SubtitleListModel *value);

    /**
     * @brief Get the secondary subtitle list model.
     *
     * @return The secondary subtitle list model, nullptr if no subtitle is
     * selected.
     */
    [[nodiscard]]
    SubtitleListModel *secondary() const noexcept;

    /**
     * @brief Set the secondary subtitle list model.
     *
     * @param value The secondary subtitle list model.
     */
    void setSecondary(SubtitleListModel *value);

signals:
    /**
     * @brief Emitted when the primary subtitle list is changed.
     *
     * @param value The new subtitle list.
     */
    void primaryChanged(SubtitleListModel *value);

    /**
     * @brief Emitted when the secondary subtitle list is changed.
     *
     * @param value The new subtitle list.
     */
    void secondaryChanged(SubtitleListModel *value);

private:
    /* Pointer to the primary subtitle list. Does not have ownership. */
    SubtitleListModel *m_primary{nullptr};

    /* Pointer to the secondary subtitle list. Does not have ownership. */
    SubtitleListModel *m_secondary{nullptr};
};
