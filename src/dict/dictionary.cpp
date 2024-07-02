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

#include "dictionary.h"

#include <algorithm>

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QReadLocker>
#include <QSettings>
#include <QWriteLocker>

#include "databasemanager.h"
#include "deconjugationquerygenerator.h"
#include "exactquerygenerator.h"

#ifdef MECAB_SUPPORT
#include "mecabquerygenerator.h"
#endif // MECAB_SUPPORT

#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/utils.h"

/* Begin Constructor/Destructor */

Dictionary::Dictionary(QObject *parent) : QObject(parent)
{
    m_db = std::make_unique<DatabaseManager>(DirectoryUtils::getDictionaryDB());

    initDictionaryOrder();
    initQueryGenerators();

    GlobalMediator *med = GlobalMediator::getGlobalMediator();
    med->setDictionary(this);
    connect(
        med, &GlobalMediator::dictionaryOrderChanged,
        this, &Dictionary::initDictionaryOrder
    );
    connect(
        med, &GlobalMediator::searchSettingsChanged,
        this, &Dictionary::initQueryGenerators
    );
}

void Dictionary::initDictionaryOrder()
{
    m_dicOrder.lock.lockForWrite();

    QSettings settings;
    settings.beginGroup(Constants::Settings::Dictionaries::GROUP);
    QStringList dicts = m_db->getDictionaries();
    m_dicOrder.map.clear();
    for (const QString &dict : dicts)
    {
        m_dicOrder.map[dict] = settings.value(dict).toInt();
    }
    settings.endGroup();

    m_dicOrder.lock.unlock();
}

void Dictionary::initQueryGenerators()
{
    QWriteLocker lock{&m_generatorsMutex};

    m_generators.clear();

    QSettings settings;
    settings.beginGroup(Constants::Settings::Search::GROUP);

    const bool exactMatching = settings.value(
        Constants::Settings::Search::Matcher::EXACT,
        Constants::Settings::Search::Matcher::EXACT_DEFAULT
    ).toBool();
    if (exactMatching)
    {
        m_generators.emplace_back(std::make_unique<ExactQueryGenerator>());
    }

    const bool deconjMatching = settings.value(
        Constants::Settings::Search::Matcher::DECONJ,
        Constants::Settings::Search::Matcher::DECONJ_DEFAULT
    ).toBool();
    if (deconjMatching)
    {
        m_generators.emplace_back(
            std::make_unique<DeconjugationQueryGenerator>()
        );
    }

#ifdef MECAB_SUPPORT

    const bool mecabIpadicMatching = settings.value(
        Constants::Settings::Search::Matcher::MECAB_IPADIC,
        Constants::Settings::Search::Matcher::MECAB_IPADIC_DEFAULT
    ).toBool();
    if (mecabIpadicMatching)
    {
        m_generators.emplace_back(std::make_unique<MeCabQueryGenerator>());
        if (!m_generators.back()->valid())
        {
            m_generators.pop_back();

            qDebug() << MeCab::getTaggerError();
            QMessageBox::critical(
                nullptr,
                "MeCab Error",
                "Could not initialize MeCab.\n"
                "Memento will still work, but search results will suffer.\n"
#if defined(Q_OS_WIN)
                "Make sure that ipadic is present in\n" +
                DirectoryUtils::getDictionaryDir()
#elif defined(APPIMAGE)
                "Please report this bug at "
                "https://github.com/ripose-jp/Memento/issues"
#elif defined(APPBUNDLE)
                "The current dictionary directory\n" +
                DirectoryUtils::getDictionaryDir() +
                "\nIf there are spaces in this path, please move Memento to a "
                "directory without spaces."
#else
                "Make sure you have a system dictionary installed by running "
                "'mecab -D' from the command line."
#endif
            );
        }
    }

#endif // MECAB_SUPPORT
}

Dictionary::~Dictionary()
{

}

/* End Constructor/Destructor */
/* Begin Term Searching Methods */

SharedTermList Dictionary::searchTerms(
    const QString query,
    const QString subtitle,
    const int index,
    const int *currentIndex)
{
    std::vector<SearchQuery> queries = generateQueries(query);
    if (index != *currentIndex)
    {
        return nullptr;
    }

    sortQueries(queries);
    filterDuplicates(queries);
    if (index != *currentIndex)
    {
        return nullptr;
    }

    /* Query the database */
    SharedTermList terms = SharedTermList(new QList<SharedTerm>);
    for (const SearchQuery &query : queries)
    {
        if (index != *currentIndex)
        {
            return nullptr;
        }

        QList<SharedTerm> results;
        QString err = m_db->queryTerms(query.deconj, results);
        if (!err.isEmpty())
        {
            qDebug() << err;
            return nullptr;
        }
        if (query.ruleFilter.size() > 0)
        {
            results.erase(
                std::remove_if(
                    results.begin(),
                    results.end(),
                    [&] (const SharedTerm &val)
                    {
                        for (const TermDefinition &def : val->definitions)
                        {
                            if (def.rules.intersects(query.ruleFilter))
                            {
                                return false;
                            }
                        }
                        return true;
                    }
                ),
                results.end()
            );
        }

        QString clozePrefix;
        QString clozeBody;
        QString clozeSuffix;
        if (!results.isEmpty())
        {
            clozePrefix = subtitle.left(index);
            clozeBody   = subtitle.mid(index, query.surface.size());
            clozeSuffix = subtitle.right(
                subtitle.size() - (index + query.surface.size())
            );
        }

        for (SharedTerm term : results)
        {
            term->sentence = subtitle;
            term->clozePrefix = clozePrefix;
            term->clozeBody = clozeBody;
            term->clozeSuffix = clozeSuffix;
            term->conjugationExplanation = query.conjugationExplanation;
        }

        terms->append(std::move(results));
    }

    sortTerms(terms);
    if (index != *currentIndex)
    {
        return nullptr;
    }

    return terms;
}

std::vector<SearchQuery> Dictionary::generateQueries(const QString &text) const
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

void Dictionary::sortQueries(std::vector<SearchQuery> &queries)
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

void Dictionary::filterDuplicates(std::vector<SearchQuery> &queries)
{
#ifdef MECAB_SUPPORT
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
#endif // MECAB_SUPPORT

    auto last = std::unique(
        std::begin(queries), std::end(queries),
        [] (const SearchQuery &lhs, const SearchQuery &rhs) -> bool
        {
            return lhs.deconj == rhs.deconj && lhs.ruleFilter == rhs.ruleFilter;
        }
    );
    queries.erase(last, std::end(queries));
}

void Dictionary::sortTerms(SharedTermList &terms) const
{
    std::sort(std::begin(*terms), std::end(*terms),
        [] (const SharedTerm &lhs, const SharedTerm &rhs) -> bool
        {
            if (lhs->clozeBody.size() != rhs->clozeBody.size())
            {
                return lhs->clozeBody.size() > rhs->clozeBody.size();
            }
            if (lhs->expression.size() != rhs->expression.size())
            {
                return lhs->expression.size() > rhs->expression.size();
            }
            if (lhs->conjugationExplanation.isEmpty() !=
                rhs->conjugationExplanation.isEmpty())
            {
                return lhs->conjugationExplanation.size() >
                    rhs->conjugationExplanation.size();
            }
            return lhs->score > rhs->score;
        }
    );

    m_dicOrder.lock.lockForRead();
    for (SharedTerm term : *terms)
    {
        std::sort(std::begin(term->definitions), std::end(term->definitions),
            [=] (const TermDefinition &lhs, const TermDefinition &rhs) -> bool
            {
                uint32_t lhsPriority = m_dicOrder.map[lhs.dictionary];
                uint32_t rhsPriority = m_dicOrder.map[rhs.dictionary];
                return lhsPriority < rhsPriority ||
                       (lhsPriority == rhsPriority && lhs.score > rhs.score);
            }
        );
        std::sort(std::begin(term->frequencies), std::end(term->frequencies),
            [=] (const Frequency &lhs, const Frequency &rhs) -> bool
            {
                return m_dicOrder.map[lhs.dictionary] <
                       m_dicOrder.map[rhs.dictionary];
            }
        );
        sortTags(term->tags);
        for (TermDefinition &def : term->definitions)
        {
            sortTags(def.tags);
        }
    }
    m_dicOrder.lock.unlock();
}

/* End Term Searching Methods */
/* Begin Kanji Searching Methods */

SharedKanji Dictionary::searchKanji(const QString ch)
{
    SharedKanji kanji = SharedKanji(new Kanji);
    m_db->queryKanji(ch, *kanji);

    /* Delete the Kanji if there are no definitions left */
    if (kanji->definitions.isEmpty())
    {
        return nullptr;
    }

    /* Sort all the information */
    m_dicOrder.lock.lockForRead();
    std::sort(kanji->frequencies.begin(), kanji->frequencies.end(),
        [=] (const Frequency &lhs, const Frequency &rhs) -> bool {
            return m_dicOrder.map[lhs.dictionary] <
                   m_dicOrder.map[rhs.dictionary];
        }
    );
    std::sort(kanji->definitions.begin(), kanji->definitions.end(),
        [=] (const KanjiDefinition &lhs, const KanjiDefinition &rhs) -> bool {
            return m_dicOrder.map[lhs.dictionary] <
                   m_dicOrder.map[rhs.dictionary];
        }
    );
    m_dicOrder.lock.unlock();
    for (KanjiDefinition &def : kanji->definitions)
    {
        sortTags(def.tags);
    }

    return kanji;
}

/* End Kanji Searching Methods */
/* Begin Dictionary Methods */

QString Dictionary::addDictionary(const QString &path)
{
    int err = m_db->addDictionary(path);
    if (err)
    {
        return m_db->errorCodeToString(err);
    }
    Q_EMIT GlobalMediator::getGlobalMediator()->dictionariesChanged();
    return "";
}

QString Dictionary::addDictionary(const QStringList &paths)
{
    for (int i = 0; i < paths.size(); ++i)
    {
        int err = m_db->addDictionary(paths[i]);
        if (err)
        {
            if (i > 0)
            {
                Q_EMIT GlobalMediator::getGlobalMediator()
                    ->dictionariesChanged();
            }
            return m_db->errorCodeToString(err);
        }
    }
    Q_EMIT GlobalMediator::getGlobalMediator()->dictionariesChanged();
    return "";
}

QString Dictionary::deleteDictionary(const QString &name)
{
    int err = m_db->deleteDictionary(name);
    if (err)
    {
        return m_db->errorCodeToString(err);
    }
    Q_EMIT GlobalMediator::getGlobalMediator()->dictionariesChanged();
    return "";
}

QString Dictionary::disableDictionaries(const QStringList &dictionaries)
{
    int err = m_db->disableDictionaries(dictionaries);
    if (err)
    {
        return m_db->errorCodeToString(err);
    }
    return "";
}

QStringList Dictionary::getDictionaries() const
{
    QStringList dictionaries = m_db->getDictionaries();

    m_dicOrder.lock.lockForRead();
    std::sort(dictionaries.begin(), dictionaries.end(),
        [=] (const QString &lhs, const QString &rhs) -> bool
        {
            return m_dicOrder.map[lhs] < m_dicOrder.map[rhs];
        }
    );
    m_dicOrder.lock.unlock();

    return dictionaries;
}

QStringList Dictionary::getDisabledDictionaries() const
{
    return m_db->getDisabledDictionaries();
}

/* End Dictionary Methods */
/* Begin Helpers */

void Dictionary::sortTags(QList<Tag> &tags) const
{
    std::sort(std::begin(tags), std::end(tags),
        [] (const Tag &lhs, const Tag &rhs) -> bool
        {
            return lhs.order < rhs.order ||
                (lhs.order == rhs.order && lhs.score > rhs.score);
        }
    );
}

/* End Helpers */
