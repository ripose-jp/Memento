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

#include <QJsonArray>
#include <QQmlListProperty>

#include "dict/data/dictionaryinfo.h"
#include "dict/data/tag.h"

class DictionarySearch;

/**
 * @brief Describes a frequency dictionary entry.
 */
class TermDefinition : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        DictionaryInfo *dictionaryInfo
        READ dictionaryInfo
        WRITE setDictionaryInfo
        NOTIFY dictionaryInfoChanged
    )

    Q_PROPERTY(
        QQmlListProperty<Tag> tags
        READ tagsQml
        NOTIFY tagsChanged
    )

    Q_PROPERTY(
        QStringList rules
        READ rules
        WRITE setRules
        NOTIFY rulesChanged
    )

    Q_PROPERTY(
        QJsonArray glossary
        READ glossary
        WRITE setGlossary
        NOTIFY glossaryChanged
    )

    Q_PROPERTY(
        int score
        READ score
        WRITE setScore
        NOTIFY scoreChanged
    )

    Q_PROPERTY(
        bool selected
        READ selected
        WRITE setSelected
        NOTIFY selectedChanged
    )

    friend class DictionarySearchController;

public:
    TermDefinition(QObject *parent = nullptr);
    virtual ~TermDefinition();

    /**
     * @brief Creates a deep copy of this object.
     *
     * @param parent The parent of the new object.
     * @return The cloned TermDefinition.
     */
    TermDefinition *clone(QObject *parent = nullptr) const;

    /**
     * @brief Gets the dictionary info for this TermDefinition.
     *
     * @return The dictionary info for this TermDefinition.
     */
    [[nodiscard]]
    DictionaryInfo *dictionaryInfo() const noexcept;

    /**
     * @brief Sets the dictionary info for this TermDefinition. Takes ownership.
     *
     * @param value The dictionary info to set.
     */
    void setDictionaryInfo(DictionaryInfo *value);

    /**
     * @brief Gets the QML property for the tags.
     *
     * @return The QML property for the tags.
     */
    [[nodiscard]]
    QQmlListProperty<Tag> tagsQml();

    /**
     * @brief Gets the tags associated with this definition.
     *
     * @return The tags associated with this definition.
     */
    [[nodiscard]]
    const QList<Tag *> &tags() const noexcept;

    /**
     * @brief Sets the tags associated with this definition. Takes ownership.
     *
     * @param value The new list of tags.
     */
    void setTags(const QList<Tag *> &value);

    /**
     * @brief Append a tag to the tags. Takes ownership.
     *
     * @param value The tag to append.
     */
    void appendTags(Tag *value);

    /**
     * @brief Get the rules of this definition.
     *
     * @return The rules of this definition.
     */
    [[nodiscard]]
    const QStringList &rules() const noexcept;

    /**
     * @brief Set the rules of this definition.
     *
     * @param value The new rules of this definition.
     */
    void setRules(const QStringList &value);

    /**
     * @brief Get the glossary of this definition.
     *
     * @return The glossary of this definition.
     */
    [[nodiscard]]
    const QJsonArray &glossary() const noexcept;

    /**
     * @brief Set the glossary of this definition.
     *
     * @param value The new glossary.
     */
    void setGlossary(const QJsonArray &value);

    /**
     * @brief Get the score of this definition.
     *
     * @return The score of this definition.
     */
    [[nodiscard]]
    int score() const noexcept;

    /**
     * @brief Set the score of this definition.
     *
     * @param value The new score of this definition.
     */
    void setScore(int value);

    /**
     * @brief Get if this definition is selected.
     *
     * @return true if this definition is selected,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool selected() const noexcept;

    /**
     * @brief Set if this definition is selected.
     *
     * @param value true if selected, false otherwise.
     */
    void setSelected(bool value);

signals:
    /**
     * @brief Emitted when the dictionary info is changed.
     *
     * @param value The new dictionary info.
     */
    void dictionaryInfoChanged(DictionaryInfo *value);

    /**
     * @brief Emitted when the tags are changed.
     */
    void tagsChanged();

    /**
     * @brief Emitted when the rules are changed.
     */
    void rulesChanged();

    /**
     * @brief Emitted when the glossary is changed.
     */
    void glossaryChanged();

    /**
     * @brief Emitted when the score is changed.
     *
     * @param value The new value.
     */
    void scoreChanged(int value);

    /**
     * @brief Emitted when selected is changed.
     *
     * @param value The new value.
     */
    void selectedChanged(bool value);

protected:
    /* The dictionary this belongs to */
    DictionaryInfo *m_dictionaryInfo{nullptr};

    /* A list of the tags associated with this entry. */
    QList<Tag *> m_tags;

    /* A list of the rules associated with this entry. */
    QStringList m_rules;

    /* A list of glossary entries for this definition. */
    QJsonArray m_glossary;

    /* Score of this definition.
     * Used for ordering. More common entries have a larger score. */
    int m_score{0};

    /* true if this definition is selected, false otherwise */
    bool m_selected{false};
};
