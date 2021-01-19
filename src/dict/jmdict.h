////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#ifndef JMDICT_H
#define JMDICT_H

#include "entry.h"
#include "sqlite.h"

#include <QString>
#include <QList>
#include <QHash>
#include <QSemaphore>
#include <QMutex>

class JMDict
{
public:
    enum QueryType
    {
        FULLTEXT,
        BEGINNING,
        EXACT
    };

    JMDict(const QString &path);
    ~JMDict();
    QList<Entry *> *query(const QString &query, const JMDict::QueryType type);
    void reopenDictionary();

private:
    sql::db *m_db;
    const QString m_path;
    QMutex m_numReadersMutex;
    QSemaphore m_readerWriter;
    int m_numReaders;
    QHash<QString, QString> m_kataToHira;

    QString katakanaToHiragana(const QString &str);
    std::string compare(QueryType type);
};

#endif // JMDICT_H