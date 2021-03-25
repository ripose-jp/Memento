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

#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <mecab.h>
#include <algorithm>

#include "../util/directoryutils.h"
#include "../util/globalmediator.h"
#include "../util/constants.h"

#define WORD_INDEX          6
#define QUERIES_PER_THREAD  4

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define MECAB_ARG ("-r " + DirectoryUtils::getDictionaryDir() + \
                       "ipadic" + SLASH + "dicrc " \
                       "-d " + DirectoryUtils::getDictionaryDir() + \
                       "ipadic").toLocal8Bit()
#elif __linux__
    #define MECAB_ARG ("")
#endif

Dictionary::Dictionary()
{
    m_db = new DatabaseManager(DirectoryUtils::getDictionaryDB());

    m_tagger = MeCab::createTagger(MECAB_ARG);
    if (m_tagger == nullptr)
        qDebug() << MeCab::getLastError();

    buildPriorities();

    GlobalMediator::getGlobalMediator()->setDictionary(this);
}

Dictionary::~Dictionary()
{
    delete m_db;
    delete m_tagger;
}

QList<Term *> *Dictionary::searchTerms(const QString &query, 
                                       const QString &subtitle,
                                       const int index,
                                       const int *currentIndex)
{
    QList<Term *> *terms = new QList<Term *>;

    // Fork worker threads for exact queries
    QList<QThread *> threads;
    for (QString str = query; !str.isEmpty(); str.chop(QUERIES_PER_THREAD))
    {
        int endSize = str.size() - QUERIES_PER_THREAD;
        if (endSize < 0)
            endSize = 0;

        QThread *worker = new ExactWorker(str, endSize, subtitle, index, currentIndex, terms, m_db);
        
        worker->start();
        threads.append(worker);
    }

    // Get lematized queries
    QList<QPair<QString, QString>> queries = generateQueries(query);
    if (!queries.isEmpty())
    {
        for (size_t i = 0; queries.constBegin() + i < queries.constEnd(); i += QUERIES_PER_THREAD)
        {
            auto endIt = queries.constBegin() + i + QUERIES_PER_THREAD;
            if (endIt > queries.constEnd())
                endIt = queries.constEnd();

            QThread *worker = 
                new MeCabWorker(queries.constBegin() + i, endIt, subtitle, index, currentIndex, terms, m_db);
            
            worker->start();
            threads.append(worker);
        }
    }

    // Wait for the exact thread to finish
    for (QThread *thread : threads)
    {
        thread->wait();
        delete thread;
    }
    
    // Sort by the length of the cloze match
    if (index == *currentIndex)
    {
        std::sort(terms->begin(), terms->end(), 
            [] (const Term *lhs, const Term *rhs) -> bool {
                return lhs->clozeBody.size() > rhs->clozeBody.size() ||
                       (lhs->clozeBody.size() == rhs->clozeBody.size() && lhs->score > rhs->score);
            }
        );
        for (Term *term : *terms)
        {
            QMap<QString, uint32_t> priorities = buildPriorities();
            std::sort(term->definitions.begin(), term->definitions.end(),
                [=] (const TermDefinition &lhs, const TermDefinition &rhs) -> bool {
                    uint32_t lhsPriority = priorities[lhs.dictionary];
                    uint32_t rhsPriority = priorities[rhs.dictionary];
                    return lhsPriority < rhsPriority || (lhsPriority == rhsPriority && lhs.score > rhs.score);
                }
            );
            std::sort(term->frequencies.begin(), term->frequencies.end(),
                [=] (const Frequency &lhs, const Frequency &rhs) -> bool {
                    return priorities[lhs.dictionary] < priorities[rhs.dictionary];
                }
            );
            sortTags(term->tags);
            for (TermDefinition &def : term->definitions)
            {
                sortTags(def.tags);
                sortTags(def.rules);
            }
        }
    }

    return terms;
}

Kanji *Dictionary::searchKanji(const QString &ch)
{
    Kanji *kanji = new Kanji;
    m_db->queryKanji(ch, *kanji);
    if (kanji->definitions.isEmpty())
    {
        delete kanji;
        return nullptr;
    }

    /* Sort all the information */
    QMap<QString, uint32_t> priorities = buildPriorities();
    std::sort(kanji->frequencies.begin(), kanji->frequencies.end(),
        [=] (const Frequency &lhs, const Frequency &rhs) -> bool {
            return priorities[lhs.dictionary] < priorities[rhs.dictionary];
        }
    );
    std::sort(kanji->definitions.begin(), kanji->definitions.end(),
        [=] (const KanjiDefinition &lhs, const KanjiDefinition &rhs) -> bool {
            return priorities[lhs.dictionary] < priorities[rhs.dictionary];
        }
    );
    for (KanjiDefinition &def : kanji->definitions)
    {
        sortTags(def.tags);
    }

    return kanji;
}

QString Dictionary::addDictionary(const QString &path)
{
    int err;
    if (err = m_db->addDictionary(path))
    {
        return m_db->errorCodeToString(err);
    }
    Q_EMIT GlobalMediator::getGlobalMediator()->dictionaryAdded();
    return "";
}

QString Dictionary::deleteDictionary(const QString &name)
{
    int err;
    if (err = m_db->deleteDictionary(name))
    {
        return m_db->errorCodeToString(err);
    }
    return "";
}

QStringList Dictionary::getDictionaries()
{
    QStringList dictionaries = m_db->getDictionaries();
    QMap<QString, uint32_t> priorities = buildPriorities();
    std::sort(dictionaries.begin(), dictionaries.end(), 
        [=] (const QString &lhs, const QString &rhs) -> bool {
            return priorities[lhs] < priorities[rhs];
        }
    );
    return dictionaries;
}

QMap<QString, uint32_t> Dictionary::buildPriorities()
{
    QMap<QString, uint32_t> priorities;
    QSettings settings;
    settings.beginGroup(DICTIONARIES_SETTINGS_KEY);
    QStringList dicts = m_db->getDictionaries();
    for (const QString &dict : dicts)
    {
        priorities[dict] = settings.value(dict, -1).toUInt();
    }
    settings.endGroup();
    return priorities;
}

QList<QPair<QString, QString>> Dictionary::generateQueries(const QString &query)
{
    QList<QPair<QString, QString>> queries;
    if (query.isEmpty())
    {
        return queries;
    }

    // Lemmatize the query
    MeCab::Lattice *lattice = MeCab::createLattice();
    QByteArray queryArr = query.toUtf8();
    lattice->set_sentence(queryArr);
    if (!m_tagger->parse(lattice))
    {
        qDebug() << "Cannot access MeCab";
        delete lattice;
        return queries;
    }

    // Generate queries
    QSet<QString> duplicates;
    for (const MeCab::Node *basenode = lattice->bos_node()->next;
         basenode;
         basenode = basenode->bnext)
    {
        QString acc = "";
        for (const MeCab::Node *node = basenode; node; node = node->next)
        {
            QString conj = acc + QString::fromUtf8(node->feature).split(',')[WORD_INDEX];
            if (duplicates.contains(conj))
            {
                break;
            }
            QString surface = QString::fromUtf8(node->surface, node->length);
        
            if (conj[conj.size() - 1] != '*' && !query.startsWith(conj))
            {
                queries.push_back(QPair<QString, QString>(conj, acc + surface));
                duplicates.insert(conj);
            }
        
            acc += surface;
        }
    }

    delete lattice;
    
    return queries;
}

void Dictionary::ExactWorker::run()
{
    while (query.size() > endSize && index == *currentIndex)
    {
        QList<Term *> results;
        db->queryTerms(query, results);

        /* Generate cloze data in entries */
        QString clozePrefix;
        QString clozeBody;
        QString clozeSuffix;
        if (!results.isEmpty())
        {
            clozePrefix = subtitle.left(index);
            clozeBody   = subtitle.mid(index, query.size());
            clozeSuffix = subtitle.right(index + query.size());
        }

        for (Term *term : results)
        {
            term->sentence    = subtitle;
            term->clozePrefix = clozePrefix;
            term->clozeBody   = clozeBody;
            term->clozeSuffix = clozeSuffix;
        }

        terms->append(results);
        query.chop(1);
    }
}

void Dictionary::MeCabWorker::run()
{
    while(begin != end && index == *currentIndex)
    {
        const QPair<QString, QString> &pair = *begin;
        QList<Term *> results;
        db->queryTerms(pair.first, results);

        /* Generate cloze data in entries */
        QString clozePrefix;
        QString clozeBody;
        QString clozeSuffix;
        if (!results.isEmpty())
        {
            clozePrefix = subtitle.left(index);
            clozeBody   = subtitle.mid(index, pair.second.size());
            clozeSuffix = subtitle.right(index + pair.second.size());
        }

        for (Term *term : results)
        {
            term->sentence    = subtitle;
            term->clozePrefix = clozePrefix;
            term->clozeBody   = clozeBody;
            term->clozeSuffix = clozeSuffix;
        }

        terms->append(results);
        ++begin;
    }
}


void Dictionary::sortTags(QList<Tag> &tags)
{
    std::sort(tags.begin(), tags.end(), 
        [] (const Tag &lhs, const Tag &rhs) -> bool {
            return lhs.order < rhs.order || (lhs.order == rhs.order && lhs.score > rhs.score);
        }
    );
}
