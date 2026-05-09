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

#include "dict/data/frequency.h"

class DictionarySearch;

/**
 * @brief Describes the common fields of an expression both term and kanji.
 */
class Expression : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QString title
        READ title
        WRITE setTitle
        NOTIFY titleChanged
    )

    Q_PROPERTY(
        QString subtitle
        READ subtitle
        WRITE setSubtitle
        NOTIFY subtitleChanged
    )

    Q_PROPERTY(
        double startTime
        READ startTime
        WRITE setStartTime
        NOTIFY startTimeChanged
    )

    Q_PROPERTY(
        double endTime
        READ endTime
        WRITE setEndTime
        NOTIFY endTimeChanged
    )

    Q_PROPERTY(
        QString subtitle2
        READ subtitle2
        WRITE setSubtitle2
        NOTIFY subtitle2Changed
    )

    Q_PROPERTY(
        double contextStartTime
        READ contextStartTime
        WRITE setContextStartTime
        NOTIFY contextStartTimeChanged
    )

    Q_PROPERTY(
        double contextEndTime
        READ contextEndTime
        WRITE setContextEndTime
        NOTIFY contextEndTimeChanged
    )

    Q_PROPERTY(
        QString context
        READ context
        WRITE setContext
        NOTIFY contextChanged
    )

    Q_PROPERTY(
        QString context2
        READ context2
        WRITE setContext2
        NOTIFY context2Changed
    )

    Q_PROPERTY(
        QString clipboard
        READ clipboard
        WRITE setClipboard
        NOTIFY clipboardChanged
    )

    Q_PROPERTY(
        QString clozeBody
        READ clozeBody
        WRITE setClozeBody
        NOTIFY clozeBodyChanged
    )

    Q_PROPERTY(
        QString clozePrefix
        READ clozePrefix
        WRITE setClozePrefix
        NOTIFY clozePrefixChanged
    )

    Q_PROPERTY(
        QString clozeSuffix
        READ clozeSuffix
        WRITE setClozeSuffix
        NOTIFY clozeSuffixChanged
    )

    Q_PROPERTY(
        QQmlListProperty<Frequency> frequencies
        READ frequenciesQml
        NOTIFY frequenciesChanged
    )

    Q_PROPERTY(
        bool ankiChecked
        READ ankiChecked
        WRITE setAnkiChecked
        NOTIFY ankiCheckedChanged
    )

    Q_PROPERTY(
        bool ankiAdding
        READ ankiAdding
        WRITE setAnkiAdding
        NOTIFY ankiAddingChanged
    )

    friend class DictionarySearch;

public:
    Expression(QObject *parent = nullptr);
    virtual ~Expression();

    /**
     * @brief Creates a deep copy of this object.
     *
     * @param parent The parent of the new object.
     * @return The cloned Expression.
     */
    Expression *clone(QObject *parent = nullptr) const;

    /**
     * @brief Get title of the media this expression came from.
     *
     * @return The title of the media.
     */
    [[nodiscard]]
    const QString &title() const noexcept;

    /**
     * @brief Set the title of the media.
     *
     * @param value The new title of the media.
     */
    void setTitle(const QString &value);

    /**
     * @brief Get current subtitle text.
     *
     * @return The current subtitle text.
     */
    [[nodiscard]]
    const QString &subtitle() const noexcept;

    /**
     * @brief Set the current subtitle text.
     *
     * @param value The current subtitle text.
     */
    void setSubtitle(const QString &value);

    /**
     * @brief Get the start time of the current subtitle.
     *
     * @return The start time of the current subtitle.
     */
    [[nodiscard]]
    double startTime() const noexcept;

    /**
     * @brief Set the start time of the current subtitle.
     *
     * @param value The start time of the current subtitle.
     */
    void setStartTime(double value);

    /**
     * @brief Get the end time of the current subtitle.
     *
     * @return The end time of the current subtitle.
     */
    [[nodiscard]]
    double endTime() const noexcept;

    /**
     * @brief Set the end time of the current subtitle.
     *
     * @param value The end time of the current subtitle.
     */
    void setEndTime(double value);

    /**
     * @brief Get the current secondary subtitle text.
     *
     * @return The secondary subtitle text.
     */
    [[nodiscard]]
    const QString &subtitle2() const noexcept;

    /**
     * @brief Set the secondary subtitle text.
     *
     * @param value The secondary subtitle text.
     */
    void setSubtitle2(const QString &value);

    /**
     * @brief Get the start time of the selected context.
     *
     * @return The start time of the selected context.
     */
    [[nodiscard]]
    double contextStartTime() const noexcept;

    /**
     * @brief Set the start time of the selected context.
     *
     * @param value The selected start time of the context.
     */
    void setContextStartTime(double value);

    /**
     * @brief Get the end time of the selected context.
     *
     * @return The end time of the selected context.
     */
    [[nodiscard]]
    double contextEndTime() const noexcept;

    /**
     * @brief Set the end time of the selected context.
     *
     * @param value The new end time.
     */
    void setContextEndTime(double value);

    /**
     * @brief Get the text of the selected context.
     *
     * @return The text of the selected context.
     */
    [[nodiscard]]
    const QString &context() const noexcept;

    /**
     * @brief Set the text of the selected context.
     *
     * @param value The new context text.
     */
    void setContext(const QString &value);

    /**
     * @brief Get the text of the selected secondary context.
     *
     * @return The text of the selected secondary context.
     */
    [[nodiscard]]
    const QString &context2() const noexcept;

    /**
     * @brief Set the text of the selected secondary context.
     *
     * @param value The new text of the selected secondary context.
     */
    void setContext2(const QString &value);

    /**
     * @brief Get the current clipboard text.
     *
     * @return The current clipboard text.
     */
    [[nodiscard]]
    const QString &clipboard() const noexcept;

    /**
     * @brief Set the current clipboard text.
     *
     * @param value The new clipboard text.
     */
    void setClipboard(const QString &value);

    /**
     * @brief Get the cloze body of the expression.
     *
     * @return The cloze body of the expression.
     */
    [[nodiscard]]
    const QString &clozeBody() const noexcept;

    /**
     * @brief Set the cloze body of the expression.
     *
     * @param value The new cloze body.
     */
    void setClozeBody(const QString &value);

    /**
     * @brief Get the cloze prefix of the expression.
     *
     * @return The cloze prefix of the expression.
     */
    [[nodiscard]]
    const QString &clozePrefix() const noexcept;

    /**
     * @brief Set the cloze suffix of the expression.
     *
     * @param value The new value of the cloze suffix.
     */
    void setClozePrefix(const QString &value);

    /**
     * @brief Get the cloze suffix of the expression.
     *
     * @return The cloze suffix.
     */
    [[nodiscard]]
    const QString &clozeSuffix() const noexcept;

    /**
     * @brief Set the cloze suffix of the expression.
     *
     * @param value The new cloze suffix.
     */
    void setClozeSuffix(const QString &value);

    /**
     * @brief Gets the QML property for the tags.
     *
     * @return The QML property for the tags.
     */
    [[nodiscard]]
    QQmlListProperty<Frequency> frequenciesQml();

    /**
     * @brief Gets the tags associated with this definition.
     *
     * @return The tags associated with this definition.
     */
    [[nodiscard]]
    const QList<Frequency *> &frequencies() const noexcept;

    /**
     * @brief Sets the tags associated with this definition. Takes ownership.
     *
     * @param value The new list of tags.
     */
    void setFrequencies(const QList<Frequency *> &value);

    /**
     * @brief Append a frequency to the end of the frequency list. Takes
     * ownership.
     *
     * @param value The frequency to add.
     */
    void appendFrequencies(Frequency *value);

    /**
     * @brief Get if Anki has been checked.
     *
     * @return true if Anki has been checked,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool ankiChecked() const noexcept;

    /**
     * @brief Set if Anki has been checked.
     *
     * @param value true if Anki has been checked, false otherwise.
     */
    void setAnkiChecked(bool value);

    /**
     * @brief Get if this expression is currently being added to Anki.
     *
     * @return true if the expression is currently being added,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool ankiAdding() const noexcept;

    /**
     * @brief Get if this expression is currently being added to Anki.
     *
     * @param value true if the expression is currently being added, false
     * otherwise.
     */
    void setAnkiAdding(bool value);

signals:
    /**
     * @brief Emitted when the title changes.
     *
     * @param value The new value.
     */
    void titleChanged(const QString &value);

    /**
     * @brief Emitted when the subtitle changes.
     *
     * @param value The new value.
     */
    void subtitleChanged(const QString &value);

    /**
     * @brief Emitted when the start time changes.
     *
     * @param value The new value.
     */
    void startTimeChanged(double value);

    /**
     * @brief Emitted when the end time changes.
     *
     * @param value The new value.
     */
    void endTimeChanged(double value);

    /**
     * @brief Emitted when the secondary subtitle changes.
     *
     * @param value The new value.
     */
    void subtitle2Changed(const QString &value);

    /**
     * @brief Emitted when the start time context changes.
     *
     * @param value The new value.
     */
    void contextStartTimeChanged(double value);

    /**
     * @brief Emitted when the end time context changes.
     *
     * @param value The new value.
     */
    void contextEndTimeChanged(double value);

    /**
     * @brief Emitted when the context changes.
     *
     * @param value The new value.
     */
    void contextChanged(const QString &value);

    /**
     * @brief Emitted when the secondary context changes.
     *
     * @param value The new value.
     */
    void context2Changed(const QString &value);

    /**
     * @brief Emitted when the clipboard changes.
     *
     * @param value The new value.
     */
    void clipboardChanged(const QString &value);

    /**
     * @brief Emitted when the cloze body changes.
     *
     * @param value The new value.
     */
    void clozeBodyChanged(const QString &value);

    /**
     * @brief Emitted when the cloze prefix changes.
     *
     * @param value The new value.
     */
    void clozePrefixChanged(const QString &value);

    /**
     * @brief Emitted when the cloze suffix changes.
     *
     * @param value The new value.
     */
    void clozeSuffixChanged(const QString &value);

    /**
     * @brief Emitted when the tags list is changed.
     *
     * @param value The new value.
     */
    void frequenciesChanged();

    /**
     * @brief Emitted when the Anki checked value is changed.
     *
     * @param value The new value.
     */
    void ankiCheckedChanged(bool value);

    /**
     * @brief Emitted when the Anki adding value is changed.
     *
     * @param value The new value.
     */
    void ankiAddingChanged(bool value);

protected:
    /* The title of the media this came from */
    QString m_title;

    /* The current subtitle */
    QString m_subtitle;

    /* The start time of the current subtitle */
    double m_startTime{0};

    /* The end time of the current subtitle */
    double m_endTime{0};

    /* The current secondary subtitle */
    QString m_subtitle2;

    /* The start time of the selected context */
    double m_contextStartTime{0};

    /* The end time of the selected context */
    double m_contextEndTime{0};

    /* The currently selected context */
    QString m_context;

    /* The current secondary context */
    QString m_context2;

    /* The current text in the user's clipboard */
    QString m_clipboard;

    /* The raw term as it was matched by Memento. */
    QString m_clozeBody;

    /* Everything in the sentence before the cloze body */
    QString m_clozePrefix;

    /* Everything in the sentence after the cloze body */
    QString m_clozeSuffix;

    /* A list of frequencies */
    QList<Frequency *> m_frequencies;

    /* true if Anki has been checked, false otherwise */
    bool m_ankiChecked{false};

    /* true if the term is being added to Anki, false otherwise */
    bool m_ankiAdding{false};
};
