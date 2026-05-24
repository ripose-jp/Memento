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

#include "dict/databasemanager.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>

#include "dict/yomidbbuilder.h"
#include "util/utils.h"

/* Begin Constructor/Destructor */

DatabaseManager::DatabaseManager(
    const QString &dbPath,
    const QString &resourcePath,
    QObject *parent) :
    QObject(parent),
    m_dbPath(dbPath.toUtf8()),
    m_resourcePath(resourcePath)
{
    if (!sqlite3_threadsafe())
    {
        qCritical(
            "The version of SQLite on this system is not threadsafe.\n "
            "Because of this, Memento will not work.\n Please install a "
            "version SQLite compiled with SQLITE_THREADSAFE=1 or 2."
        );
        return;
    }

    if (yomi_prepare_db(m_dbPath, nullptr) ||
        sqlite3_open_v2(
            m_dbPath,
            &m_db,
            SQLITE_OPEN_READONLY | SQLITE_OPEN_FULLMUTEX,
            nullptr
        ) != SQLITE_OK)
    {
        m_db = nullptr;
        qCritical("Could not open dictionary database");
        return;
    }

    m_moraSkipChar = {
        "ぁ",
        "ぃ",
        "ぅ",
        "ぇ",
        "ぉ",
        "ゃ",
        "ゅ",
        "ょ",
        "ァ",
        "ィ",
        "ゥ",
        "ェ",
        "ォ",
        "ャ",
        "ュ",
        "ョ",
    };

    initCache();
}

DatabaseManager::~DatabaseManager()
{
    QWriteLocker lock{&m_dbLock};
    clearTagCache();
    clearDictionaryCache();
    sqlite3_close_v2(m_db);
}

/* End Constructor/Destructor */
/* Begin Initializers */

void DatabaseManager::loadDictionaryAssets(DictionaryInfo *info) const
{
    constexpr const char *STYLE_FILENAME = "styles.css";

    QString stylePath = m_resourcePath;
    stylePath += QDir::separator();
    stylePath += info->name();
    stylePath += QDir::separator();
    stylePath += STYLE_FILENAME;

    if (!QFile::exists(stylePath))
    {
        return;
    }
    QFile styleFile(stylePath);
    if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    info->setStyles(styleFile.readAll());
    styleFile.close();
}

int DatabaseManager::initCache()
{
    constexpr const char *QUERY_DICTIONARY =
        "SELECT dic_id, title, "
            "EXISTS ("
                "SELECT 1 "
                "FROM dict_disabled "
                "WHERE dict_disabled.dic_id == directory.dic_id"
            ") AS disabled "
        "FROM directory;";

    constexpr int COLUMN_DICTIONARY_DIC_ID = 0;
    constexpr int COLUMN_DICTIONARY_TITLE = 1;
    constexpr int COLUMN_DICTIONARY_DISABLED = 2;

    constexpr const char *QUERY_TAGS =
        "SELECT dic_id, category, name, ord, notes, score "
            "FROM tag_bank;";

    constexpr int COLUMN_TAG_DIC_ID = 0;
    constexpr int COLUMN_TAG_CATEGORY = 1;
    constexpr int COLUMN_TAG_NAME = 2;
    constexpr int COLUMN_TAG_ORDER = 3;
    constexpr int COLUMN_TAG_NOTES = 4;
    constexpr int COLUMN_TAG_SCORE = 5;

    int ret = 0;
    sqlite3_stmt *stmt = nullptr;
    int step = 0;

    /* Empty the cache */
    clearTagCache();
    clearDictionaryCache();

    /* Build dictionary cache */
    if (sqlite3_prepare_v2(m_db, QUERY_DICTIONARY, -1, &stmt, nullptr) != SQLITE_OK)
    {
        ret = -1;
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        DictionaryInfo *info = new DictionaryInfo;
        info->setId(sqlite3_column_int64(stmt, COLUMN_DICTIONARY_DIC_ID));
        info->setName(reinterpret_cast<const char *>(
            sqlite3_column_text(stmt, COLUMN_DICTIONARY_TITLE)
        ));
        info->setEnabled(
            !sqlite3_column_int64(stmt, COLUMN_DICTIONARY_DISABLED)
        );
        loadDictionaryAssets(info);

        m_dictionaryCache.insert(info->id(), info);
    }
    if (isStepError(step))
    {
        ret = -1;
        goto cleanup;
    }
    sqlite3_finalize(stmt);
    stmt = nullptr;

    /* Build tag cache */
    if (sqlite3_prepare_v2(m_db, QUERY_TAGS, -1, &stmt, nullptr) != SQLITE_OK)
    {
        ret = -1;
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        uint64_t id = sqlite3_column_int64(stmt, COLUMN_TAG_DIC_ID);

        Tag *tag = new Tag;
        tag->setDictionaryInfo(getDictionary(id)->clone(tag));
        tag->setName(reinterpret_cast<const char *>(
            sqlite3_column_text(stmt, COLUMN_TAG_NAME)
        ));
        tag->setCategory(reinterpret_cast<const char *>(
            sqlite3_column_text(stmt, COLUMN_TAG_CATEGORY)
        ));
        tag->setNotes(reinterpret_cast<const char *>(
            sqlite3_column_text(stmt, COLUMN_TAG_NOTES)
        ));
        tag->setOrder(sqlite3_column_int(stmt, COLUMN_TAG_ORDER));
        tag->setScore(sqlite3_column_int(stmt, COLUMN_TAG_SCORE));

        m_tagCache[id].insert(tag->name(), tag);
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

void DatabaseManager::clearDictionaryCache()
{
    for (DictionaryInfo *info : m_dictionaryCache)
    {
        info->deleteLater();
    }
    m_dictionaryCache.clear();
}

void DatabaseManager::clearTagCache()
{
    for (QHash<QString, Tag *> &tagDict : m_tagCache)
    {
        for (Tag *tag : tagDict)
        {
            tag->deleteLater();
        }
    }
    m_tagCache.clear();
}

/* End Initializers */
/* Begin Properties */

bool DatabaseManager::modifyingDatabase() const noexcept
{
    return m_modifyingDatabase;
}

void DatabaseManager::setModifyingDatabase(bool value)
{
    if (m_modifyingDatabase == value)
    {
        return;
    }
    m_modifyingDatabase = value;
    emit modifyingDatabaseChanged(value);
}

/* End Properties */
/* Begin Dictionary Database Modifiers */

int DatabaseManager::addDictionary(QString path)
{
    QWriteLocker lock{&m_dbLock};

    setModifyingDatabase(true);
    QByteArray cpath = path.toUtf8();
    QByteArray resPath = m_resourcePath.toUtf8();
    int ret = yomi_process_dictionary(cpath, m_dbPath, resPath);
    initCache();
    setModifyingDatabase(false);

    return ret;
}

int DatabaseManager::deleteDictionary(int64_t id)
{
    QWriteLocker lock{&m_dbLock};

    setModifyingDatabase(true);
    QByteArray resPath = m_resourcePath.toUtf8();
    int ret = yomi_delete_dictionary(id, m_dbPath, resPath);
    initCache();
    setModifyingDatabase(false);

    return ret;
}

int DatabaseManager::enableDictionary(int64_t id)
{
    QWriteLocker lock{&m_dbLock};

    setModifyingDatabase(true);
    int ret = yomi_enable_dictionary(id, m_dbPath);
    initCache();
    setModifyingDatabase(false);

    return ret;
}


int DatabaseManager::disableDictionary(int64_t id)
{
    QWriteLocker lock{&m_dbLock};

    setModifyingDatabase(true);
    int ret = yomi_disable_dictionary(id, m_dbPath);
    initCache();
    setModifyingDatabase(false);

    return ret;
}

/* End Dictionary Database Modifiers */
/* Begin Database Getters */

QList<DictionaryInfo *> DatabaseManager::getDictionaries(QObject *parent) const
{
    QReadLocker lock{&m_dbLock};

    QList<DictionaryInfo *> infos;
    infos.reserve(m_dictionaryCache.size());
    for (DictionaryInfo *info : m_dictionaryCache)
    {
        infos.emplaceBack(info->clone(parent));
    }
    return infos;
}

QList<Term *> DatabaseManager::queryTerms(
    QString query, QObject *parent, QString *error) const
{
    constexpr const char *QUERY =
        "SELECT DISTINCT expression, reading "
            "FROM term_bank "
            "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND "
            "(expression = ? OR reading = ?);";

    constexpr const char *QUERY_WITH_KATAKANA =
        "SELECT DISTINCT expression, reading "
            "FROM term_bank "
            "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND "
                "(expression = ? OR reading = ? OR "
                "expression = ? OR reading = ?);";

    constexpr const char *QUERY_WITH_HALFWIDTH =
        "SELECT DISTINCT expression, reading "
            "FROM term_bank "
            "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND "
                "(expression = ? OR reading = ? OR "
                "expression = ? OR reading = ? OR "
                "expression = ? OR reading = ?);";

    constexpr int QUERY_RAW_EXPRESSION_IDX = 1;
    constexpr int QUERY_RAW_READING_IDX = 2;
    constexpr int QUERY_KATA_EXPRESSION_IDX = 3;
    constexpr int QUERY_KATA_READING_IDX = 4;
    constexpr int QUERY_HALF_EXPRESSION_IDX = 5;
    constexpr int QUERY_HALF_READING_IDX = 6;

    constexpr int COLUMN_EXPRESSION = 0;
    constexpr int COLUMN_READING = 1;

    QWriteLocker lock{&m_dbLock};

    QByteArray exp = query.toUtf8();
    QByteArray katakana = halfToFull(query).toUtf8();
    bool containsHalf = katakana != exp;
    QByteArray hiragana = kataToHira(katakana).toUtf8();
    bool containsKata = hiragana != katakana;
    sqlite3_stmt *stmt = nullptr;
    const char *sql_query = nullptr;
    int step = 0;
    QList<Term *> terms;

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
    if (sqlite3_prepare_v2(m_db, sql_query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        if (error)
        {
            *error = tr("Could not prepare database query");
        }
        goto error;
    }
    if (sqlite3_bind_text(stmt, QUERY_RAW_EXPRESSION_IDX, exp, -1, nullptr) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_RAW_READING_IDX,    exp, -1, nullptr) != SQLITE_OK)
    {
        if (error)
        {
            *error = tr("Could not bind values to statement");
        }
        goto error;
    }
    if (containsKata &&
        (
            sqlite3_bind_text(stmt, QUERY_KATA_EXPRESSION_IDX, hiragana, -1, nullptr) != SQLITE_OK ||
            sqlite3_bind_text(stmt, QUERY_KATA_READING_IDX,    hiragana, -1, nullptr) != SQLITE_OK
        ))
    {
        if (error)
        {
            *error = tr("Could not bind values to statement");
        }
        goto error;
    }
    if (containsHalf &&
        (
            sqlite3_bind_text(stmt, QUERY_HALF_EXPRESSION_IDX, katakana, -1, nullptr) != SQLITE_OK ||
            sqlite3_bind_text(stmt, QUERY_HALF_READING_IDX,    katakana, -1, nullptr) != SQLITE_OK
        ))
    {
        if (error)
        {
            *error = tr("Could not bind values to statement");
        }
        goto error;
    }

    /* Create a term for each entry */
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        Term *term = new Term(parent);
        term->setExpression(reinterpret_cast<const char *>(
            sqlite3_column_text(stmt, COLUMN_EXPRESSION)
        ));
        term->setReading(reinterpret_cast<const char *>(
            sqlite3_column_text(stmt, COLUMN_READING)
        ));
        if (addFrequencies(term))
        {
            qDebug(
                "Could not add frequencies for %s",
                qUtf8Printable(term->expression())
            );
        }
        if (addPitches(term))
        {
            qDebug(
                "Could not add pitches for %s",
                qUtf8Printable(term->expression())
            );
        }
        terms.emplaceBack(term);
    }
    if (isStepError(step))
    {
        if (error)
        {
            *error = tr("Error when executing sqlite query. Code %1").arg(step);
        }
        goto error;
    }

    /* Add data to each term */
    if (populateTerms(terms))
    {
        if (error)
        {
            *error = tr("Error getting term information");
        }
        goto error;
    }

    /* Return results on success */
    sqlite3_finalize(stmt);

    return terms;

error:
    /* Free up memory on failure */
    sqlite3_finalize(stmt);
    qDeleteAll(terms);
    terms.clear();

    return {};
}

Kanji *DatabaseManager::queryKanji(
    QString query, QObject *parent, QString *error) const
{
    constexpr const char *QUERY =
        "SELECT dic_id, onyomi, kunyomi, tags, meanings, stats FROM kanji_bank "
            "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND (char = ?);";

    constexpr int COLUMN_DIC_ID = 0;
    constexpr int COLUMN_ONYOMI = 1;
    constexpr int COLUMN_KUNYOMI = 2;
    constexpr int COLUMN_TAGS = 3;
    constexpr int COLUMN_MEANINGS = 4;
    constexpr int COLUMN_STATS = 5;

    constexpr const char *TAG_NAME_STATS = "misc";
    constexpr const char *TAG_NAME_CLAS = "class";
    constexpr const char *TAG_NAME_CODE = "code";
    constexpr const char *TAG_NAME_INDEX = "index";

    QWriteLocker lock{&m_dbLock};

    Kanji *kanji = new Kanji(parent);
    kanji->setCharacter(query);
    addFrequencies(kanji);

    QByteArray ch = query.toUtf8();
    sqlite3_stmt *stmt = nullptr;
    int step = 0;

    /* Query for the database for the definitions */
    if (sqlite3_prepare_v2(m_db, QUERY, -1, &stmt, nullptr) != SQLITE_OK)
    {
        if (error)
        {
            *error = tr("Could not prepare database query");
        }
        goto error;
    }
    if (sqlite3_bind_text(stmt, 1, ch, -1, nullptr) != SQLITE_OK)
    {
        if (error)
        {
            *error = tr("Could not bind values to statement");
        }
        goto error;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int64_t id = sqlite3_column_int64(stmt, COLUMN_DIC_ID);

        KanjiDefinition *def = new KanjiDefinition(kanji);

        def->setDictionaryInfo(getDictionary(id)->clone(def));
        def->setOnyomi(QString(
            reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, COLUMN_ONYOMI)
            )
        ).split(' '));
        def->setKunyomi(QString(
            reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, COLUMN_KUNYOMI)
            )
        ).split(' '));
        def->setGlossary(jsonArrayToStringList(
            reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, COLUMN_MEANINGS)
            )
        ));
        def->setTags(
            getTags(
                id,
                reinterpret_cast<const char *>(
                    sqlite3_column_text(stmt, COLUMN_TAGS)
                ),
                kanji
            )
        );

        QVariantMap map = QJsonDocument::fromJson(
            reinterpret_cast<const char *>(
                sqlite3_column_text(stmt, COLUMN_STATS)
            )
        ).toVariant().toMap();
        for (const auto &[key, value] : map.asKeyValueRange())
        {
            Tag *tag = m_tagCache[id][key]->clone(def);
            tag->setValue(value.toString());
            if (tag->category() == TAG_NAME_INDEX)
            {
                def->appendIndices(tag);
            }
            else if (tag->category() == TAG_NAME_STATS)
            {
                def->appendStats(tag);
            }
            else if (tag->category() == TAG_NAME_CLAS)
            {
                def->appendClasses(tag);
            }
            else if (tag->category() == TAG_NAME_CODE)
            {
                def->appendCodes(tag);
            }
            else
            {
                delete tag;
                continue;
            }
        }

        kanji->appendDefinitions(def);
    }
    if (isStepError(step))
    {
        if (error)
        {
            *error = tr("Error while executing kanji query");
        }
        goto error;
    }

    sqlite3_finalize(stmt);

    return kanji;

error:
    sqlite3_finalize(stmt);
    delete kanji;

    return nullptr;
}

/* End Database Getters */
/* Begin Query Helpers */

int DatabaseManager::populateTerms(const QList<Term *> &terms) const
{
    constexpr const char *QUERY =
        "SELECT dic_id, score, def_tags, glossary, rules, term_tags "
            "FROM term_bank "
            "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND "
                "(expression = ? AND reading = ?);";

    constexpr int QUERY_EXP_IDX = 1;
    constexpr int QUERY_READING_IDX = 2;

    constexpr int COLUMN_DIC_ID = 0;
    constexpr int COLUMN_SCORE = 1;
    constexpr int COLUMN_DEF_TAGS = 2;
    constexpr int COLUMN_GLOSSARY = 3;
    constexpr int COLUMN_RULES = 4;
    constexpr int COLUMN_TERM_TAGS = 5;

    int ret = 0;
    sqlite3_stmt *stmt = nullptr;
    int step = 0;
    QByteArray exp;
    QByteArray reading;

    for (Term *term : terms)
    {
        exp = term->expression().toUtf8();
        reading = term->reading().toUtf8();

        if (sqlite3_prepare_v2(m_db, QUERY, -1, &stmt, nullptr) != SQLITE_OK)
        {
            ret = -1;
            goto cleanup;
        }
        if (sqlite3_bind_text(stmt, QUERY_EXP_IDX,     exp,     -1, nullptr) != SQLITE_OK ||
            sqlite3_bind_text(stmt, QUERY_READING_IDX, reading, -1, nullptr) != SQLITE_OK)
        {
            ret = -1;
            goto cleanup;
        }

        QList<Tag *> tags;
        while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
        {
            const int64_t id = sqlite3_column_int64(stmt, COLUMN_DIC_ID);

            /* These fields are accumulated */
            term->setScore(
                term->score() + sqlite3_column_int(stmt, COLUMN_SCORE)
            );
            accumulateTags(
                id,
                reinterpret_cast<const char *>(
                    sqlite3_column_text(stmt, COLUMN_TERM_TAGS)
                ),
                tags,
                term
            );

            TermDefinition *def = new TermDefinition(term);
            def->setDictionaryInfo(getDictionary(id)->clone(def));
            def->setGlossary(QJsonDocument::fromJson(
                reinterpret_cast<const char *>(
                    sqlite3_column_text(stmt, COLUMN_GLOSSARY)
                )
            ).array());
            def->setScore(sqlite3_column_int(stmt, COLUMN_SCORE));
            def->setTags(
                getTags(
                    id,
                    reinterpret_cast<const char *>(
                        sqlite3_column_text(stmt, COLUMN_DEF_TAGS)
                    ),
                    term
                )
            );
            def->setRules(QString(
                reinterpret_cast<const char *>(
                    sqlite3_column_text(stmt, COLUMN_RULES)
                )
            ).split(' '));

            term->appendDefinitions(def);
        }
        term->setTags(tags);

        if (isStepError(step))
        {
            ret = -1;
            goto cleanup;
        }

        sqlite3_finalize(stmt);
        stmt = nullptr;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

DictionaryInfo *DatabaseManager::getDictionary(const int64_t id) const
{
    return m_dictionaryCache[id];
}

QList<Tag *> DatabaseManager::getTags(
    const int64_t id,
    const QString &tagStr,
    QObject *parent) const
{
    QStringList tagList = tagStr.split(" ");

    QList<Tag *> tags;
    tags.reserve(tagList.size());
    for (const QString &tagName : tagList)
    {
        if (tagName.isEmpty())
        {
            continue;
        }
        const Tag *tag = m_tagCache[id][tagName];
        if (tag == nullptr)
        {
            continue;
        }
        else if (tag->name().isEmpty())
        {
            continue;
        }
        tags.emplaceBack(tag->clone(parent));
    }
    return tags;
}

void DatabaseManager::accumulateTags(
    const int64_t id,
    const QString &tagStr,
    QList<Tag *> &tags,
    QObject *parent) const
{
    QStringList tagList = tagStr.split(" ");

    for (const QString &tagName : tagList)
    {
        if (tagName.isEmpty())
        {
            continue;
        }
        const Tag *tag = m_tagCache[id][tagName];
        if (tag == nullptr)
        {
            continue;
        }
        else if (tag->name().isEmpty())
        {
            continue;
        }

        bool exists = std::any_of(
            std::begin(tags), std::end(tags),
            [tag] (const Tag *other) -> bool
            {
                return tag->dictionaryInfo()->id() == other->dictionaryInfo()->id() &&
                    tag->name() == other->name() &&
                    tag->category() == other->category() &&
                    tag->notes() == other->notes() &&
                    tag->order() == other->order() &&
                    tag->score() == other->score();
            }
        );
        if (exists)
        {
            continue;
        }

        tags.emplaceBack(tag->clone(parent));
    }
}

int DatabaseManager::addFrequencies(Term *term) const
{
    constexpr const char *QUERY =
        "SELECT dic_id, data, type "
            "FROM term_meta_bank "
            "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND "
                "(expression = ? AND mode = 'freq');";

    int error{0};
    term->setFrequencies(
        getFrequencies(QUERY, term->expression(), term->reading(), term, &error)
    );
    return error;
}

int DatabaseManager::addFrequencies(Kanji *kanji) const
{
    constexpr const char *QUERY =
        "SELECT dic_id, data, type "
            "FROM kanji_meta_bank "
            "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND "
                "(expression = ? AND mode = 'freq');";

    int error{0};
    kanji->setFrequencies(
        getFrequencies(QUERY, kanji->character(), "", kanji, &error)
    );
    return error;
}

QList<Frequency *> DatabaseManager::getFrequencies(
    const char *query,
    const QString &expression,
    const QString &reading,
    QObject *parent,
    int *error) const
{
    constexpr const char *OBJ_READING_KEY = "reading";
    constexpr const char *OBJ_FREQ_KEY = "frequency";
    constexpr const char *OBJ_VALUE_KEY = "value";
    constexpr const char *OBJ_DISPLAY_KEY = "displayValue";

    int ret = 0;
    sqlite3_stmt *stmt = nullptr;
    int step = 0;
    QByteArray exp = expression.toUtf8();
    QList<Frequency *> frequencies;

    if (sqlite3_prepare_v2(m_db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        qDebug("Could not prepare frequency query");
        ret = -1;
        goto cleanup;
    }
    if (sqlite3_bind_text(stmt, 1, exp, -1, nullptr) != SQLITE_OK)
    {
        qDebug("Error binding expression to frequency query");
        ret = -1;
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        QString freqStr;
        switch (static_cast<yomi_blob_t>(sqlite3_column_int(stmt, 2)))
        {
        case YOMI_BLOB_TYPE_STRING:
            freqStr = reinterpret_cast<const char *>(
                sqlite3_column_blob(stmt, 1)
            );
            break;

        case YOMI_BLOB_TYPE_INT:
            freqStr = QString::number(
                *reinterpret_cast<const uint64_t *>(
                    sqlite3_column_blob(stmt, 1)
                )
            );
            break;

        case YOMI_BLOB_TYPE_OBJECT:
        {
            QJsonObject obj = QJsonDocument::fromJson(
                reinterpret_cast<const char *>(sqlite3_column_blob(stmt, 1))
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
                freqStr = QString::number(obj[OBJ_FREQ_KEY].toDouble());
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
                    freqStr = QString::number(obj[OBJ_VALUE_KEY].toDouble());
                }
                else
                {
                    continue;
                }
            }

            /* Fourth scenario:
             * [
             *      "<term>","freq",
             *      {
             *          "value": <number>,
             *          "displayValue": "<stylized frequency string>"
             *      }
             * ]
             */
            else if (obj[OBJ_DISPLAY_KEY].isString() ||
                obj[OBJ_VALUE_KEY].isDouble())
            {
                /* Check for the type that should be shown */
                if (obj[OBJ_DISPLAY_KEY].isString())
                {
                    freqStr = obj[OBJ_DISPLAY_KEY].toString();
                }
                else if (obj[OBJ_VALUE_KEY].isDouble())
                {
                    freqStr = QString::number(obj[OBJ_VALUE_KEY].toDouble());
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

        int64_t id = sqlite3_column_int64(stmt, 0);

        Frequency *f = new Frequency(parent);
        f->setDictionaryInfo(getDictionary(id)->clone(f));
        f->setFrequency(freqStr);
        frequencies.emplaceBack(f);
    }
    if (isStepError(step))
    {
        qDebug("Error executing sqlite frequency query");
        ret = -1;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);
    if (ret)
    {
        qDeleteAll(frequencies);
        frequencies.clear();
    }
    if (error)
    {
        *error = ret;
    }

    return frequencies;
}

int DatabaseManager::addPitches(Term *term) const
{
    constexpr const char *QUERY =
        "SELECT dic_id, data "
            "FROM term_meta_bank "
            "WHERE dic_id NOT IN (SELECT dic_id FROM dict_disabled) AND "
                "(expression = ? AND mode = 'pitch');";

    constexpr const char *OBJ_READING_KEY = "reading";
    constexpr const char *OBJ_PITCHES_KEY = "pitches";
    constexpr const char *OBJ_POSITION_KEY = "position";

    int ret = 0;
    sqlite3_stmt *stmt = nullptr;
    int step = 0;
    QByteArray exp = term->expression().toUtf8();

    if (sqlite3_prepare_v2(m_db, QUERY, -1, &stmt, nullptr) != SQLITE_OK)
    {
        qDebug("Could not prepare pitch query");
        ret = -1;
        goto cleanup;
    }
    if (sqlite3_bind_text(stmt, 1, exp, -1, nullptr) != SQLITE_OK)
    {
        qDebug("Error binding expression to pitch query");
        ret = -1;
        goto cleanup;
    }
    while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        QJsonObject obj = QJsonDocument::fromJson(
            reinterpret_cast<const char *>(sqlite3_column_blob(stmt, 1))
        ).object();
        const QString reading = obj[OBJ_READING_KEY].toString();
        if (reading != term->reading() && reading != term->expression())
        {
            continue;
        }

        int64_t id = sqlite3_column_int64(stmt, 0);

        Pitch *pitch = new Pitch(term);
        pitch->setDictionaryInfo(getDictionary(id)->clone(pitch));

        /* Add mora */
        QStringList mora;
        QString currentMora;
        for (const QChar &ch : reading)
        {
            if (!currentMora.isEmpty() && !m_moraSkipChar.contains(ch))
            {
                mora.emplaceBack(currentMora);
                currentMora.clear();
            }
            currentMora += ch;
        }
        if (!currentMora.isEmpty())
        {
            mora.append(currentMora);
        }
        pitch->setMora(mora);

        /* Add pitch positions */
        QList<int> positions;
        QJsonArray arr = obj[OBJ_PITCHES_KEY].toArray();
        for (const QJsonValue &val : arr)
        {
            positions.emplaceBack(val.toObject()[OBJ_POSITION_KEY].toInt());
        }
        pitch->setPositions(positions);

        term->appendPitches(pitch);
    }
    if (isStepError(step))
    {
        qDebug("Error executing sqlite pitch query");
        ret = -1;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

/* End Query Helpers */
/* Begin Helpers */

QString DatabaseManager::errorCodeToString(const int code) const
{
    switch (code)
    {
    case YOMI_ERR_ADDING_INDEX:
        return tr("Could not add index.json");
    case YOMI_ERR_ADDING_KANJI:
        return tr("Could not add kanji terms");
    case YOMI_ERR_ADDING_KANJI_META:
        return tr("Could not add kanji metadata");
    case YOMI_ERR_ADDING_TAGS:
        return tr("Could not add tags");
    case YOMI_ERR_ADDING_TERMS:
        return tr("Could not add terms");
    case YOMI_ERR_ADDING_TERMS_META:
        return tr("Could not add term metadata");
    case YOMI_ERR_DB:
        return tr("Could not open database");
    case YOMI_ERR_NEWER_VERSION:
        return tr("Database is of a newer version");
    case YOMI_ERR_OPENING_DIC:
        return tr("Could not open dictionary file");
    case YOMI_ERR_DELETE:
        return tr("Could not execute delete query on database");
    case YOMI_ERR_EXTRACTING_RESOURCES:
        return tr("Could not extract dictionary resources");
    case YOMI_ERR_REMOVING_RESOURCES:
        return tr("Could not remove dictionary resources");
    default:
        return tr("Unknown error");
    }
}

QString DatabaseManager::halfToFull(const QString &query)
{
    static const QChar HALFWIDTH_LOW(0xFF61);
    static const QChar HALFWIDTH_HIGH(0xFF9F);
    static const QChar HALFWIDTH_VOICED(0xFF9E);
    static const QChar HALFWIDTH_SEMI_VOICED(0xFF9F);

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
    qsizetype i{0};
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

QString DatabaseManager::kataToHira(QString query)
{
    static const QChar KATAKANA_LOW(0x30A1);
    static const QChar KATAKANA_HIGH(0x30F6);
    static const QChar HIRAGANA_LOW(0x3041);
    [[maybe_unused]] static const QChar HIRAGANA_HIGH(0x3096);

    for (QChar &ch : query)
    {
        ushort code = ch.unicode();
        if (code >= KATAKANA_LOW && code <= KATAKANA_HIGH)
        {
            ch = QChar(HIRAGANA_LOW.unicode() +
                (code - KATAKANA_LOW.unicode()));
        }
    }
    return query;
}

QStringList DatabaseManager::jsonArrayToStringList(const char *jsonstr)
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
