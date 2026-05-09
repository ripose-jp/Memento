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

#include "dict/data/dictionaryinfo.h"

class DictionarySearch;

/**
 * @brief Describes a pitch dictionary entry.
 */
class Pitch : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        DictionaryInfo *dictionaryInfo
        READ dictionaryInfo
        WRITE setDictionaryInfo
        NOTIFY dictionaryInfoChanged
    )

    Q_PROPERTY(
        QStringList mora
        READ mora
        WRITE setMora
        NOTIFY moraChanged
    )

    Q_PROPERTY(
        QList<int> positions
        READ positions
        WRITE setPositions
        NOTIFY positionsChanged
    )

    friend class DictionarySearch;

public:
    Pitch(QObject *parent = nullptr);
    virtual ~Pitch();

    /**
     * @brief Creates a deep copy of this object.
     *
     * @param parent The parent of the new object.
     * @return The cloned Pitch.
     */
    Pitch *clone(QObject *parent = nullptr) const;

    /**
     * @brief Gets the dictionary info for this Pitch.
     *
     * @return The dictionary info for this Pitch.
     */
    [[nodiscard]]
    DictionaryInfo *dictionaryInfo() const noexcept;

    /**
     * @brief Sets the dictionary info for this Pitch. Takes ownership.
     *
     * @param value The dictionary info to set.
     */
    void setDictionaryInfo(DictionaryInfo *value);

    /**
     * @brief Get the mora for this pitch.
     *
     * @return The list of mora.
     */
    [[nodiscard]]
    const QStringList &mora() const noexcept;

    /**
     * @brief Set the mora for this pitch.
     *
     * @param value The list of mora.
     */
    void setMora(const QStringList &value);

    /**
     * @brief Get the positions for this pitch.
     *
     * @return The positions for this pitch.
     */
    [[nodiscard]]
    const QList<int> &positions() const noexcept;

    /**
     * @brief Set the position for this pitch.
     *
     * @param value The positions for this pitch.
     */
    void setPositions(const QList<int> &value);

signals:
    /**
     * @brief Emitted when the dictionary info is changed.
     *
     * @param value The new dictionary info.
     */
    void dictionaryInfoChanged(DictionaryInfo *value);

    /**
     * @brief Emitted when the mora are changed.
     */
    void moraChanged();

    /**
     * @brief Emitted when the positions are changed.
     */
    void positionsChanged();

protected:
    /* The dictionary this belongs to */
    DictionaryInfo *m_dictionaryInfo{nullptr};

    /* A list of all the mora that appear in the expression. */
    QStringList m_mora;

    /* The pitch "position". See https://i.imgur.com/oEjxhWU.png */
    QList<int> m_positions;
};
