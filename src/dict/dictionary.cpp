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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define MECAB_ARG ("-r " + DirectoryUtils::getDictionaryDir() + SLASH + \
                       "naist-jdic" + SLASH + "dicrc " \
                       "-d " + DirectoryUtils::getDictionaryDir() + SLASH + \
                       "naist-jdic").toUtf8()
#elif __linux__
    #define MECAB_ARG ""
#endif

Dictionary::Dictionary()
{
    QString path = DirectoryUtils::getDictionaryDir() + JMDICT_DB_NAME;
    m_dictionary = new JMDict(path);

    m_tagger = MeCab::createTagger(MECAB_ARG);
    qDebug() << MECAB_ARG;
    if (m_tagger == nullptr)
        qDebug() << MeCab::getLastError();
}

Dictionary::~Dictionary()
{
    delete m_dictionary;
    delete m_tagger;
}

QList<Entry *> *Dictionary::search(const QString &query, 
                                   const QString &subtitle,
                                   const int index,
                                   const int *currentIndex)
{
    // Fork worker thread for exact queries
    QList<Entry *> *entries = new QList<Entry *>();
    ExactWorker *worker =
        new ExactWorker(query, subtitle, index, currentIndex, entries, 
                        m_dictionary);
    worker->start();

    // Get lematized queries
    MeCab::Lattice *lattice = lemmatizeQuery(query);
    if (lattice)
    {
        QList<QPair<QString, QString>> queries = 
            generateQueries(lattice, query);
        delete lattice;
        lattice = nullptr;

        for (auto it = queries.begin(); 
             it != queries.end() && index == *currentIndex;
             ++it)
        {
            QPair<QString, QString> pair = *it;
            QList<Entry *> *results = 
                m_dictionary->query(pair.first, JMDict::EXACT);

            if (!results->isEmpty())
            {
                for (auto it = results->begin(); it != results->end(); ++it)
                {
                    (*it)->m_sentence = new QString(subtitle);
                    QString *sentence = (*it)->m_sentence;
                    (*it)->m_clozePrefix = new QString(sentence->left(index));
                    (*it)->m_clozeBody = new QString(pair.second);
                    (*it)->m_clozeSuffix = 
                        new QString(sentence->right(
                            sentence->size() - (index + pair.second.size())));
                }
                entries->append(*results);
            }
            
            delete results;
        }
    }

    // Wait for the exact thread to finish
    worker->wait();

    // Sort by the length of the cloze match
    struct cloze_compare
    {
        bool operator()(const Entry *lhs, const Entry *rhs)
        {
            return lhs->m_clozeBody->size() > rhs->m_clozeBody->size();
        }
    } comp;
    if (index == *currentIndex)
    {
        std::sort(entries->begin(), entries->end(), comp);
    }

    return entries;
}

MeCab::Lattice *Dictionary::lemmatizeQuery(const QString &query)
{
    MeCab::Lattice *lattice = MeCab::createLattice();
    lattice->set_sentence(query.toLocal8Bit().data());
    if (!m_tagger->parse(lattice))
    {
        qDebug() << "Cannot access MeCab";
        qDebug() << query.toLocal8Bit().data();
        delete lattice;
        return nullptr;
    }
    return lattice;
}

QList<QPair<QString, QString>>
    Dictionary::generateQueries(MeCab::Lattice *lattice, const QString &query)
{
    QList<QPair<QString, QString>> queries;

    QString acc = "";
    for (const MeCab::Node *node = lattice->bos_node()->next;
         node != lattice->eos_node(); node = node->next)
    {
        QString conj = acc + QString(node->feature).split(',')[WORD_INDEX];
        QString surface = QString::fromUtf8(node->surface, node->length);
        
        if (!query.startsWith(conj))
        {
            queries.push_front(QPair<QString, QString>(conj, acc + surface));
        }
        
        acc += surface;
    }

    return queries;
}

void Dictionary::ExactWorker::run()
{
    QList<Entry *> *results = nullptr;
    while (!query.isEmpty() && index == *currentIndex)
    {
        results = dictionary->query(query, JMDict::EXACT);

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

void Dictionary::reopenDictionary()
{
    m_dictionary->reopenDictionary();
}
