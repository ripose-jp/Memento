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

#include "dictionarysearch.h"

#include <mecab.h>

#include "../util/directoryutils.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define MECAB_ARG ("-r " + DirectoryUtils::getDictionaryDir() + SLASH + \
                       "naist-jdic" + SLASH + "dicrc " \
                       "-d " + DirectoryUtils::getDictionaryDir() + SLASH + \
                       "naist-jdic").toUtf8()
#elif __linux__
    #define MECAB_ARG ""
#endif

DictionarySearch::DictionarySearch()
{
    QString path = DirectoryUtils::getDictionaryDir() + JMDICT_DB_NAME;
    m_dictionary = new JMDict(path);

    m_tagger = MeCab::createTagger(MECAB_ARG);
    if (m_tagger == nullptr)
        qDebug() << MeCab::getLastError();
}

DictionarySearch::~DictionarySearch()
{
    delete m_dictionary;
    delete m_tagger;
}

QList<Entry *> *DictionarySearch::search(const QString query,
                                         const QString &subtitle,
                                         const int index,
                                         const int *currentIndex)
{
    if (query.isEmpty())
    {
        return nullptr;
    }

    // Fork worker thread for exact queries
    QList<Entry *> *exact_entries = new QList<Entry *>();
    ExactWorker *worker =
        new ExactWorker(query, subtitle, index, currentIndex, exact_entries,
                        m_dictionary);
    worker->start();

    MeCab::Lattice *lattice = lemmatizeQuery(query);
    if (lattice)
    {
        QList<QPair<QString, unsigned int>> queries = generateQueries(lattice);
    }
}

MeCab::Lattice *DictionarySearch::lemmatizeQuery(const QString &query)
{
    MeCab::Lattice *lattice = MeCab::createLattice();
    lattice->set_sentence(query.toLocal8Bit().data());
    if (m_tagger->parse(lattice))
    {
        qDebug() << "Cannot access MeCab";
        delete lattice;
        return nullptr;
    }
    return lattice;
}

QList<QPair<QString, unsigned int>>
    DictionarySearch::generateQueries(MeCab::Lattice *lattice,
                                      const QString &query)
{
    QList<QPair<QString, unsigned int>> queries;

    for (const MeCab::Node *node = lattice->bos_node()->next;
         node != lattice->eos_node(); node = node->next)
    {

    }

    return queries;
}

void DictionarySearch::ExactWorker::run()
{
    QList<Entry *> *results = nullptr;
    while (!query.isEmpty() && index != *currentIndex)
    {
        results = dictionary->query(query, JMDict::FULLTEXT);

        if (!results->isEmpty())
        {
            // Generate cloze data in entries
            for (auto it = results->begin(); it != results->end(); ++it)
            {
                (*it)->m_sentence = new QString(subtitle);
                QString *sentence = (*it)->m_sentence;
                (*it)->m_clozePrefix = 
                    new QString(sentence->left(index));
                (*it)->m_clozeBody = 
                    new QString(sentence->mid(index, query.size()));
                (*it)->m_clozeSuffix = 
                    new QString(
                        sentence->right(
                            sentence->size() - (index + query.size())));
            }
        }
        entries->append(*results);

        delete results;
        results = nullptr;

        query.chop(1);
    }
}

void DictionarySearch::deleteEntries(QList<Entry *> *entries)
{
    for (auto it = entries->begin(); it != entries->end(); ++it)
    {
        delete *it;
    }
    delete entries;
}