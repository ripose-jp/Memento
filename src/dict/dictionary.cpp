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

#include <QDebug>
#include <mecab.h>

#include "../util/directoryutils.h"

#define WORD_INDEX 6
#define MECAB_QUERY_THREADS 3
#define EXACT_QUERY_THREADS 3

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
}

Dictionary::~Dictionary()
{
    delete m_db;
    delete m_tagger;
}

QList<Term *> *Dictionary::search(const QString &query, 
                                  const QString &subtitle,
                                  const int index,
                                  const int *currentIndex)
{
    QList<Term *> *terms = new QList<Term *>;

    // Fork worker threads for exact queries
    QList<QThread *> threads;
    size_t queriesPerThread = query.size() / EXACT_QUERY_THREADS + 1;
    for (size_t i = 0; 
         i < EXACT_QUERY_THREADS && queriesPerThread * i < query.size();
         ++i)
    {
        QString str = query;
        str.chop(queriesPerThread * i);

        int endSize = str.size() - queriesPerThread;
        endSize = endSize < 0 ? 0 : endSize;

        QThread *worker =
            new ExactWorker(str, endSize, subtitle, index, currentIndex,
                            terms, m_db);
        
        worker->start();
        threads.append(worker);
    }

    // Get lematized queries
    QList<QPair<QString, QString>> queries = generateQueries(query);
    if (!queries.isEmpty())
    {
        queriesPerThread = queries.size() / MECAB_QUERY_THREADS + 1;
        for (size_t i = 0;
             i < MECAB_QUERY_THREADS && 
             queries.constBegin() + queriesPerThread * i < queries.constEnd();
             ++i)
        {
            auto endIt = queries.constBegin() + queriesPerThread * (i + 1);
            endIt = endIt < queries.constEnd() ? endIt : queries.constEnd();

            QThread *worker = 
                new MeCabWorker(
                    queries.constBegin() + queriesPerThread * i, endIt,
                    subtitle, index, currentIndex, terms, m_db);
            
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
    struct cloze_compare
    {
        bool operator()(const Term *lhs, const Term *rhs)
        {
            return lhs->clozeBody.size() > rhs->clozeBody.size() ||
                   (lhs->clozeBody.size() == rhs->clozeBody.size() && lhs->score > rhs->score);
        }
    } comp;
    if (index == *currentIndex)
    {
        std::sort(terms->begin(), terms->end(), comp);
    }

    return terms;
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

        // Generate cloze data in entries
        for (Term *term : results)
        {
            term->sentence    = subtitle;
            term->clozePrefix = term->sentence.left(index);
            term->clozeBody   = term->sentence.mid(index, query.size());
            term->clozeSuffix = term->sentence.right(index + query.size());
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

        // Generate cloze data in entries
        for (Term *term : results)
        {
            term->sentence    = subtitle;
            term->clozePrefix = term->sentence.left(index);
            term->clozeBody   = term->sentence.mid(index, pair.second.size());
            term->clozeSuffix = term->sentence.right(index + pair.second.size());
        }

        terms->append(results);
        ++begin;
    }
}
