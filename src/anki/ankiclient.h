////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#include <memory>

#include <QNetworkAccessManager>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroQmlTask>
#include <QCoroTask>
#else
#include <qcoro/qcorotask.h>
#include <qcoro/qml/qcoroqmltask.h>
#endif // MEMENTO_SYSTEM_QCORO

#include "anki/glossarybuilder.h"
#include "anki/notebuilder.h"
#include "dict/data/kanji.h"
#include "dict/data/term.h"

class Context;

/**
 * @brief Object for communicating with AnkiConnect.
 */
class AnkiClient : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Create a new AnkiClient.
     *
     * @param context The application context.
     * @param parent The parent of this object.
     */
    AnkiClient(Context *context, QObject *parent = nullptr);

    /**
     * @brief Test the connection to the AnkiConnect server.
     *
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask testConnection();

    /**
     * @brief Test the connection to the AnkiConnect server.
     *
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> testConnectionAsync();

    /**
     * @brief Get the names of all Anki decks.
     *
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return decks: A QStringList of all deck names.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask getDeckNames();

    /**
     * @brief Get the names of all Anki decks.
     *
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return decks: A QStringList of all deck names.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> getDeckNamesAsync();

    /**
     * @brief Get the names of all model decks.
     *
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return models: A QStringList of all model names.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask getModelNames();

    /**
     * @brief Get the names of all model decks.
     *
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return models: A QStringList of all model names.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> getModelNamesAsync();

    /**
     * @brief Get the field names for the model.
     *
     * @param model The name of the model.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return fields: A QStringList of all field names.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask getFieldNames(const QString &model);

    /**
     * @brief Get the field names for the model.
     *
     * @param model The name of the model.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return fields: A QStringList of all field names.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> getFieldNamesAsync(const QString &model);

    /**
     * @brief Get if a term note is addable for a given term.
     *
     * @param term The term to check.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return expression: true if the expression is addable, false otherwise.
     * @return reading: true if the reading is addable, false otherwise.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask noteAddable(const Term *term);

    /**
     * @brief Get if a term note is addable for a given term.
     *
     * @param term The term to check.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return expression: true if the expression is addable, false otherwise.
     * @return reading: true if the reading is addable, false otherwise.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> noteAddableAsync(const Term *term);

    /**
     * @brief Get if a kanji note is addable for a given kanji.
     *
     * @param kanji The kanji to check.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return character: true if the character is addable, false otherwise.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask noteAddable(const Kanji *kanji);

    /**
     * @brief Get if a kanji note is addable for a given kanji.
     *
     * @param kanji The kanji to check.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return character: true if the character is addable, false otherwise.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> noteAddableAsync(const Kanji *kanji);

    /**
     * @brief Add a term note to Anki.
     *
     * @param term The term to add.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return id: The ID of the added note.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask addNote(const Term *term);

    /**
     * @brief Add a term note to Anki.
     *
     * @param term The term to add.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return id: The ID of the added note.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> addNoteAsync(const Term *term);

    /**
     * @brief Add a kanji note to Anki.
     *
     * @param kanji The kanji to add.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return id: The ID of the added note.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask addNote(const Kanji *kanji);

    /**
     * @brief Add a kanji note to Anki.
     *
     * @param kanji The kanji to add.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return id: The ID of the added note.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> addNoteAsync(const Kanji *kanji);

    /**
     * @brief Open an Anki GUI window searching for a query in a deck.
     *
     * @param deck The name of the deck to search in.
     * @param query The query to search.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return ids: The IDs of the search query.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask openBrowse(
        const QString &deck, const QString &query);

    /**
     * @brief Open an Anki GUI window searching for a query in a deck.
     *
     * @param deck The name of the deck to search in.
     * @param query The query to search.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return ids: The IDs of the search query.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> openBrowseAsync(
        const QString &deck, const QString &query);

    /**
     * @brief Open a window for all cards that might be detected as duplicates
     * for the given term.
     *
     * @param term The term to check for duplicates.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return ids: The IDs of the duplicates.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask openDuplicates(const Term *term);

    /**
     * @brief Open a window for all cards that might be detected as duplicates
     * for the given term.
     *
     * @param term The term to check for duplicates.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return ids: The IDs of the duplicates.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> openDuplicatesAsync(const Term *term);

    /**
     * @brief Open a window for all cards that might be detected as duplicates
     * for the given kanji.
     *
     * @param kanji The kanji to check for duplicates.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return ids: The IDs of the duplicates.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask openDuplicates(const Kanji *kanji);

    /**
     * @brief Open a window for all cards that might be detected as duplicates
     * for the given kanji.
     *
     * @param kanji The kanji to check for duplicates.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return ids: The IDs of the duplicates.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> openDuplicatesAsync(const Kanji *kanji);

signals:
    /**
     * @brief Emitted when the context is changed.
     *
     * @param value The application context.
     */
    void contextChanged(const Context *value);

    /**
     * @brief Emitted when the profile is changed.
     *
     * @param value The current profile.
     */
    void profileChanged(const AnkiProfile *value);

private slots:
    /**
     * @brief Update the saved subtitle filter regex.
     *
     * @param filter The regex string to use.
     */
    void updateSubtitleFilterRegex(const QString &filter);

private:
    /**
     * @brief Add a note to Anki using an Anki::Note::Context
     *
     * @param profile The profile to use when adding the note.
     * @param ctx The with note information.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return id: The ID of the added note.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> addNoteHelper(
        const AnkiProfile *profile, Anki::Note::Context &&ctx);

    /**
     * @brief Add media file to Anki.
     *
     * @param profile The profile to use.
     * @param files A mapping of file paths to file names.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return result: A string list of added filenames.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> addMedia(
        const AnkiProfile *profile, QList<GlossaryBuilder::FileInfo> fileMap);

    /**
     * @brief Make a request to AnkiConnect that returns a list of strings.
     *
     * @param profile The AnkiProfile to use.
     * @param action The AnkiConnect 'verb'.
     * @param params The parameters of the action if applicable.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return result: The list of strings.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> requestStringList(
        const AnkiProfile *profile,
        const QString &action,
        const QJsonObject &params = QJsonObject());

    /**
     * @brief Make a request to Anki that returns a list of bools.
     *
     * @param profile The AnkiProfile to use.
     * @param action The AnkiConnect 'verb'.
     * @param params Parameters for the action.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return result: The list of booleans.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> requestBoolList(
        const AnkiProfile *profile,
        const QString &action,
        const QJsonObject &params = QJsonObject());

    /**
     * @brief Make a request to Anki that returns an integer.
     *
     * @param profile The AnkiProfile to use.
     * @param action The AnkiConnect 'verb'.
     * @param params Parameters for the action.
     * @return A QVariantMap with properties.
     * @return success: true on success, false on error.
     * @return error: Human readable error string when success is false, nothing
     * otherwise.
     * @return result: The returned integer.
     */
    [[nodiscard]]
    QCoro::Task<QVariantMap> requestInt(
        const AnkiProfile *profile,
        const QString &action,
        const QJsonObject &params = QJsonObject());

    /**
     * Makes a request to AnkiConnect.
     * @param profile The AnkiProfile to use.
     * @param action The AnkiConnection 'verb' to execute.
     * @param params The parameters of the action.
     * @return A QNetworkReply where the result will be received.
     */
    [[nodiscard]]
    QCoro::Task<std::unique_ptr<QNetworkReply>> makeRequest(
        const AnkiProfile *profile,
        const QString &action,
        const QJsonObject &params = QJsonObject());

    /**
     * @brief Error check the AnkiConnect reply.
     *
     * @param reply The reply to error check.
     * @param[out] error The reason the command failed. Empty string if no
     * error.
     * @return The JSON object AnkiConnect replied with.
     */
    [[nodiscard]]
    QJsonObject processReply(QNetworkReply &reply, QString &error);

    /**
     * @brief Get the current profile.
     *
     * @return The current profile.
     */
    [[nodiscard]]
    const AnkiProfile *profile() const noexcept;

    /**
     * @brief Populate the data of the expression.
     *
     * @param[out] expression The expression to populate.
     */
    void populate(Expression *expression) const;

    /* The application context */
    const Context *m_context{nullptr};

    /* The Network Manager for this object */
    QNetworkAccessManager m_manager;

    /* The regular expression to filter subtitles with */
    QRegularExpression m_subtitleFilterRegex;
};
