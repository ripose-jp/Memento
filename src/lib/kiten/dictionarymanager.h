/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#ifndef KITEN_DICTIONARYMANAGER_H
#define KITEN_DICTIONARYMANAGER_H

#include <QMap>
#include <QPair>

class DictFile;
class DictQuery;
class DictionaryPreferenceDialog;
class EntryList;
class KConfig;
class KConfigSkeleton;
class QString;
class QStringList;
class QWidget;

/**
 * @short The DictionaryManager class is the fundamental dictionary management class.
 * All interfaces with the rest of the programs using the various dictionaries will
 * work through this "interface class" to keep the formatting and other such
 * nasty details away from programs and sections which just want to use the
 * dictionary without bothering with the internal formatting details. As a
 * general rule, call this class with a DictQuery to get a list of
 * entries as the result.
 *
 * The idea is that the interfaces need to know how to load a query, pass the
 * query to dictionary.	DictionaryManager will return to them an EntryList object,
 * each Entry knows how to display itself (via the magic of C++ polymorphism).
 * There are some setup and preference handling methods which complicate
 * things, but generally speaking this is the way this should work.
 *
 * @author Joseph Kerian <jkerian@gmail.com>
 */

class DictionaryManager
{
  public:
    /**
     * Basic constructor
     */
    DictionaryManager();
    /**
     * Basic destructor
     */
    virtual ~DictionaryManager();

    /**
     * Open a specified dictionary, and load it under this manager's control
     *
     * @param file the filename, suitable for using with QFile::setFileName()
     * @param name the name of the file, which may be related to file, but perhaps not,
     *             for all future dealings with this file, this name will be the key value
     * @param type the known dictionary type of this file
     */
    bool addDictionary( const QString &file, const QString &name, const QString &type );
    /**
     * Removes all previously loaded dictionaries (if any).
     */
    void removeAllDictionaries();
    /**
     * Close a dictionary by name
     *
     * @param name the name of the dictionary file, as given in addDictionary
     */
    bool removeDictionary( const QString &name );
    /**
     * List names of each open dictionary
     */
    QStringList listDictionaries() const;
    /**
     * Returns type and file for an open dictionary of a given
     *
     * @param name the name of the dictionary whose information we are looking for
     */
    QPair<QString, QString> listDictionaryInfo( const QString &name ) const;
    /**
     * Lists all dictionaries of a given type (Convenient for preference dialogs)
     *
     * @param type the type of dictionaries to list
     */
    QStringList listDictionariesOfType( const QString &type ) const;
    /**
     * This is the main search routine that most of kiten should use
     *
     * @param query the DictQuery object describing the search to conduct
     */
    EntryList *doSearch( const DictQuery &query ) const;
    /**
     * A simple method for searching inside of a given set of results
     *
     * @param query the new query that will pare down our results list, there is no requirement that
     *              this query includes the query that generated the EntryList, the results are calculated
     *              only out of the second parameter
     * @param list the list of results to search for the above query in
     */
    EntryList *doSearchInList( const DictQuery &query, const EntryList *list ) const;
    /**
     * Get a list of all supported dictionary types. Useful for preference code
     */
    static QStringList listDictFileTypes();
    /**
     * Given a config and parent widget, return a mapping from dictionary types to preference dialogs.
     * If a particular dictionary type does not provide a preference dialog, it will not be included in this list,
     * so occasionally keys(returnvalue) != listDictFileTypes()
     *
     * @param config the config skeleton
     * @param parent the parent widget, as per the normal Qt widget system
     */
    static QMap<QString,DictionaryPreferenceDialog*>
            generatePreferenceDialogs( KConfigSkeleton *config, QWidget *parent = nullptr );
    /**
     * Compiles a list of all fields beyond the basic three (word/pronunciation/meaning) that all dictionary
     * types support. This can be used to generate a preference dialog, or provide more direct references.
     * The return value is "full name of the field" => "abbreviation usable in search string"
     */
    static QMap<QString,QString> generateExtendedFieldsList();

    /**
     * Load general settings
     */
    void loadSettings( const KConfig &config );

  private:
    /**
     * Static method, used to create the polymorphic dictFile object. Do not use externally.
     * If you are adding a new dictionary type, see the instructions in the code.
     */
    static DictFile *makeDictFile( const QString &type );
    class Private;
    Private* const d;
};

#endif
