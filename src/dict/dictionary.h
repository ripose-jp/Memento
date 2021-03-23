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

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <QString>
#include <QList>
#include <QThread>
#include "databasemanager.h"
#include "expression.h"

namespace MeCab
{
    class Tagger;
    class Lattice;
}

class Dictionary
{
public:
    Dictionary();
    ~Dictionary();
    QList<Term *> *searchTerms(const QString &query,
                               const QString &subtitle,
                               const int index,
                               const int *currentIndex);
    Kanji *searchKanji(const QString &character);

    QString addDictionary(const QString &path);
    QString deleteDictionary(const QString &name);

    QStringList getDictionaries();

private:
    DatabaseManager         *m_db;
    MeCab::Tagger           *m_tagger;

    QList<QPair<QString, QString>> generateQueries(const QString &query);
    void sortTags(QList<Tag> &tags);
    QMap<QString, uint32_t> buildPriorities();
                                                   
    class ExactWorker : public QThread
    {
    public:
        ExactWorker(const QString &query,
                    const int endSize,
                    const QString &subtitle,
                    const int index,
                    const int *currentIndex,
                    QList<Term *> *terms,
                    DatabaseManager *db)
                        : query(query),
                          endSize(endSize),
                          subtitle(subtitle),
                          index(index),
                          currentIndex(currentIndex),
                          terms(terms),
                          db(db) {}
        void run() override;

    private:
        QString query;
        const int endSize;
        const QString &subtitle;
        const int index;
        const int *currentIndex;
        QList<Term *> *terms;
        DatabaseManager *db;
    };

    class MeCabWorker : public QThread
    {
    public:
        MeCabWorker(QList<QPair<QString, QString>>::const_iterator begin,
                    QList<QPair<QString, QString>>::const_iterator end,
                    const QString &subtitle,
                    const int index,
                    const int *currentIndex,
                    QList<Term *> *terms,
                    DatabaseManager *db)
                        : begin(begin),
                          end(end),
                          subtitle(subtitle),
                          index(index),
                          currentIndex(currentIndex),
                          terms(terms),
                          db(db) {}
        void run() override;

    private:
        QList<QPair<QString, QString>>::const_iterator begin, end;
        const QString &subtitle;
        const int index;
        const int *currentIndex;
        QList<Term *> *terms;
        DatabaseManager *db;
    };
};

#endif // DICTIONARY_H