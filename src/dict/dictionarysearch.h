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

#ifndef DICTIONARYSEARCH_H
#define DICTIONARYSEARCH_H

#include <QString>
#include <QList>
#include <QThread>
#include "entry.h"
#include "jmdict.h"

namespace MeCab
{
    class Tagger;
}

class DictionarySearch
{
public:
    DictionarySearch();
    ~DictionarySearch();
    QList<Entry *> *search(const QString query,
                           const QString &subtitle,
                           const int index,
                           const int *currentIndex);

private:
    JMDict *m_dictionary;
    MeCab::Tagger *m_tagger;

    MeCab::Lattice *lemmatizeQuery(const QString &query);
    QList<QPair<QString, unsigned int>> generateQueries(MeCab::Lattice *lattice,
                                                        const QString &query);
    void deleteEntries(QList<Entry *> *entries);

    class ExactWorker : public QThread
    {
    public:
        ExactWorker(const QString &query,
                    const QString &subtitle,
                    const int index,
                    const int *currentIndex,
                    QList<Entry *> *entries,
                    JMDict *dictionary)
                        : query(query),
                          subtitle(subtitle),
                          index(index),
                          currentIndex(currentIndex),
                          entries(entries),
                          dictionary(dictionary) {}
        void run() override;

    private:
        QString query;
        const QString &subtitle;
        const int index;
        const int *currentIndex;
        QList<Entry *> *entries;
        JMDict *dictionary;
    };
};

#endif // DICTIONARYSEARCH_H