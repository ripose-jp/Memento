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

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QObject>

#include <QList>
#include <QReadWriteLock>
#include <QString>

#include <memory>
#include <vector>

#include "expression.h"
#include "querygenerator.h"

class DatabaseManager;

/**
 * The intended API for interacting with the database.
 */
class Dictionary : public QObject
{
    Q_OBJECT

public:
    Dictionary(QObject *parent = nullptr);
    virtual ~Dictionary();

    /**
     * Searches for all terms in the query.
     * @param query        The query to look for terms in. Only matches terms
     *                     that start from the beginning of the query.
     * @param subtitle     The subtitle the query appears in.
     * @param index        The index into the subtitle where the query begins.
     * @param currentIndex A pointer to the current index. If this value is no
     *                     different from the index before this method is done,
     *                     the search is aborted.
     * @return A list of all the terms found, nullptr if the search was aborted.
     *         Belongs to the caller.
     */
    SharedTermList searchTerms(
        const QString query,
        const QString subtitle,
        const int index,
        const int *currentIndex);

    /**
     * Searches for a single kanji.
     * @param character The kanji to search for. Should be a single character.
     * @return A kanji containing all the information that was found.
     */
    SharedKanji searchKanji(const QString character);

    /**
     * Adds a dictionary.
     * @param path The path to the dictionary.
     * @return Empty string on success, error string on error.
     */
    QString addDictionary(const QString &path);

    /**
     * Adds multiple dictionaries.
     * @param paths The paths to the dictionaries.
     * @return Empty string on success, error string on error.
     */
    QString addDictionary(const QStringList &paths);

    /**
     * Deletes a dictionary.
     * @param name The name of the dictionary.
     * @return Empty string on success, error string on error.
     */
    QString deleteDictionary(const QString &name);

    /**
     * Sets the set of disabled dictionaries to the provided list.
     * @param dictionaries The list of dictionaries.
     * @return Empty string on success, error string on error.
     */
    QString disableDictionaries(const QStringList &dictionaries);

    /**
     * Gets a list of dictionaries ordered by user preference.
     * @return A list of dictionaries ordered by user preference.
     */
    QStringList getDictionaries() const;

    /**
     * Gets the list of disabled dictionaries.
     * @return The names of all disabled dictionaries.
     */
    QStringList getDisabledDictionaries() const;

private Q_SLOTS:
    /**
     * Populates the dictionary order map.
     */
    void initDictionaryOrder();

    /**
     * Populates the list of QueryGenerators.
     */
    void initQueryGenerators();

private:
    /**
     * Generate queries from text.
     * @param text The text to generate queries from.
     * @return The list of SearchQuery.
     */
    [[nodiscard]]
    std::vector<SearchQuery> generateQueries(const QString &text) const;

    /**
     * Sorties queries in order from ascending length of the surface.
     * @param[out] queries The list of queries to sort.
     */
    static void sortQueries(std::vector<SearchQuery> &queries);

    /**
     * Filters out duplicates from the queries vector.
     * @param[out] queries The queries to filter duplicates from.
     */
    static void filterDuplicates(std::vector<SearchQuery> &queries);

    /**
     * Sort the term list by priority and length.
     * @param[out] terms The term list to sort.
     */
    void sortTerms(SharedTermList &terms) const;

    /**
     * Sorts tag by descending order, breaking ties on ascending score.
     * @param[out] tags The list of tags to sort.
     */
    void sortTags(QList<Tag> &tags) const;

    /* The DatabaseManager */
    std::unique_ptr<DatabaseManager> m_db;

    /* Mutex for the list of query generators */
    mutable QReadWriteLock m_generatorsMutex;

    /* List of QueryGenerators */
    std::vector<std::unique_ptr<QueryGenerator>> m_generators;

    /* Contains dictionary priority information. */
    struct DictOrder
    {
        /* Maps dictionary names to priorities. */
        QHash<QString, int> map;

        /* Used for locking for reading and writing. */
        mutable QReadWriteLock lock;
    } m_dicOrder;
};

#endif // DICTIONARY_H
