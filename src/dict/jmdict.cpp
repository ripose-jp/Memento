#include "jmdict.h"

#include "../util/directoryutils.h"

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <exception>
#include <memory>

sql::db *m_db = new sql::db(QString(DirectoryUtils::getDictionaryDir() + JMDICT_DB_NAME).toStdString());

JMDict::JMDict()
{

}

JMDict::~JMDict()
{

}

void JMDict::query(const std::string &query, const QueryType type)
{
    m_db->exec(
          sql::query("SELECT DISTINCT entry FROM reading WHERE kana" + compare(type)) % query,
          showEntry);
    m_db->exec(
          sql::query("SELECT DISTINCT entry FROM kanji WHERE kanji" + compare(type)) % query,
          showEntry);
}

std::string JMDict::compare(QueryType type)
{
    if (QueryType::EXACT)
        return "=%Q";
    else if (QueryType::FULLTEXT)
        return " LIKE '%%%q%%'";
    return " LIKE '%q%%'";
}

int accumulate(void* to, int, char** what, char**) {
    std::string& app = *static_cast<std::string*>(to);
    if (app.size())
        app += ", ";
    app += *what;
    return 0;
}

int showGloss(void* s, int, char** value, char**) {
    std::string& sense = *static_cast<std::string*>(s);
    if (sense != value[0]) {
        sense = value[0];
        std::cout << "  " << std::setw(2) << sense << ")  ";
    }
    else
        std::cout << "       ";
    std::cout << value[1] << std::endl;
    return 0;
}

int showEntry(void*, int, char** value, char**) {
    std::string kanji, kana;
    m_db->exec(
          sql::query("SELECT kanji FROM kanji WHERE entry=%s") % *value,
          accumulate, &kanji);
    m_db->exec(
          sql::query("SELECT kana FROM reading WHERE entry=%s") % *value,
          accumulate, &kana);

    if (kanji.size())
        std::cout << kanji << " (" << kana << ')';
    else
        std::cout << kana;

    std::cout << std::endl;
    
    std::string sense;
    m_db->exec(
          sql::query("SELECT sense, gloss FROM gloss WHERE lang=%Q AND entry=%s "
                   "ORDER BY sense") % std::string("eng") % *value,
          showGloss, &sense);
    return 0;
}