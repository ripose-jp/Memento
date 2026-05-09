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

#include <QQmlListProperty>

#include "dict/data/dictionaryinfo.h"
#include "dict/data/tag.h"

class DictionarySearch;

/**
 * @brief Describes a kanji definition.
 */
class KanjiDefinition : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        DictionaryInfo *dictionaryInfo
        READ dictionaryInfo
        WRITE setDictionaryInfo
        NOTIFY dictionaryInfoChanged
    )

    Q_PROPERTY(
        QStringList onyomi
        READ onyomi
        WRITE setOnyomi
        NOTIFY onyomiChanged
    )

    Q_PROPERTY(
        QStringList kunyomi
        READ kunyomi
        WRITE setKunyomi
        NOTIFY kunyomiChanged
    )

    Q_PROPERTY(
        QStringList glossary
        READ glossary
        WRITE setGlossary
        NOTIFY glossaryChanged
    )

    Q_PROPERTY(
        QQmlListProperty<Tag> tags
        READ tagsQml
        NOTIFY tagsChanged
    )

    Q_PROPERTY(
        QQmlListProperty<Tag> stats
        READ statsQml
        NOTIFY statsChanged
    )

    Q_PROPERTY(
        QQmlListProperty<Tag> classes
        READ classesQml
        NOTIFY classesChanged
    )

    Q_PROPERTY(
        QQmlListProperty<Tag> codes
        READ codesQml
        NOTIFY codesChanged
    )

    Q_PROPERTY(
        QQmlListProperty<Tag> indices
        READ indicesQml
        NOTIFY indicesChanged
    )

    friend class DictionarySearch;

public:
    KanjiDefinition(QObject *parent = nullptr);
    virtual ~KanjiDefinition();

    /**
     * @brief Creates a deep copy of this object.
     *
     * @param parent The parent of the new object.
     * @return The cloned KanjiDefinition.
     */
    KanjiDefinition *clone(QObject *parent = nullptr) const;

    /**
     * @brief Gets the dictionary info for this KanjiDefinition.
     *
     * @return The dictionary info for this KanjiDefinition.
     */
    [[nodiscard]]
    DictionaryInfo *dictionaryInfo() const noexcept;

    /**
     * @brief Sets the dictionary info for this KanjiDefinition. Takes ownership.
     *
     * @param value The dictionary info to set.
     */
    void setDictionaryInfo(DictionaryInfo *value);

    /**
     * @brief Get the onyomi of the kanji.
     *
     * @return The onyomi of the kanji.
     */
    [[nodiscard]]
    const QStringList &onyomi() const noexcept;

    /**
     * @brief Set the onyomi of the kanji.
     *
     * @param value The new onyomi.
     */
    void setOnyomi(const QStringList &value);

    /**
     * @brief Set the kunyomi of the kanji.
     *
     * @return The kunyomi of the kanji.
     */
    [[nodiscard]]
    const QStringList &kunyomi() const noexcept;

    /**
     * @brief Set the kunyomi of the kanji.
     *
     * @param value The new kunyomi.
     */
    void setKunyomi(const QStringList &value);

    /**
     * @brief Get the glossary of the kanji.
     *
     * @return The glossary of the kanji.
     */
    [[nodiscard]]
    const QStringList &glossary() const noexcept;

    /**
     * @brief Set the glossary of the kanji.
     *
     * @param value The new glossary.
     */
    void setGlossary(const QStringList &value);

    /**
     * @brief Get the QML property for the tags.
     *
     * @return The QML property for the tags.
     */
    [[nodiscard]]
    QQmlListProperty<Tag> tagsQml();

    /**
     * @brief Get the tags associated with this definition.
     *
     * @return The tags associated with this definition.
     */
    [[nodiscard]]
    const QList<Tag *> &tags() const noexcept;

    /**
     * @brief Set the tags associated with this definition. Takes ownership.
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
     * @brief Get the QML property for stats.
     *
     * @return The list of stats.
     */
    [[nodiscard]]
    QQmlListProperty<Tag> statsQml();

    /**
     * @brief Get the stats for the definition.
     *
     * @return The list of stats for the definition.
     */
    [[nodiscard]]
    const QList<Tag *> &stats() const noexcept;

    /**
     * @brief Set the stats of the definition. Takes ownership.
     *
     * @param value The stats of the definition.
     */
    void setStats(const QList<Tag *> &value);

    /**
     * @brief Append a tag to the stats. Takes ownership.
     *
     * @param value The stats to append.
     */
    void appendStats(Tag *value);

    /**
     * @brief Get the QML property for classes.
     *
     * @return The classes for this definition.
     */
    [[nodiscard]]
    QQmlListProperty<Tag> classesQml();

    /**
     * @brief Get the classes for this definition.
     *
     * @return The classes of this definition.
     */
    [[nodiscard]]
    const QList<Tag *> &classes() const noexcept;

    /**
     * @brief Set the classes of the definition. Takes ownership.
     *
     * @param value The new classes.
     */
    void setClasses(const QList<Tag *> &value);

    /**
     * @brief Append a tag to the classes. Takes ownership.
     *
     * @param value The tag to append.
     */
    void appendClasses(Tag *value);

    /**
     * @brief The QML property for codes.
     *
     * @return The codes of this definition.
     */
    [[nodiscard]]
    QQmlListProperty<Tag> codesQml();

    /**
     * @brief The codes of this definition.
     *
     * @return The codes of this definition.
     */
    [[nodiscard]]
    const QList<Tag *> &codes() const noexcept;

    /**
     * @brief Set the codes of this definition. Takes ownership.
     *
     * @param value The new codes.
     */
    void setCodes(const QList<Tag *> &value);

    /**
     * @brief Append a tag to the codes. Takes ownership.
     *
     * @param value The tag to append.
     */
    void appendCodes(Tag *value);

    /**
     * @brief Get the QML property for indices.
     *
     * @return The indices of this definition.
     */
    [[nodiscard]]
    QQmlListProperty<Tag> indicesQml();

    /**
     * @brief Get the indices of this definition.
     *
     * @return The indices of this definition.
     */
    [[nodiscard]]
    const QList<Tag *> &indices() const noexcept;

    /**
     * @brief Set the indices of this definition. Takes ownership.
     *
     * @param value The new indices.
     */
    void setIndices(const QList<Tag *> &value);

    /**
     * @brief Append a tag to the indices. Takes ownership.
     *
     * @param value The tag to append.
     */
    void appendIndices(Tag *value);

signals:
    /**
     * @brief Emitted when the dictionary info is changed.
     *
     * @param value The new dictionary info.
     */
    void dictionaryInfoChanged(DictionaryInfo *value);

    /**
     * @brief Emitted when the onyomi are charged.
     */
    void onyomiChanged();

    /**
     * @brief Emitted when the kunyomi are changed.
     */
    void kunyomiChanged();

    /**
     * @brief Emitted when the glossary is changed.
     */
    void glossaryChanged();

    /**
     * @brief Emitted when the tags list is changed.
     */
    void tagsChanged();

    /**
     * @brief Emitted when the stats are changed.
     */
    void statsChanged();

    /**
     * @brief Emitted when the classes are changed.
     */
    void classesChanged();

    /**
     * @brief Emitted when the codes are changed.
     */
    void codesChanged();

    /**
     * @brief Emitted when the indicies are changed.
     */
    void indicesChanged();

protected:
    /* The dictionary this belongs to */
    DictionaryInfo *m_dictionaryInfo{nullptr};

    /* The onyomi (Chinese) readings of the kanji. */
    QStringList m_onyomi;

    /* The kunyomi (Japanese) readings of the kanji. */
    QStringList m_kunyomi;

    /* A list of definitions for this kanji. */
    QStringList m_glossary;

    /* The list of tags belonging to this definition. */
    QList<Tag *> m_tags;

    /* Statistics for this definition.
     * The notes field of the Tag contains the description. */
    QList<Tag *> m_stats;

    /* Classifications for this definition.
     * The notes field of the Tag contains the description. */
    QList<Tag *> m_classes;

    /* Codepoints for this definition.
     * The notes field of the Tag contains the description. */
    QList<Tag *> m_codes;

    /* Dictionary Indices for this definition.
     * The notes field of the Tag contains the description. */
    QList<Tag *> m_indices;
};
