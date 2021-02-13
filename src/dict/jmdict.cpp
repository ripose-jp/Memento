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
static void accumulate(QString *base, QString *extra, char **value);
static int buildDefinition(void *void_entry, int, char **value, char **);

struct query_data
{
    sql::db *db;
    Entry *current_entry;
    QList<Entry *> *entires;
    QString entryId;
};

JMDict::JMDict(const QString &path) : m_db(new sql::db(path.toStdString())),
                                      m_path(path),
                                      m_readerWriter(QSemaphore(1))
{
    m_kataToHira["ア"] = "あ";
    m_kataToHira["イ"] = "い";
    m_kataToHira["ウ"] = "う";
    m_kataToHira["エ"] = "え";
    m_kataToHira["オ"] = "お";    
    m_kataToHira["カ"] = "か";
    m_kataToHira["キ"] = "き";
    m_kataToHira["ク"] = "く";
    m_kataToHira["ケ"] = "け";
    m_kataToHira["コ"] = "こ";
    m_kataToHira["サ"] = "さ";
    m_kataToHira["シ"] = "し";
    m_kataToHira["ス"] = "す";
    m_kataToHira["セ"] = "せ";
    m_kataToHira["ソ"] = "そ";
    m_kataToHira["タ"] = "た";
    m_kataToHira["チ"] = "ち";
    m_kataToHira["ツ"] = "つ";
    m_kataToHira["テ"] = "て";
    m_kataToHira["ト"] = "と";
    m_kataToHira["ナ"] = "な";
    m_kataToHira["ニ"] = "に";
    m_kataToHira["ヌ"] = "ぬ";
    m_kataToHira["ネ"] = "ね";
    m_kataToHira["ノ"] = "の";
    m_kataToHira["ハ"] = "は";
    m_kataToHira["ヒ"] = "ひ";
    m_kataToHira["フ"] = "ふ";
    m_kataToHira["ヘ"] = "へ";
    m_kataToHira["ホ"] = "ほ";
    m_kataToHira["マ"] = "ま";
    m_kataToHira["ミ"] = "み";
    m_kataToHira["ム"] = "む";
    m_kataToHira["メ"] = "め";
    m_kataToHira["モ"] = "も";
    m_kataToHira["ヤ"] = "や";
    m_kataToHira["ユ"] = "ゆ";
    m_kataToHira["ヨ"] = "よ";
    m_kataToHira["ラ"] = "ら";
    m_kataToHira["リ"] = "り";
    m_kataToHira["ル"] = "る";
    m_kataToHira["レ"] = "れ";
    m_kataToHira["ロ"] = "ろ";
    m_kataToHira["ワ"] = "わ";
    m_kataToHira["ヰ"] = "ゐ";
    m_kataToHira["ヱ"] = "ゑ";
    m_kataToHira["ヲ"] = "を";
    m_kataToHira["ン"] = "ん";
    
    m_kataToHira["ァ"] = "ぁ";
    m_kataToHira["ィ"] = "ぃ";
    m_kataToHira["ゥ"] = "ぅ";
    m_kataToHira["ェ"] = "ぇ";
    m_kataToHira["ォ"] = "ぉ";
    m_kataToHira["ヮ"] = "ゎ";
    m_kataToHira["ャ"] = "ゃ";
    m_kataToHira["ュ"] = "ゅ";
    m_kataToHira["ョ"] = "ょ";
    m_kataToHira["ッ"] = "っ";

    m_kataToHira["ヴ"] = "ゔ";
    m_kataToHira["ガ"] = "が";
    m_kataToHira["ギ"] = "ぎ";
    m_kataToHira["グ"] = "ぐ";
    m_kataToHira["ゲ"] = "げ";
    m_kataToHira["ゴ"] = "ご";
    m_kataToHira["ザ"] = "ざ";
    m_kataToHira["ジ"] = "じ";
    m_kataToHira["ズ"] = "ず";
    m_kataToHira["ゼ"] = "ぜ";
    m_kataToHira["ゾ"] = "ぞ";
    m_kataToHira["ダ"] = "だ";
    m_kataToHira["ヂ"] = "ぢ";
    m_kataToHira["ヅ"] = "づ";
    m_kataToHira["デ"] = "で";
    m_kataToHira["ド"] = "ど";
    m_kataToHira["バ"] = "ば";
    m_kataToHira["ビ"] = "び";
    m_kataToHira["ブ"] = "ぶ";
    m_kataToHira["ベ"] = "べ";
    m_kataToHira["ボ"] = "ぼ";
    m_kataToHira["パ"] = "ぱ";
    m_kataToHira["ピ"] = "ぴ";
    m_kataToHira["プ"] = "ぷ";
    m_kataToHira["ペ"] = "ぺ";
    m_kataToHira["ポ"] = "ぽ";
}

JMDict::~JMDict()
{
    delete m_db;
}

void JMDict::reopenDictionary()
{
    m_readerWriter.acquire();
    delete m_db;
    m_db = new sql::db(m_path.toStdString());
    m_readerWriter.release();
}

QList<Entry *> *JMDict::query(const QString &query, const QueryType type)
{
    QString cleanedQuery = katakanaToHiragana(query);
    struct query_data querydata;
    querydata.db = m_db;
    querydata.current_entry = 0;
    querydata.entires = new QList<Entry *>;

    m_numReadersMutex.lock();
    ++m_numReaders;
    if (m_numReaders == 1)
        m_readerWriter.acquire();
    m_numReadersMutex.unlock();
    
    m_db->exec(
        sql::query("SELECT DISTINCT entry FROM reading WHERE kana " 
                   + compare(type) + " OR kana " + compare(type))
                   % query.toStdString() % cleanedQuery.toStdString(),
        buildEntry, &querydata
    );
    m_db->exec(
        sql::query("SELECT DISTINCT entry FROM kanji WHERE kanji "
                   + compare(type) + " OR kanji " + compare(type))
                   % query.toStdString() % cleanedQuery.toStdString(),
        buildEntry, &querydata
    );

    m_numReadersMutex.lock();
    --m_numReaders;
    if (m_numReaders == 0)
        m_readerWriter.release();
    m_numReadersMutex.unlock();

    return querydata.entires;
}

QString JMDict::katakanaToHiragana(const QString &str)
{
    QString res;
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        res += m_kataToHira.contains(*it) ? m_kataToHira.value(*it) : *it;
    }
    return res;
}

std::string JMDict::compare(QueryType type)
{
    if (QueryType::EXACT)
        return "=%Q";
    else if (QueryType::FULLTEXT)
        return " LIKE '%%%q%%'";
    else if (QueryType::BEGINNING)
        return " LIKE '%q%%'";
}

int buildEntry(void *void_query_data, int, char **value, char **)
{
    struct query_data *query_data = 
        static_cast<struct query_data *>(void_query_data);

    if (query_data->entryId.isEmpty() || query_data->entryId != *value)
    {
        query_data->current_entry = new Entry;
        query_data->entryId = *value;
    }
    
    sql::db *db = query_data->db;
    Entry *entry = query_data->current_entry;

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

    query_data->entires->push_back(query_data->current_entry);

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

void accumulate(QString *base, QString *extra, char **value) {
    if (base->isEmpty())
    {
        *base = *value;
        return;
    }

    if (!extra->isEmpty())
    {
        *extra += ", ";
    }
    *extra += *value;
}

int buildDefinition(void *void_entry, int, char **value, char **)
{
    Entry *entry = static_cast<Entry *>(void_entry);
    QList<QList<QString>> &definitions = *entry->m_descriptions;
    
    uint sense = atoi(value[SENSE_INDEX]);
    if (definitions.size() != sense)
    {
        definitions.push_back(QList<QString>());
        definitions[sense - 1].push_back(QString(value[POS_INDEX]));
    }
    definitions[sense - 1].push_back(QString(value[GLOSS_INDEX]));

    return 0;
}