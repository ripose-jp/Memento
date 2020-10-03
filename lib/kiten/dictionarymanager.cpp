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

#include "dictionarymanager.h"

#include "dictfile.h"
#include "dictionarypreferencedialog.h"
#include "dictquery.h"
#include "entry.h"
#include "entrylist.h"
#include "kitenmacros.h"

#include <QDebug>
#include <QString>

/* Includes to handle various types of dictionaries
IMPORTANT: To add a dictionary type, add the header file here and add it to the
 if statement under addDictionary() */
#include "DictEdict/dictfileedict.h"
#include "DictKanjidic/dictfilekanjidic.h"

class DictionaryManager::Private
{
  public:
    /**
     * List of dictionaries, indexed by name
     */
    QHash<QString,DictFile*> dictManagers;
};

#if 0
class debug_entry : public Entry
{
  public:
    debug_entry(QString word) : Entry( QString( "libkiten" ), word
                              , QStringList(), QStringList() ), count( counter++ )
                              { }
    virtual Entry * clone() const { return new debug_entry( *this ); }
    virtual bool loadEntry( const QString& ) { return false; }
    virtual QString dumpEntry() const { return ""; }
    virtual bool sort( const debug_entry &that, const QStringList &dicts,
                    const QStringList &fields )
                    { return this->count < that.count; }

    int count;
    static int counter;
};
int debug_entry::counter = 0;
#endif

/**
 * The constructor. Set autodelete on our dictionary list
 */
DictionaryManager::DictionaryManager()
: d( new Private )
{
}

/**
 * Delete everything in our hash
 */
DictionaryManager::~DictionaryManager()
{
  {
    QMutableHashIterator<QString, DictFile*> it( d->dictManagers );
    while( it.hasNext() )
    {
      it.next();
      delete it.value();
      it.remove();
    }
  }

  delete d;
}

/**
 * Given a named Dict file/name/type... create and add the object if it
 * seems to work properly on creation.
 */
bool DictionaryManager::addDictionary( const QString &file
                                     , const QString &name
                                     , const QString &type )
{
  if( d->dictManagers.contains( name ) ) //This name already exists in the list!
  {
    return false;
  }

  DictFile *newDict = makeDictFile( type );
  if( newDict == nullptr )
  {
    return false;
  }

  if( ! newDict->loadDictionary( file, name ) )
  {
    qDebug() << "Dictionary load FAILED: " << newDict->getName();
    delete newDict;
    return false;
  }

  qDebug() << "Dictionary Loaded : " << newDict->getName();
  d->dictManagers.insert( name, newDict );
  return true;
}

/**
 * Examine the DictQuery and farm out the search to the specialized dict
 * managers. Note that a global search limit will probably be implemented
 * either here or in the DictFile implementations... probably both
 *
 * @param query the query, see DictQuery documentation
 */
EntryList *DictionaryManager::doSearch( const DictQuery &query ) const
{
  EntryList *ret = new EntryList();
  #if 0
  if( query.getMeaning() == "(libkiten)" )
  {
    ret->append( new debug_entry( "Summary of libkiten data" ) );
    foreach( const QString &dict, listDictionaries() )
    {
      ret->append( new debug_entry( dict ) );
    }
    return ret;
  }
  #endif

  // There are two basic modes.... one in which the query
  // specifies the dictionary list, one in which it does not
  QStringList dictsFromQuery = query.getDictionaries();
  if( dictsFromQuery.isEmpty() )
  {
    // None specified, search all
    foreach( DictFile *it, d->dictManagers )
    {
      qDebug() << "Searching in " << it->getName() << "dictionary.";
      EntryList *temp = it->doSearch( query );
      if( temp )
      {
        ret->appendList( temp );
      }
      delete temp;
    }
  }
  else
  {
    foreach( const QString &target, dictsFromQuery )
    {
      DictFile *newestFound = d->dictManagers.find( target ).value();
      if( newestFound != nullptr )
      {
        EntryList *temp = newestFound->doSearch( query );
        if( temp )
        {
          ret->appendList( temp );
        }
        delete temp;
      }
    }
  }

  ret->setQuery( query ); //Store the query for later use.
  qDebug() << "From query: '" << query.toString() << "' Found " << ret->count() << " results";
  qDebug() << "Incoming match type: " << query.getMatchType() << " Outgoing: " << ret->getQuery().getMatchType();
  return ret;
}

/**
 * For this case, we let polymorphism do most of the work. We assume that the user wants
 * to pare down the results, so we let the individual entry matching methods run over the
 * new query and accept (and copy) any of those that pass.
 */
EntryList *DictionaryManager::doSearchInList( const DictQuery &query, const EntryList *list ) const
{
  EntryList *ret = new EntryList();

  foreach( Entry* it, *list )
  {
    if( it->matchesQuery( query ) )
    {
      Entry *x = it->clone();
      ret->append( x );
    }
  }

  ret->setQuery( query + list->getQuery() );
  return ret;
}

QMap<QString, QString> DictionaryManager::generateExtendedFieldsList()
{
  QMap<QString,QString> result;
  QStringList dictTypes = listDictFileTypes();
  foreach( const QString &dictType, dictTypes )
  {
    DictFile *tempDictFile = makeDictFile( dictType );
    QMap<QString,QString> tempList = tempDictFile->getSearchableAttributes();
    QMap<QString,QString>::const_iterator it = tempList.constBegin();
    while( it != tempList.constEnd() )
    {
      if( ! result.contains( it.key() ) )
      {
        result.insert( it.key(), it.value() );
      }
      ++it;
    }
    delete tempDictFile;
  }

  return result;
}

QMap<QString,DictionaryPreferenceDialog*>
DictionaryManager::generatePreferenceDialogs( KConfigSkeleton *config, QWidget *parent )
{
  QMap<QString,DictionaryPreferenceDialog*> result;
  QStringList dictTypes = listDictFileTypes();
  foreach( const QString &dictType, dictTypes )
  {
    DictFile *tempDictFile = makeDictFile( dictType );
    DictionaryPreferenceDialog *newDialog = tempDictFile->preferencesWidget( config, parent );

    if( newDialog == nullptr ) continue;
    result.insert( dictType, newDialog );
    delete tempDictFile;
  }

  return result;
}

/**
 * Return a list of the dictionaries by their name (our key)
 * Note that this dictionary name does not necessarily have to have anything
 * to do with the actual dictionary name...
 */
QStringList DictionaryManager::listDictionaries() const
{
  QStringList ret;
  foreach( DictFile *it, d->dictManagers )
  {
    ret.append( it->getName() );
  }

  return ret;
}

/**
 * IMPORTANT: To add a dictionary type, you have to manually add the creation
 * step here, the prev method, and \#include your header file above. If you have
 * fully implemented the interface in DictionaryManager.h, It should simply work.
 */
QStringList DictionaryManager::listDictFileTypes()
{
  QStringList list;
  list.append( EDICT );
  list.append( KANJIDIC );
  
  //Add your dictionary type here!

  return list;
}

/**
 * Return the dictionary type and file used by a named dictionary.
 * returns a pair of empty QStrings if you specify an invalid name
 *
 * @param name the name of the dictionary, as given in the addDictionary method
 */
QPair<QString, QString> DictionaryManager::listDictionaryInfo( const QString &name ) const
{
  if( ! d->dictManagers.contains( name ) ) //This name not in list!
  {
    return qMakePair( QString(), QString() );
  }

  return qMakePair( d->dictManagers[ name ]->getName(), d->dictManagers[ name ]->getFile() );
}

/**
 * Return a list of the names of each dictionary of a given type.
 *
 * @param type the type of the dictionary we want a list of
 */
QStringList DictionaryManager::listDictionariesOfType( const QString &type ) const
{
  QStringList ret;
  QHash<QString, DictFile*>::const_iterator it = d->dictManagers.constBegin();
  while( it != d->dictManagers.constEnd() )
  {
    if( it.value()->getType() == type )
    {
      ret.append( it.key() );
    }

    ++it;
  }

  return ret;
}

void DictionaryManager::loadSettings( const KConfig &config )
{
  //TODO
}

/**
 * IMPORTANT: To add a dictionary type, you have to manually add the creation
 * step here, the next method, and \#include your header file above. If you have
 * fully implemented the interface in dictionarymanager.h, It should simply work.
 */
DictFile *DictionaryManager::makeDictFile( const QString &type )
{
  if( type == EDICT )
  {
    return new DictFileEdict();
  }
  else if( type == KANJIDIC )
  {
    return new DictFileKanjidic();
  }

  //Add new dictionary types here!!!

  return NULL;
}

void DictionaryManager::removeAllDictionaries()
{
  qDeleteAll(d->dictManagers);
  d->dictManagers.clear();
}

/**
 * Remove a dictionary from the list, and delete the dictionary object
 * (it should close files, deallocate memory, etc).
 *
 * @param name the name of the dictionary, as given in the addDictionary method
 */
bool DictionaryManager::removeDictionary( const QString &name )
{
  DictFile *file = d->dictManagers.take( name );
  delete file;
  return true;
}
