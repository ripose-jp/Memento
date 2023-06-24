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

#include "yomidbbuilder.h"

#include <errno.h>
#include <json-c/json.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <zip.h>

#ifdef _WIN32
#include <windows.h>

#include <direct.h>
#include <shellapi.h>
#include <stringapiset.h>
#include <tchar.h>
#else
#define __USE_XOPEN_EXTENDED 500

#include <ftw.h>
#endif

#define INDEX_FILE              "index.json"
#define TAG_BANK_FORMAT         "tag_bank_%u.json"
#define TERM_BANK_FORMAT        "term_bank_%u.json"
#define TERM_META_BANK_FORMAT   "term_meta_bank_%u.json"
#define KANJI_BANK_FORMAT       "kanji_bank_%u.json"
#define KANJI_META_BANK_FORMAT  "kanji_meta_bank_%u.json"

#define FILENAME_BUFFER_SIZE  256

#define STAT_ERR                    -1
#define INVALID_SIZE_ERR            -2
#define MALLOC_FAILURE_ERR          -3
#define ZIP_FILE_OPEN_ERR           -4
#define ZIP_FILE_READ_ERR           -5
#define UNSUPPORTED_FORMAT_ERR      -6
#define JSON_WRONG_TYPE_ERR         -7
#define JSON_MISSING_KEY_ERR        -8
#define STATEMENT_PREPARE_ERR       -9
#define STATEMENT_BIND_ERR          -10
#define STATEMENT_STEP_ERR          -11
#define DB_NEW_VERSION_ERR          -12
#define CREATE_DB_ERR               -13
#define DB_TABLE_DROP_ERR           -14
#define DB_CREATE_TABLE_ERR         -15
#define TAG_WRONG_SIZE_ERR          -16
#define TERM_WRONG_SIZE_ERR         -17
#define KANJI_WRONG_SIZE_ERR        -18
#define META_WRONG_SIZE_ERR         -19
#define UNKNOWN_BANK_TYPE_ERR       -20
#define UNKNOWN_DATA_TYPE_ERR       -21
#define PRAGMA_SET_ERR              -22
#define TRANSACTION_ERR             -23
#define DB_ALTER_TABLE_ERR          -24

typedef enum bank_type
{
    tag_bank,
    term_bank,
    term_meta_bank,
    kanji_bank,
    kanji_meta_bank
} bank_type;

/**
 * Begins an database transaction
 * @param db The database to begin a transaction on
 * @returns Error code
 */
static int begin_transaction(sqlite3 *db)
{
    char *errmsg = NULL;
    sqlite3_exec(db, "BEGIN EXCLUSIVE TRANSACTION;", NULL, NULL, &errmsg);
    if (errmsg)
    {
        fprintf(stderr, "Could not begin transaction\nError: %s\n", errmsg);
        sqlite3_free(errmsg);
        return TRANSACTION_ERR;
    }
    return 0;
}

/**
 * Commits a database transaction
 * @param db The database commit to
 * @returns Error code
 */
static int commit_transaction(sqlite3 *db)
{
    char *errmsg = NULL;
    sqlite3_exec(db, "COMMIT;", NULL, NULL, &errmsg);
    if (errmsg)
    {
        fprintf(stderr, "Could not commit transaction\nError: %s\n", errmsg);
        sqlite3_free(errmsg);
        return TRANSACTION_ERR;
    }
    return 0;
}

/**
 * Rolls back the current transaction.
 * @param db The database to rollback the transaction on.
 * @return Error code
 */
static int rollback_transaction(sqlite3 *db)
{
    char *errmsg = NULL;
    sqlite3_exec(db, "ROLLBACK;", NULL, NULL, &errmsg);
    if (errmsg)
    {
        fprintf(stderr, "Could not commit transaction\nError: %s\n", errmsg);
        sqlite3_free(errmsg);
        return TRANSACTION_ERR;
    }
    return 0;
}

/**
 * Drops all the tables provided in argv
 * @param   db   The database to drop tables from
 * @param   argc The number of values in the next argument
 * @param   argv The array of table names to drop
 * @return Error code
 */
static int drop_all_tables_callback(void *db, int argc, char **argv, char **unused __attribute__((unused)))
{
    for (char **ptr = argv; ptr < &argv[argc]; ++ptr)
    {
        if (strncmp(*ptr, "sqlite_", 7) == 0)
        {
            continue;
        }

        char *drop_query = sqlite3_mprintf("DROP TABLE IF EXISTS %Q;", *ptr);
        char *errmsg     = NULL;

        if (drop_query == NULL)
        {
            return MALLOC_FAILURE_ERR;
        }

        sqlite3_exec((sqlite3 *)db, drop_query, NULL, NULL, &errmsg);
        sqlite3_free(drop_query);

        if (errmsg)
        {
            fprintf(stderr, "Could not drop table %s\nError: %s\n", *ptr, errmsg);
            sqlite3_free(errmsg);
        }
    }

    return 0;
}

/**
 * Brings the database schema up to the current version's specifications
 * @param   db      The database to modify
 * @return Error code
 */
static int create_db(sqlite3 *db)
{
    int   ret    = 0;
    char *errmsg = NULL;
    char *pragma = NULL;

    /* Drops all the tables in the db */
    /* This behavior may change in future versions */
    sqlite3_exec(db, "SELECT name FROM sqlite_master WHERE type='table'",
                 drop_all_tables_callback, db, &errmsg);
    if (errmsg)
    {
        fprintf(stderr, "Failed to drop tables\nError: %s\n", errmsg);
        ret = DB_TABLE_DROP_ERR;
        goto cleanup;
    }

    /* Create all needed tables */
    sqlite3_exec(
        db,
        "CREATE TABLE directory ("
            "dic_id     INTEGER     PRIMARY KEY,"
            "title      TEXT        NOT NULL UNIQUE,"
            "format     INTEGER     NOT NULL,"
            "revision   TEXT        NOT NULL,"
            "sequenced  INTEGER     NOT NULL"   // Boolean
        ");"
        "CREATE TRIGGER directory_remove AFTER DELETE ON directory "
        "BEGIN "
            "DELETE FROM dict_disabled   WHERE dic_id = old.dic_id;"
            "DELETE FROM tag_bank        WHERE dic_id = old.dic_id;"
            "DELETE FROM term_bank       WHERE dic_id = old.dic_id;"
            "DELETE FROM term_meta_bank  WHERE dic_id = old.dic_id;"
            "DELETE FROM kanji_bank      WHERE dic_id = old.dic_id;"
            "DELETE FROM kanji_meta_bank WHERE dic_id = old.dic_id;"
        "END;"

        "CREATE TABLE dict_disabled ("
            "dic_id     INTEGER     PRIMARY KEY"
        ");"

        "CREATE TABLE tag_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "name       TEXT        NOT NULL,"
            "category   TEXT        NOT NULL,"
            "ord        INTEGER     NOT NULL,"
            "notes      TEXT        NOT NULL,"
            "score      INTEGER     NOT NULL,"
            "PRIMARY KEY(dic_id, name)"
        ");"
        "CREATE INDEX idx_tag_bank_name ON tag_bank(dic_id, name);"

        "CREATE TABLE term_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "expression TEXT        NOT NULL,"
            "reading    TEXT        NOT NULL,"
            "def_tags   TEXT        NOT NULL,"  // Space separated list
            "rules      TEXT        NOT NULL,"  // Space separated list
            "score      INTEGER     NOT NULL,"
            "glossary   TEXT        NOT NULL,"  // Json array
            "sequence   INTEGER     NOT NULL,"
            "term_tags  TEXT        NOT NULL"   // Space separated list
        ");"
        "CREATE INDEX idx_term_bank_exp     ON term_bank(expression);"
        "CREATE INDEX idx_term_bank_reading ON term_bank(reading);"
        "CREATE INDEX idx_term_bank_combo   ON term_bank(expression, reading);"

        "CREATE TABLE term_meta_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "expression TEXT        NOT NULL,"
            "mode       TEXT        NOT NULL,"
            "type       INTEGER     NOT NULL,"  // Type of data in the blob
            "data       BLOB"                   // Data defined by mode
        ");"
        "CREATE INDEX idx_term_meta_exp ON term_meta_bank(expression, mode);"

        "CREATE TABLE kanji_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "char       TEXT        NOT NULL,"
            "onyomi     TEXT        NOT NULL,"  // Space separated list
            "kunyomi    TEXT        NOT NULL,"  // Space separated list
            "tags       TEXT        NOT NULL,"  // Space separated list
            "meanings   TEXT        NOT NULL,"  // Json array
            "stats      TEXT        NOT NULL"   // Json object
        ");"
        "CREATE INDEX idx_kanji_bank_char ON kanji_bank(char);"

        "CREATE TABLE kanji_meta_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "expression TEXT        NOT NULL,"
            "mode       TEXT        NOT NULL,"
            "type       INTEGER     NOT NULL," // Type of data in the blob
            "data       BLOB"                  // Data defined by mode
        ");"
        "CREATE INDEX idx_kanji_meta_exp ON kanji_meta_bank(expression, mode);",
        NULL, NULL, &errmsg
    );
    if (errmsg)
    {
        fprintf(stderr, "Failed to create tables\nError: %s\n", errmsg);
        ret = DB_CREATE_TABLE_ERR;
        goto cleanup;
    }

    /* Update the user_version pragma */
    pragma = sqlite3_mprintf("PRAGMA user_version = %d;", YOMI_DB_VERSION);
    if (pragma == NULL)
    {
        fprintf(stderr, "Could not allocate memory for query\n");
        ret = MALLOC_FAILURE_ERR;
        goto cleanup;
    }
    sqlite3_exec(db, pragma, NULL, NULL, &errmsg);
    if (errmsg)
    {
        fprintf(stderr, "Failed to create tables\nError: %s\n", errmsg);
        ret = DB_CREATE_TABLE_ERR;
        goto cleanup;
    }

cleanup:
    sqlite3_free(errmsg);
    sqlite3_free(pragma);

    return ret;
}

static int update_v1_to_v2(sqlite3 *db)
{
    int        ret     = 0;
    const int  version = 2;
    char      *pragma  = NULL;
    char      *errmsg  = NULL;

    pragma = sqlite3_mprintf(
        "ALTER TABLE term_meta_bank  ADD type INTEGER NOT NULL DEFAULT 0;"
        "ALTER TABLE kanji_meta_bank ADD type INTEGER NOT NULL DEFAULT 0;"

        "UPDATE term_meta_bank "
            "SET   type = %u "
            "WHERE mode = 'pitch';"
        "UPDATE kanji_meta_bank "
            "SET   type = %u "
            "WHERE mode = 'pitch';"

        "UPDATE term_meta_bank "
            "SET   type = %u "
            "WHERE mode = 'freq';"
        "UPDATE kanji_meta_bank "
            "SET   type = %u "
            "WHERE mode = 'freq';"

        "PRAGMA user_version = %d;",
        YOMI_BLOB_TYPE_OBJECT, YOMI_BLOB_TYPE_OBJECT,
        YOMI_BLOB_TYPE_INT,    YOMI_BLOB_TYPE_INT,
        version
    );
    if (pragma == NULL)
    {
        fprintf(stderr, "Could not allocate memory for query\n");
        ret = MALLOC_FAILURE_ERR;
        goto cleanup;
    }

    if (sqlite3_exec(db, pragma, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr,
            "Failed to update database from version 1 to 2.\n"
            "Error: %s\n"
            "Query: %s\n",
            errmsg, pragma
        );
        ret = DB_ALTER_TABLE_ERR;
        goto cleanup;
    }

cleanup:
    sqlite3_free(errmsg);
    sqlite3_free(pragma);

    return ret;
}

static int update_v2_to_v3(sqlite3 *db)
{
    int        ret     = 0;
    const int  version = 3;
    char      *pragma  = NULL;
    char      *errmsg  = NULL;

    pragma = sqlite3_mprintf(
        "CREATE TABLE dict_disabled ("
            "dic_id     INTEGER     PRIMARY KEY"
        ");"

        "DROP TRIGGER directory_remove;"
        "CREATE TRIGGER directory_remove AFTER DELETE ON directory "
        "BEGIN "
            "DELETE FROM tag_bank        WHERE dic_id = old.dic_id;"
            "DELETE FROM term_bank       WHERE dic_id = old.dic_id;"
            "DELETE FROM term_meta_bank  WHERE dic_id = old.dic_id;"
            "DELETE FROM kanji_bank      WHERE dic_id = old.dic_id;"
            "DELETE FROM kanji_meta_bank WHERE dic_id = old.dic_id;"
            "DELETE FROM dict_disabled   WHERE dic_id = old.dic_id;"
        "END;"

        "PRAGMA user_version = %d;",
        version
    );

    if (pragma == NULL)
    {
        fprintf(stderr, "Could not allocate memory for query\n");
        ret = MALLOC_FAILURE_ERR;
        goto cleanup;
    }

    if (sqlite3_exec(db, pragma, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr,
            "Failed to update database from version 2 to 3.\n"
            "Error: %s\n"
            "Query: %s\n",
            errmsg, pragma
        );
        ret = DB_ALTER_TABLE_ERR;
        goto cleanup;
    }

cleanup:
    sqlite3_free(errmsg);
    sqlite3_free(pragma);

    return ret;
}

static int update_v3_to_v4(sqlite3 *db)
{
    int        ret     = 0;
    const int  version = 4;
    char      *pragma  = NULL;
    char      *errmsg  = NULL;

    pragma = sqlite3_mprintf(
        "UPDATE term_bank SET reading = '' WHERE expression = reading;"
        "PRAGMA user_version = %d;",
        version
    );

    if (pragma == NULL)
    {
        fprintf(stderr, "Could not allocate memory for query\n");
        ret = MALLOC_FAILURE_ERR;
        goto cleanup;
    }

    if (sqlite3_exec(db, pragma, NULL, NULL, &errmsg) != SQLITE_OK)
    {
        fprintf(stderr,
            "Failed to update database from version 3 to 4.\n"
            "Error: %s\n"
            "Query: %s\n",
            errmsg, pragma
        );
        ret = DB_ALTER_TABLE_ERR;
        goto cleanup;
    }

cleanup:
    sqlite3_free(errmsg);
    sqlite3_free(pragma);

    return ret;
}

/**
 * Create the tables in the database if they do not already exist
 * @param   db The database to add tables to
 * @return Error code
 */
static int prepare_db(sqlite3 *db)
{
    int           ret          = 0;
    int           user_version = 0;
    sqlite3_stmt *stmt         = NULL;
    char         *errmsg       = NULL;

    /* Check if the schema is an empty file */
    if (sqlite3_prepare_v2(db, "PRAGMA user_version;", -1, &stmt, NULL) != SQLITE_OK)
    {
        ret = STATEMENT_PREPARE_ERR;
        goto cleanup;
    }
    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        fprintf(stderr, "Could not execute user_version check\n");
        ret = STATEMENT_STEP_ERR;
        goto cleanup;
    }
    user_version = sqlite3_column_int(stmt, 0);
    if (user_version > YOMI_DB_VERSION)
    {
        fprintf(stderr, "Expected user_version %d, got newer version %d\n",
                YOMI_DB_VERSION, user_version);
        ret = DB_NEW_VERSION_ERR;
        goto cleanup;
    }

    switch (user_version)
    {
    case 0:
        if ((ret = create_db(db)))
        {
            fprintf(
                stderr,
                "Could not update the database, reported version %d\n",
                user_version
            );
            goto cleanup;
        }
        break;

    case 1:
        if ((ret = update_v1_to_v2(db)))
        {
            goto cleanup;
        }
        __attribute__((fallthrough));

    case 2:
        if ((ret = update_v2_to_v3(db)))
        {
            goto cleanup;
        }
        __attribute__((fallthrough));

    case 3:
        if ((ret = update_v3_to_v4(db)))
        {
            goto cleanup;
        }
    }

    /* Set all PRAGMA value to their expected values */
    sqlite3_exec(db, "PRAGMA recursive_triggers = true;", NULL, NULL, &errmsg);
    if (errmsg)
    {
        fprintf(stderr, "Could not set PRAGMA values\nError: %s\n", errmsg);
        ret = PRAGMA_SET_ERR;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);
    sqlite3_free(errmsg);

    return ret;
}

/**
 * Parse the file in the archive and returns its object representation
 * @param       archive  Archive containing the file
 * @param       filename Name of the file to get the size of
 * @param[out]  obj      The Json object
 * @return Error code
 */
static int get_json_obj(zip_t *archive, const char *filename, json_object **obj)
{
    int              ret        = 0;
    struct zip_stat  st;
    char            *contents   = NULL;
    struct zip_file *file       = NULL;
    zip_int64_t      bytes_read = 0;

    /* Get the size of the file */
    zip_stat_init(&st);
    if (zip_stat(archive, filename, 0, &st))
    {
        fprintf(stderr, "Failed to stat file %s\n", filename);
        ret = STAT_ERR;
        goto cleanup;
    }
    if (!(st.valid & ZIP_STAT_SIZE))
    {
        fprintf(stderr, "%s did not have a valid size\n", filename);
        ret = INVALID_SIZE_ERR;
        goto cleanup;
    }

    /* Allocate buffer for the contents of the file */
    contents = (char *)malloc(sizeof(char) * (st.size + 1));
    if (!contents)
    {
        fprintf(stderr, "Failed to allocate memory to read %s\n", filename);
        ret = MALLOC_FAILURE_ERR;
        goto cleanup;
    }

    /* Read filename into the contents buffer */
    file = zip_fopen(archive, filename, 0);
    if (!file)
    {
        fprintf(stderr, "Could not open %s\n", filename);
        ret = ZIP_FILE_OPEN_ERR;
        goto cleanup;
    }
    bytes_read = zip_fread(file, contents, st.size);
    if (bytes_read == -1 || (zip_uint64_t)bytes_read != st.size)
    {
        fprintf(stderr, "Did not read expected number of bytes in %s\n", filename);
#ifdef __APPLE__
        fprintf(stderr, "Expected bytes: %llu\tActual bytes read %llu\n", st.size, bytes_read);
#elif defined(_WIN32)
        fprintf(stderr, "Expected bytes: %lld\tActual bytes read %lld\n", st.size, bytes_read);
#else
        fprintf(stderr, "Expected bytes: %ld\tActual bytes read %ld\n", st.size, bytes_read);
#endif
        ret = ZIP_FILE_READ_ERR;
        goto cleanup;
    }
    contents[st.size] = '\0';

    /* Parse the contents into a json object */
    *obj = json_tokener_parse(contents);

cleanup:
    free(contents);
    if (file)
    {
        zip_fclose(file);
    }

    return ret;
}

/**
 * Get the json object from the parent object of type and check for correctness
 * @param      parent The object to get the value from
 * @param      key    The key to the value
 * @param      type   The type of the value
 * @param[out] obj    The object to set to the value object
 * @return Error code
 */
static int get_obj_from_obj(json_object *parent, const char *key,
                            json_type type, json_object **obj)
{
    if (!json_object_object_get_ex(parent, key, obj))
    {
        fprintf(stderr, "Could not get \"%s\" key from object\n%s\n", key,
                json_object_to_json_string_ext(parent, JSON_C_TO_STRING_PRETTY));
        return JSON_MISSING_KEY_ERR;
    }
    if (!json_object_is_type(*obj, type))
    {
        fprintf(stderr, "Key \"%s\" is not of type %s\n%s\n", key, json_type_to_name(type),
                json_object_to_json_string_ext(parent, JSON_C_TO_STRING_PRETTY));
        return JSON_WRONG_TYPE_ERR;
    }

    return 0;
}

/* Begin add_index defines */

#define TITLE_KEY     "title"
#define FORMAT_KEY    "format"
#define REV_KEY       "revision"
#define SEQ_KEY       "sequenced"

#define QUERY   "INSERT OR REPLACE INTO directory (title, format, revision, sequenced) VALUES (?, ?, ?, ?);"
#define TITLE_INDEX     1
#define FORMAT_INDEX    2
#define REV_INDEX       3
#define SEQ_INDEX       4

/**
 * Adds the yomichan index.json file to the database
 * @param      dict_archive The dictionary archive holding index.json
 * @param      db           The database
 * @param[out] id           The id of the dictionary
 * @return Error code
 */
static int add_index(zip_t *dict_archive, sqlite3 *db, sqlite3_int64 *id)
{
    int           ret       = 0;
    json_object  *index_obj = NULL;
    json_object  *ret_obj   = NULL;

    const char   *title     = NULL;
    int32_t       format    = 0;
    const char   *revision  = NULL;
    json_bool     sequenced = 0;

    sqlite3_stmt *stmt      = NULL;
    int           step      = 0;

    /* Get the index object from the index file */
    if ((ret = get_json_obj(dict_archive, INDEX_FILE, &index_obj)))
    {
        goto cleanup;
    }
    if (!json_object_is_type(index_obj, json_type_object))
    {
        fprintf(stderr, "Returned index json was not an object\n");
        ret = JSON_WRONG_TYPE_ERR;
        goto cleanup;
    }

    /* Get the fields from the json object */
    if ((ret = get_obj_from_obj(index_obj, TITLE_KEY, json_type_string, &ret_obj)))
        goto cleanup;
    title = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_obj(index_obj, FORMAT_KEY, json_type_int, &ret_obj)))
        goto cleanup;
    format = json_object_get_int(ret_obj);

    if ((ret = get_obj_from_obj(index_obj, REV_KEY, json_type_string, &ret_obj)))
        goto cleanup;
    revision = json_object_get_string(ret_obj);

    if (!json_object_object_get_ex(index_obj, SEQ_KEY, &ret_obj))
    {
        sequenced = 0;
    }
    else
    {
        if ((ret = get_obj_from_obj(index_obj, SEQ_KEY, json_type_boolean, &ret_obj)))
            goto cleanup;
        sequenced = json_object_get_boolean(ret_obj);
    }

    /* Check that the format is valid */
    if (format != YOMI_DB_FORMAT_VERSION)
    {
        fprintf(stderr, "Unsupported dictionary format %d different from supported %d\n",
                format, YOMI_DB_FORMAT_VERSION);
        ret = UNSUPPORTED_FORMAT_ERR;
        goto cleanup;
    }

    /* Put dictionary metadata into the index table */
    if (sqlite3_prepare_v2(db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Could not prepare sqlite statement\n");
        fprintf(stderr, "Query: %s\n", QUERY);
        ret = STATEMENT_PREPARE_ERR;
        goto cleanup;
    }
    if (sqlite3_bind_text(stmt, TITLE_INDEX,  title,    -1, NULL) != SQLITE_OK ||
        sqlite3_bind_int (stmt, FORMAT_INDEX, format            ) != SQLITE_OK ||
        sqlite3_bind_text(stmt, REV_INDEX,    revision, -1, NULL) != SQLITE_OK ||
        sqlite3_bind_int (stmt, SEQ_INDEX,    sequenced         ) != SQLITE_OK)
    {
        fprintf(stderr, "Could not bind values to sqlite statement\n");
        ret = STATEMENT_BIND_ERR;
        goto cleanup;
    }
    if ((step = sqlite3_step(stmt)) != SQLITE_DONE)
    {
        fprintf(stderr, "Could not commit to database, sqlite3 error code %d\n", step);
        ret = STATEMENT_STEP_ERR;
        goto cleanup;
    }

    /* Copy the title into name */
    *id = sqlite3_last_insert_rowid(db);

cleanup:
    json_object_put(index_obj);
    sqlite3_finalize(stmt);

    return ret;
}

#undef TITLE_KEY
#undef FORMAT_KEY
#undef REV_KEY
#undef SEQ_KEY

#undef QUERY
#undef TITLE_INDEX
#undef FORMAT_INDEX
#undef REV_INDEX
#undef SEQ_INDEX

/* End add_index defines */

/**
 * Checks the type and returns an array object
 * @param      arr     The array to obtains values from
 * @param      idx     The index of the element to return
 * @param      type    The type of the element
 * @param[out] ret_obj The json object to return to
 * @return 0 on the correct type, JSON_WRONG_TYPE_ERR otherwise
 */
static int get_obj_from_array(json_object *arr, size_t idx, json_type type, json_object **ret_obj)
{
    *ret_obj = json_object_array_get_idx(arr, idx);
    if (!json_object_is_type(*ret_obj, type))
    {
#ifdef _WIN32
        fprintf(stderr, "Expected index %llu to be of type %s\n%s\n", idx, json_type_to_name(type),
                        json_object_to_json_string_ext(arr, JSON_C_TO_STRING_PRETTY));
#else
        fprintf(stderr, "Expected index %lu to be of type %s\n%s\n", idx, json_type_to_name(type),
                json_object_to_json_string_ext(arr, JSON_C_TO_STRING_PRETTY));
#endif
        *ret_obj = NULL;
        return JSON_WRONG_TYPE_ERR;
    }
    return 0;
}

/* Begin add_tag defines */

#define TAG_ARRAY_SIZE  5

#define QUERY   "INSERT INTO tag_bank (dic_id, name, category, ord, notes, score) "\
                    "VALUES (?, ?, ?, ?, ?, ?);"

#define NAME_INDEX      0
#define CATEGORY_INDEX  1
#define ORDER_INDEX     2
#define NOTES_INDEX     3
#define SCORE_INDEX     4

#define QUERY_DIC_ID_INDEX    1
#define QUERY_NAME_INDEX      2
#define QUERY_CATEGORY_INDEX  3
#define QUERY_ORDER_INDEX     4
#define QUERY_NOTES_INDEX     5
#define QUERY_SCORE_INDEX     6

/**
 * Add the tag stored in the json array
 * @param db  The database to add the tag to
 * @param tag The tag array to add to the database
 * @param id  The id of the dictionary the tag belongs to
 * @return Error code
 */
static int add_tag(sqlite3 *db, json_object *tag, const sqlite3_int64 id)
{
    int           ret      = 0;
    json_object  *ret_obj  = NULL;

    const char   *name     = NULL;
    const char   *category = NULL;
    int           order    = 0;
    const char   *notes    = NULL;
    int           score    = 0;

    sqlite3_stmt *stmt     = NULL;
    int           step     = 0;

    /* Make sure the length of the tag array is correct */
    if (json_object_array_length(tag) != TAG_ARRAY_SIZE)
    {
#ifdef _WIN32
        fprintf(stderr, "Expected tag array of size %u, got %llu\n",
                TAG_ARRAY_SIZE, json_object_array_length(tag));
#else
        fprintf(stderr, "Expected tag array of size %u, got %lu\n",
                TAG_ARRAY_SIZE, json_object_array_length(tag));
#endif
        ret = TAG_WRONG_SIZE_ERR;
        goto cleanup;
    }

    /* Get the objects to add to the database */
    if ((ret = get_obj_from_array(tag, NAME_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    name = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(tag, CATEGORY_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    category = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(tag, ORDER_INDEX, json_type_int, &ret_obj)))
        goto cleanup;
    order = json_object_get_int(ret_obj);

    if ((ret = get_obj_from_array(tag, NOTES_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    notes = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(tag, SCORE_INDEX, json_type_int, &ret_obj)))
        goto cleanup;
    score = json_object_get_int(ret_obj);

    /* Add tag to the database */
    if (sqlite3_prepare_v2(db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Could not prepare sqlite statement\n");
        fprintf(stderr, "Query: %s\n", QUERY);
        ret = STATEMENT_PREPARE_ERR;
        goto cleanup;
    }
    if (sqlite3_bind_int (stmt, QUERY_DIC_ID_INDEX,   id                ) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_NAME_INDEX,     name,     -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_CATEGORY_INDEX, category, -1, NULL) != SQLITE_OK ||
        sqlite3_bind_int (stmt, QUERY_ORDER_INDEX,    order             ) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_NOTES_INDEX,    notes,    -1, NULL) != SQLITE_OK ||
        sqlite3_bind_int (stmt, QUERY_SCORE_INDEX,    score             ) != SQLITE_OK)
    {
        fprintf(stderr, "Could not bind values to sqlite statement\n");
        ret = STATEMENT_BIND_ERR;
        goto cleanup;
    }
    if ((step = sqlite3_step(stmt)) != SQLITE_DONE)
    {
        fprintf(stderr, "Could not commit to database, sqlite3 error code %d\n", step);
        ret = STATEMENT_STEP_ERR;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

#undef TAG_ARRAY_SIZE

#undef QUERY

#undef NAME_INDEX
#undef CATEGORY_INDEX
#undef ORDER_INDEX
#undef NOTES_INDEX
#undef SCORE_INDEX

#undef QUERY_DIC_ID_INDEX
#undef QUERY_NAME_INDEX
#undef QUERY_CATEGORY_INDEX
#undef QUERY_ORDER_INDEX
#undef QUERY_NOTES_INDEX
#undef QUERY_SCORE_INDEX

/* End add_tag defines */
/* Begin add_term defines */

#define TERM_ARRAY_SIZE     8

#define QUERY   "INSERT INTO term_bank "\
                    "(dic_id, expression, reading, def_tags, rules, score, glossary, sequence, term_tags) "\
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);"

#define EXPRESSION_INDEX    0
#define READING_INDEX       1
#define DEF_TAGS_INDEX      2
#define RULES_INDEX         3
#define SCORE_INDEX         4
#define GLOSSARY_INDEX      5
#define SEQUENCE_INDEX      6
#define TERM_TAGS_INDEX     7

#define QUERY_DIC_ID_INDEX          1
#define QUERY_EXPRESSION_INDEX      2
#define QUERY_READING_INDEX         3
#define QUERY_DEF_TAGS_INDEX        4
#define QUERY_RULES_INDEX           5
#define QUERY_SCORE_INDEX           6
#define QUERY_GLOSSARY_INDEX        7
#define QUERY_SEQUENCE_INDEX        8
#define QUERY_TERM_TAGS_INDEX       9

/**
 * Add the term stored in the json array
 * @param db   The database to add the tag to
 * @param term The term array to add to the database
 * @param id   The id of the dictionary the tag belongs to
 * @return Error code
 */
static int add_term(sqlite3 *db, json_object *term, const sqlite3_int64 id)
{
    int           ret       = 0;
    json_object  *ret_obj   = NULL;

    const char   *exp       = NULL;
    const char   *reading   = NULL;
    const char   *def_tags  = NULL;
    const char   *rules     = NULL;
    int           score     = 0;
    const char   *glossary  = NULL;
    int           sequence  = 0;
    const char   *term_tags = NULL;

    sqlite3_stmt *stmt      = NULL;
    int           step      = 0;

    /* Make sure the length of the term array is correct */
    if (json_object_array_length(term) != TERM_ARRAY_SIZE)
    {
#ifdef _WIN32
        fprintf(stderr, "Expected term array of size %u, got %llu\n",
                TERM_ARRAY_SIZE, json_object_array_length(term));
#else
        fprintf(stderr, "Expected term array of size %u, got %lu\n",
                TERM_ARRAY_SIZE, json_object_array_length(term));
#endif
        ret = TERM_WRONG_SIZE_ERR;
        goto cleanup;
    }

    /* Get the objects to add to the database */
    if ((ret = get_obj_from_array(term, EXPRESSION_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    exp = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(term, READING_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    reading = json_object_get_string(ret_obj);

    ret_obj = json_object_array_get_idx(term, DEF_TAGS_INDEX);
    switch (json_object_get_type(ret_obj))
    {
        case json_type_string:
            def_tags = json_object_get_string(ret_obj);
            ret = 0;
            break;
        case json_type_null:
            def_tags = "";
            ret = 0;
            break;
        default:
            fprintf(stderr, "Expected index %u to be of type string or null\n%s\n",
                    DEF_TAGS_INDEX,
                    json_object_to_json_string_ext(term, JSON_C_TO_STRING_PRETTY
            ));
            ret_obj = NULL;
            ret = JSON_WRONG_TYPE_ERR;
            goto cleanup;
    }

    if ((ret = get_obj_from_array(term, RULES_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    rules = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(term, SCORE_INDEX, json_type_int, &ret_obj)))
        goto cleanup;
    score = json_object_get_int(ret_obj);

    if ((ret = get_obj_from_array(term, GLOSSARY_INDEX, json_type_array, &ret_obj)))
        goto cleanup;
    glossary = json_object_to_json_string(ret_obj);

    if ((ret = get_obj_from_array(term, SEQUENCE_INDEX, json_type_int, &ret_obj)))
        goto cleanup;
    sequence = json_object_get_int(ret_obj);

    if ((ret = get_obj_from_array(term, TERM_TAGS_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    term_tags = json_object_get_string(ret_obj);

    /* Make sure that expression and reading aren't the same */
    if (!strcmp(exp, reading))
    {
        reading = "";
    }

    /* Add term to the database */
    if (sqlite3_prepare_v2(db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Could not prepare sqlite statement\n");
        fprintf(stderr, "Query: %s\n", QUERY);
        ret = STATEMENT_PREPARE_ERR;
        goto cleanup;
    }
    if (sqlite3_bind_int (stmt, QUERY_DIC_ID_INDEX,     id                 ) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_EXPRESSION_INDEX, exp,       -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_READING_INDEX,    reading,   -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_DEF_TAGS_INDEX,   def_tags,  -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_RULES_INDEX,      rules,     -1, NULL) != SQLITE_OK ||
        sqlite3_bind_int (stmt, QUERY_SCORE_INDEX,      score              ) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_GLOSSARY_INDEX,   glossary,  -1, NULL) != SQLITE_OK ||
        sqlite3_bind_int (stmt, QUERY_SEQUENCE_INDEX,   sequence           ) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_TERM_TAGS_INDEX,  term_tags, -1, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Could not bind values to sqlite statement\n");
        ret = STATEMENT_BIND_ERR;
        goto cleanup;
    }
    if ((step = sqlite3_step(stmt)) != SQLITE_DONE)
    {
        fprintf(stderr, "Could not commit to database, sqlite3 error code %d\n", step);
        ret = STATEMENT_STEP_ERR;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

#undef TERM_ARRAY_SIZE

#undef QUERY

#undef EXPRESSION_INDEX
#undef READING_INDEX
#undef DEF_TAGS_INDEX
#undef RULES_INDEX
#undef SCORE_INDEX
#undef GLOSSARY_INDEX
#undef SEQUENCE_INDEX
#undef TERM_TAGS_INDEX

#undef QUERY_DIC_ID_INDEX
#undef QUERY_EXPRESSION_INDEX
#undef QUERY_READING_INDEX
#undef QUERY_DEF_TAGS_INDEX
#undef QUERY_RULES_INDEX
#undef QUERY_SCORE_INDEX
#undef QUERY_GLOSSARY_INDEX
#undef QUERY_SEQUENCE_INDEX
#undef QUERY_TERM_TAGS_INDEX

/* End add_term defines */
/* Begin add_kanji defines */

#define KANJI_ARRAY_SIZE     6

#define QUERY   "INSERT INTO kanji_bank "\
                    "(dic_id, char, onyomi, kunyomi, tags, meanings, stats) "\
                    "VALUES (?, ?, ?, ?, ?, ?, ?);"

#define CHAR_INDEX          0
#define ONYOMI_INDEX        1
#define KUNYOMI_INDEX       2
#define TAGS_INDEX          3
#define MEANINGS_INDEX      4
#define STATS_INDEX         5

#define QUERY_DIC_ID_INDEX        1
#define QUERY_CHAR_INDEX          2
#define QUERY_ONYOMI_INDEX        3
#define QUERY_KUNYOMI_INDEX       4
#define QUERY_TAGS_INDEX          5
#define QUERY_MEANINGS_INDEX      6
#define QUERY_STATS_INDEX         7

/**
 * Add the kanji stored in the json array
 * @param db    The database to add the tag to
 * @param kanji The kanji array to add to the database
 * @param id    The id of the dictionary the tag belongs to
 * @return Error code
 */
static int add_kanji(sqlite3 *db, json_object *kanji, const sqlite3_int64 id)
{
    int           ret       = 0;
    json_object  *ret_obj   = NULL;

    const char   *character = NULL;
    const char   *onyomi    = NULL;
    const char   *kunyomi   = NULL;
    const char   *tags      = NULL;
    const char   *meanings  = NULL;
    const char   *stats     = NULL;

    sqlite3_stmt *stmt      = NULL;
    int           step      = 0;

    /* Make sure the length of the term array is correct */
    if (json_object_array_length(kanji) != KANJI_ARRAY_SIZE)
    {
#ifdef _WIN32
        fprintf(stderr, "Expected kanji array of size %u, got %llu\n",
                KANJI_ARRAY_SIZE, json_object_array_length(kanji));
#else
        fprintf(stderr, "Expected kanji array of size %u, got %lu\n",
                KANJI_ARRAY_SIZE, json_object_array_length(kanji));
#endif
        ret = KANJI_WRONG_SIZE_ERR;
        goto cleanup;
    }

    /* Get the objects to add to the database */
    if ((ret = get_obj_from_array(kanji, CHAR_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    character = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(kanji, ONYOMI_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    onyomi = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(kanji, KUNYOMI_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    kunyomi = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(kanji, TAGS_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    tags = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(kanji, MEANINGS_INDEX, json_type_array, &ret_obj)))
        goto cleanup;
    meanings = json_object_to_json_string(ret_obj);

    if ((ret = get_obj_from_array(kanji, STATS_INDEX, json_type_object, &ret_obj)))
        goto cleanup;
    stats = json_object_to_json_string(ret_obj);

    /* Add term to the database */
    if (sqlite3_prepare_v2(db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Could not prepare sqlite statement\n");
        fprintf(stderr, "Query: %s\n", QUERY);
        ret = STATEMENT_PREPARE_ERR;
        goto cleanup;
    }
    if (sqlite3_bind_int (stmt, QUERY_DIC_ID_INDEX,   id                 ) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_CHAR_INDEX,     character, -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_ONYOMI_INDEX,   onyomi,    -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_KUNYOMI_INDEX,  kunyomi,   -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_TAGS_INDEX,     tags,      -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_MEANINGS_INDEX, meanings,  -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_STATS_INDEX,    stats,     -1, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Could not bind values to sqlite statement\n");
        ret = STATEMENT_BIND_ERR;
        goto cleanup;
    }
    if ((step = sqlite3_step(stmt)) != SQLITE_DONE)
    {
        fprintf(stderr, "Could not commit to database, sqlite3 error code %d\n", step);
        ret = STATEMENT_STEP_ERR;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

#undef KANJI_ARRAY_SIZE

#undef QUERY

#undef CHAR_INDEX
#undef ONYOMI_INDEX
#undef KUNYOMI_INDEX
#undef TAGS_INDEX
#undef MEANINGS_INDEX
#undef STATS_INDEX

#undef QUERY_DIC_ID_INDEX
#undef QUERY_CHAR_INDEX
#undef QUERY_ONYOMI_INDEX
#undef QUERY_KUNYOMI_INDEX
#undef QUERY_TAGS_INDEX
#undef QUERY_MEANINGS_INDEX
#undef QUERY_STATS_INDEX

/* End add_kanji defines */
/* Begin add_meta defines */

#define META_ARRAY_SIZE     3

#define EXPRESSION_INDEX    0
#define MODE_INDEX          1
#define DATA_INDEX          2

#define QUERY_DIC_ID_INDEX          1
#define QUERY_EXPRESSION_INDEX      2
#define QUERY_MODE_INDEX            3
#define QUERY_TYPE_INDEX            4
#define QUERY_DATA_INDEX            5

/**
 * Add the metadata stored in the json array
 * @param db    The database to add the tag to
 * @param meta  The tag array to add to the database
 * @param id    The id of the dictionary the tag belongs to
 * @param query The query to execute on the database with (?, ?, ?) bindable fields
 * @return Error code
 */
static int add_meta(sqlite3 *db, json_object *meta, const sqlite3_int64 id, const char *query)
{
    int         ret         = 0;
    json_object *ret_obj    = NULL;

    const char  *exp        = NULL;
    const char  *mode       = NULL;
    const void  *data       = NULL;
    yomi_blob_t type        = 0;
    size_t      data_len    = 0;
    int64_t     data_int    = 0;
    json_bool   data_bool   = 0;
    double      data_double = 0.0;
    int         data_null   = 0;

    sqlite3_stmt *stmt      = NULL;
    int           step      = 0;

    /* Make sure the length of the metadata array is correct */
    if (json_object_array_length(meta) != META_ARRAY_SIZE)
    {
#ifdef _WIN32
        fprintf(stderr, "Expected metadata array of size %u, got %llu\n",
                META_ARRAY_SIZE, json_object_array_length(meta));
#else
        fprintf(stderr, "Expected metadata array of size %u, got %lu\n",
                META_ARRAY_SIZE, json_object_array_length(meta));
#endif
        ret = META_WRONG_SIZE_ERR;
        goto cleanup;
    }

    /* Get the objects to add to the database */
    if ((ret = get_obj_from_array(meta, EXPRESSION_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    exp = json_object_get_string(ret_obj);

    if ((ret = get_obj_from_array(meta, MODE_INDEX, json_type_string, &ret_obj)))
        goto cleanup;
    mode = json_object_get_string(ret_obj);

    /* Get the proper type from the data field of the array */
    ret_obj = json_object_array_get_idx(meta, DATA_INDEX);
    switch (json_object_get_type(ret_obj))
    {
    case json_type_array:
        data = json_object_to_json_string_length(ret_obj, JSON_C_TO_STRING_SPACED, &data_len);
        ++data_len;
        type = YOMI_BLOB_TYPE_ARRAY;
        break;
    case json_type_object:
        data = json_object_to_json_string_length(ret_obj, JSON_C_TO_STRING_SPACED, &data_len);
        ++data_len;
        type = YOMI_BLOB_TYPE_OBJECT;
        break;
    case json_type_int:
        data_int = json_object_get_int64(ret_obj);
        data = &data_int;
        data_len = sizeof(int64_t);
        type = YOMI_BLOB_TYPE_INT;
        break;
    case json_type_boolean:
        data_bool = json_object_get_boolean(ret_obj);
        data = &data_bool;
        data_len = sizeof(json_bool);
        type = YOMI_BLOB_TYPE_BOOLEAN;
        break;
    case json_type_double:
        data_double = json_object_get_double(ret_obj);
        data = &data_double;
        data_len = sizeof(double);
        type = YOMI_BLOB_TYPE_DOUBLE;
        break;
    case json_type_string:
        data = json_object_get_string(ret_obj);
        data_len = json_object_get_string_len(ret_obj) + 1;
        type = YOMI_BLOB_TYPE_STRING;
        break;
    case json_type_null:
        data_null = 1;
        type = YOMI_BLOB_TYPE_NULL;
        break;
    default:
        fprintf(stderr, "Unknown json type stored in data\n");
        ret = UNKNOWN_DATA_TYPE_ERR;
        goto cleanup;
    }

    /* Add metadata to the database */
    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Could not prepare sqlite statement\n");
        fprintf(stderr, "Query: %s\n", query);
        ret = STATEMENT_PREPARE_ERR;
        goto cleanup;
    }
    if (sqlite3_bind_int (stmt, QUERY_DIC_ID_INDEX,     id            ) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_EXPRESSION_INDEX, exp,  -1, NULL) != SQLITE_OK ||
        sqlite3_bind_text(stmt, QUERY_MODE_INDEX,       mode, -1, NULL) != SQLITE_OK ||
        sqlite3_bind_int (stmt, QUERY_TYPE_INDEX,       type)           != SQLITE_OK)
    {
        fprintf(stderr, "Could not bind values to sqlite statement\n");
        ret = STATEMENT_BIND_ERR;
        goto cleanup;
    }
    if (data_null)
    {
        if (sqlite3_bind_null(stmt, QUERY_DATA_INDEX) != SQLITE_OK)
        {
            fprintf(stderr, "Could not bind null data to statement\n");
            ret = STATEMENT_BIND_ERR;
            goto cleanup;
        }
    }
    else
    {
        if (sqlite3_bind_blob(stmt, QUERY_DATA_INDEX, data, data_len, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Could not bind data blob to statement\n");
            ret = STATEMENT_BIND_ERR;
            goto cleanup;
        }
    }
    if ((step = sqlite3_step(stmt)) != SQLITE_DONE)
    {
        fprintf(stderr, "Could not commit to database, sqlite3 error code %d\n", step);
        ret = STATEMENT_STEP_ERR;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);

    return ret;
}

#undef META_ARRAY_SIZE

#undef EXPRESSION_INDEX
#undef MODE_INDEX
#undef DATA_INDEX

#undef QUERY_DIC_ID_INDEX
#undef QUERY_EXPRESSION_INDEX
#undef QUERY_MODE_INDEX
#undef QUERY_DATA_INDEX

/* End add_meta defines */

/**
 * Add the term meta data stored in the json object
 * @param db        The database to add the metadata to
 * @param term_meta The term metadata json array to add
 * @param id        The id of the dictionary the term metadata belongs to
 * @return Error code
 */
static int add_term_meta(sqlite3 *db, json_object *term_meta, const sqlite3_int64 id)
{
    return add_meta(
        db,
        term_meta,
        id,
        "INSERT INTO term_meta_bank (dic_id, expression, mode, type, data) "
            "VALUES (?, ?, ?, ?, ?);"
    );
}

/**
 * Add the kanji meta data stored in the json object
 * @param db         The database to add the metadata to
 * @param kanji_meta The kanji metadata json array to add
 * @param id         The id of the dictionary the kanji metadata belongs to
 * @return Error code
 */
static int add_kanji_meta(sqlite3 *db, json_object *kanji_meta, const sqlite3_int64 id)
{
    return add_meta(
        db,
        kanji_meta,
        id,
        "INSERT INTO kanji_meta_bank (dic_id, expression, mode, type, data) "
            "VALUES (?, ?, ?, ?, ?);"
    );
}

/**
 * Adds a yomichan bank type to the database
 * @param dict_archive The dictionary archive holding index.json
 * @param db           The database
 * @param id           The id of the dictionary
 * @param type         The type of bank to add to the database
 * @return Error code
 */
static int add_dic_files(zip_t *dict_archive, sqlite3 *db, const sqlite3_int64 id, bank_type type)
{
    int           ret         = 0;
    const char   *file_format = NULL;
    size_t        fileno      = 1;
    json_object  *outer_arr   = NULL;
    json_object  *inner_arr   = NULL;
    char         *errmsg      = NULL;
    int (*add_item)(sqlite3 *, json_object *, const sqlite3_int64) = NULL;

    /* Set the type of files to iterate over and the function used to add them */
    switch (type)
    {
    case tag_bank:
        file_format = TAG_BANK_FORMAT;
        add_item    = add_tag;
        break;
    case term_bank:
        file_format = TERM_BANK_FORMAT;
        add_item    = add_term;
        break;
    case term_meta_bank:
        file_format = TERM_META_BANK_FORMAT;
        add_item    = add_term_meta;
        break;
    case kanji_bank:
        file_format = KANJI_BANK_FORMAT;
        add_item    = add_kanji;
        break;
    case kanji_meta_bank:
        file_format = KANJI_META_BANK_FORMAT;
        add_item    = add_kanji_meta;
        break;
    default:
        fprintf(stderr, "Unknown bank_type value %d\n", type);
        ret = UNKNOWN_BANK_TYPE_ERR;
        goto cleanup;
    }

    /* Iterate over all the files  */
    char filename[FILENAME_BUFFER_SIZE];
    snprintf(filename, FILENAME_BUFFER_SIZE, file_format, fileno++);
    filename[FILENAME_BUFFER_SIZE - 1] = '\0';
    while (zip_name_locate(dict_archive, filename, 0) != -1)
    {
        /* Parse the current tag bank into Json */
        if ((ret = get_json_obj(dict_archive, filename, &outer_arr)))
        {
            goto cleanup;
        }
        if (!json_object_is_type(outer_arr, json_type_array))
        {
            fprintf(stderr, "Returned object was not of type array\n");
            ret = JSON_WRONG_TYPE_ERR;
            goto cleanup;
        }

        /* Iterate over all the outer arrays */
        for (size_t i = 0; i < json_object_array_length(outer_arr); ++i)
        {
            /* Get the inner array which contains tag info */
            inner_arr = json_object_array_get_idx(outer_arr, i);
            if (!json_object_is_type(inner_arr, json_type_array))
            {
                fprintf(stderr, "Tag array is of the incorrect type\n");
                ret = JSON_WRONG_TYPE_ERR;
                goto cleanup;
            }

            /* Add the tag to the database */
            if ((ret = (*add_item)(db, inner_arr, id)))
            {
                fprintf(stderr, "Could not add %s\n", filename);
                goto cleanup;
            }
        }

        /* Prepare for the next iteration of the loop */
        snprintf(filename, FILENAME_BUFFER_SIZE, file_format, fileno++);
        filename[FILENAME_BUFFER_SIZE - 1] = '\0';
        json_object_put(outer_arr);
        outer_arr  = NULL;
        inner_arr = NULL;
    }

cleanup:
    json_object_put(outer_arr);
    sqlite3_free(errmsg);

    return ret;
}

#ifdef _WIN32
/**
 * Converts a UTF-8 string to an LPWSTR.
 * @param str The UTF-8 string to convert.
 * @return An LPSTR. Must be freed with free(). NULL on error.
 */
static LPWSTR utf8_to_lpwstr(const char *str)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if (len == 0)
    {
        return NULL;
    }
    LPWSTR lpwstr = malloc(sizeof(WCHAR) * len);
    if (MultiByteToWideChar(CP_UTF8, 0, str, -1, lpwstr, len) == 0)
    {
        free(lpwstr);
        return NULL;
    }
    return lpwstr;
}

/**
 * Removes a path on Windows.
 * @param path The path to remove in UTF-8 encoding.
 * @return Error code.
 */
static int remove_path(const char *path)
{
    int    ret     = 0;
    LPWSTR pszFrom = NULL;

    int len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    if (len == 0)
    {
        ret = -1;
        goto cleanup;
    }
    pszFrom = malloc(sizeof(WCHAR) * (len + 2));
    len = MultiByteToWideChar(CP_UTF8, 0, path, -1, pszFrom, len);
    if (len == 0)
    {
        ret = -2;
        goto cleanup;
    }
    pszFrom[len] = '\0';
    pszFrom[len + 1] = '\0';
    if (pszFrom[len - 1] == '\\')
    {
        pszFrom[len - 1] = '\0';
    }
    for (LPWSTR ptr = pszFrom; *ptr; ++ptr)
    {
        if (*ptr == '/')
        {
            *ptr = '\\';
        }
    }

    /* Delete the directory contents */
    SHFILEOPSTRUCTW fileop = {
        .hwnd = NULL, // no status display
        .wFunc = FO_DELETE, // delete operation
        .pFrom = pszFrom, // source file name as double null terminated string
        .pTo = NULL, // no destination needed
        .fFlags = FOF_NOCONFIRMATION | FOF_SILENT, // do not prompt the user
        .fAnyOperationsAborted = FALSE,
        .lpszProgressTitle = NULL,
        .hNameMappings = NULL,
    };
    if ((ret = SHFileOperationW(&fileop)))
    {
        goto cleanup;
    }

cleanup:
    free(pszFrom);

    return ret;
}

/**
 * Creates a path if it doesn't already exist.
 * @param path The path to create.
 * @return mkdir error code.
 */
static int make_path(const char *path)
{
    int    ret       = 0;
    LPWSTR path_copy = NULL;
    LPWSTR ptr       = NULL;

    /* Convert to LPSTR */
    path_copy = utf8_to_lpwstr(path);
    if (path_copy == NULL)
    {
        ret = -1;
        goto cleanup;
    }

    /* Seek to the first seperator */
    ptr = path_copy;
    while (*ptr && *ptr != '/')
    {
        ++ptr;
    }
    if (*ptr == '\0')
    {
        goto cleanup;
    }

    /* Create the path */
    for (ptr += 1; *ptr; ++ptr)
    {
        if (*ptr != '/')
        {
            continue;
        }

        *ptr = '\0';
        if (_wmkdir(path_copy) && errno != EEXIST)
        {
            ret = errno;
            goto cleanup;
        }
        *ptr = '/';
    }
    if (_wmkdir(path_copy) && errno != EEXIST)
    {
        ret = errno;
        goto cleanup;
    }

cleanup:
    free(path_copy);

    return ret;
}

#else
/**
 * Deletes the files pass in the path argument.
 * @param path The path of the file/folder to delete.
 * @param sbuf Unused.
 * @param type Unused.
 * @param ftwb Unused.
 * @return 0 on success, errno on failure.
 */
static int remove_path_callback(
    const char *path,
    const struct stat *sbuf __attribute__((unused)),
    int type __attribute__((unused)),
    struct FTW *ftwb __attribute__((unused)))
{
    if (remove(path))
    {
        fprintf(stderr, "Could not remove file:\n%s\n", strerror(errno));
        return errno;
    }
    return 0;
}

/**
 * Recursively deletes a path.
 * @param path The path to recursively delete.
 * @return 0 on success, errno on failure.
 */
static int remove_path(const char *path)
{
    if (nftw(path, remove_path_callback, 10, FTW_DEPTH|FTW_MOUNT|FTW_PHYS))
    {
        return errno;
    }
    return 0;
}

/**
 * Creates a path if it doesn't already exist.
 * @param path The path to create.
 * @return mkdir error code.
 */
static int make_path(const char *path)
{
    int   ret       = 0;
    char *path_copy = strdup(path);
    char *ptr       = strchr(path_copy, '/');

    if (ptr == NULL)
    {
        goto cleanup;
    }

    for (ptr += 1; *ptr; ++ptr)
    {
        if (*ptr != '/')
        {
            continue;
        }

        *ptr = '\0';
        if (mkdir(path_copy, 0755) && errno != EEXIST)
        {
            ret = errno;
            goto cleanup;
        }
        *ptr = '/';
    }
    if (mkdir(path_copy, 0755) && errno != EEXIST)
    {
        ret = errno;
        goto cleanup;
    }

cleanup:
    free(path_copy);

    return ret;
}
#endif

/**
 * Concatenates two paths and normalizes all seperators to use /.
 * @param start The begining of the path.
 * @param end   The ending of the path.
 * @return The two paths concatinated. Must be freed with free().
 */
static char *concat_paths(const char *start, const char *end)
{
    size_t  start_len = strlen(start);
    size_t  end_len   = strlen(end);
    char   *path      = malloc(start_len + end_len + 2);
    strcpy(path, start);

    /* Make sure the start path ends in a seperator */
#ifdef _WIN32
    if (start[start_len - 1] != '/' || start[start_len - 1] != '\\')
#else
    if (start[start_len - 1] != '/')
#endif
    {
        path[start_len++] = '/';
    }

    /* Copy the ending */
    strcpy(&path[start_len], end);

#ifdef _WIN32
    /* Normalize path to use forward slashes on Windows */
    for (char *ptr = path; *ptr; ++ptr)
    {
        if (*ptr == '\\')
        {
            *ptr = '/';
        }
    }
#endif

    return path;
}

#define TITLE_KEY "title"

#define REGEX_SKIP_FILE \
    "^index\\.json$|" \
    "^tag_bank_[1-9][0-9\\(\\)]*\\.json$|" \
    "^term_bank_[1-9][0-9\\(\\)]*\\.json$|" \
    "^term_meta_bank_[1-9][0-9\\(\\)]*\\.json$|" \
    "^kanji_bank_[1-9][0-9\\(\\)]*\\.json$|" \
    "^kanji_meta_bank_[1-9][0-9\\(\\)]*\\.json$"

#ifdef _WIN32
/* This is returned from SHFileOperationW when folders in the path don't exist */
#define DE_INVALIDFILES 0x7C
#endif

/**
 * Extracts resources also in the archive if they exist.
 * @param dict_archive The dictionary archive to extract resources from.
 * @param res_dir      Path to the resource directory.
 * @return Error code.
 */
static int extract_resources(zip_t *dict_archive, const char *res_dir)
{
    int          ret        = 0;
    regex_t      rt;
    regex_t     *file_regex = NULL;
    json_object *obj        = NULL;
    json_object *ret_obj    = NULL;
    const char  *dict_name  = NULL;
    char        *base_path  = NULL;
    const char *file_name   = NULL;

    /* Get the name of the dictionary */
    if ((ret = get_json_obj(dict_archive, INDEX_FILE, &obj)))
    {
        fprintf(stderr, "Failed to open index file\n");
        goto cleanup;
    }
    if ((ret = get_obj_from_obj(obj, TITLE_KEY, json_type_string, &ret_obj)))
    {
        fprintf(stderr, "Failed to get title from index object\n");
        goto cleanup;
    }
    dict_name = json_object_get_string(ret_obj);

    /* Empty the base path if it already exists */
    base_path = concat_paths(res_dir, dict_name);
    ret = remove_path(base_path);
#ifdef _WIN32
    switch (ret)
    {
    case DE_INVALIDFILES:
    case ERROR_FILE_NOT_FOUND:
        ret = 0;
        break;
    default:
        break;
    }
#else
    ret = ret && ret != ENOENT ? ret : 0;
#endif
    if (ret)
    {
        fprintf(stderr, "Could not remove base path: %s\n", base_path);
#ifdef _WIN32
        fprintf(stderr, "Error Code: %x\n", ret);
#else
        fprintf(stderr, "%s\n", strerror(ret));
#endif
        goto cleanup;
    }
    if ((ret = make_path(base_path)))
    {
        fprintf(stderr, "Could not create path: %s\n%s\n", base_path, strerror(ret));
        goto cleanup;
    }

    /* Initialize the file regex filter */
    if ((ret = regcomp(&rt, REGEX_SKIP_FILE, REG_EXTENDED)))
    {
        fprintf(stderr, "Could not compile regex\nError: %d\n", ret);
        goto cleanup;
    }
    file_regex = &rt; // if regcomp fails, regfree is UB if passed non-NULL

    /* Iterate over files */
    for (zip_int64_t i = 0; i < zip_get_num_entries(dict_archive, 0); ++i)
    {
        file_name = zip_get_name(dict_archive, i, 0);
        /* Skip unwanted files */
        switch (regexec(file_regex, file_name, 0, NULL, 0))
        {
        case REG_NOMATCH:
            break;
        case 0:
            continue;
        default:
            fprintf(stderr, "Error occurring in matching file regex\n");
            goto cleanup;
        }

        /* Check if the file is a directory */
        size_t file_name_len = strlen(file_name);
        if (file_name[file_name_len - 1] == '/')
        {
            char *path = concat_paths(base_path, file_name);
            make_path(path);
            free(path);
            continue;
        }

        /* Open file that are actual files */
        zip_file_t *zip_file = zip_fopen_index(dict_archive, i, 0);
        if (zip_file == NULL)
        {
            fprintf(stderr, "Could not open resource file in archive\n");
            goto cleanup;
        }

        /* Open the system file */
        char *file_path = concat_paths(base_path, file_name);
#ifdef _WIN32
        LPWSTR wFilePath = utf8_to_lpwstr(file_path);
        FILE *file = _wfopen(wFilePath, L"wb+");
        free(wFilePath);
        wFilePath = NULL;
#else
        FILE *file = fopen(file_path, "wb+");
#endif
        if (file == NULL)
        {
            fprintf(stderr, "Could not open file for writing\n%s\n", file_path);
            free(file_path);
            if (zip_file)
            {
                zip_fclose(zip_file);
            }
            goto cleanup;
        }
        free(file_path);
        file_path = NULL;

        /* Buffer and write the file */
        char buf[BUFSIZ];
        zip_int64_t bytes_read = 0;
        while ((bytes_read = zip_fread(zip_file, buf, sizeof(buf))))
        {
            fwrite(buf, sizeof(char), bytes_read, file);
        }
        fclose(file);
        if (zip_file)
        {
            zip_fclose(zip_file);
        }
    }

cleanup:
    if (file_regex)
    {
        regfree(file_regex);
    }
    json_object_put(obj);
    free(base_path);

    return ret;
}

#undef TITLE_KEY

#undef REGEX_SKIP_FILE

int yomi_prepare_db(const char *db_file, sqlite3 **db)
{
    int      ret          = 0;
    sqlite3 *db_loc       = NULL;
    int      prepare_code = 0;

    /* Open or create the database */
    if (sqlite3_open_v2(db_file, &db_loc, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) != SQLITE_OK)
    {
        ret = YOMI_ERR_DB;
        goto cleanup;
    }

    /* Make sure the database is setup */
    if ((prepare_code = prepare_db(db_loc)))
    {
        ret = prepare_code == DB_NEW_VERSION_ERR ? YOMI_ERR_NEWER_VERSION : YOMI_ERR_DB;
        goto cleanup;
    }

    /* Return the create database if not null */
    if (db)
    {
        *db = db_loc;
        return ret;
    }

cleanup:
    sqlite3_close_v2(db_loc);
    if (db)
        *db = NULL;

    return ret;
}

int yomi_process_dictionary(const char *dict_file, const char *db_file, const char *res_dir)
{
    int            ret          = 0;
    int            err          = 0;
    zip_t         *dict_archive = NULL;
    sqlite3       *db           = NULL;
    sqlite3_int64  id           = 0;

    /* Open dictionary archive */
    dict_archive = zip_open(dict_file, ZIP_RDONLY, &err);
    if (err)
    {
        ret = YOMI_ERR_OPENING_DIC;
        goto error;
    }

    /* Open or create the database */
    if ((ret = yomi_prepare_db(db_file, &db)))
    {
        goto error;
    }

    /* Process the index file */
    if ((ret = begin_transaction(db)))
    {
        goto error;
    }
    if (add_index(dict_archive, db, &id))
    {
        ret = YOMI_ERR_ADDING_INDEX;
        goto error;
    }

    /* Process the tag banks */
    if (add_dic_files(dict_archive, db, id, tag_bank))
    {
        ret = YOMI_ERR_ADDING_TAGS;
        goto error;
    }

    /* Process term banks */
    if (add_dic_files(dict_archive, db, id, term_bank))
    {
        ret = YOMI_ERR_ADDING_TERMS;
        goto error;
    }

    /* Process term bank metadata */
    if (add_dic_files(dict_archive, db, id, term_meta_bank))
    {
        ret = YOMI_ERR_ADDING_TERMS_META;
        goto error;
    }

    /* Process kanji banks */
    if (add_dic_files(dict_archive, db, id, kanji_bank))
    {
        ret = YOMI_ERR_ADDING_KANJI;
        goto error;
    }

    /* Process kanji bank metadata */
    if (add_dic_files(dict_archive, db, id, kanji_meta_bank))
    {
        ret = YOMI_ERR_ADDING_KANJI_META;
        goto error;
    }

    /* Extract any resources that also exist in the archive */
    if (extract_resources(dict_archive, res_dir))
    {
        ret = YOMI_ERR_EXTRACTING_RESOURCES;
        goto error;
    }

    /* Commit the transaction */
    if ((ret = commit_transaction(db)))
    {
        goto error;
    }

    zip_close(dict_archive);
    sqlite3_close_v2(db);

    return ret;

error:
    rollback_transaction(db);
    zip_close(dict_archive);
    sqlite3_close_v2(db);

    return ret;
}

int yomi_delete_dictionary(const char *dict_name, const char *db_file, const char *res_dir)
{
    int            ret  = 0;
    sqlite3       *db   = NULL;
    sqlite3_stmt  *stmt = NULL;
    int            step = 0;
    char          *path = NULL;

    /* Open or create the database */
    if ((ret = yomi_prepare_db(db_file, &db)))
    {
        goto cleanup;
    }

    /* Remove the dictionary from the index */
    if (sqlite3_prepare_v2(db, "DELETE FROM directory WHERE (title = ?);", -1, &stmt, NULL) != SQLITE_OK)
    {
        ret = YOMI_ERR_DELETE;
        goto cleanup;
    }
    if (sqlite3_bind_text(stmt, 1, dict_name, -1, NULL) != SQLITE_OK)
    {
        ret = YOMI_ERR_DELETE;
        goto cleanup;
    }
    if ((step = sqlite3_step(stmt)) != SQLITE_DONE)
    {
        ret = YOMI_ERR_DELETE;
        goto cleanup;
    }

    /* Remove any resources */
    path = concat_paths(res_dir, dict_name);
    ret = remove_path(path);
    ret = ret && ret != ENOENT ? ret : 0;
    if (ret)
    {
        ret = YOMI_ERR_REMOVING_RESOURCES;
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);
    free(path);

    return ret;
}

#define QUERY   "INSERT INTO dict_disabled " \
                    "SELECT dic_id FROM directory WHERE title = ?;"

int yomi_disable_dictionaries(const char **dict_name, size_t len, const char *db_file)
{
    int           ret    = 0;
    sqlite3      *db     = NULL;
    sqlite3_stmt *stmt   = NULL;
    char         *errmsg = NULL;
    int           step   = 0;

    /* Open or create the database */
    if ((ret = yomi_prepare_db(db_file, &db)))
    {
        goto error;
    }

    /* Begin the transaction */
    if ((ret = begin_transaction(db)))
    {
        goto error;
    }

    /* Drop all rows in the table */
    sqlite3_exec(db, "DELETE FROM dict_disabled;", NULL, NULL, &errmsg);
    if (errmsg)
    {
        fprintf(stderr, "Could not delete rows from dict_disabled table.\nError: %s", errmsg);
        sqlite3_free(errmsg);
        ret = YOMI_ERR_DELETE;
        goto error;
    }

    /* Add all disabled dictionaries */
    for (size_t i = 0; i < len; ++i)
    {
        if (sqlite3_prepare_v2(db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Could not prepare sqlite statement\n");
            fprintf(stderr, "Query: %s\n", QUERY);
            ret = STATEMENT_PREPARE_ERR;
            goto error;
        }

        if (sqlite3_bind_text(stmt, 1, dict_name[i], -1, NULL) != SQLITE_OK)
        {
            fprintf(stderr, "Could not bind values to sqlite statement\n");
            ret = STATEMENT_BIND_ERR;
            goto error;
        }

        if ((step = sqlite3_step(stmt)) != SQLITE_DONE)
        {
            fprintf(stderr, "Could not commit to database, sqlite3 error code %d\n", step);
            ret = STATEMENT_STEP_ERR;
            goto error;
        }
    }

    /* Commit the transaction */
    if ((ret = commit_transaction(db)))
    {
        goto error;
    }

    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);

    return ret;

error:
    rollback_transaction(db);
    sqlite3_finalize(stmt);
    sqlite3_close_v2(db);

    return ret;
}
