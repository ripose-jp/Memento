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

#include "dict/dictionary.h"

#include <QQmlListProperty>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroTask>
#else
#include <qcoro/qcorotask.h>
#endif // MEMENTO_SYSTEM_QCORO

#include "dict/data/data.h"
#include "dict/querygenerator.h"
#include "setting/settings.h"

/**
 * @brief Handles a single dictionary search.
 */
class DictionarySearch : public Dictionary
{
    Q_OBJECT

    Q_PROPERTY(
        Settings *settings
        READ settings
        WRITE setSettings
        NOTIFY settingsChanged
        REQUIRED
    )

    Q_PROPERTY(
        QQmlListProperty<Term> terms
        READ termsQml
        NOTIFY termsChanged
    )

    Q_PROPERTY(
        Kanji *kanji
        READ kanji
        NOTIFY kanjiChanged
    )

public:
    DictionarySearch(QObject *parent = nullptr);
    virtual ~DictionarySearch();

    /**
     * @brief Get the settings object.
     *
     * @return The settings object.
     */
    [[nodiscard]]
    Settings *settings() const noexcept;

    /**
     * @brief Set the settings object.
     *
     * @param value The settings object.
     */
    void setSettings(Settings *value);

    /**
     * @brief Searches for all terms. Popuplates the terms property.
     *
     * @param query The query to look up. Only matches terms from the start of
     * the query.
     * @param text The text containing the query.
     * @param index The index into text where query start from.
     */
    Q_INVOKABLE void searchTerms(
        const QString &query, const QString &text, qsizetype index);

    /**
     * @brief Search for all kanji in a query. Populates the kanji property.
     *
     * @param character The character to search for.
     * @param text The text the kanji originates from.
     * @param index The index into the text where the kanji
     */
    Q_INVOKABLE void searchKanji(
        const QString &query, const QString &text, qsizetype index);

    /**
     * @brief Clears the result of the last search.
     */
    Q_INVOKABLE void clearResults();

    /**
     * @brief Clears the terms property.
     */
    Q_INVOKABLE void clearTerms();

    /**
     * @brief Clears the kanji property.
     */
    Q_INVOKABLE void clearKanji();

    /**
     * @brief Get the terms found from the last search.
     *
     * @return The terms found from the last search.
     */
    [[nodiscard]]
    QQmlListProperty<Term> termsQml();

    /**
     * @brief Get the kanji found from the last search.
     *
     * @return The kanji found in the last search.
     */
    [[nodiscard]]
    Kanji *kanji() const noexcept;

signals:
    /**
     * @brief Emitted when the settings object is changed.
     *
     * @param value The new settings object.
     */
    void settingsChanged(Settings *value);

    /**
     * @brief Emitted when terms are changed.
     */
    void termsChanged();

    /**
     * @brief Emitted when the kanji are changed.
     */
    void kanjiChanged();

    /**
     * @brief Emitted when all outstanding searches are finished.
     */
    void searchesComplete();

private slots:
    /**
     * @brief Updates generators and dictionary order when
     */
    void handleSettingsChanged();

    /**
     * @brief Keeps generators up to date with settings.
     */
    void updateGenerators();

    /**
     * @brief Updates the dictionary order.
     */
    void updateDictionaryOrder();

private:
    /**
     * @brief Searches for all terms. Popuplates the terms property.
     *
     * @param query The query to look up. Only matches terms from the start of
     * the query.
     * @param text The text containing the query.
     * @param index The index into text where query start from.
     * @return An awaitable task.
     */
    QCoro::Task<void> searchTermsAsync(
        QString query, QString text, qsizetype index);

    /**
     * @brief Synchronously searches for terms.
     *
     * @param query The query to look up. Only matches terms from the start of
     * the query.
     * @param text The text containing the query.
     * @param index The index into text where query start from.
     * @return The list of terms.
     */
    [[nodiscard]]
    QList<Term *> searchTermsSync(QString query, QString text, qsizetype index);

    /**
     * @brief Search for all kanji in a query. Populates the kanji property.
     *
     * @param character The character to search for.
     * @param text The text the kanji originates from.
     * @param index The index into the text where the kanji
     * @return An awaitable task.
     */
    QCoro::Task<void> searchKanjiAsync(
        QString query, QString text, qsizetype index);

    /**
     * @brief Synchronously searches for kanji.
     *
     * @param character The character to search for.
     * @param text The text the kanji originates from.
     * @param index The index into the text where the kanji
     * @return The kanji found.
     */
    [[nodiscard]]
    Kanji *searchKanjiSync(
        QString query, QString text, qsizetype index);

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
    void sortTerms(QList<Term *> &terms) const;

    /**
     * Sorts tag by descending order, breaking ties on ascending score.
     * @param[out] tags The list of tags to sort.
     */
    void sortTags(QList<Tag *> &tags) const;

    /* The system settings */
    Settings *m_settings{nullptr};

    /* The number of running searches */
    int m_runningSearches{0};

    /* Mutex for the list of query generators */
    mutable QReadWriteLock m_generatorsMutex;

    /* List of QueryGenerators */
    std::vector<std::unique_ptr<QueryGenerator>> m_generators;

    /* Mutex for accessing dictionary order */
    mutable QReadWriteLock m_dictionaryOrderMutex;

    /* Maps dictionary IDs to priorities. */
    QHash<int64_t, qsizetype> m_dictionaryOrder;

    /* The terms of the last search */
    QList<Term *> m_terms;

    /* The kanji of the last search */
    Kanji *m_kanji{nullptr};
};
