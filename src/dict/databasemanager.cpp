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

#include "databasemanager.h"

extern "C"
{
#include "yomidbbuilder.h"
}
#include <QJsonDocument>
#include <QJsonArray>

DatabaseManager::DatabaseManager(const QString &path) : m_dbpath(path), m_readerCount(0)
{
    if (yomi_prepare_db(m_dbpath.toLocal8Bit(), NULL) ||
        sqlite3_open_v2(m_dbpath.toLocal8Bit(), &m_db, SQLITE_OPEN_READONLY, NULL) != SQLITE_OK)
    {
        m_db = nullptr;
        qDebug() << "Could not open dictionary database";
    }

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

    buildCache();
}

DatabaseManager::~DatabaseManager()
{
    sqlite3_close_v2(m_db);
}

int DatabaseManager::addDictionary(const QString &path)
{
    m_databaseLock.lock();
    int ret = yomi_process_dictionary(path.toLocal8Bit(), m_dbpath.toLocal8Bit());
    buildCache();
    m_databaseLock.unlock();
    return ret;
}

int DatabaseManager::deleteDictionary(const QString &name)
{
    m_databaseLock.lock();
    int ret = yomi_delete_dictionary(name.toUtf8(), m_dbpath.toLocal8Bit());
    buildCache();
    m_databaseLock.unlock();
    return ret;
}

QString DatabaseManager::errorCodeToString(const int code)
{
    switch (code)
    {
    case YOMI_ERR_ADDING_INDEX:
        return "Could not add index.json";
    case YOMI_ERR_ADDING_KANJI:
        return "Could not add kanji terms";
    case YOMI_ERR_ADDING_KANJI_META:
        return "Could not add kanji metadata";
    case YOMI_ERR_ADDING_TAGS:
        return "Could not add tags";
    case YOMI_ERR_ADDING_TERMS:
        return "Could not add terms";
    case YOMI_ERR_ADDING_TERMS_META:
        return "Could not add term metadata";
    case YOMI_ERR_DB:
        return "Could not open database";
    case YOMI_ERR_NEWER_VERSION:
        return "Database is of a newer version";
    case YOMI_ERR_OPENING_DIC:
        return "Could not open dictionary file";
    case YOMI_ERR_DELETE:
        return "Could not execute delete query on database";
    default:
        return "Unknown error";
    }
}

QStringList DatabaseManager::getDictionaries()
{
    incrementReaders();

    QStringList   dictionaries;
    sqlite3_stmt *stmt = NULL;
    int           step = 0;

    if (sqlite3_prepare_v2(m_db, "SELECT title FROM directory;", -1, &stmt, NULL) != SQLITE_OK)
        goto cleanup;
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        dictionaries.append((const char *)sqlite3_column_text(stmt, 0));
    }

cleanup:
    sqlite3_finalize(stmt);
    decrementReaders();

    return dictionaries;
}

#define QUERY               "SELECT expression, reading "\
                                "FROM term_bank "\
                                "WHERE (expression = ? OR reading = ?) "\
                                "GROUP BY expression, reading;"
#define QUERY_WITH_KATAKANA "SELECT expression, reading "\
                                "FROM term_bank "\
                                "WHERE (expression = ? OR reading = ? OR reading = ?) "\
                                "GROUP BY expression, reading;"

#define QUERY_EXP_IDX           1
#define QUERY_READING_HIRA_IDX  2
#define QUERY_READING_KATA_IDX  3

#define COLUMN_EXPRESSION       0
#define COLUMN_READING          1

QString DatabaseManager::queryTerms(const QString &query, QList<Term *> &terms)
{
    if (m_db == nullptr)
        return "Database is invalid";

    /* Try to acquire the database lock, early return if we can't */
    if (!incrementReaders())
        return "";
    
    QString       ret;
    QByteArray    exp          = query.toUtf8();
    QByteArray    hiragana     = kataToHira(query).toUtf8();
    bool          containsKata = hiragana != exp;
    sqlite3_stmt *stmt         = NULL;
    int           step         = 0;
    QList<Term *> termList;

    /* Query for all the different terms in the database */
    if (sqlite3_prepare_v2(m_db,
                           containsKata ? QUERY_WITH_KATAKANA : QUERY,
                           -1, &stmt, NULL) != SQLITE_OK)
    {
        ret = "Could not prepare database query";
        goto error;
    }
    if (sqlite3_bind_text(stmt, QUERY_EXP_IDX,          exp,      -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_READING_HIRA_IDX, hiragana, -1, NULL) != SQLITE_OK)
    {
        ret = "Could not bind values to statement";
        goto error;
    }
    if (containsKata && 
        sqlite3_bind_text(stmt, QUERY_READING_KATA_IDX, exp, -1, NULL) != SQLITE_OK)
    {
        ret = "Could not bind values to statement";
        goto error;
    }

    /* Create a term for each entry */
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        Term *term       = new Term;
        term->expression = (const char *)sqlite3_column_text(stmt, COLUMN_EXPRESSION);
        term->reading    = (const char *)sqlite3_column_text(stmt, COLUMN_READING);
        if (addFrequencies(term))
            qDebug() << "Could not add frequencies for" << term->expression;
        termList.append(term);
    }
    if (isStepError(step))
    {
        ret = "Error when executing sqlite query. Code " + step;
        goto error;
    }

    /* Add data to each term */
    if (populateTerms(termList))
    {
        ret = "Error getting term information";
        goto error;
    }

    /* Return results on success */
    decrementReaders();
    sqlite3_finalize(stmt);
    terms.append(termList);

    return ret;

error:
    /* Free up memory on failure */
    decrementReaders();
    sqlite3_finalize(stmt);
    for (Term *term : termList)
        delete term;

    return ret;
}

#undef QUERY
#undef QUERY_WITH_KATAKANA

#undef QUERY_EXP_IDX
#undef QUERY_READING_HIRA_IDX
#undef QUERY_READING_KATA_IDX

#undef COLUMN_EXPRESSION
#undef COLUMN_READING

#define QUERY   "SELECT dic_id, data "\
                    "FROM term_meta_bank "\
                    "WHERE (expression = ? AND mode = 'freq');"

int DatabaseManager::addFrequencies(Term *term)
{
    int           ret  = 0;
    sqlite3_stmt *stmt = NULL;
    int           step = 0;
    QByteArray    exp  = term->expression.toUtf8();

    if (sqlite3_prepare_v2(m_db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        qDebug() << "Could not prepare frequency query";
        ret = -1;
        goto cleanup;
    }
    if (sqlite3_bind_text(stmt, 1, exp, -1, NULL) != SQLITE_OK)
    {
        qDebug() << "Error binding expression to frequency query";
        ret = -1;
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        term->frequencies.append(TermFrequency {
            getDictionary(sqlite3_column_int64(stmt, 0)),
            *(const uint64_t *)sqlite3_column_blob(stmt, 1)
        });
    }
    if (isStepError(step))
    {
        qDebug() << "Error executing sqlite frequency query";
        ret = -1;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

#undef QUERY

#define QUERY   "SELECT dic_id, score, def_tags, glossary, rules, term_tags "\
                    "FROM term_bank "\
                    "WHERE (expression = ? AND reading = ?);"

#define QUERY_EXP_IDX       1
#define QUERY_READING_IDX   2

#define COLUMN_DIC_ID       0
#define COLUMN_SCORE        1
#define COLUMN_DEF_TAGS     2
#define COLUMN_GLOSSARY     3
#define COLUMN_RULES        4
#define COLUMN_TERM_TAGS    5

int DatabaseManager::populateTerms(const QList<Term *> &terms)
{
    int           ret     = 0;
    sqlite3_stmt *stmt    = NULL;
    int           step    = 0;
    QByteArray    exp;
    QByteArray    reading;

    for (Term *term : terms)
    {
        exp     = term->expression.toUtf8();
        reading = term->reading.toUtf8();

        if (sqlite3_prepare_v2(m_db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
        {
            ret = -1;
            goto cleanup;
        }
        if (sqlite3_bind_text(stmt, QUERY_EXP_IDX,     exp,     -1, NULL) != SQLITE_OK ||
            sqlite3_bind_text(stmt, QUERY_READING_IDX, reading, -1, NULL) != SQLITE_OK)
        {
            ret = -1;
            goto cleanup;
        }

        while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            const uint64_t id = sqlite3_column_int64(stmt, COLUMN_DIC_ID);

            term->score += sqlite3_column_int(stmt, COLUMN_SCORE);

            addTags(id, (const char *)sqlite3_column_text(stmt, COLUMN_TERM_TAGS), term->tags);

            Definition def;
            def.dictionary = getDictionary(id);
            addTags(id, (const char *)sqlite3_column_text(stmt, COLUMN_DEF_TAGS), def.tags);
            addTags(id, (const char *)sqlite3_column_text(stmt, COLUMN_RULES), def.rules);
            def.glossary   = jsonArrayToStringList((const char *)sqlite3_column_text(stmt, COLUMN_GLOSSARY));
            term->definitions.append(def);
        }
        if (isStepError(step))
        {
            ret = -1;
            goto cleanup;
        }

        sqlite3_finalize(stmt);
        stmt = NULL;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

#undef QUERY

#undef QUERY_EXP_IDX
#undef QUERY_READING_IDX

#undef COLUMN_DIC_ID
#undef COLUMN_SCORE
#undef COLUMN_DEF_TAGS
#undef COLUMN_GLOSSARY
#undef COLUMN_RULES
#undef COLUMN_TERM_TAGS

#define QUERY_DICTIONARY    "SELECT dic_id, title FROM directory;"
#define QUERY_TAGS          "SELECT dic_id, category, name, ord, notes, score FROM tag_bank;"

#define COLUMN_DIC_ID       0
#define COLUMN_CATEGORY     1
#define COLUMN_NAME         2
#define COLUMN_ORDER        3
#define COLUMN_NOTES        4
#define COLUMN_SCORE        5

int DatabaseManager::buildCache()
{
    int           ret;
    sqlite3_stmt *stmt = NULL;
    int           step = 0;

    /* Empty the cache */
    m_tagCache.clear();
    m_dictionaryCache.clear();

    /* Build dictionary cache */
    if (sqlite3_prepare_v2(m_db, QUERY_DICTIONARY, -1, &stmt, NULL) != SQLITE_OK)
    {
        ret = -1;
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        uint64_t id   = sqlite3_column_int64(stmt, 0);
        QString title = (const char *)sqlite3_column_text(stmt, 1);
        m_dictionaryCache.insert(id, title);
    }
    if (isStepError(step))
    {
        ret = -1;
        goto cleanup;
    }
    sqlite3_finalize(stmt);
    stmt = NULL;

    /* Build tag cache */
    if (sqlite3_prepare_v2(m_db, QUERY_TAGS, -1, &stmt, NULL) != SQLITE_OK)
    {
        ret = -1;
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        uint64_t id = sqlite3_column_int64(stmt, COLUMN_DIC_ID);
        QHash<QString, Tag> &tags = m_tagCache[id];

        Tag tag;
        tag.name     = (const char *)sqlite3_column_text(stmt, COLUMN_NAME);
        tag.category = (const char *)sqlite3_column_text(stmt, COLUMN_CATEGORY);
        tag.notes    = (const char *)sqlite3_column_text(stmt, COLUMN_NOTES);
        tag.order    = sqlite3_column_int(stmt, COLUMN_ORDER);
        tag.score    = sqlite3_column_int(stmt, COLUMN_SCORE);
        tags.insert(tag.name, tag);

        m_tagCache[id] = tags;
    }
    if (isStepError(step))
    {
        ret = -1;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

#undef QUERY_DICTIONARY
#undef QUERY_TAGS

#undef COLUMN_DIC_ID
#undef COLUMN_CATEGORY
#undef COLUMN_NAME
#undef COLUMN_ORDER
#undef COLUMN_NOTES
#undef COLUMN_SCORE

QString DatabaseManager::getDictionary(const uint64_t id)
{
    return m_dictionaryCache[id];
}

void DatabaseManager::addTags(const uint64_t id, const QString &tagStr, QList<Tag> &tags)
{
    QStringList tagList = tagStr.split(" ");

    for (const QString &tagName : tagList)
    {
        const Tag &tag = m_tagCache[id][tagName];
        if (!tag.name.isEmpty() && !tags.contains(tag))
        {
            tags.append(tag);
        }
    }
}

bool DatabaseManager::incrementReaders()
{
    bool ret = true;

    m_readerLock.lock();
    if (m_readerCount == 0)
    {
        if (!(ret = m_databaseLock.tryLock()))
            goto cleanup;
    }
    ++m_readerCount;
cleanup:
    m_readerLock.unlock();

    return ret;
}

void DatabaseManager::decrementReaders()
{
    m_readerLock.lock();
    --m_readerCount;
    if (m_readerCount == 0)
        m_databaseLock.unlock();
    m_readerLock.unlock();
}

QString DatabaseManager::kataToHira(const QString &query)
{
    QString res;
    for (auto it = query.begin(); it != query.end(); ++it)
    {
        QString ch = m_kataToHira[*it];
        res += ch.isEmpty() ? *it : ch;
    }
    return res;
}

QStringList DatabaseManager::jsonArrayToStringList(const char *jsonstr)
{
    QJsonDocument document = QJsonDocument::fromJson(jsonstr);
    QJsonArray    array    = document.array();
    QStringList   list;

    for (auto it = array.constBegin(); it != array.constEnd(); ++it)
    {
        if (it->isString())
            list.append(it->toString());
    }

    return list;
}
