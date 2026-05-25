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

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

#include <QHash>
#include <QPointer>
#include <QReadWriteLock>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroTask>
#else
#include <qcoro/qcorotask.h>
#endif // MEMENTO_SYSTEM_QCORO

#include "setting/settings.h"
#include "dict/querygenerator.h"

/**
 * @brief Object managing the generation of search results.
 */
class DictionarySearchController : public Dictionary
{
    Q_OBJECT

public:
    /**
     * @brief Construct a new DictionarySearchController.
     *
     * @param settings The application settings object.
     * @param parent The parent of this
     */
    DictionarySearchController(Settings *settings, QObject *parent = nullptr);
    virtual ~DictionarySearchController();

    /**
     * @brief Get the static DictionarySearchController instance.
     *
     * @return A pointer to the static DictionarySearchController.
     */
    static DictionarySearchController *instance() noexcept;

    /**
     * @brief Create the static DictionarySearchController instance.
     *
     * @param settings A pointer to the settings object.
     * @return A pointer to the static DictionarySearchController.
     */
    static DictionarySearchController *createInstance(Settings *settings);

    /**
     * @brief Destroy the static DictionarySearchController instance.
     */
    static void destroyInstance();

    /**
     * @brief Searches for all terms.
     *
     * @param query The query to look up. Only matches terms from the start of
     * the query.
     * @param text The text containing the query.
     * @param index The index into text where query start from.
     * @return An awaitable task that returns a list of terms. Belongs to the
     * caller.
     */
    [[nodiscard]]
    QCoro::Task<QList<Term *>> searchTermsAsync(
        QString query, QString text, qsizetype index);

    /**
     * @brief Search for all kanji in a query.
     *
     * @param character The character to search for.
     * @param text The text the kanji originates from.
     * @param index The index into the text where the kanji
     * @return An awaitable task that returns a Kanji. Belongs to the caller.
     */
    [[nodiscard]]
    QCoro::Task<Kanji *> searchKanjiAsync(
        QString query, QString text, qsizetype index);

private slots:
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
     * @brief Synchronously searches for terms.
     *
     * @param query The query to look up. Only matches terms from the start of
     * the query.
     * @param text The text containing the query.
     * @param index The index into text where query start from.
     * @return The list of terms.
     */
    [[nodiscard]]
    QList<Term *> searchTermsSync(
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

    /**
     * @brief RAII object for guarding searches against race conditions.
     *
     * This exists to prevent DictionarySearchController from being deleted
     * before all outstanding searches complete.
     */
    class SearchGuard
    {
    public:
        SearchGuard(const SearchGuard &) = delete;
        SearchGuard &operator=(const SearchGuard &) = delete;
        SearchGuard(SearchGuard &&other) noexcept;
        SearchGuard &operator=(SearchGuard &&other) noexcept;
        ~SearchGuard();

    private:
        friend class DictionarySearchController;

        /**
         * @brief Create a new SearchGuard.
         *
         * @param controller The DictionarySearchController object to guard.
         */
        explicit SearchGuard(DictionarySearchController *controller) noexcept;

        /**
         * @brief Calls releaseSearchGuard in the controller.
         */
        void reset() noexcept;

        /* Pointer to the dictionary controller being guarded */
        DictionarySearchController *m_controller{nullptr};
    };

    /**
     * @brief Acquire a guard that keeps this object alive for a search worker.
     *
     * @return A guard, or std::nullopt if shutdown has started.
     */
    [[nodiscard]]
    std::optional<SearchGuard> acquireSearchGuard();

    /**
     * @brief Release a search guard and notify waiters if no workers remain.
     */
    void releaseSearchGuard() noexcept;

    /* Static pointer to the global instance */
    static inline DictionarySearchController *m_instance{nullptr};

    /* The application settings object */
    QPointer<Settings> m_settings{nullptr};

    /* Mutex for the list of query generators */
    mutable QReadWriteLock m_generatorsMutex;

    /* List of QueryGenerators */
    std::vector<std::unique_ptr<QueryGenerator>> m_generators;

    /* Mutex for accessing dictionary order */
    mutable QReadWriteLock m_dictionaryOrderMutex;

    /* Maps dictionary IDs to priorities. */
    QHash<int64_t, qsizetype> m_dictionaryOrder;

    /* Mutex for the lifetime of queued and running searches */
    std::mutex m_searchLifetimeMutex;

    /* Signaled when all queued and running searches are finished */
    std::condition_variable m_noActiveSearches;

    /* Number of searches scheduled but not yet finished */
    uint64_t m_activeSearches{0};

    /* True once this object has started shutting down */
    bool m_shuttingDown{false};
};
