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

#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QVector>

#include "yomidbbuilder.h"

#include "util/utils.h"

/* Begin Constructor/Destructor */

DatabaseManager::DatabaseManager(const QString &path)
    : m_dbpath(path.toUtf8())
{
    if (!sqlite3_threadsafe())
    {
        QMessageBox::critical(
            0, "SQLite Error",
            "The version of SQLite on this system is not threadsafe.\n "
            "Because of this, Memento will not work.\n Please install a "
            "version SQLite compiled with SQLITE_THREADSAFE=1 or 2."
        );
        QApplication::exit(EXIT_FAILURE);
    }

    if (yomi_prepare_db(m_dbpath, NULL) ||
        sqlite3_open_v2(
            m_dbpath,
            &m_db,
            SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX,
            NULL
        ) != SQLITE_OK)
    {
        m_db = nullptr;
        qDebug() << "Could not open dictionary database";
    }

    m_moraSkipChar << "ぁ"
                   << "ぃ"
                   << "ぅ"
                   << "ぇ"
                   << "ぉ"
                   << "ゃ"
                   << "ゅ"
                   << "ょ"
                   << "ァ"
                   << "ィ"
                   << "ゥ"
                   << "ェ"
                   << "ォ"
                   << "ャ"
                   << "ュ"
                   << "ョ";

    initCache();
}

DatabaseManager::~DatabaseManager()
{
    sqlite3_close_v2(m_db);
}

/* End Constructor/Destructor */
/* Begin Initializers */

#define QUERY_DICTIONARY    "SELECT dic_id, title FROM directory;"
#define QUERY_TAGS          "SELECT dic_id, category, name, ord, notes, score " \
                                "FROM tag_bank;"

#define COLUMN_DIC_ID       0
#define COLUMN_CATEGORY     1
#define COLUMN_NAME         2
#define COLUMN_ORDER        3
#define COLUMN_NOTES        4
#define COLUMN_SCORE        5

int DatabaseManager::initCache()
{
    int           ret  = 0;
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
        tag.dictionary = m_dictionaryCache[id],
        tag.name = (const char *)sqlite3_column_text(stmt, COLUMN_NAME),
        tag.category = (const char *)sqlite3_column_text(stmt, COLUMN_CATEGORY),
        tag.notes = (const char *)sqlite3_column_text(stmt, COLUMN_NOTES),
        tag.order = sqlite3_column_int(stmt, COLUMN_ORDER),
        tag.score = sqlite3_column_int(stmt, COLUMN_SCORE),
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

/* End Initializers */
/* Begin Dictionary Database Modifiers */

int DatabaseManager::addDictionary(const QString &path)
{
    m_dbLock.lockForWrite();
    QByteArray cpath = path.toUtf8();
    QByteArray respath = DirectoryUtils::getDictionaryResourceDir().toUtf8();
    int ret = yomi_process_dictionary(cpath, m_dbpath, respath);
    initCache();
    m_dbLock.unlock();
    return ret;
}

int DatabaseManager::deleteDictionary(const QString &name)
{
    m_dbLock.lockForWrite();
    QByteArray cname = name.toUtf8();
    QByteArray respath = DirectoryUtils::getDictionaryResourceDir().toUtf8();
    int ret = yomi_delete_dictionary(cname, m_dbpath, respath);
    initCache();
    m_dbLock.unlock();
    return ret;
}

int DatabaseManager::disableDictionaries(const QStringList &dicts)
{
    QVector<QByteArray> byteDicts;
    for (const QString &dict : dicts)
    {
        byteDicts << dict.toUtf8();
    }
    QVector<const char *> cDicts;
    for (const QByteArray &dict : byteDicts)
    {
        cDicts << dict.data();
    }

    m_dbLock.lockForWrite();
    int ret = yomi_disable_dictionaries(cDicts.data(), cDicts.size(), m_dbpath);
    m_dbLock.unlock();
    return ret;
}

/* End Dictionary Database Modifiers */
/* Begin Database Getters */

#define QUERY   "SELECT title FROM directory;"

QStringList DatabaseManager::getDictionaries() const
{
    m_dbLock.lockForRead();

    QStringList   dictionaries;
    sqlite3_stmt *stmt  = NULL;
    int           step  = 0;

    if (sqlite3_prepare_v2(m_db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        dictionaries.append((const char *)sqlite3_column_text(stmt, 0));
    }

cleanup:
    sqlite3_finalize(stmt);
    m_dbLock.unlock();

    return dictionaries;
}

#undef QUERY

#define QUERY   "SELECT dic_id FROM dict_disabled;"

QStringList DatabaseManager::getDisabledDictionaries() const
{
    m_dbLock.lockForRead();

    QStringList   dictionaries;
    sqlite3_stmt *stmt  = NULL;
    int           step  = 0;

    if (sqlite3_prepare_v2(m_db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int64_t id = sqlite3_column_int64(stmt, 0);
        dictionaries << getDictionary(id);
    }

cleanup:
    sqlite3_finalize(stmt);
    m_dbLock.unlock();

    return dictionaries;
}

#undef QUERY

#define QUERY               "SELECT DISTINCT expression, reading "\
                                "FROM term_bank "\
                                "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND " \
                                    "(expression = ? OR reading = ?);"
#define QUERY_WITH_KATAKANA "SELECT DISTINCT expression, reading "\
                                "FROM term_bank "\
                                "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND " \
                                    "(expression = ? OR reading = ? OR " \
                                     "expression = ? OR reading = ?);"
#define QUERY_WITH_HALFWIDTH "SELECT DISTINCT expression, reading "\
                                "FROM term_bank "\
                                "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND " \
                                    "(expression = ? OR reading = ? OR " \
                                     "expression = ? OR reading = ? OR " \
                                     "expression = ? OR reading = ?);"

#define QUERY_RAW_EXPRESSION_IDX    1
#define QUERY_RAW_READING_IDX       2
#define QUERY_KATA_EXPRESSION_IDX   3
#define QUERY_KATA_READING_IDX      4
#define QUERY_HALF_EXPRESSION_IDX   5
#define QUERY_HALF_READING_IDX      6

#define COLUMN_EXPRESSION       0
#define COLUMN_READING          1

QString DatabaseManager::queryTerms(const QString &query, QList<SharedTerm> &terms) const
{
    if (m_db == nullptr)
    {
        return "Database is invalid";
    }

    /* Try to acquire the database lock, early return if we can't */
    if (!m_dbLock.tryLockForRead())
    {
        return "";
    }

    QString       ret;
    QByteArray    exp          = query.toUtf8();
    QByteArray    katakana     = halfToFull(query).toUtf8();
    bool          containsHalf = katakana != exp;
    QByteArray    hiragana     = kataToHira(katakana).toUtf8();
    bool          containsKata = hiragana != katakana;
    sqlite3_stmt *stmt         = NULL;
    const char   *sql_query    = NULL;
    int           step         = 0;
    QList<SharedTerm> termList;

    if (containsHalf)
    {
        sql_query = QUERY_WITH_HALFWIDTH;
    }
    else if (containsKata)
    {
        sql_query = QUERY_WITH_KATAKANA;
    }
    else
    {
        sql_query = QUERY;
    }

    /* Query for all the different terms in the database */
    if (sqlite3_prepare_v2(m_db, sql_query, -1, &stmt, NULL) != SQLITE_OK)
    {
        ret = "Could not prepare database query";
        goto error;
    }
    if (sqlite3_bind_text(stmt, QUERY_RAW_EXPRESSION_IDX, exp, -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_RAW_READING_IDX,    exp, -1, NULL) != SQLITE_OK)
    {
        ret = "Could not bind values to statement";
        goto error;
    }
    if (containsKata &&
        (
            sqlite3_bind_text(stmt, QUERY_KATA_EXPRESSION_IDX, hiragana, -1, NULL) != SQLITE_OK ||
            sqlite3_bind_text(stmt, QUERY_KATA_READING_IDX,    hiragana, -1, NULL) != SQLITE_OK
        ))
    {
        ret = "Could not bind values to statement";
        goto error;
    }
    if (containsHalf &&
        (
            sqlite3_bind_text(stmt, QUERY_HALF_EXPRESSION_IDX, katakana, -1, NULL) != SQLITE_OK ||
            sqlite3_bind_text(stmt, QUERY_HALF_READING_IDX,    katakana, -1, NULL) != SQLITE_OK
        ))
    {
        ret = "Could not bind values to statement";
        goto error;
    }

    /* Create a term for each entry */
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        SharedTerm term(new Term);
        term->expression = (const char *)sqlite3_column_text(stmt, COLUMN_EXPRESSION);
        term->reading    = (const char *)sqlite3_column_text(stmt, COLUMN_READING);
        if (addFrequencies(*term))
            qDebug() << "Could not add frequencies for" << term->expression;
        if (addPitches(*term))
            qDebug() << "Could not add pitches for" << term->expression;
        termList.append(term);
    }
    if (isStepError(step))
    {
        ret = "Error when executing sqlite query. Code " + QString::number(step);
        goto error;
    }

    /* Add data to each term */
    if (populateTerms(termList))
    {
        ret = "Error getting term information";
        goto error;
    }

    /* Return results on success */
    sqlite3_finalize(stmt);
    m_dbLock.unlock();
    terms.append(termList);

    return ret;

error:
    /* Free up memory on failure */
    m_dbLock.unlock();
    sqlite3_finalize(stmt);

    return ret;
}

#undef QUERY
#undef QUERY_WITH_KATAKANA
#undef QUERY_WITH_HALFWIDTH

#undef QUERY_RAW_EXPRESSION_IDX
#undef QUERY_RAW_READING_IDX
#undef QUERY_KATA_EXPRESSION_IDX
#undef QUERY_KATA_READING_IDX
#undef QUERY_HALF_EXPRESSION_IDX
#undef QUERY_HALF_READING_IDX

#undef COLUMN_EXPRESSION
#undef COLUMN_READING

#define QUERY   "SELECT dic_id, onyomi, kunyomi, tags, meanings, stats FROM kanji_bank "\
                    "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND (char = ?);"

#define COLUMN_DIC_ID       0
#define COLUMN_ONYOMI       1
#define COLUMN_KUNYOMI      2
#define COLUMN_TAGS         3
#define COLUMN_MEANINGS     4
#define COLUMN_STATS        5

#define TAG_NAME_STATS      "misc"
#define TAG_NAME_CLAS       "class"
#define TAG_NAME_CODE       "code"
#define TAG_NAME_INDEX      "index"

QString DatabaseManager::queryKanji(const QString &query, Kanji &kanji) const
{
    if (m_db == nullptr)
    {
        return "Database is invalid";
    }

    /* Try to acquire the database lock, early return if we can't */
    if (!m_dbLock.tryLockForRead())
    {
        return "";
    }

    QString       ret;
    QByteArray    ch   = query.toUtf8();
    sqlite3_stmt *stmt = NULL;
    int           step = 0;

    kanji.character = query;
    addFrequencies(kanji);

    /* Query for the database for the definitions */
    if (sqlite3_prepare_v2(m_db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        ret = "Could not prepare database query";
        goto cleanup;
    }
    if (sqlite3_bind_text(stmt, 1, ch, -1, NULL) != SQLITE_OK)
    {
        ret = "Could not bind values to statement";
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) != SQLITE_DONE)
    {
        uint64_t id = sqlite3_column_int64(stmt, COLUMN_DIC_ID);

        KanjiDefinition def;
        def.dictionary = getDictionary(id),
        def.onyomi = QString(
                (const char *)sqlite3_column_text(stmt, COLUMN_ONYOMI)
            ).split(' '),
        def.kunyomi = QString(
                (const char *)sqlite3_column_text(stmt, COLUMN_KUNYOMI)
            ).split(' '),
        def.glossary = jsonArrayToStringList(
                (const char *)sqlite3_column_text(stmt, COLUMN_MEANINGS)
            );
        addTags(
            id, (const char *)sqlite3_column_text(stmt, COLUMN_TAGS), def.tags
        );

        QVariantMap map = QJsonDocument::fromJson(
                (const char *)sqlite3_column_text(stmt, COLUMN_STATS)
            ).toVariant().toMap();
        for (auto it = map.constKeyValueBegin();
             it != map.constKeyValueEnd();
             ++it)
        {
            Tag tag = m_tagCache[id][it->first];
            QList<QPair<Tag, QString>> *list = nullptr;
            if (tag.category == TAG_NAME_INDEX)
            {
                list = &def.index;
            }
            else if (tag.category == TAG_NAME_STATS)
            {
                list = &def.stats;
            }
            else if (tag.category == TAG_NAME_CLAS)
            {
                list = &def.clas;
            }
            else if (tag.category == TAG_NAME_CODE)
            {
                list = &def.code;
            }
            else
            {
                continue;
            }
            list->append(
                QPair<Tag, QString>(std::move(tag), it->second.toString())
            );
        }

        kanji.definitions.append(def);
    }
    if (isStepError(step))
    {
        ret = "Error while executing kanji query";
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);
    m_dbLock.unlock();

    return ret;
}

#undef QUERY

#undef COLUMN_DIC_ID
#undef COLUMN_ONYOMI
#undef COLUMN_KUNYOMI
#undef COLUMN_TAGS
#undef COLUMN_MEANINGS
#undef COLUMN_STATS

#undef TAG_NAME_STATS
#undef TAG_NAME_CLAS
#undef TAG_NAME_CODE
#undef TAG_NAME_INDEX

/* End Database Getters */
/* Begin Query Helpers */

#define QUERY   "SELECT dic_id, score, def_tags, glossary, rules, term_tags "\
                    "FROM term_bank "\
                    "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND " \
                        "(expression = ? AND reading = ?);"

#define QUERY_EXP_IDX       1
#define QUERY_READING_IDX   2

#define COLUMN_DIC_ID       0
#define COLUMN_SCORE        1
#define COLUMN_DEF_TAGS     2
#define COLUMN_GLOSSARY     3
#define COLUMN_RULES        4
#define COLUMN_TERM_TAGS    5

int DatabaseManager::populateTerms(const QList<SharedTerm> &terms) const
{
    int           ret     = 0;
    sqlite3_stmt *stmt    = NULL;
    int           step    = 0;
    QByteArray    exp;
    QByteArray    reading;

    for (SharedTerm term : terms)
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
            addTags(
                id,
                (const char *)sqlite3_column_text(stmt, COLUMN_TERM_TAGS),
                term->tags
            );

            TermDefinition def;
            def.dictionary = getDictionary(id);
            def.glossary = QJsonDocument::fromJson(
                (const char *)sqlite3_column_text(stmt, COLUMN_GLOSSARY)
            ).array();
            def.score = sqlite3_column_int(stmt, COLUMN_SCORE);
            addTags(
                id,
                (const char *)sqlite3_column_text(stmt, COLUMN_DEF_TAGS),
                def.tags
            );
            QStringList rules = QString(
                (const char *)sqlite3_column_text(stmt, COLUMN_RULES)
            ).split(' ');
            def.rules = {
                std::move_iterator{std::begin(rules)},
                std::move_iterator{std::end(rules)}
            };
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

QString DatabaseManager::getDictionary(const uint64_t id) const
{
    return m_dictionaryCache[id];
}

void DatabaseManager::addTags(const uint64_t  id,
                              const QString  &tagStr,
                              QList<Tag>     &tags) const
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

#define QUERY   "SELECT dic_id, data, type "\
                    "FROM term_meta_bank "\
                    "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND " \
                        "(expression = ? AND mode = 'freq');"

int DatabaseManager::addFrequencies(Term &term) const
{
    return addFrequencies(
        QUERY, term.expression, term.frequencies, term.reading
    );
}

#undef QUERY

#define QUERY   "SELECT dic_id, data, type "\
                    "FROM kanji_meta_bank "\
                    "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND " \
                        "(expression = ? AND mode = 'freq');"

int DatabaseManager::addFrequencies(Kanji &kanji) const
{
    return addFrequencies(QUERY, kanji.character, kanji.frequencies);
}

#undef QUERY

#define OBJ_READING_KEY     "reading"
#define OBJ_FREQ_KEY        "frequency"
#define OBJ_VALUE_KEY       "value"
#define OBJ_DISPLAY_KEY     "displayValue"

int DatabaseManager::addFrequencies(
    const char *query,
    const QString &expression,
    QList<Frequency> &freq,
    const QString &reading) const
{
    int           ret  = 0;
    sqlite3_stmt *stmt = NULL;
    int           step = 0;
    QByteArray    exp  = expression.toUtf8();

    if (sqlite3_prepare_v2(m_db, query, -1, &stmt, NULL) != SQLITE_OK)
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
        QString freqStr;
        switch ((yomi_blob_t)sqlite3_column_int(stmt, 2))
        {
        case YOMI_BLOB_TYPE_STRING:
            freqStr = (const char *)sqlite3_column_blob(stmt, 1);
            break;

        case YOMI_BLOB_TYPE_INT:
            freqStr = QString::number(
                *(const uint64_t *)sqlite3_column_blob(stmt, 1)
            );
            break;
        case YOMI_BLOB_TYPE_OBJECT:
        {
            QJsonObject obj = QJsonDocument::fromJson(
                (const char *)sqlite3_column_blob(stmt, 1)
            ).object();

            /* Check if this frequency is dependant on reading */
            if (obj[OBJ_READING_KEY].isString())
            {
                if (obj[OBJ_READING_KEY].toString() != reading)
                {
                    continue;
                }
            }

            /* First scenario:
             * [
             *     "<term>","freq",{"reading":"<reading>","frequency":<number>}
             * ]
             */
            if (obj[OBJ_FREQ_KEY].isDouble())
            {
                freqStr = QString::number(obj[OBJ_FREQ_KEY].toInt());
            }

            /* Second scenario:
             * [
             *     "<term>","freq",{"reading":"<reading>","frequency": "<frequency string>">}
             * ]
             */
            else if (obj[OBJ_FREQ_KEY].isString())
            {
                freqStr = obj[OBJ_FREQ_KEY].toString();
            }

            /* Third scenario:
             * [
             *     "<term>","freq",
             *     {"reading":"<reading>",
             *        "frequency": {"value": <number>, "displayValue": "<stylized frequency string>"}
             *     }
             * ]
             */
            else if (obj[OBJ_FREQ_KEY].isObject())
            {
                obj = obj[OBJ_FREQ_KEY].toObject();
                /* Check for the type that should be shown */
                if (obj[OBJ_DISPLAY_KEY].isString())
                {
                    freqStr = obj[OBJ_DISPLAY_KEY].toString();
                }
                else if (obj[OBJ_VALUE_KEY].isDouble())
                {
                    freqStr = QString::number(obj[OBJ_VALUE_KEY].toInt());
                }
                else
                {
                    continue;
                }
            }
            break;
        }
        default:
            continue;
        }
        freq.append(Frequency {
            getDictionary(sqlite3_column_int64(stmt, 0)),
            freqStr
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

#undef OBJ_READING_KEY
#undef OBJ_FREQ_KEY
#undef OBJ_VALUE_KEY
#undef OBJ_DISPLAY_KEY

#define QUERY   "SELECT dic_id, data "\
                    "FROM term_meta_bank "\
                    "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND " \
                        "(expression = ? AND mode = 'pitch');"

#define OBJ_READING_KEY     "reading"
#define OBJ_PITCHES_KEY     "pitches"
#define OBJ_POSITION_KEY    "position"

int DatabaseManager::addPitches(Term &term) const
{
    int           ret  = 0;
    sqlite3_stmt *stmt = NULL;
    int           step = 0;
    QByteArray    exp  = term.expression.toUtf8();

    if (sqlite3_prepare_v2(m_db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        qDebug() << "Could not prepare pitch query";
        ret = -1;
        goto cleanup;
    }
    if (sqlite3_bind_text(stmt, 1, exp, -1, NULL) != SQLITE_OK)
    {
        qDebug() << "Error binding expression to pitch query";
        ret = -1;
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        QJsonObject obj = QJsonDocument::fromJson(
                (const char *)sqlite3_column_blob(stmt, 1)
            ).object();
        const QString reading = obj[OBJ_READING_KEY].toString();
        if (reading != term.reading && reading != term.expression)
        {
            continue;
        }

        Pitch pitch;
        pitch.dictionary = getDictionary(sqlite3_column_int64(stmt, 0));

        /* Add mora */
        QString currentMora;
        for (const QChar &ch : reading)
        {
            if (!currentMora.isEmpty() && !m_moraSkipChar.contains(ch))
            {
                pitch.mora.append(currentMora);
                currentMora.clear();
            }
            currentMora += ch;
        }
        if (!currentMora.isEmpty())
        {
            pitch.mora.append(currentMora);
        }

        /* Add pitch positions */
        QJsonArray arr = obj[OBJ_PITCHES_KEY].toArray();
        for (const QJsonValue &val : arr)
        {
            pitch.position.append(val.toObject()[OBJ_POSITION_KEY].toInt());
        }

        term.pitches.append(pitch);
    }
    if (isStepError(step))
    {
        qDebug() << "Error executing sqlite pitch query";
        ret = -1;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

#undef QUERY

#undef OBJ_READING_KEY
#undef OBJ_PITCHES_KEY
#undef OBJ_POSITION_KEY

/* End Query Helpers */
/* Begin Helpers */

QString DatabaseManager::errorCodeToString(const int code) const
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
    case YOMI_ERR_EXTRACTING_RESOURCES:
        return "Could not extract dictionary resources";
    case YOMI_ERR_REMOVING_RESOURCES:
        return "Could not remove dictionary resources";
    default:
        return "Unknown error";
    }
}

static const QChar HALFWIDTH_LOW(0xFF61);
static const QChar HALFWIDTH_HIGH(0xFF9F);
static const QChar HALFWIDTH_VOICED(0xFF9E);
static const QChar HALFWIDTH_SEMI_VOICED(0xFF9F);

QString DatabaseManager::halfToFull(const QString &query) const
{
    /* While initializing a new map every time this conversion is done seems and
     * is stupid, QMap is not thread safe. An older version of Memento used to
     * disregarded this and it led to a horrible bug that caused CPU usage to
     * spike to 100% until Memento was closed.
     *
     * While it might seem obvious that half-width should be easily mappable to
     * full-width with offsets, this is not the case. Unicode in their infinite
     * wisdom decided the ordering for half-width characters should be different
     * from their full width counter parts and that half width characters should
     * use two characters to represent something like ガ using two characters
     * (ｶﾞ).
     */
    QMap<QString, QString> charMap{
        {"｡", "。"},
        {"｢", "「"},
        {"｣", "」"},
        {"､", "、"},
        {"･", "・"},
        {"ｦ", "ヲ"},
        {"ｧ", "ァ"},
        {"ｨ", "ィ"},
        {"ｩ", "ゥ"},
        {"ｪ", "ェ"},
        {"ｫ", "ォ"},
        {"ｬ", "ャ"},
        {"ｭ", "ュ"},
        {"ｮ", "ョ"},
        {"ｯ", "ッ"},
        {"ｰ", "ー"},
        {"ｱ", "ア"},
        {"ｲ", "イ"},
        {"ｳ", "ウ"},
        {"ｴ", "エ"},
        {"ｵ", "オ"},
        {"ｶ", "か"},
        {"ｶﾞ", "が"},
        {"ｷ", "キ"},
        {"ｷﾞ", "ギ"},
        {"ｸ", "ク"},
        {"ｸﾞ", "グ"},
        {"ｹ", "ケ"},
        {"ｹﾞ", "ゲ"},
        {"ｺ", "コ"},
        {"ｺﾞ", "ゴ"},
        {"ｻ", "サ"},
        {"ｻﾞ", "ザ"},
        {"ｼ", "シ"},
        {"ｼﾞ", "ジ"},
        {"ｽ", "ス"},
        {"ｽﾞ", "ズ"},
        {"ｾ", "セ"},
        {"ｾﾞ", "ゼ"},
        {"ｿ", "ソ"},
        {"ｿﾞ", "ゾ"},
        {"ﾀ", "タ"},
        {"ﾀﾞ", "ダ"},
        {"ﾁ", "チ"},
        {"ﾁﾞ", "ヂ"},
        {"ﾂ", "ツ"},
        {"ﾂﾞ", "ヅ"},
        {"ﾃ", "テ"},
        {"ﾃﾞ", "デ"},
        {"ﾄ", "ト"},
        {"ﾄﾞ", "ド"},
        {"ﾅ", "ナ"},
        {"ﾆ", "ニ"},
        {"ﾇ", "ヌ"},
        {"ﾈ", "ネ"},
        {"ﾉ", "ノ"},
        {"ﾊ", "ハ"},
        {"ﾊﾞ", "バ"},
        {"ﾊﾟ", "パ"},
        {"ﾋ", "ヒ"},
        {"ﾋﾞ", "ビ"},
        {"ﾋﾟ", "ピ"},
        {"ﾌ", "フ"},
        {"ﾌﾞ", "ブ"},
        {"ﾌﾟ", "プ"},
        {"ﾍ", "ヘ"},
        {"ﾍﾞ", "ベ"},
        {"ﾍﾟ", "ペ"},
        {"ﾎ", "ホ"},
        {"ﾎﾞ", "ボ"},
        {"ﾎﾟ", "ポ"},
        {"ﾏ", "マ"},
        {"ﾐ", "ミ"},
        {"ﾑ", "ム"},
        {"ﾒ", "メ"},
        {"ﾓ", "モ"},
        {"ﾔ", "ヤ"},
        {"ﾕ", "ユ"},
        {"ﾖ", "ヨ"},
        {"ﾗ", "ラ"},
        {"ﾘ", "リ"},
        {"ﾙ", "ル"},
        {"ﾚ", "レ"},
        {"ﾛ", "ロ"},
        {"ﾜ", "ワ"},
        {"ﾝ", "ン"},
    };

    QString res;
    int i;
    for (i = 0; i < query.size() - 1; ++i)
    {
        if (HALFWIDTH_LOW <= query[i] && query[i] <= HALFWIDTH_HIGH)
        {
            if ((query[i + 1] == HALFWIDTH_VOICED ||
                 query[i + 1] == HALFWIDTH_SEMI_VOICED) &&
                charMap.contains(query.mid(i, 2)))
            {
                res += charMap[query.mid(i, 2)];
                ++i;
            }
            else
            {
                res += charMap[query[i]];
            }
        }
        else
        {
            res += query[i];
        }
    }
    if (i < query.size())
    {
        if (HALFWIDTH_LOW <= query[i] && query[i] <= HALFWIDTH_HIGH &&
            charMap.contains(query[i]))
        {
            res += charMap[query[i]];
        }
        else
        {
            res += query[i];
        }
    }
    return res;
}

static const QChar KATAKANA_LOW(0x30A1);
static const QChar KATAKANA_HIGH(0x30F6);
static const QChar HIRAGANA_LOW(0x3041);
[[maybe_unused]] static const QChar HIRAGANA_HIGH(0x3096);

QString DatabaseManager::kataToHira(QString query) const
{
    for (QChar &ch : query)
    {
        ushort code = ch.unicode();
        if (code >= KATAKANA_LOW && code <= KATAKANA_HIGH)
        {
            ch = QChar(HIRAGANA_LOW.unicode() + (code - KATAKANA_LOW.unicode()));
        }
    }
    return query;
}

QStringList DatabaseManager::jsonArrayToStringList(const char *jsonstr) const
{
    QJsonDocument document = QJsonDocument::fromJson(jsonstr);
    QJsonArray array = document.array();
    QStringList list;

    for (const QJsonValue &val : array)
    {
        if (val.isString())
        {
            list.append(val.toString());
        }
    }

    return list;
}

bool inline DatabaseManager::isStepError(const int step)
{
    return step != SQLITE_ROW && step != SQLITE_DONE;
}

/* End Helpers */
