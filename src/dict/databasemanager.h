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

#include <QReadWriteLock>
#include <QSet>
#include <QString>

#include <sqlite3.h>

#include "dict/data/data.h"

/**
 * @brief Manages all interaction with the dictionary database on the backend.
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a database manager with the specified database. Creates
     * the database if it doesn't already exist.
     *
     * @param dbPath The path to the dictionary database.
     * @param resourcePath Path to the resource directory.
     * @param parent The parent of this manager.
     */
    DatabaseManager(
        const QString &dbPath,
        const QString &resourcePath,
        QObject *parent = nullptr);
    virtual ~DatabaseManager();

    /**
     * @brief Get if the database is being modified.
     *
     * @return true if the data is being modified,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool modifyingDatabase() const noexcept;

    /**
     * @brief Adds a dictionary to the database.
     *
     * @param path Path to the dictionary.
     * @return Error code. Can be turned into a string with a call to
     * errorCodeToString().
     */
    int addDictionary(QString path);

    /**
     * @brief Deletes a dictionary from the database.
     *
     * @param id The id of the dictionary.
     * @return Error code. Can be turned into a string with a call to
     * errorCodeToString().
     */
    int deleteDictionary(int64_t id);

    /**
     * @brief Enables a dictionary.
     *
     * @param id The id of the dictionary.
     * @return Error code. Can be turned into a string with a call to
     * errorCodeToString().
     */
    int enableDictionary(int64_t id);

    /**
     * @brief Disables a dictionary.
     *
     * @param id The id of the dictionary.
     * @return Error code. Can be turned into a string with a call to
     * errorCodeToString().
     */
    int disableDictionary(int64_t id);

    /**
     * @brief Get the list of dictionaries in the database. Belongs to the
     * caller.
     *
     * @param parent The parent of the DictionaryInfos.
     * @return A list of dictionary metadata.
     */
    [[nodiscard]]
    QList<DictionaryInfo *> getDictionaries(QObject *parent = nullptr) const;

    /**
     * @brief Searches for terms that exactly match the query. Does automatic
     * conversion from katakana to hiragana.
     *
     * @param query The term to query for.
     * @param parent The parent of the terms.
     * @param[out] error The reason for failure on error. Empty on success.
     * @return A list of all the terms found. An empty list on error.
     */
    [[nodiscard]]
    QList<Term *> queryTerms(
        QString query,
        QObject *parent = nullptr,
        QString *error = nullptr) const;

    /**
     * @brief Searches for kanji that exactly match the query.
     *
     * @param query The kanji to look for. Should be a single character.
     * @param parent The parent of the kanji.
     * @param[out] The reason for failure on error. Empty on success.
     * @return The kanji if found, nullptr if not. nullptr on error.
     */
    [[nodiscard]]
    Kanji *queryKanji(
        QString query,
        QObject *parent = nullptr,
        QString *error = nullptr) const;

    /**
     * @brief Translates an error code to a human readable string.
     *
     * @param code The error code.
     * @return A string describing the error.
     */
    [[nodiscard]]
    QString errorCodeToString(int code) const;

signals:
    /**
     * @brief Emitted when the modifying database property is updated.
     *
     * @param value true if modifying the database, false otherwise.
     */
    void modifyingDatabaseChanged(bool value);

private:
    /**
     * @brief Load dictionary assets into the dictionary info.
     *
     * @param info The info to load assets into.
     */
    void loadDictionaryAssets(DictionaryInfo *info) const;

    /**
     * @brief Initializes the dictionary cache so IDs can be quickly mapped to
     * DictionaryInfo.
     */
    int initCache();

    /**
     * @brief Clears the dictionary cache.
     */
    void clearDictionaryCache();

    /**
     * @brief Clears the tag cache.
     */
    void clearTagCache();

    /**
     * @brief Set if the database is being modified.
     *
     * @param value true if the database is being modified, false otherwise.
     */
    void setModifyingDatabase(bool value);

    /**
     * @brief Get the dictionary that corresponds to the ID.
     *
     * @param id The id of the dictionary to look for.
     * @return The DictionaryInfo if found, nullptr if not.
     */
    [[nodiscard]]
    DictionaryInfo *getDictionary(int64_t id) const;

    /**
     * @brief Populates term information for queryTerms.
     *
     * @param[out] terms A list of Term objects with the expression and reading
     * fields populated.
     * @return An SQLite error code on failure.
     */
    int populateTerms(const QList<Term *> &terms) const;

    /**
     * @brief Helper method for retrieving tag information.
     *
     * @param id The id of the dictionary the tag comes from.
     * @param tagStr The name of the tag.
     * @param parent The parent of the Tag objects.
     * @return The list of tags. Belongs to the caller.
     */
    [[nodiscard]]
    QList<Tag *> getTags(
        const int64_t id,
        const QString &tagStr,
        QObject *parent = nullptr) const;

    /**
     * @brief Helper method for retrieving tag information. Accumulates tags in
     * an array without duplicates.
     *
     * @param id The ID of the tag dictionary.
     * @param tagStr The tag string.
     * @param[out] tags The list to accumulate to.
     * @param parent The parent of the Tag objects.
     */
    void accumulateTags(
        const int64_t id,
        const QString &tagStr,
        QList<Tag *> &tags,
        QObject *parent = nullptr) const;

    /**
     * @brief Adds term frequencies to a Term struct.
     *
     * @param[out] term The term struct to add frequencies to.
     * @return An SQLite error code on failure.
     */
    int addFrequencies(Term *term) const;

    /**
     * @brief Adds kanji frequencies to a Kanji struct.
     *
     * @param[out] kanji The kanji struct to add frequencies to.
     * @return An SQLite error code on failure.
     */
    int addFrequencies(Kanji *kanji) const;

    /**
     * @brief Adds frequencies to a frequency list. Should probably not be
     * called directly.
     *
     * @param query The sql query to use on the database. Must take
     * one bind.
     * @param expression The first sql bind.
     * @param reading The reading of the term if available.
     * @param parent The parent of the frequencies.
     * @param[out] error The SQLite error code on failure.
     * @return The frequencies found. Empty on error. Belongs to the caller.
     */
    QList<Frequency *> getFrequencies(
        const char *query,
        const QString &expression,
        const QString &reading = QString(),
        QObject *parent = nullptr,
        int *error = nullptr) const;

    /**
     * @brief Helper method for adding pitch accents to a Term.
     *
     * @param[out] term The term to add pitch accents to. Must have the
     * expression field set.
     * @return An SQLite error code on failure.
     */
    int addPitches(Term *term) const;

    /**
     * @brief Converts all the katakana in a string to their equivalent
     * hiragana.
     *
     * @param query The string to convert.
     * @return Query with all the katakana replaced with hiragana.
     */
    [[nodiscard]]
    static QString kataToHira(QString query);

    /**
     * @brief Converts a raw JSON array of strings to a QStringList.
     *
     * @param jsonstr A raw JSON string representing an array of strings.
     * @return The QStringList containing all the elements of the JSON string.
     */
    [[nodiscard]]
    static QStringList jsonArrayToStringList(const char *jsonstr);

    /**
     * @brief Helper method for determining if an SQLite step resulted in an
     * error.
     *
     * @param step The value returned form sqlite3_step().
     * @return true if code is an error, false otherwise.
     */
    [[nodiscard]]
    static bool inline isStepError(const int step);

    /* Saved path to the database */
    const QByteArray m_dbPath;

    /* Saved path to the resource directory */
    const QString m_resourcePath;

    /* A readonly connection to the dictionary database. */
    sqlite3 *m_db{nullptr};

    /* Locks the database for reading and writing. */
    mutable QReadWriteLock m_dbLock;

    /* A set containing special characters that cannot be independent mora. */
    QSet<QString> m_moraSkipChar;

    /* Maps dictionary IDs to dictionary names. Must be deleted manually. */
    QHash<int64_t, DictionaryInfo *> m_dictionaryCache;

    /* Maps dictionary IDs to a mapping between tag names and Tag structs.
     * Must be deleted manually. */
    QHash<int64_t, QHash<QString, Tag *>> m_tagCache;

    /* true if the database is being modified, false otherwise */
    std::atomic_bool m_modifyingDatabase{false};
};
