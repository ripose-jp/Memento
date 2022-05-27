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
#include <mecab.h>
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>

#include "../util/constants.h"
#include "../util/globalmediator.h"
#include "../util/utils.h"
#include "databasemanager.h"

/**
 * A pair to search for. The deconjugated string is used for querying the
 * database and the surface string is used for cloze generation.
 */
struct SearchPair
{
    /* The deconjugated string */
    QString deconj;

    /* The raw conjugated string */
    QString surface;
};

/* Begin Static Helpers */
#if defined(Q_OS_WIN)
/**
 * This whole section is necessary on Windows because MeCab has a bug that
 * prevents it from loading dictionaries if there are spaces in the path on
 * Windows. If Memento is to be install in "Program Files", this quickly
 * becomes an issue. This workaround turns all long paths into space-less
 * short paths.
 */

#include <fileapi.h>

/**
 * Takes a Windows long path and returns an 8.3/short path.
 * @param path The Window long path to convert.
 * @return A Windows short path, or the empty string on error.
 */
static QByteArray toWindowsShortPath(const QString &path)
{
    QByteArray pathArr = path.toUtf8();
    DWORD length = 0;
    TCHAR *buf = NULL;

    length = GetShortPathName(pathArr.constData(), NULL, 0);
    if (length == 0)
    {
        return "";
    }

    buf = new TCHAR[length];
    length = GetShortPathName(pathArr, buf, length);
    if (length == 0)
    {
        delete[] buf;
        return "";
    }

    QByteArray ret = QByteArray(buf);
    delete[] buf;
    return ret;
}

/**
 * Generates the MeCab argument on Windows.
 * @return An argument to pass MeCab so it uses the install's ipadic.
 */
static QByteArray genMecabArg()
{
    QByteArray arg = "-r ";
    arg += toWindowsShortPath(
        DirectoryUtils::getDictionaryDir() + "ipadic" + SLASH + "dicrc"
    );
    arg += " -d ";
    arg += toWindowsShortPath(DirectoryUtils::getDictionaryDir() + "ipadic");
    return arg;
}

#endif
/* End Static Helpers */
/* Begin Constructor/Destructor */

#if defined(Q_OS_WIN)
#define MECAB_ARG (genMecabArg())
#elif defined(APPIMAGE) || defined(APPBUNDLE)
#define MECAB_ARG ( \
    "-r " + DirectoryUtils::getDictionaryDir() + "ipadic" + SLASH + "dicrc " \
    "-d " + DirectoryUtils::getDictionaryDir() + "ipadic" \
).toUtf8()
#else
#define MECAB_ARG ("")
#endif

Dictionary::Dictionary(QObject *parent) : QObject(parent)
{
    m_db = new DatabaseManager(DirectoryUtils::getDictionaryDB());

    QByteArray mecabArg = MECAB_ARG;
    m_tagger = MeCab::createTagger(mecabArg);
    if (m_tagger == nullptr)
    {
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

    initDictionaryOrder();

    GlobalMediator *med = GlobalMediator::getGlobalMediator();
    med->setDictionary(this);
    connect(
        med, &GlobalMediator::dictionaryOrderChanged,
        this, &Dictionary::initDictionaryOrder
    );
}

#undef MECAB_ARG

void Dictionary::initDictionaryOrder()
{
    m_dicOrder.lock.lockForWrite();

    QSettings settings;
    settings.beginGroup(SETTINGS_DICTIONARIES);
    QStringList dicts = m_db->getDictionaries();
    m_dicOrder.map.clear();
    for (const QString &dict : dicts)
    {
        m_dicOrder.map[dict] = settings.value(dict).toInt();
    }
    settings.endGroup();

    m_dicOrder.lock.unlock();
}

Dictionary::~Dictionary()
{
    delete m_db;
    delete m_tagger;
}

/* End Constructor/Destructor */
/* Begin Term Searching Methods */

void Dictionary::ExactWorker::run()
{
    while (query.size() > endSize && index == *currentIndex)
    {
        QList<SharedTerm> results;
        db.queryTerms(query, results);

        /* Generate cloze data in entries */
        QString clozePrefix;
        QString clozeBody;
        QString clozeSuffix;
        if (!results.isEmpty())
        {
            clozePrefix = subtitle.left(index);
            clozeBody   = subtitle.mid(index, query.size());
            clozeSuffix = subtitle.right(
                    subtitle.size() - (index + query.size())
                );
        }

        for (SharedTerm term : results)
        {
            term->sentence = subtitle;
            term->clozePrefix = clozePrefix;
            term->clozeBody = clozeBody;
            term->clozeSuffix = clozeSuffix;
        }

        terms.append(results);
        query.chop(1);
    }
}

void Dictionary::MeCabWorker::run()
{
    while (begin != end && index == *currentIndex)
    {
        const SearchPair &pair = *begin;
        QList<SharedTerm> results;
        db.queryTerms(pair.deconj, results);

        /* Generate cloze data in entries */
        QString clozePrefix;
        QString clozeBody;
        QString clozeSuffix;
        if (!results.isEmpty())
        {
            clozePrefix = subtitle.left(index);
            clozeBody   = subtitle.mid(index, pair.surface.size());
            clozeSuffix = subtitle.right(
                subtitle.size() - (index + pair.surface.size())
            );
        }

        for (SharedTerm term : results)
        {
            term->sentence = subtitle;
            term->clozePrefix = clozePrefix;
            term->clozeBody = clozeBody;
            term->clozeSuffix = clozeSuffix;
        }

        terms.append(results);
        ++begin;
    }
}

#define WORD_INDEX 6

/**
 * Recursively generates queries and surface strings from a node.
 * @param node The node to start at. Usually the next node after the BOS
 *             node. Is nullptr safe.
 * @return A list of conjugated string and surface (raw) strings. Belongs to
 *         the caller.
 */
static QList<SearchPair> *generateQueriesHelper(
    const MeCab::Node *node)
{
    QList<SearchPair> *queries = new QList<SearchPair>;
    while (node)
    {
        QString deconj =
            QString::fromUtf8(node->feature).split(',')[WORD_INDEX];
        QString surface = QString::fromUtf8(node->surface, node->length);
        if (deconj != "*")
        {
            queries->push_back(SearchPair{
                .deconj = deconj,
                .surface = surface
            });
        }

        if (node->next)
        {
            QList<SearchPair> *subQueries = generateQueriesHelper(node->next);
            for (SearchPair &p : *subQueries)
            {
                p.deconj.prepend(surface);
                p.surface.prepend(surface);
                queries->push_back(p);
            }
            delete subQueries;
        }

        node = node->bnext;
    }
    return queries;
}

#undef WORD_INDEX

QList<SearchPair> Dictionary::generateQueries(const QString &query)
{
    QList<SearchPair> queries;
    if (query.isEmpty() || m_tagger == nullptr)
    {
        return queries;
    }

    /* Lemmatize the query */
    MeCab::Lattice *lattice = MeCab::createLattice();
    QByteArray queryArr = query.toUtf8();
    lattice->set_sentence(queryArr);
    if (!m_tagger->parse(lattice))
    {
        qDebug() << "Cannot access MeCab";
        delete lattice;
        return queries;
    }

    /* Generate queries */
    QList<SearchPair> *unfiltered =
        generateQueriesHelper(lattice->bos_node()->next);
    QSet<QString> duplicates;
    for (const SearchPair &p : *unfiltered)
    {
        if (query.startsWith(p.deconj) || duplicates.contains(p.deconj))
        {
            continue;
        }
        queries << p;
        duplicates << p.deconj;
    }
    delete unfiltered;
    delete lattice;

    return queries;
}

/* The maximum number of queries one thread can be accountable for. */
#define QUERIES_PER_THREAD 4

SharedTermList Dictionary::searchTerms(
    const QString query,
    const QString subtitle,
    const int index,
    const int *currentIndex)
{
    SharedTermList terms = SharedTermList(new QList<SharedTerm>);

    /* Fork worker threads for exact queries */
    QList<DictionaryWorker *> workers;
    for (QString str = query; !str.isEmpty(); str.chop(QUERIES_PER_THREAD))
    {
        int endSize = str.size() - QUERIES_PER_THREAD;
        if (endSize < 0)
        {
            endSize = 0;
        }

        DictionaryWorker *worker = new ExactWorker(
            str,
            endSize,
            subtitle,
            index,
            currentIndex,
            *m_db
        );

        worker->start();
        workers.append(worker);
    }

    /* Get lemmatized queries */
    QList<SearchPair> queries = generateQueries(query);
    if (!queries.isEmpty())
    {
        for (size_t i = 0;
             queries.constBegin() + i < queries.constEnd();
             i += QUERIES_PER_THREAD)
        {
            auto endIt = queries.constBegin() + i + QUERIES_PER_THREAD;
            if (endIt > queries.constEnd())
                endIt = queries.constEnd();

            DictionaryWorker *worker = new MeCabWorker(
                queries.constBegin() + i,
                endIt,
                subtitle,
                index,
                currentIndex,
                *m_db
            );

            worker->start();
            workers.append(worker);
        }
    }

    /* Wait for the exact thread to finish */
    for (DictionaryWorker *worker : workers)
    {
        worker->wait();
        terms->append(worker->terms);
        delete worker;
    }

    /* Sort the results by cloze length and score */
    if (index != *currentIndex)
    {
        return nullptr;
    }
    std::sort(terms->begin(), terms->end(),
        [] (const SharedTerm lhs, const SharedTerm rhs) -> bool {
            return lhs->clozeBody.size() > rhs->clozeBody.size() ||
                (
                    lhs->clozeBody.size() == rhs->clozeBody.size() &&
                    lhs->score > rhs->score
                );
        }
    );

    /* Sort internal term data */
    if (index != *currentIndex)
    {
        return nullptr;
    }

    m_dicOrder.lock.lockForRead();
    for (SharedTerm term : *terms)
    {
        std::sort(term->definitions.begin(), term->definitions.end(),
            [=] (const TermDefinition &lhs, const TermDefinition &rhs) -> bool {
                uint32_t lhsPriority = m_dicOrder.map[lhs.dictionary];
                uint32_t rhsPriority = m_dicOrder.map[rhs.dictionary];
                return lhsPriority < rhsPriority ||
                       (lhsPriority == rhsPriority && lhs.score > rhs.score);
            }
        );
        std::sort(term->frequencies.begin(), term->frequencies.end(),
            [=] (const Frequency &lhs, const Frequency &rhs) -> bool {
                return m_dicOrder.map[lhs.dictionary] <
                       m_dicOrder.map[rhs.dictionary];
            }
        );
        sortTags(term->tags);
        for (TermDefinition &def : term->definitions)
        {
            sortTags(def.tags);
            sortTags(def.rules);
        }
    }
    m_dicOrder.lock.unlock();

    return terms;
}

#undef QUERIES_PER_THREAD

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

QStringList Dictionary::getDictionaries()
{
    QStringList dictionaries = m_db->getDictionaries();

    m_dicOrder.lock.lockForRead();
    std::sort(dictionaries.begin(), dictionaries.end(),
        [=] (const QString &lhs, const QString &rhs) -> bool {
            return m_dicOrder.map[lhs] < m_dicOrder.map[rhs];
        }
    );
    m_dicOrder.lock.unlock();

    return dictionaries;
}

QStringList Dictionary::getDisabledDictionaries()
{
    return m_db->getDisabledDictionaries();
}

/* End Dictionary Methods */
/* Begin Helpers */

void Dictionary::sortTags(QList<Tag> &tags)
{
    std::sort(tags.begin(), tags.end(),
        [] (const Tag &lhs, const Tag &rhs) -> bool {
            return lhs.order < rhs.order ||
                (lhs.order == rhs.order && lhs.score > rhs.score);
        }
    );
}

/* End Helpers */
