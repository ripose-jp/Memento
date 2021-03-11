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

#include <json-c/json.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

#define IS_STEP_ERR(x) ((x) != SQLITE_OK && (x) != SQLITE_ROW && (x) != SQLITE_DONE)

#define INDEX_FILE              "index.json"
#define TAG_BANK_FORMAT         "tag_bank_%u.json"
#define TERM_BANK_FORMAT        "term_bank_%u.json"
#define TERM_META_BANK_FORMAT   "term_meta_bank_%u.json"
#define TERM_BANK_FORMAT        "kanji_bank_%u.json"
#define TERM_META_BANK_FORMAT   "kanji_meta_bank_%u.json"

#define FILENAME_BUFFER_SIZE  256

#define STAT_ERR              -1
#define INVALID_SIZE_ERR      -2
#define MALLOC_FAILURE_ERR    -3
#define ZIP_FILE_OPEN_ERR     -4
#define ZIP_FILE_READ_ERR     -5
#define JSON_WRONG_TYPE_ERR   -7
#define JSON_MISSING_KEY_ERR  -8
#define STATEMENT_PREPARE_ERR -9
#define STATEMENT_BIND_ERR    -10
#define STATEMENT_STEP_ERR    -11
#define DB_NEW_VERSION_ERR    -12
#define CREATE_DB_ERR         -13
#define DB_TABLE_DROP_ERR     -14
#define DB_CREATE_TABLE_ERR   -15
#define TAG_WRONG_SIZE_ERR    -16

struct yomi_index
{
    char    *title;
    int     format;
    char    *revision;
    int     sequenced;
} typedef yomi_index;

struct yomi_tag
{
    char    *name;
    char    *category;
    int     order;
    char    *notes;
    int     score;
} typedef yomi_tag;

struct yomi_meta
{
    char    *expression;
    char    *mode;
    char    *data;
} typedef yomi_meta;

struct yomi_term
{
    char    *expression;
    char    *reading;
    char    **definition_tags;
    char    **rules;
    int     score;
    char    *glossary;
    int     sequence;
    char    **term_tags;
} typedef yomi_term;

/**
 * Drops all the tables provided in argv
 * @param   db   The database to drop tables from
 * @param   argc The number of values in the next arguement
 * @param   argv The array of table names to drop
 * @return Error code
 */
static int drop_all_tables_callback(void *db, int argc, char **argv, char **)
{
    for (char **ptr = argv; ptr < &argv[argc]; ++ptr)
    {
        char *drop_query = sqlite3_mprintf("DROP TABLE IF EXISTS %Q;", *ptr),
             *errmsg     = NULL;
        
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
 * @param   version The current reported version of the database
 * @return Error code
 */
static int create_db(sqlite3 *db, const int version)
{
    int   ret    = 0;
    char *errmsg = NULL,
         *pragma = NULL;

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
        "CREATE TABLE index ("
            "dic_id     INTEGER     PRIMARY KEY AUTOINCREMENT,"
            "title      TEXT        NOT NULL UNIQUE,"
            "format     INTEGER     NOT NULL,"
            "revision   TEXT        NOT NULL,"
            "sequenced  INTEGER     NOT NULL"   // Boolean
        ");"

        "CREATE TABLE tag_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "name       TEXT        NOT NULL,"
            "category   TEXT        NOT NULL,"
            "order      INTEGER     NOT NULL,"
            "notes      TEXT        NOT NULL,"
            "score      INTEGER     NOT NULL,"
            "PRIMARY KEY(dic_id, name)"
        ");"
        "CREATE INDEX idx_tag_bank_name     ON term_bank(dic_id, name);"

        "CREATE TABLE term_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "expression TEXT        NOT NULL,"
            "reading    TEXT        NOT NULL,"
            "def_tags   TEXT        NOT NULL,"  // Space seperated list
            "rules      TEXT        NOT NULL,"  // Space seperated list
            "score      INTEGER     NOT NULL,"
            "glossary   TEXT        NOT NULL,"  // Json array
            "sequence   INTEGER     NOT NULL,"
            "term_tags  TEXT        NOT NULL"   // Space seperated list
        ");"
        "CREATE INDEX idx_term_bank_exp     ON term_bank(expression);"
        "CREATE INDEX idx_term_bank_reading ON term_bank(reading);"

        "CREATE TABLE term_meta_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "expression TEXT        NOT NULL,"
            "mode       TEXT        NOT NULL,"
            "data       BLOB        NOT NULL,"   // Data defined by mode
            "PRIMARY KEY(dic_id, expression)"
        ");"
        "CREATE INDEX idx_term_meta_exp     ON term_bank(expression);"

        "CREATE TABLE kanji_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "char       TEXT        NOT NULL,"
            "onyomi     TEXT        NOT NULL,"  // Space seperated list
            "kunyomi    TEXT        NOT NULL,"  // Space seperated list
            "tags       TEXT        NOT NULL,"  // Space seperated list
            "meanings   TEXT        NOT NULL,"  // Json array
            "stats      TEXT        NOT NULL"   // Json object
        ");"
        "CREATE INDEX idx_kanji_bank_char   ON term_bank(char);"

        "CREATE TABLE kanji_meta_bank ("
            "dic_id     INTEGER     NOT NULL,"
            "expression TEXT        NOT NULL,"
            "mode       TEXT        NOT NULL,"
            "data       BLOB        NOT NULL,"   // Data defined by mode
            "PRIMARY KEY(dic_id, expression)"
        ");"
        "CREATE INDEX idx_kanji_meta_exp    ON term_bank(expression);",
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

/**
 * Create the tables in the database if they do not already exist
 * @param   db The database to add tables to
 * @return Error code
 */
static int prepare_db(sqlite3 *db)
{
    int           ret          = 0;
    sqlite3_stmt *stmt         = NULL;
    int           step         = 0,
                  user_version = 0;

    /* Check if the schema is an empty file */
    if (sqlite3_prepare_v2(db, "PRAGMA user_version;", -1, &stmt, NULL) != SQLITE_OK)
    {
        ret = STATEMENT_PREPARE_ERR;
        goto cleanup;
    }
    step = sqlite3_step(stmt);
    if (IS_STEP_ERR(step))
    {
        fprintf(stderr, "Could not execute user_version check\n");
        ret = STATEMENT_STEP_ERR;
        goto cleanup;
    }
    user_version = sqlite3_column_int(stmt, 0);
    if (user_version > YOMI_DB_VERSION)
    {
        fprintf(stderr, "Expected user_version %d, got newer version %d\n", YOMI_DB_VERSION, user_version);
        ret = DB_NEW_VERSION_ERR;
        goto cleanup;
    }
    else if (user_version < YOMI_DB_VERSION && (ret = create_db(db, user_version)))
    {
        fprintf(stderr, "Could not update the database, reported version %d\n", user_version);
        goto cleanup;
    }

cleanup:
    sqlite3_finalize(stmt);

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
    zip_file        *file       = NULL;
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
    if (bytes_read == -1 || bytes_read != st.size)
    {
        fprintf(stderr, "Did not read expected number of bytes in %s\n", filename);
        fprintf(stderr, "Expected bytes: %dl\tActual bytes read %dl\n", st.size, bytes_read);
        ret = ZIP_FILE_READ_ERR;
        goto cleanup;
    }
    contents[st.size] = '\0';

    /* Parse the contents into a json object */
    *obj = json_tokener_parse(contents);

cleanup:
    free(contents);
    zip_fclose(file);

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
static int get_obj_from_obj(const json_object *parent, const char *key,
                            json_type type, json_object **obj)
{
    if (!json_object_object_get_ex(parent, key, obj))
    {
        fprintf(stderr, "Could not get \"%s\" key from the object\n", key);
        return JSON_MISSING_KEY_ERR;
    }
    if (!json_object_is_type(*obj, type))
    {
        fprintf(stderr, "Key \"%s\" is not of type %s\n", key, json_type_to_name(type));
        return JSON_WRONG_TYPE_ERR;
    }

    return 0;
}

#define TITLE_KEY     "title"
#define FORMAT_KEY    "format"
#define REV_KEY       "revision"
#define SEQ_KEY       "sequenced"

#define QUERY "INSERT INTO index (title, format, revision, sequenced) VALUES (?, ?, ?, ?);"
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
    json_object  *index_obj = NULL,
                 *ret_obj   = NULL;
    const char   *title     = NULL;
    size_t        title_len = 0;
    int32_t       format    = 0;
    const char   *revision  = NULL;
    json_bool     sequenced = 0;
    sqlite3_stmt *stmt      = NULL;
    int           step      = 0;

    /* Get the index object from the index file */
    if (ret = get_json_obj(dict_archive, INDEX_FILE, &index_obj))
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
    if (ret = get_obj_from_obj(index_obj, TITLE_KEY, json_type_string, &ret_obj))
        goto cleanup;
    title     = json_object_get_string(ret_obj);
    title_len = json_object_get_string_len(ret_obj);

    if (ret = get_obj_from_obj(index_obj, FORMAT_KEY, json_type_int, &ret_obj))
        goto cleanup;
    format = json_object_get_int(ret_obj);

    if (ret = get_obj_from_obj(index_obj, REV_KEY, json_type_string, &ret_obj))
        goto cleanup;
    revision = json_object_get_string(ret_obj);

    if (ret = get_obj_from_obj(index_obj, SEQ_KEY, json_type_boolean, &ret_obj))
        goto cleanup;
    sequenced = json_object_get_boolean(ret_obj);

    /* Put dictionary metadata into the index table */
    if (sqlite3_prepare_v2(db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Could not prepare sqlite statement\n");
        fprintf(stderr, "Query: \n", QUERY);
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
    step = sqlite3_step(stmt);
    if (IS_STEP_ERR(step))
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
        fprintf(stderr, "Expected index %ul to be of type %s\n", idx, json_type_to_name(type));
        *ret_obj = NULL;
        return JSON_WRONG_TYPE_ERR;
    }
    return 0;
}

#define TAG_ARRAY_SIZE  5

#define QUERY   "INSERT INTO tag_bank (dic_id, name, category, order, notes, score) "\
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
 * Add the tag stored in the json array, tag
 * @param db  The database to add the tag to
 * @param tag The tag array to add to the database
 * @param id  The id of the dictionary the tag belongs to
 * @return Error code
 */
static int add_tag(sqlite3 *db, json_object *tag, const sqlite3_int64 id)
{
    int           ret      = 0;
    json_object  *ret_obj  = NULL;
    const char   *name     = NULL,
                 *category = NULL;
    int           order    = 0;
    const char   *notes    = NULL;
    int           score    = 0;
    sqlite3_stmt *stmt     = NULL;
    int           step     = 0;

    /* Make sure the length of the tag array is correct */
    if (json_object_array_length(tag) != TAG_ARRAY_SIZE)
    {
        fprintf(stderr, "Expected tag array of size %ul, got %ul\n", 
                TAG_ARRAY_SIZE, json_object_array_length(tag));
        ret = TAG_WRONG_SIZE_ERR;
        goto cleanup;
    }

    /* Get the objects to add to the database */
    if (ret = get_obj_from_array(tag, NAME_INDEX, json_type_string, &ret_obj))
        goto cleanup;
    name = json_object_get_string(ret_obj);

    if (ret = get_obj_from_array(tag, CATEGORY_INDEX, json_type_string, &ret_obj))
        goto cleanup;
    category = json_object_get_string(ret_obj);

    if (ret = get_obj_from_array(tag, ORDER_INDEX, json_type_int, &ret_obj))
        goto cleanup;
    order = json_object_get_int(ret_obj);

    if (ret = get_obj_from_array(tag, NOTES_INDEX, json_type_string, &ret_obj))
        goto cleanup;
    notes = json_object_get_string(ret_obj);

    if (ret = get_obj_from_array(tag, SCORE_INDEX, json_type_int, &ret_obj))
        goto cleanup;
    score = json_object_get_int(ret_obj);

    /* Add tag to the database */
    if (sqlite3_prepare_v2(db, QUERY, -1, &stmt, NULL) != SQLITE_OK)
    {
        fprintf(stderr, "Could not prepare sqlite statement\n");
        fprintf(stderr, "Query: \n", QUERY);
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
    step = sqlite3_step(stmt);
    if (IS_STEP_ERR(step))
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

/**
 * Add the tag files to the database
 * @param dict_archive The dictionary archive holding index.json
 * @param db           The database
 * @param id           The id of the dictionary
 * @return Error code
 */
static int add_tags(zip_t *dict_archive, sqlite3 *db, const sqlite3_int64 id)
{
    int           ret       = 0;
    size_t        fileno    = 1;
    json_object  *tags_arr  = NULL;
    json_object  *inner_arr = NULL;

    /* Iterate over all the files  */
    char filename[FILENAME_BUFFER_SIZE];
    snprintf(filename, FILENAME_BUFFER_SIZE, TAG_BANK_FORMAT, fileno++);
    filename[FILENAME_BUFFER_SIZE - 1] = '\0';
    while (zip_name_locate(dict_archive, filename, 0) != -1)
    {
        /* Parse the current tag bank into Json */
        if (ret = get_json_obj(dict_archive, filename, &tags_arr))
        {
            goto cleanup;
        }
        if (!json_object_is_type(tags_arr, json_type_array))
        {
            fprintf(stderr, "Returned object was not of type array\n");
            ret = JSON_WRONG_TYPE_ERR;
            goto cleanup;
        }

        /* Iterate over all the outer arrays */
        for (size_t i = 0; i < json_object_array_length(tags_arr); ++i)
        {
            /* Get the inner array which contains tag info */
            inner_arr = json_object_array_get_idx(tags_arr, i);
            if (!json_object_is_type(inner_arr, json_type_array))
            {
                fprintf(stderr, "Tag array is of the incorrect type\n");
                ret = JSON_WRONG_TYPE_ERR;
                goto cleanup;
            }

            /* Add the tag to the database */
            if (ret = add_tag(db, inner_arr, id))
            {
                fprintf(stderr, "Could not add tag at index %ul\n", i);
                goto cleanup;
            }
        }

        /* Prepare for the next iteration of the loop */
        snprintf(filename, FILENAME_BUFFER_SIZE, TAG_BANK_FORMAT, fileno++);
        filename[FILENAME_BUFFER_SIZE - 1] = '\0';
        tags_arr  = NULL;
        inner_arr = NULL;
    }

cleanup:
    json_object_put(tags_arr);

    return ret;
}

/**
 * Process the archive in dict_file and add it the sqlite database in db_file
 * @param   dict_file   The zip archive containing the yomichan dictionary
 * @param   db_file     Path to the sqlite database
 * @return Error code
 */
int process_dictionary(const char *dict_file, const char *db_file)
{
    int            ret          = 0,
                   err          = 0;
    zip_t         *dict_archive = NULL;
    sqlite3       *db           = NULL;
    sqlite3_int64  id           = 0;
    int            prepare_code = 0;

    /* Open dictionary archive */
    dict_archive = zip_open(dict_file, ZIP_RDONLY, &err);
    if (err)
    {
        ret = YOMI_ERR_OPENING_DIC;
        goto cleanup;
    }

    /* Open or create the database */
    if (sqlite3_open_v2(db_file, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) != SQLITE_OK)
    {
        ret = YOMI_ERR_DB;
        goto cleanup;
    }

    /* Make sure the database is setup */
    if (prepare_code = prepare_db(db))
    {
        ret = prepare_code == DB_NEW_VERSION_ERR ? YOMI_ERR_NEWER_VERSION : YOMI_ERR_DB;
        goto cleanup;
    }

    /* Process the index file */
    if (add_index(dict_archive, db, &id))
    {
        ret = YOMI_ERR_ADDING_INDEX;
        goto cleanup;
    }

    /* Process the tag banks */
    if (add_tags(dict_archive, db, id))
    {
        ret = YOMI_ERR_ADDING_TAGS;
        goto cleanup;
    }

cleanup:
    zip_close(dict_archive);
    sqlite3_close_v2(db);

    return ret;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Invalid arguement count %d\n", argc);
        printf("Usage: <dictionary> <database>\n");
        return -1;
    }

    return process_dictionary(argv[1], argv[2]);
}