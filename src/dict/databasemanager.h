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

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QReadWriteLock>
#include <QSet>
#include <QString>
#include <sqlite3.h>

#include "expression.h"

/**
 * Manages all interaction with the dictionary database on the backend.
 */
class DatabaseManager
{
public:
    /**
     * Constructs a database manager with the specified database. Creates the
     * database if it doesn't already exist.
     * @param path The path to the dictionary database.
     */
    DatabaseManager(const QString &path);
    ~DatabaseManager();

    /**
     * Adds a dictionary to the database.
     * @param path Path to the dictionary.
     * @return Error code. Can be turned into a string with a call to
     *         errorCodeToString().
     */
    int addDictionary(const QString &path);

    /**
     * Deletes a dictionary from the database.
     * @param name The name of the dictionary.
     * @return Error code. Can be turned into a string with a call to
     *         errorCodeToString().
     */
    int deleteDictionary(const QString &name);

    /**
     * Sets the disabled dictionaries.
     * @param dictionaries A list of dictionary names to disable.
     * @return Error code. Can be turned into a string with a call to
     *         errorCodeToString().
     */
    int disableDictionaries(const QStringList &dictionaries);

    /**
     * Translates an error code from add/deleteDictionary() to a human readable
     * string.
     * @param code The error code.
     * @return A string describing the error.
     */
    QString errorCodeToString(const int code) const;

    /**
     * Gets a list of dictionary names in the database in arbitrary order.
     * @return A list of dictionary names.
     */
    QStringList getDictionaries() const;

    /**
     * Gets the list of disabled dictionaries.
     * @return The names of all disabled dictionaries.
     */
    QStringList getDisabledDictionaries() const;

    /**
     * Searches for terms that exactly match the query. Does automatic
     * conversion from katakana to hiragana.
     * @param      query The term to query for.
     * @param[out] terms A list of matching terms. Belongs to the caller.
     * @return Empty string on success, error string on error.
     */
    QString queryTerms(const QString &query, QList<SharedTerm> &terms) const;

    /**
     * Searches for kanji that exactly match the query.
     * @param      query The kanji to look for. Should be a single character.
     * @param[out] kanji The Kanji struct to populate.
     * @return Empty string on success, error string on error.
     */
    QString queryKanji(const QString &query, Kanji &kanji) const;

private:
    /**
     * Initializes the dictionary cache so ids can be quickly mapped to names.
     */
    int initCache();

    /**
     * Gets the name of the dictionary corresponding the ID.
     * @param id The id of the dictionary to look for.
     * @return The name of the dictionary if found, empty string otherwise.
     */
    QString getDictionary(const uint64_t id) const;

    /**
     * Helper method for queryTerms.
     * @param[out] terms A list of Term structs with the expression and reading
     *                   fields populated.
     * @return An SQLite error code on failure.
     */
    int populateTerms(const QList<SharedTerm> &terms) const;

    /**
     * Helper method for retrieving tag information.
     * @param      id     The id of the dictionary the tag comes from.
     * @param      tagStr The name of the tag.
     * @param[out] tags   The list to put the tags in.
     */
    void addTags(const uint64_t  id,
                 const QString  &tagStr,
                 QList<Tag>     &tags) const;

    /**
     * Adds term frequencies to a Term struct.
     * @param[out] term The term struct to add frequencies to.
     * @return An SQLite error code on failure.
     */
    int addFrequencies(Term &term) const;

    /**
     * Adds kanji frequencies to a Kanji struct.
     * @param[out] kanji The kanji struct to add frequencies to.
     * @return An SQLite error code on failure.
     */
    int addFrequencies(Kanji &kanji) const;

    /**
     * Adds frequencies to a frequency list. Should probably not be called
     * directly.
     * @param      query      The sql query to use on the database. Must take
     *                        one bind.
     * @param      expression The first sql bind.
     * @param[out] freq       The list to add frequencies to.
     * @param      reading    The reading of the term if available.
     * @return An SQLite error code on failure.
     */
    int addFrequencies(
        const char *query,
        const QString &expression,
        QList<Frequency> &freq,
        const QString &reading = QString()) const;

    /**
     * Helper method for adding pitch accents to a Term.
     * @param[out] term The term to add pitch accents to. Must have the
     *                  expression field set.
     * @return An SQLite error code on failure.
     */
    int addPitches(Term &term) const;

    /**
     * Converts half-width katakana to full-width katakana.
     * @param query The query string to convert.
     */
    QString halfToFull(const QString &query) const;

    /**
     * Converts all the katakana in a string to their equivalent hiragana.
     * @param query The string to convert.
     * @return Query with all the katakana replaced with hiragana.
     */
    QString kataToHira(QString query) const;

    /**
     * Converts a raw JSON array of strings to a QStringList.
     * @param jsonstr A raw JSON string representing an array of strings.
     * @return The QStringList containing all the elements of the JSON string.
     */
    QStringList jsonArrayToStringList(const char *jsonstr) const;

    /**
     * Helper method for determining if an SQLite step resulted in an error.
     * @param step The value returned form sqlite3_step().
     * @return true if code is an error, false otherwise.
     */
    static bool inline isStepError(const int step);

    /* A readonly connection to the dictionary database. */
    sqlite3 *m_db;

    /* Locks the database for reading and writing. */
    mutable QReadWriteLock m_dbLock;

    /* Saved path to the database. */
    const QByteArray m_dbpath;

    /* A set containing special characters that cannot be independent mora. */
    QSet<QString> m_moraSkipChar;

    /* Maps dictionary IDs to dictionary names. */
    QHash<const uint64_t, QString> m_dictionaryCache;

    /* Maps dictionary IDs to a mapping between tag names and Tag structs. */
    QHash<const uint64_t, QHash<QString, Tag>> m_tagCache;
};

#endif // DATABASEMANAGER_H
