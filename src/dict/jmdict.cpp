/*
jmdict, a frontend to the JMdict file. http://mandrill.fuxx0r.net/jmdict.php
Copyright (C) 2004 Florian Bluemel (florian.bluemel@uni-dortmund.de)
Copyright (C) 2020 Ripose

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "jmdict.h"

#include <cstdlib>

#define SENSE_INDEX 0
#define POS_INDEX 1
#define GLOSS_INDEX 2

static int buildEntry(void *void_query_data, int, char **value, char **);
static int accumulateKanji(void *void_entry, int, char **value, char **);
static int accumulateKana(void *void_entry, int, char **value, char **);
static void accumulate(std::string *base, std::string *extra, char **value);
static int buildDefinition(void *void_entry, int, char **value, char **);

struct query_data
{
    sql::db *db;
    Entry *entry;
    std::string entryId;
};

JMDict::JMDict(const std::string &path) : m_db(new sql::db(path)) {}

JMDict::~JMDict()
{
    delete m_db;
}

Entry *JMDict::query(const std::string &query, const QueryType type)
{
    struct query_data querydata;
    querydata.db = m_db;
    querydata.entry = new Entry();

    m_db->exec(
        sql::query("SELECT DISTINCT entry FROM reading WHERE kana " 
                   + compare(type)) % query,
        buildEntry, &querydata
    );
    m_db->exec(
        sql::query("SELECT DISTINCT entry FROM kanji WHERE kanji "
                   + compare(type)) % query,
        buildEntry, &querydata
    );

    return querydata.entry;
}

std::string JMDict::compare(QueryType type)
{
    if (QueryType::EXACT)
        return "=%Q";
    else if (QueryType::FULLTEXT)
        return " LIKE '%%%q%%'";
    return " LIKE '%q%%'";
}

int buildEntry(void *void_query_data, int, char **value, char **)
{
    struct query_data *query_data = 
        static_cast<struct query_data *>(void_query_data);
    sql::db *db = query_data->db;
    Entry *entry = query_data->entry;
    if (query_data->entryId.empty())
    {
        query_data->entryId = *value;
    }
    else if (query_data->entryId != *value)
    {
        return 0; // This is a new definition, skip over
    }

    db->exec(
        sql::query("SELECT kanji FROM kanji WHERE entry=%s") % *value,
        accumulateKanji, entry
    );
    db->exec(
        sql::query("SELECT kana FROM reading WHERE entry=%s") % *value,
        accumulateKana, entry
    );
    db->exec(
        sql::query("SELECT sense, pos, gloss FROM gloss WHERE entry=%s "
                   "ORDER BY sense") % *value,
        buildDefinition, entry
    );

    return 0;
}

int accumulateKanji(void *void_entry, int, char **value, char **)
{
    Entry *entry = static_cast<Entry *>(void_entry);
    accumulate(entry->m_kanji, entry->m_altkanji, value);
    return 0;
}

int accumulateKana(void *void_entry, int, char **value, char **)
{
    Entry *entry = static_cast<Entry *>(void_entry);
    accumulate(entry->m_kana, entry->m_altkana, value);
    return 0;
}

void accumulate(std::string *base, std::string *extra, char **value) {
    if (base->empty())
    {
        *base = *value;
        return;
    }

    if (!extra->empty())
    {
        *extra += ", ";
    }
    *extra += *value;
}

int buildDefinition(void *void_entry, int, char **value, char **)
{
    Entry *entry = static_cast<Entry *>(void_entry);
    std::vector<std::vector<std::string>> &definitions = *entry->m_descriptions;
    
    uint sense = atoi(value[SENSE_INDEX]);
    if (definitions.size() != sense)
    {
        definitions.push_back(std::vector<std::string>());
        definitions[sense - 1].push_back(std::string(value[POS_INDEX]));
    }
    definitions[sense - 1].push_back(std::string(value[GLOSS_INDEX]));

    return 0;
}