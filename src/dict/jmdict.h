#ifndef JMDICT_H
#define JMDICT_H

#include "entry.h"
#include "sqlite.h"

class JMDict
{
public:
    enum QueryType
    {
        FULLTEXT,
        BEGINNING,
        EXACT
    };

    JMDict(const std::string &path);
    ~JMDict();
    Entry *query(const std::string &query, const JMDict::QueryType type);

private:
    sql::db *m_db;

    std::string compare(QueryType type);
};

#endif // JMDICT_H