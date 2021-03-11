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

#define YOMI_DB_VERSION         1

#define YOMI_ERR_OPENING_DIC    1
#define YOMI_ERR_DB             2
#define YOMI_ERR_NEWER_VERSION  3
#define YOMI_ERR_ADDING_INDEX   4

/**
 * Process the archive in dict_file and add it the sqlite database in db_file
 * @param   dict_file   The zip archive containing the yomichan dictionary
 * @param   db_file     Path to the sqlite database
 * @return Error code
 */
int process_dictionary(const char *dict_file, const char *db_file);

#endif // YOMIDBBUILDER_H