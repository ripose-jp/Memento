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

#define INDEX_FILE        "index.json"
#define TAG_FILE_FORMAT   "tag_bank_%u.json"
#define TERM_BANK_FORMAT  "term_bank_%u.json"

#define STAT_ERR              -1
#define INVALID_SIZE_ERR      -2
#define MALLOC_FAILURE_ERR    -3
#define ZIP_FILE_OPEN_ERR     -4
#define ZIP_FILE_READ_ERR     -5
#define JSON_OPEN_ERR         -6
#define JSON_WRONG_TYPE_ERR   -7
#define JSON_MISSING_KEY_ERR  -8
#define STATEMENT_PREPARE_ERR -9
#define STATEMENT_BIND_ERR    -10
#define STATEMENT_COMMIT_ERR  -11

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
    char    **glossary;
    int     sequence;
    char    **term_tags;
} typedef yomi_term;

/**
 * Parse the file in the archive and returns its object representation
 * @param       archive  Archive containing the file
 * @param       filename Name of the file to get the size of
 * @param[out]  obj      The Json object
 * @return Error code
 */
static int get_json_obj(zip_t *archive, const char *filename, json_object **obj)
{
    int             ret         = 0;
    struct zip_stat st;
    char            *contents   = NULL;
    zip_file        *file       = NULL;
    zip_int64_t     bytes_read  = 0;

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

#define QUERY "INSERT INTO \"index\" (\"title\", \"format\", \"revision\", \"sequenced\") VALUES (?, ?, ?, ?);"
#define TITLE_INDEX     1
#define FORMAT_INDEX    2
#define REV_INDEX       3
#define SEQ_INDEX       4

/**
 * Adds the yomichan index.json file to the database
 * @param      dict_archive The dictionary archive holding index.json
 * @param      db           The database
 * @param[out] name         The name of the dictionary
 * @return Error code
 */
static int add_index(zip_t *dict_archive, sqlite3 *db, char **name)
{
    int          ret        = 0;
    json_object  *index_obj = NULL,
                 *ret_obj   = NULL;
    const char   *title     = NULL;
    size_t       title_len  = 0;
    int32_t      format     = 0;
    const char   *revision  = NULL;
    json_bool    sequenced  = 0;
    sqlite3_stmt *stmt      = NULL;
    int          step       = 0;

    /* Get the index object from the index file */
    if (get_json_obj(dict_archive, INDEX_FILE, &index_obj))
    {
        ret = JSON_OPEN_ERR;
        goto cleanup;
    }
    if (!json_object_is_type(index_obj, json_type_object))
    {
        fprintf(stderr, "Returned index json was not an object\n");
        ret = JSON_WRONG_TYPE_ERR;
        goto cleanup;
    }

    /* Get the fields from the json object */
    if (get_obj_from_obj(index_obj, TITLE_KEY, json_type_string, &ret_obj))
        goto cleanup;
    title = json_object_get_string(ret_obj);
    title_len = json_object_get_string_len(ret_obj);

    if (get_obj_from_obj(index_obj, FORMAT_KEY, json_type_int, &ret_obj))
        goto cleanup;
    format = json_object_get_int(ret_obj);

    if (get_obj_from_obj(index_obj, REV_KEY, json_type_string, &ret_obj))
        goto cleanup;
    revision = json_object_get_string(ret_obj);

    if (get_obj_from_obj(index_obj, SEQ_KEY, json_type_boolean, &ret_obj))
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
    if (sqlite3_bind_text(stmt, TITLE_INDEX, title, -1, NULL)  != SQLITE_OK ||
        sqlite3_bind_int(stmt, FORMAT_INDEX, format)           != SQLITE_OK ||
        sqlite3_bind_text(stmt, REV_INDEX, revision, -1, NULL) != SQLITE_OK ||
        sqlite3_bind_int(stmt, SEQ_INDEX, sequenced)           != SQLITE_OK)
    {
        fprintf(stderr, "Could not bind values to sqlite statement\n");
        ret = STATEMENT_BIND_ERR;
        goto cleanup;
    }
    step = sqlite3_step(stmt);
    if (step != SQLITE_OK || step != SQLITE_ROW || step != SQLITE_DONE)
    {
        fprintf(stderr, "Could not commit to database, sqlite3 error code %d\n", step);
        ret = STATEMENT_COMMIT_ERR;
        goto cleanup;
    }

    /* Copy the title into name */
    *name = (char *)malloc(sizeof(char) * (title_len + 1));
    strncpy(*name, title, title_len);
    (*name)[title_len] = '\0';

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
 * Process the archive in dict_file and add it the sqlite database in db_file
 * @param   dict_file   The zip archive containing the yomichan dictionary
 * @param   db_file     Path to the sqlite database
 * @return Error code
 */
int process_dictionary(const char *dict_file, const char *db_file)
{
    int     ret             = 0;
    zip_t   *dict_archive   = NULL;
    sqlite3 *db             = NULL;
    char    *name           = NULL;

    /* Open dictionary archive */
    int err = 0;
    dict_archive = zip_open(dict_file, ZIP_RDONLY, &err);
    if (err)
    {
        ret = YOMI_ERR_OPENING_DIC;
        goto cleanup;
    }

    /* Open or create the database */
    if (sqlite3_open_v2(db_file, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL))
    {
        ret = YOMI_ERR_OPENING_DB;
        goto cleanup;
    }

    /* Process the index file */
    if (add_index(dict_archive, db, &name))
    {
        ret = YOMI_ERR_ADDING_INDEX;
        goto cleanup;
    }

cleanup:
    zip_close(dict_archive);
    sqlite3_close_v2(db);
    free(name);

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