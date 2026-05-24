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
 * @brief Describes a tag from a dictionary.
 */
class Tag : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        DictionaryInfo *dictionaryInfo
        READ dictionaryInfo
        WRITE setDictionaryInfo
        NOTIFY dictionaryInfoChanged
    )

    Q_PROPERTY(
        QString name
        READ name
        WRITE setName
        NOTIFY nameChanged
    )

    Q_PROPERTY(
        QString category
        READ category
        WRITE setCategory
        NOTIFY categoryChanged
    )

    Q_PROPERTY(
        QString notes
        READ notes
        WRITE setNotes
        NOTIFY notesChanged
    )

    Q_PROPERTY(
        int order
        READ order
        WRITE setOrder
        NOTIFY orderChanged
    )

    Q_PROPERTY(
        int score
        READ score
        WRITE setScore
        NOTIFY scoreChanged
    )

    Q_PROPERTY(
        QString value
        READ value
        WRITE setValue
        NOTIFY valueChanged
    )

    friend class DictionarySearchController;

public:
    Tag(QObject *parent = nullptr);
    virtual ~Tag();

    /**
     * @brief Creates a deep copy of this object.
     *
     * @param parent The parent of the new object.
     * @return The cloned Tag.
     */
    Tag *clone(QObject *parent = nullptr) const;

    /**
     * @brief Gets the dictionary info for this Tag.
     *
     * @return The dictionary info for this Tag.
     */
    [[nodiscard]]
    DictionaryInfo *dictionaryInfo() const noexcept;

    /**
     * @brief Sets the dictionary info for this Tag. Takes ownership.
     *
     * @param value The dictionary info to set.
     */
    void setDictionaryInfo(DictionaryInfo *value);

    /**
     * @brief Get the name of the tag.
     *
     * @return The name of the tag.
     */
    [[nodiscard]]
    const QString &name() const noexcept;

    /**
     * @brief Set the name of the tag.
     *
     * @param value The new name.
     */
    void setName(const QString &value);

    /**
     * @brief Get the category of the tag.
     *
     * @return The category of the tag.
     */
    [[nodiscard]]
    const QString &category() const noexcept;

    /**
     * @brief Set the category of the tag.
     *
     * @param value The new category.
     */
    void setCategory(const QString &value);

    /**
     * @brief Get the notes of the tag.
     *
     * @return The notes of the tag.
     */
    [[nodiscard]]
    const QString &notes() const noexcept;

    /**
     * @brief Set the notes of the tag.
     *
     * @param value The new notes.
     */
    void setNotes(const QString &value);

    /**
     * @brief Get the order of the tag.
     *
     * @return The new order.
     */
    [[nodiscard]]
    int order() const noexcept;

    /**
     * @brief Set the order of the tag.
     *
     * @param value The new order.
     */
    void setOrder(int value);

    /**
     * @brief Get the score of the tag.
     *
     * @return The score of the tag.
     */
    [[nodiscard]]
    int score() const noexcept;

    /**
     * @brief Set the score of the tag.
     *
     * @param value The new score.
     */
    void setScore(int value);

    /**
     * @brief Get the value of the tag. Maybe empty.
     *
     * @return The value of the tag.
     */
    [[nodiscard]]
    const QString &value() const noexcept;

    /**
     * @brief Set the value of the tag.
     *
     * @param value The new value of the tag.
     */
    void setValue(const QString &value);

signals:
    /**
     * @brief Emitted when the dictionary info is changed.
     *
     * @param value The new dictionary info.
     */
    void dictionaryInfoChanged(DictionaryInfo *value);

    /**
     * @brief Emitted when the name is changed.
     *
     * @param value The new value.
     */
    void nameChanged(const QString &value);

    /**
     * @brief Emitted when the category is changed.
     *
     * @param value The new value.
     */
    void categoryChanged(const QString &value);

    /**
     * @brief Emitted when the notes are changed.
     *
     * @param value The new value.
     */
    void notesChanged(const QString &value);

    /**
     * @brief Emitted when the order is changed.
     *
     * @param value The new value.
     */
    void orderChanged(int value);

    /**
     * @brief Emitted when the score is changed.
     *
     * @param value The new value.
     */
    void scoreChanged(int value);

    /**
     * @brief Emitted when the value is changed.
     *
     * @param value The new value.
     */
    void valueChanged(const QString &value);

protected:
    /* The dictionary this belongs to */
    DictionaryInfo *m_dictionaryInfo{nullptr};

    /* The name of tag */
    QString m_name;

    /* The category of this tag */
    QString m_category;

    /* Further information about this tag */
    QString m_notes;

    /* Order of the tag. Used for ordering. */
    int m_order{0};

    /* Score of the tag. Used for ordering. */
    int m_score{0};

    /* The value of the tag. May be empty. */
    QString m_value;
};
