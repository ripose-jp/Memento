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

#include "dict/dictionarysearch.h"

#include <QtConcurrentRun>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroFuture>
#else
#include <qcoro/core/qcorofuture.h>
#endif // MEMENTO_SYSTEM_QCORO

#include "dict/deconjugationquerygenerator.h"
#include "dict/exactquerygenerator.h"
#ifdef MEMENTO_MECAB_SUPPORT
#include "dict/mecabquerygenerator.h"
#endif // MEMENTO_MECAB_SUPPORT

/* Begin Constructor/Destructor */

DictionarySearch::DictionarySearch(QObject *parent) : Dictionary(parent)
{
    connect(
        this, &DictionarySearch::settingsChanged,
        this, &DictionarySearch::handleSettingsChanged,
        Qt::QueuedConnection
    );
}

DictionarySearch::~DictionarySearch()
{
    /* Prevent deletion before all threads return */
    if (m_runningSearches > 0)
    {
        QEventLoop loop;
        connect(
            this, &DictionarySearch::searchesComplete,
            &loop, &QEventLoop::quit
        );
        loop.exec();
    }

    qDeleteAll(m_terms);
    m_terms.clear();
    delete m_kanji;
    m_kanji = nullptr;
}

/* End Constructor/Destructor */
/* Begin Settings Methods */

Settings *DictionarySearch::settings() const noexcept
{
    return m_settings;
}

void DictionarySearch::setSettings(Settings *value)
{
    if (m_settings == value)
    {
        return;
    }
    if (m_settings)
    {
        m_settings->disconnect(this);
    }
    m_settings = value;
    emit settingsChanged(value);
}

/* End Settings Methods */
/* Begin Search Methods */

void DictionarySearch::searchTerms(
    const QString &query, const QString &text, qsizetype index)
{
    ++m_termsSearchId;
    searchTermsAsync(query, text, index);
}

QCoro::Task<void> DictionarySearch::searchTermsAsync(
    QString query, QString text, qsizetype index)
{
    const quint64 searchId = m_termsSearchId;
    ++m_runningSearches;

    QList<Term *> terms = co_await QtConcurrent::run(
        &DictionarySearch::searchTermsSync,
        this,
        std::move(query),
        std::move(text),
        index
    );
    if (!terms.isEmpty() && settings() != nullptr)
    {
        terms.front()->setAutoPlay(settings()->searchAutoPlayAudio());
    }

    if (searchId == m_termsSearchId)
    {
        clearTermsLater();
        m_terms = std::move(terms);
        emit termsChanged();
    }
    else
    {
        for (Term *term : terms)
        {
            term->deleteLater();
        }
    }

    if (--m_runningSearches == 0)
    {
        emit searchesComplete();
    }
}

QList<Term *> DictionarySearch::searchTermsSync(
    QString query, QString text, qsizetype index)
{
    std::vector<SearchQuery> queries = generateQueries(query);

    sortQueries(queries);
    filterDuplicates(queries);

    /* Query the database */
    QList<Term *> terms;
    for (const SearchQuery &query : queries)
    {
        QString err;
        QList<Term *> results = m_db->queryTerms(query.deconj, nullptr, &err);
        if (!err.isEmpty())
        {
            qDeleteAll(results);
            results.clear();
            qWarning("Could not complete query: %s", qUtf8Printable(err));
            return {};
        }
        if (query.ruleFilter.size() > 0)
        {
            QList<Term *> filtered;
            for (Term *term : results)
            {
                bool keep = false;
                for (const TermDefinition *def : term->definitions())
                {
                    QSet<QString> rules{
                        std::begin(def->rules()), std::end(def->rules())
                    };
                    if (rules.intersects(query.ruleFilter))
                    {
                        keep = true;
                        break;
                    }
                }
                if (keep)
                {
                    filtered.emplaceBack(term);
                }
                else
                {
                    delete term;
                }
            }
            results = std::move(filtered);
        }

        QString clozePrefix;
        QString clozeBody;
        QString clozeSuffix;
        if (!results.isEmpty())
        {
            clozePrefix = text.left(index);
            clozeBody = text.mid(index, query.surface.size());
            clozeSuffix = text.right(
                text.size() - (index + query.surface.size())
            );
        }

        for (Term *term : results)
        {
            term->setClozePrefix(clozePrefix);
            term->setClozeBody(clozeBody);
            term->setClozeSuffix(clozeSuffix);
            term->setConjugationExplanation(query.conjugationExplanation);
        }

        terms.append(std::move(results));
    }

    sortTerms(terms);

    for (Term *term : terms)
    {
        term->moveToThread(thread());
        term->setParent(this);
    }

    return terms;
}

void DictionarySearch::searchKanji(
    const QString &character, const QString &text, qsizetype index)
{
    ++m_kanjiSearchId;
    searchKanjiAsync(character, text, index);
}

QCoro::Task<void> DictionarySearch::searchKanjiAsync(
    QString character, QString text, qsizetype index)
{
    const quint64 searchId = m_kanjiSearchId;
    ++m_runningSearches;

    Kanji *kanji = co_await QtConcurrent::run(
        &DictionarySearch::searchKanjiSync,
        this,
        std::move(character),
        std::move(text),
        index
    );

    if (searchId == m_kanjiSearchId)
    {
        clearKanjiLater();
        m_kanji = kanji;
        emit kanjiChanged();
    }
    else if (kanji)
    {
        kanji->deleteLater();
    }

    if (--m_runningSearches == 0)
    {
        emit searchesComplete();
    }
}

Kanji *DictionarySearch::searchKanjiSync(
    QString character, QString text, qsizetype index)
{
    QString err;
    Kanji *kanji = m_db->queryKanji(character, nullptr, &err);

    if (kanji == nullptr)
    {
        return nullptr;
    }

    /* Delete the Kanji if there are no definitions left */
    if (kanji->definitions().isEmpty())
    {
        delete kanji;
        return nullptr;
    }

    /* Add information */
    kanji->setClozePrefix(text.left(index));
    kanji->setClozeBody(text.mid(index, character.size()));
    kanji->setClozeSuffix(text.right(
        text.size() - (index + character.size())
    ));

    /* Sort all the information */
    m_dictionaryOrderMutex.lockForRead();
    std::sort(std::begin(kanji->m_frequencies), std::end(kanji->m_frequencies),
        [this] (const Frequency *lhs, const Frequency *rhs) -> bool
        {
            return m_dictionaryOrder[lhs->dictionaryInfo()->id()] <
                   m_dictionaryOrder[rhs->dictionaryInfo()->id()];
        }
    );
    std::sort(std::begin(kanji->m_definitions), std::end(kanji->m_definitions),
        [this] (const KanjiDefinition *lhs, const KanjiDefinition *rhs) -> bool
        {
            return m_dictionaryOrder[lhs->dictionaryInfo()->id()] <
                   m_dictionaryOrder[rhs->dictionaryInfo()->id()];
        }
    );
    m_dictionaryOrderMutex.unlock();
    for (KanjiDefinition *def : kanji->m_definitions)
    {
        sortTags(def->m_tags);
    }

    kanji->moveToThread(thread());
    kanji->setParent(this);

    return kanji;
}

std::vector<SearchQuery> DictionarySearch::generateQueries(
    const QString &text) const
{
    QReadLocker lock{&m_generatorsMutex};

    std::vector<SearchQuery> queries;
    for (const std::unique_ptr<QueryGenerator> &gen : m_generators)
    {
        std::vector<SearchQuery> currQueries = gen->generateQueries(text);
        queries.insert(
            std::end(queries),
            std::make_move_iterator(std::begin(currQueries)),
            std::make_move_iterator(std::end(currQueries))
        );
    }
    return queries;
}

void DictionarySearch::sortQueries(std::vector<SearchQuery> &queries)
{
    std::sort(
        std::begin(queries), std::end(queries),
        [] (const SearchQuery &lhs, const SearchQuery &rhs) -> bool
        {
            if (lhs.deconj == rhs.deconj)
            {
                return lhs.surface.size() > rhs.surface.size();
            }
            return lhs.deconj > rhs.deconj;
        }
    );
}

void DictionarySearch::filterDuplicates(std::vector<SearchQuery> &queries)
{
#ifdef MEMENTO_MECAB_SUPPORT
    /* Remove all duplicates that both MeCab and the deconjugator got.
     * Prefer deconjugator results over MeCab. */
    QSet<QString> deconjQueries;
    for (const SearchQuery &query : queries)
    {
        if (query.source == SearchQuery::Source::deconj)
        {
            deconjQueries.insert(query.deconj);
        }
    }
    queries.erase(
        std::remove_if(
            std::begin(queries), std::end(queries),
            [&deconjQueries] (const SearchQuery &query) -> bool
            {
                switch (query.source)
                {
                    case SearchQuery::Source::mecab:
                        return deconjQueries.contains(query.deconj);

                    case SearchQuery::Source::deconj:
                    case SearchQuery::Source::exact:
                        return false;
                }
                return false;
            }
        ),
        std::end(queries)
    );
#endif // MEMENTO_MECAB_SUPPORT

    auto last = std::unique(
        std::begin(queries), std::end(queries),
        [] (const SearchQuery &lhs, const SearchQuery &rhs) -> bool
        {
            return lhs.deconj == rhs.deconj && lhs.ruleFilter == rhs.ruleFilter;
        }
    );
    queries.erase(last, std::end(queries));

    std::erase_if(
        queries,
        [] (const SearchQuery &query) -> bool
        {
            return query.deconj.isEmpty();
        }
    );
}

void DictionarySearch::sortTerms(QList<Term *> &terms) const
{
    std::sort(
        std::begin(terms), std::end(terms),
        [] (const Term *lhs, const Term *rhs) -> bool
        {
            if (lhs->clozeBody().size() != rhs->clozeBody().size())
            {
                return lhs->clozeBody().size() > rhs->clozeBody().size();
            }
            if (lhs->expression().size() != rhs->expression().size())
            {
                return lhs->expression().size() > rhs->expression().size();
            }
            if (lhs->conjugationExplanation().isEmpty() !=
                rhs->conjugationExplanation().isEmpty())
            {
                return lhs->conjugationExplanation().size() >
                    rhs->conjugationExplanation().size();
            }
            return lhs->score() > rhs->score();
        }
    );

    m_dictionaryOrderMutex.lockForRead();
    for (Term *term : terms)
    {
        std::sort(
            std::begin(term->m_definitions), std::end(term->m_definitions),
            [this]
            (const TermDefinition *lhs, const TermDefinition *rhs) -> bool
            {
                qsizetype lhsPriority =
                    m_dictionaryOrder[lhs->dictionaryInfo()->id()];
                qsizetype rhsPriority =
                m_dictionaryOrder[rhs->dictionaryInfo()->id()];
                return lhsPriority < rhsPriority ||
                    (
                        lhsPriority == rhsPriority &&
                        lhs->score() > rhs->score()
                    );
            }
        );
        std::sort(
            std::begin(term->m_frequencies), std::end(term->m_frequencies),
            [this] (const Frequency *lhs, const Frequency *rhs) -> bool
            {
                return m_dictionaryOrder[lhs->dictionaryInfo()->id()] <
                       m_dictionaryOrder[rhs->dictionaryInfo()->id()];
            }
        );
        sortTags(term->m_tags);
        for (TermDefinition *def : term->definitions())
        {
            sortTags(def->m_tags);
        }
    }
    m_dictionaryOrderMutex.unlock();
}

void DictionarySearch::sortTags(QList<Tag *> &tags) const
{
    std::sort(
        std::begin(tags), std::end(tags),
        [] (const Tag *lhs, const Tag *rhs) -> bool
        {
            return lhs->order() < rhs->order() ||
                (lhs->order() == rhs->order() && lhs->score() > rhs->score());
        }
    );
}

/* End Search Methods */
/* Begin Properties */

void DictionarySearch::clearResults()
{
    clearTerms();
    clearKanji();
}

void DictionarySearch::clearTerms()
{
    ++m_termsSearchId;
    clearTermsLater();
}

void DictionarySearch::clearTermsLater()
{
    if (m_terms.isEmpty())
    {
        return;
    }
    QList<Term *> terms = std::move(m_terms);
    m_terms.clear();
    emit termsChanged();
    for (Term *term : terms)
    {
        term->deleteLater();
    }
}

void DictionarySearch::clearKanji()
{
    ++m_kanjiSearchId;
    clearKanjiLater();
}

void DictionarySearch::clearKanjiLater()
{
    if (m_kanji)
    {
        m_kanji->deleteLater();
        m_kanji = nullptr;
        emit kanjiChanged();
    }
}

QQmlListProperty<Term> DictionarySearch::termsQml()
{
    return QQmlListProperty<Term>(this, &m_terms);
}

Kanji *DictionarySearch::kanji() const noexcept
{
    return m_kanji;
}

/* End Properties */
/* Begin Handlers */

void DictionarySearch::handleSettingsChanged()
{
    if (settings() == nullptr)
    {
        return;
    }

    connect(
        settings(), &Settings::searchMatcherExactChanged,
        this, &DictionarySearch::updateGenerators,
        Qt::QueuedConnection
    );
    connect(
        settings(), &Settings::searchMatcherDeconjChanged,
        this, &DictionarySearch::updateGenerators,
        Qt::QueuedConnection
    );
    connect(
        settings(), &Settings::dictionaryOrderChanged,
        this, &DictionarySearch::updateDictionaryOrder,
        Qt::QueuedConnection
    );
#ifdef MEMENTO_MECAB_SUPPORT
    connect(
        settings(), &Settings::searchMatcherMecabIpadicChanged,
        this, &DictionarySearch::updateGenerators,
        Qt::QueuedConnection
    );
#endif // MEMENTO_MECAB_SUPPORT

    updateGenerators();
    updateDictionaryOrder();
}

void DictionarySearch::updateGenerators()
{
    QWriteLocker lock{&m_generatorsMutex};

    m_generators.clear();

    if (settings() == nullptr)
    {
        return;
    }

    if (settings()->searchMatcherExact())
    {
        m_generators.emplace_back(std::make_unique<ExactQueryGenerator>());
    }
    if (settings()->searchMatcherDeconj())
    {
        m_generators.emplace_back(
            std::make_unique<DeconjugationQueryGenerator>()
        );
    }
#ifdef MEMENTO_MECAB_SUPPORT
    if (settings()->searchMatcherMecabIpadic())
    {
        m_generators.emplace_back(std::make_unique<MeCabQueryGenerator>());
    }
#endif // MEMENTO_MECAB_SUPPORT
}

void DictionarySearch::updateDictionaryOrder()
{
    QWriteLocker lock{&m_dictionaryOrderMutex};

    m_dictionaryOrder.clear();
    if (settings() == nullptr)
    {
        return;
    }

    const QList<int64_t> &order = settings()->dictionaryOrder();
    for (qsizetype i{0}; i < order.size(); ++i)
    {
        m_dictionaryOrder.emplace(order[i], i);
    }
}

/* End Handlers */
