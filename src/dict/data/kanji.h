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

#include "dict/data/expression.h"

#include <QQmlListProperty>

#include "dict/data/kanjidefinition.h"

class DictionarySearch;

/**
 * @brief Describes the data in a kanji entry.
 */
class Kanji : public Expression
{
    Q_OBJECT

    Q_PROPERTY(
        QString character
        READ character
        WRITE setCharacter
        NOTIFY characterChanged
    )

    Q_PROPERTY(
        QQmlListProperty<KanjiDefinition> definitions
        READ definitionsQml
        NOTIFY definitionsChanged
    )

    Q_PROPERTY(
        bool addable
        READ addable
        WRITE setAddable
        NOTIFY addableChanged
    )

    friend class DictionarySearch;

public:
    Kanji(QObject *parent = nullptr);
    virtual ~Kanji();

    /**
     * @brief Creates a deep copy of this object.
     *
     * @param parent The parent of the new object.
     * @return The cloned Kanji.
     */
    Kanji *clone(QObject *parent = nullptr) const;

    /**
     * @brief Get the character of this kanji.
     *
     * @return The character text.
     */
    [[nodiscard]]
    const QString &character() const noexcept;

    /**
     * @brief Set the character of this kanji.
     *
     * @param value The new character.
     */
    void setCharacter(const QString &value);

    /**
     * @brief Get the QML accessible kanji definitions.
     *
     * @return The list of kanji definitions.
     */
    [[nodiscard]]
    QQmlListProperty<KanjiDefinition> definitionsQml();

    /**
     * @brief Get the kanji definitions.
     *
     * @return The kanji definitions.
     */
    [[nodiscard]]
    const QList<KanjiDefinition *> &definitions() const noexcept;

    /**
     * @brief Set the kanji definitions. Takes ownership.
     *
     * @param value The new kanji definitions.
     */
    void setDefinitions(const QList<KanjiDefinition *> &value);

    /**
     * @brief Append a definition to the definitions. Takes ownership.
     *
     * @param value The definition to append.
     */
    void appendDefinitions(KanjiDefinition *value);

    /**
     * @brief Get if this kanji is addable to Anki.
     *
     * @return true if the kanji is addable to Anki,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool addable() const noexcept;

    /**
     * @brief Set if this kanji is addable to Anki.
     *
     * @param value true if this kanji is addable to Anki, false otherwise.
     */
    void setAddable(bool value);

signals:
    /**
     * @brief Emitted when the character is changed.
     *
     * @param value The new value.
     */
    void characterChanged(const QString &value);

    /**
     * @brief Emitted when the definitions are changed.
     */
    void definitionsChanged();

    /**
     * @brief Emitted when the addable value is changed.
     *
     * @param value The new value.
     */
    void addableChanged(bool value);

protected:
    /* The kanji character */
    QString m_character;

    /* A list of definitions belonging to the kanji */
    QList<KanjiDefinition *> m_definitions;

    /* true if this kanji is addable to Anki, false otherwise */
    bool m_addable{false};
};
