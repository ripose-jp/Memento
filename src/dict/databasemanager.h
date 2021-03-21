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

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <QString>
#include <QHash>
#include <QMutex>
#include "expression.h"

class DatabaseManager
{
public:
    DatabaseManager(const QString &path);
    ~DatabaseManager();

    int addDictionary(const QString &path);
    int deleteDictionary(const QString &name);
    QString errorCodeToString(const int code);
    QStringList getDictionaries();
    QString queryTerms(const QString &query, QList<Term *> &terms);
    QString queryKanji(const QString &query, Kanji &kanji);

private:
    sqlite3                *m_db;
    const QString           m_dbpath;
    QHash<QString, QString> m_kataToHira;

    QMutex   m_databaseLock;
    QMutex   m_readerLock;
    uint32_t m_readerCount;

    QHash<const uint64_t, QString>             m_dictionaryCache;
    QHash<const uint64_t, QHash<QString, Tag>> m_tagCache;

    int populateTerms(const QList<Term *> &terms);
    QString getDictionary(const uint64_t id);
    void addTags(const uint64_t id, const QString &tagStr, QList<Tag> &tags);

    int addFrequencies(Term &term);
    int addFrequencies(Kanji &kanji);
    int addFrequencies(QList<Frequency> &freq, const QString &expression, const char *query);

    int buildCache();

    bool incrementReaders();
    void decrementReaders();

    QString kataToHira(const QString &query);
    QStringList jsonArrayToStringList(const char *jsonstr);
    static bool inline isStepError(const int step)
        { return step != SQLITE_ROW && step != SQLITE_DONE; }
};

#endif // DATABASEMANAGER_H