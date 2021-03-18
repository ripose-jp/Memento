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

private:
    sqlite3 *m_db;
    const QString m_dbpath;
    QHash<QString, QString> m_kataToHira;
    QHash<const uint64_t, QString> m_dictionaries;

    int populateTerms(const QList<Term *> &terms);
    QString getDictionary(const uint64_t id);
    int addFrequencies(Term *term);
    int addTags(const uint64_t id, const QString &tagStr, QList<Tag> &tags);
    int cacheDictionaries();

    QString kataToHira(const QString &query);
    QStringList jsonArrayToStringList(const char *jsonstr);
    static bool inline isStepError(const int step)
        { return step != SQLITE_ROW && step != SQLITE_DONE; }
};

#endif // DATABASEMANAGER_H