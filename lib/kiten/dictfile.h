/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
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

#ifndef KITEN_DICTFILE_H
#define KITEN_DICTFILE_H

#include <QMap>
#include <QString>

class DictQuery;
class DictionaryPreferenceDialog;
class Entry;
class EntryList;
class KConfig;
class KConfigSkeleton;
class QStringList;
class QWidget;

/**
 * @short Abstract base class, used internally by the library for handling different types of dictionaries
 * This is a virtual class that enforces the interface between the DictionaryManager
 * class and the DictionaryManager.handler files. IMPLEMENT in combination with an
 * Entry subclass (if needed) to add a new dictionary format. Also see the addDictionary
 * method in the DictionaryManager class.
 *
 * This documentation is mostly for those who are adding a new type of dictionary to
 * kiten. This class is not exported outside of the library. */
class /* NO_EXPORT */ DictFile
{
  private:
    /**
     * You are not allowed to create a dictFile subclass without specifying the type-name
     */
    DictFile() {}
  public:
    /**
     * Use this constructor for your subclasses. Dictionary subclasses MUST specify their type
     * at creation.
     */
    explicit DictFile( const QString& dictionaryTypeName ) : m_dictionaryType( dictionaryTypeName ) {}
    /**
     * Destructor
     */
    virtual ~DictFile() {}
    /**
     * This method allows the user to test if a dictionary is the proper type for this format.
     * This process is allowed to take some time, but nonetheless you should find checking the format
     * of a few hundred entries sufficient for this.
     *
     * @param filename the name of the file, suitable for using with QFile::setFileName() */
    virtual bool validDictionaryFile( const QString &filename ) = 0;
    /**
     * Is this query relevant to this dictionary type? Usually this will return true,
     * unless the query specifies extended attributes that the dictionary does not provide.
     *
     * @param query the query to examine for relevance to this dictionary type */
    virtual bool validQuery( const DictQuery &query ) = 0;
    /**
     * This actually conducts the search on the given query. This is usually most of the work
     *
     * @param query the DictQuery that specifies what results to return
     */
    virtual EntryList *doSearch( const DictQuery &query ) = 0;
    /**
     * Load a dictionary as at system startup.
     *
     * @param file the file to open, in a format suitable for use with QFile::setFileName()
     * @param name the name of the file to open, used in various user-interface aspects.
     *             It may be related to the file parameter, but perhaps not.
     */
    virtual bool loadDictionary( const QString &file, const QString &name ) = 0;
    /**
     * Load a new dictionary. This is called with the assumption that this dictionary
     * has not been opened previously, in case you need to build an index or other activity.
     * If you do not re-implement this method, it simply calls loadDictionary().
     *
     * @param file the file to open, in a format suitable for use with QFile::setFileName()
     * @param name the name of the file to open, used in various user-interface aspects.
     *             It may be related to the file parameter, but perhaps not.
     */
    virtual bool loadNewDictionary( const QString &file, const QString &name )
            { return loadDictionary( file, name ); }
    /**
     * Return a list of the fields that can be displayed, note the following
     * should probably always be returned: --NewLine--, Word/Kanji, Meaning,
     * Reading.  This function is passed a list originally containing those
     * four items. This function is used to enumerate possible types the user
     * chooses to have displayed in the preferences dialog box.
     * This will often be a very similar list to getSearchableAttributes(),
     * but due to optional forms of spelling and other situations, it may
     * not be exactly the same. Note: The "Dictionary" option will be
     * appended to your list at the end.
     */
    virtual QStringList listDictDisplayOptions( QStringList ) const = 0 ;

    /**
     * Returns the name of the dictionary
     */
    virtual QString getName() const { return m_dictionaryName; }
    /**
     * Returns the type of files this dictFile object deals with
     */
    virtual QString getType() const { return m_dictionaryType; }
    /**
     * Returns the file that this is working with, usually used in the preferences display
     */
    virtual QString getFile() const { return m_dictionaryFile; }
    /**
     * Fetch a list of searchable attributes and their codes
     */
    virtual const QMap<QString,QString> &getSearchableAttributes() const
                                      { return m_searchableAttributes; }
  protected:
    /**
     * Name is the 'primary key' of the list of dictionaries. You will want to
     * place this into your Entry objects to identify where they came from
     * (fairly important)
     */
    QString m_dictionaryName;

    /**
     * This is mostly a placeholder, but your class will get asked what file
     * it is using, so either be sure to put something here, or override
     * getFile() and respond with something that will be sensical in a
     * dictionary selection dialog box.
     */
    QString m_dictionaryFile;

    /**
     * This MUST BE SET IN THE CONSTRUCTOR. The dictionary class occasionally
     * uses this value and it's important for it to be set at anytime after the
     * constructor is called. It also must be unique to the dictionary type. If
     * relevant, specify dictionary versions here.
     */
    QString m_dictionaryType;
    /**
     * This allows the programming user to see a list
     * of possible search types (probably through a drop down menu).
     * You may also find it useful in your dictFile implementation
     * to translate from extended attribute keys into the simpler one or two letter
     * code keys. These should take the format of:
     * (Kanji Grade => G), (Strokes => S), (Heisig Number => H)
     * for a simple example appropriate to kanji.
     */
    QMap<QString,QString> m_searchableAttributes;
};

#endif
