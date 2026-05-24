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

#include "dict/data/pitch.h"
#include "dict/data/tag.h"
#include "dict/data/termdefinition.h"

class DictionarySearch;

/**
 * @brief Describes the data in a term entry.
 */
class Term : public Expression
{
    Q_OBJECT

    Q_PROPERTY(
        QString expression
        READ expression
        WRITE setExpression
        NOTIFY expressionChanged
    )

    Q_PROPERTY(
        QString reading
        READ reading
        WRITE setReading
        NOTIFY readingChanged
    )

    Q_PROPERTY(
        QString conjugationExplanation
        READ conjugationExplanation
        WRITE setConjugationExplanation
        NOTIFY conjugationExplanationChanged
    )

    Q_PROPERTY(
        bool readingAsExpression
        READ readingAsExpression
        WRITE setReadingAsExpression
        NOTIFY readingAsExpressionChanged
    )

    Q_PROPERTY(
        QQmlListProperty<Tag> tags
        READ tagsQml
        NOTIFY tagsChanged
    )

    Q_PROPERTY(
        QQmlListProperty<Pitch> pitches
        READ pitchesQml
        NOTIFY pitchesChanged
    )

    Q_PROPERTY(
        QQmlListProperty<TermDefinition> definitions
        READ definitionsQml
        NOTIFY definitionsChanged
    )

    Q_PROPERTY(
        QStringList selection
        READ selection
        WRITE setSelection
        NOTIFY selectionChanged
    )

    Q_PROPERTY(
        int score
        READ score
        WRITE setScore
        NOTIFY scoreChanged
    )

    Q_PROPERTY(
        QString audioSourceName
        READ audioSourceName
        WRITE setAudioSourceName
        NOTIFY audioSourceNameChanged
    )

    Q_PROPERTY(
        QString audioUrl
        READ audioUrl
        WRITE setAudioUrl
        NOTIFY audioUrlChanged
    )

    Q_PROPERTY(
        QString audioSkipHash
        READ audioSkipHash
        WRITE setAudioSkipHash
        NOTIFY audioSkipHashChanged
    )

    Q_PROPERTY(
        bool collapsed
        READ collapsed
        WRITE setCollapsed
        NOTIFY collapsedChanged
    )

    Q_PROPERTY(
        bool autoPlay
        READ autoPlay
        WRITE setAutoPlay
        NOTIFY autoPlayChanged
    )

    Q_PROPERTY(
        bool addableExpression
        READ addableExpression
        WRITE setAddableExpression
        NOTIFY addableExpressionChanged
    )

    Q_PROPERTY(
        bool addableReading
        READ addableReading
        WRITE setAddableReading
        NOTIFY addableReadingChanged
    )

    friend class DictionarySearchController;

public:
    Term(QObject *parent = nullptr);
    virtual ~Term();

    /**
     * @brief Creates a deep copy of this object.
     *
     * @param parent The parent of the new object.
     * @return The cloned Term.
     */
    Term *clone(QObject *parent = nullptr) const;

    /**
     * @brief Get the expression of this term. If there is no kanji, this is
     * kana.
     *
     * @return The expression of this term.
     */
    [[nodiscard]]
    const QString &expression() const noexcept;

    /**
     * @brief Set the expression of this term.
     *
     * @param value The new expression.
     */
    void setExpression(const QString &value);

    /**
     * @brief Get the reading of this term. If there are no kanji in the
     * expression, this is empty.
     *
     * @return The reading of this term.
     */
    [[nodiscard]]
    const QString &reading() const noexcept;

    /**
     * @brief Set the readin of this term.
     *
     * @param value The new reading.
     */
    void setReading(const QString &value);

    /**
     * @brief Get the conjugation explanation of this term.
     *
     * @return The conjugation explanation.
     */
    [[nodiscard]]
    const QString &conjugationExplanation() const noexcept;

    /**
     * @brief Sets the conjugation explanation.
     *
     * @param value The new conjugation explanation.
     */
    void setConjugationExplanation(const QString &value);

    /**
     * @brief Get if the reading should be used for the {expression}
     *
     * @return true if the reading should populate the {expression} marker,
     * @return false if it should not.
     */
    [[nodiscard]]
    bool readingAsExpression() const noexcept;

    /**
     * @brief Set if the reading should be used as the expression.
     *
     * @param value The new value.
     */
    void setReadingAsExpression(bool value);

    /**
     * @brief Get the QML accessible tags.
     *
     * @return The tags for this term.
     */
    [[nodiscard]]
    QQmlListProperty<Tag> tagsQml();

    /**
     * @brief The tags of this term.
     *
     * @return The list of tags.
     */
    [[nodiscard]]
    const QList<Tag *> &tags() const noexcept;

    /**
     * @brief Set the tags of this term. Takes ownership.
     *
     * @param value The new tags.
     */
    void setTags(const QList<Tag *> &value);

    /**
     * @brief Append a tag to the tags. Takes ownership.
     *
     * @param value The tag to append.
     */
    void appendTags(Tag *value);

    /**
     * @brief Get the QML accessible pitches.
     *
     * @return The pitches for this term.
     */
    [[nodiscard]]
    QQmlListProperty<Pitch> pitchesQml();

    /**
     * @brief Get the pitches for this term.
     *
     * @return The pitches for this term.
     */
    [[nodiscard]]
    const QList<Pitch *> &pitches() const noexcept;

    /**
     * @brief Set the pitches for this term. Takes ownership.
     *
     * @param value The new pitches.
     */
    void setPitches(const QList<Pitch *> &value);

    /**
     * @brief Append a pitch to the pitches. Takes ownership.
     *
     * @param value The pitch to append.
     */
    void appendPitches(Pitch *value);

    /**
     * @brief Get the QML accessible definitions.
     *
     * @return The definitions of this term.
     */
    [[nodiscard]]
    QQmlListProperty<TermDefinition> definitionsQml();

    /**
     * @brief Get the definitions of this term.
     *
     * @return The definitions of this term.
     */
    [[nodiscard]]
    const QList<TermDefinition *> &definitions() const noexcept;

    /**
     * @brief Set the definition of this term. Takes ownership.
     *
     * @param value The new definitions.
     */
    void setDefinitions(const QList<TermDefinition *> &value);

    /**
     * @brief Append a definition to the definitions. Takes ownership.
     *
     * @param value The definition to append.
     */
    void appendDefinitions(TermDefinition *value);

    /**
     * @brief Get the glossary selection for this term.
     *
     * @return The selected glossary text.
     */
    [[nodiscard]]
    const QStringList &selection() const noexcept;

    /**
     * @brief Set the selected glossary text.
     *
     * @param value The new selected glossary text.
     */
    void setSelection(const QStringList &value);

    /**
     * @brief Get the score of this term.
     *
     * @return The score of this term.
     */
    [[nodiscard]]
    int score() const noexcept;

    /**
     * @brief Set the score of this term.
     *
     * @param value The new score.
     */
    void setScore(int value);

    /**
     * @brief Get the name of the audio source of this term.
     *
     * @return The name of the audio source.
     */
    [[nodiscard]]
    const QString &audioSourceName() const noexcept;

    /**
     * @brief Set the audio source of this term.
     *
     * @param value The new name the audio source.
     */
    void setAudioSourceName(const QString &value);

    /**
     * @brief Get the URL of the audio source.
     *
     * @return The URL of the audio source.
     */
    [[nodiscard]]
    const QString &audioUrl() const noexcept;

    /**
     * @brief Set the URL of the audio source.
     *
     * @param value The new URL of the audio source.
     */
    void setAudioUrl(const QString &value);

    /**
     * @brief Get the MD5 hash of audio files to ignore.
     *
     * @return The MD5 hash of the audio file to ignore.
     */
    [[nodiscard]]
    const QString &audioSkipHash() const noexcept;

    /**
     * @brief Set the MD5 hash of the audio file to ignore.
     *
     * @param value The new skip hash.
     */
    void setAudioSkipHash(const QString &value);

    /**
     * @brief Get the if this term is collapsed.
     *
     * @return true if collapsed,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool collapsed() const noexcept;

    /**
     * @brief Set if this term is collapsed.
     *
     * @param value true to collapse the term, false otherwise.
     */
    void setCollapsed(bool value);

    /**
     * @brief Get if this term should be auto played.
     *
     * @return true to auto play audio show,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool autoPlay() const noexcept;

    /**
     * @brief Set if this term should be auto played.
     *
     * @param value true to auto play audio on show, false otherwise.
     */
    void setAutoPlay(bool value);

    /**
     * @brief Get if this term is addable to Anki as an expression.
     *
     * @return true if this term is addable as an expression,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool addableExpression() const noexcept;

    /**
     * @brief Set the if this term is addable to Anki as an expression.
     *
     * @param value true if this term is addable as an expression, false
     * otherwise.
     */
    void setAddableExpression(bool value);

    /**
     * @brief Get if this term is addable to Anki as a reading.
     *
     * @return true if this term is addable as a reading,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool addableReading() const noexcept;

    /**
     * @brief Set the if this term is addable to Anki as a reading.
     *
     * @param value true if this term is addable as a reading, false otherwise.
     */
    void setAddableReading(bool value);

signals:
    /**
     * @brief Emitted when the expression is changed.
     *
     * @param value The new value.
     */
    void expressionChanged(const QString &value);

    /**
     * @brief Emitted when the reading is changed.
     *
     * @param value The new value.
     */
    void readingChanged(const QString &value);

    /**
     * @brief Emitted when the conjugation explanation is changed.
     *
     * @param value The new value.
     */
    void conjugationExplanationChanged(const QString &value);

    /**
     * @brief Emitted when the reading as expression is changed.
     *
     * @param value The new value.
     */
    void readingAsExpressionChanged(bool value);

    /**
     * @brief Emitted when the tags are changed.
     */
    void tagsChanged();

    /**
     * @brief Emitted when the pitches are changed.
     */
    void pitchesChanged();

    /**
     * @brief Emitted when the definitions are changed.
     */
    void definitionsChanged();

    /**
     * @brief Emitted when the selection is changed.
     */
    void selectionChanged();

    /**
     * @brief Emitted when the score is changed.
     *
     * @param value The new value.
     */
    void scoreChanged(int value);

    /**
     * @brief Emitted when the audio source name is changed.
     *
     * @param value The new value.
     */
    void audioSourceNameChanged(const QString &value);

    /**
     * @brief Emitted when the audio URL is changed.
     *
     * @param value The new value.
     */
    void audioUrlChanged(const QString &value);

    /**
     * @brief Emitted when the skip hash is changed.
     *
     * @param value The new value.
     */
    void audioSkipHashChanged(const QString &value);

    /**
     * @brief Emitted when the collapsed value is changed.
     *
     * @param value The new value.
     */
    void collapsedChanged(bool value);

    /**
     * @brief Emitted when the auto play value is changed.
     *
     * @param value The new value.
     */
    void autoPlayChanged(bool value);

    /**
     * @brief Emitted when the ability to add this expression to Anki changes.
     *
     * @param value The new value.
     */
    void addableExpressionChanged(bool value);

    /**
     * @brief Emitted when the ability to add this reading to Anki changes.
     *
     * @param value The new value.
     */
    void addableReadingChanged(bool value);

protected:
    /* The expression of the term (includes kanji if it exists). */
    QString m_expression;

    /* The reading of a term. Usually empty if the term includes no kanji. */
    QString m_reading;

    /* The conjugation explanation of a term.
     * Usually empty if the term was not conjugated. */
    QString m_conjugationExplanation;

    /* true if the reading should be used as the expression, false otherwise */
    bool m_readingAsExpression{false};

    /* The list of tags applicable to this term. */
    QList<Tag *> m_tags;

    /* The list of pitches for this term. */
    QList<Pitch *> m_pitches;

    /* The list of definitions for this term. */
    QList<TermDefinition *> m_definitions;

    /* The text selected in the glossary */
    QStringList m_selection;

    /* The score of this term.
     * Larger values generally mean this term is more common. */
    int m_score{0};

    /* The name of the audio source */
    QString m_audioSourceName;

    /* The URL of the selected audio source */
    QString m_audioUrl;

    /* The MD5 audio skip hash for the audio source */
    QString m_audioSkipHash;

    /* true if this should be collapsed when shown */
    bool m_collapsed{false};

    /* true if this should have the audio auto played */
    bool m_autoPlay{false};

    /* true if this term is addable to Anki as an expression, false otherwise */
    bool m_addableExpression{false};

    /* true if this term is addable to Anki as a reading, false otherwise */
    bool m_addableReading{false};
};
