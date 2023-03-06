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

#ifndef YOMIDBBUILDER_H
#define YOMIDBBUILDER_H

#include <sqlite3.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define YOMI_DB_VERSION                 4
#define YOMI_DB_FORMAT_VERSION          3

#define YOMI_ERR_OPENING_DIC            1
#define YOMI_ERR_DB                     2
#define YOMI_ERR_NEWER_VERSION          3
#define YOMI_ERR_ADDING_INDEX           4
#define YOMI_ERR_ADDING_TAGS            5
#define YOMI_ERR_ADDING_TERMS           6
#define YOMI_ERR_ADDING_TERMS_META      7
#define YOMI_ERR_ADDING_KANJI           8
#define YOMI_ERR_ADDING_KANJI_META      9
#define YOMI_ERR_DELETE                 10
#define YOMI_ERR_EXTRACTING_RESOURCES   11
#define YOMI_ERR_REMOVING_RESOURCES     12

typedef enum yomi_blob_t
{
    YOMI_BLOB_TYPE_NULL     = 0,
    YOMI_BLOB_TYPE_INT      = 1,
    YOMI_BLOB_TYPE_STRING   = 2,
    YOMI_BLOB_TYPE_DOUBLE   = 3,
    YOMI_BLOB_TYPE_OBJECT   = 4,
    YOMI_BLOB_TYPE_ARRAY    = 5,
    YOMI_BLOB_TYPE_BOOLEAN  = 6,
} yomi_blob_t;

/**
 * Prepare a dictionary database if one doesn't already exist
 * @param      db_file The location of the database file
 * @param[out] db      A pointer to the database to set. Safe if NULL.
 * @return Error code
 */
int yomi_prepare_db(const char *db_file, sqlite3 **db);

/**
 * Process the archive in dict_file and add it the sqlite database in db_file
 * @param dict_file The zip archive containing the yomichan dictionary
 * @param db_file   Path to the sqlite database
 * @param res_dir   The directory additional dictionary resources should be
 *                  stored in. Must already exist, will not be created.
 * @return Error code
 */
int yomi_process_dictionary(const char *dict_file, const char *db_file, const char *res_dir);

/**
 * Remove a dictionary from a database if it exists
 * @param dict_name Name of the dictionary to remove
 * @param db_file   The location of the database file
 * @param res_dir   The directory additional dictionary resources are stored in.
 *                  Must already exist, will not be created.
 * @return Error code
 */
int yomi_delete_dictionary(const char *dict_name, const char *db_file, const char *res_dir);

/**
 * Disables all the named dictionaries.
 * @param dict_name An array of dictionary names
 * @param len       The length of the dictionary array
 * @param db_file   The location of the database file
 * @return Error code
 */
int yomi_disable_dictionaries(const char **dict_name, size_t len, const char *db_file);

#ifdef __cplusplus
}
#endif

#endif // YOMIDBBUILDER_H
