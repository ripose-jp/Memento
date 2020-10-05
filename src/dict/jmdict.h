#ifndef JMDICT_H
#define JMDICT_H

#include "entry.h"
#include "sqlite.h"

#include <QList>
#include <QString>

#define JMDICT_DB_NAME "JMDict_e"

class JMDict
{
public:
    JMDict();
    ~JMDict();

    enum QueryType
    {
        FULLTEXT,
        BEGINNING,
        EXACT
    };

    //void query(const QString &query, const QueryType type, const std::function <void (&QList<Entry>)> &callBack);
    void query(const std::string &query, const JMDict::QueryType type);

private:
    std::string compare(QueryType type);
};

static int accumulate(void* to, int, char** what, char**);
static int showGloss(void* s, int, char** value, char**);
static int showEntry(void*, int, char** value, char**);

#endif // JMDICT_H