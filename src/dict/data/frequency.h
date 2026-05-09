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
 * @brief Describes a frequency dictionary entry.
 */
class Frequency : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        DictionaryInfo *dictionaryInfo
        READ dictionaryInfo
        WRITE setDictionaryInfo
        NOTIFY dictionaryInfoChanged
    )

    Q_PROPERTY(
        QString frequency
        READ frequency
        WRITE setFrequency
        NOTIFY frequencyChanged
    )

    friend class DictionarySearch;

public:
    Frequency(QObject *parent = nullptr);
    virtual ~Frequency();

    /**
     * @brief Creates a deep copy of this object.
     *
     * @param parent The parent of the new object.
     * @return The cloned Frequency.
     */
    Frequency *clone(QObject *parent = nullptr) const;

    /**
     * @brief Gets the dictionary info for this Frequency.
     *
     * @return The dictionary info for this Frequency.
     */
    [[nodiscard]]
    DictionaryInfo *dictionaryInfo() const noexcept;

    /**
     * @brief Sets the dictionary info for this Frequency. Takes ownership.
     *
     * @param value The dictionary info to set.
     */
    void setDictionaryInfo(DictionaryInfo *value);

    /**
     * @brief Gets the frequency of the expression/kanji/etc.
     *
     * @return The frequency of the expression/kanji/etc.
     */
    [[nodiscard]]
    const QString &frequency() const noexcept;

    /**
     * @brief Sets the frequency of the expression/kanji/etc.
     *
     * @param value The new frequency value.
     */
    void setFrequency(const QString &value);

signals:
    /**
     * @brief Emitted when the dictionary info is changed.
     *
     * @param value The new dictionary info.
     */
    void dictionaryInfoChanged(DictionaryInfo *value);

    /**
     * @brief Emitted when the frequency is changed.
     *
     * @param value The new frequency value.
     */
    void frequencyChanged(const QString &value);

protected:
    /* The dictionary this belongs to */
    DictionaryInfo *m_dictionaryInfo{nullptr};

    /* The frequency of the expression/kanji/etc. */
    QString m_frequency;
};
