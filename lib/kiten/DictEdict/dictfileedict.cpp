/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
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

#include "dictfileedict.h"

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextCodec>
#include <QTextStream>
#include <QVector>

#include "deinflection.h"
#include "../dictquery.h"
#include "entryedict.h"
#include "../entrylist.h"
#include "../kitenmacros.h"

QString     *DictFileEdict::deinflectionLabel = nullptr;
QStringList *DictFileEdict::displayFields = nullptr;
QString     *DictFileEdict::wordType = nullptr;

/**
 * Per instructions in the super-class, this constructor basically sets the
 * dictionaryType member variable to identify this as an edict-type database handler.
 */
DictFileEdict::DictFileEdict()
: DictFile( EDICT )
, m_deinflection( 0 )
, m_hasDeinflection( false )
{
  m_dictionaryType = EDICT;
  m_searchableAttributes.insert( QStringLiteral("common"), QStringLiteral("common") );
}

/**
 * The destructor... ditch our memory maps and close our files here
 * (if they were open).
 */
DictFileEdict::~DictFileEdict()
{
  delete m_deinflection;
  m_deinflection = nullptr;
}

QMap<QString,QString> DictFileEdict::displayOptions() const
{
  QMap<QString,QString> list;
  list[ QStringLiteral("Part of speech(type)") ] = QStringLiteral("type");
  return list;
}

/**
 * Do a search, respond with a list of entries.
 * The general strategy will be to take the first word of the query, and do a
 * binary search on the dictionary for that item. Take all results and filter
 * them using the rest of the query with the validate method.
 */
EntryList *DictFileEdict::doSearch( const DictQuery &query )
{
  if( query.isEmpty() || ! m_edictFile.valid() )	//No query or dict, no results.
  {
    return new EntryList();
  }

  qDebug()<< "Search from : " << getName();

  QString firstChoice = query.getWord();
  if( firstChoice.length() == 0 )
  {
    firstChoice = query.getPronunciation();
    if( firstChoice.length() == 0 )
    {
      firstChoice = query.getMeaning().split( ' ' ).first().toLower();
      if( firstChoice.length() == 0 )
      {
        //The nastiest situation... we have to assemble a search string
        //from the first property
        QList<QString> keys = query.listPropertyKeys();
        if( keys.size() == 0 ) //Shouldn't happen... but maybe in the future
        {
          return new EntryList();
        }
        firstChoice = keys[ 0 ];
        firstChoice = firstChoice + query.getProperty( firstChoice );
        //TODO: doSearch: some accommodation for searching for ranges and such of properties
      }
    }
  }
  else
  {
    // Only search for one kanji or the
    // binary lookup mechanism breaks
    firstChoice = firstChoice.at( 0 );
  }

  QVector<QString> preliminaryResults = m_edictFile.findMatches( firstChoice );

  if( preliminaryResults.size() == 0 )	//If there were no matches... return an empty list
  {
    return new EntryList();
  }

  EntryList *results = new EntryList();
  foreach( const QString &it, preliminaryResults )
  {
//     qDebug() << "result: " << it << endl;
    Entry *result = makeEntry( it );
    EntryEdict *resultEdict = static_cast<EntryEdict*>( result );
    if( result->matchesQuery( query ) && resultEdict->matchesWordType( query ) )
    {
      results->append( result );
    }
    else
    {
      delete result;
    }
  }

  // At this point we should have some preliminary results
  // and if there were no matches, it probably means the user
  // input was a verb or adjective, so we have to deinflect it.
  bool isAnyQuery       = query.getMatchWordType() == DictQuery::Any;
  bool isVerbQuery      = query.getMatchWordType() == DictQuery::Verb;
  bool isAdjectiveQuery = query.getMatchWordType() == DictQuery::Adjective;
  if( results->count() == 0 && ( isAnyQuery || isVerbQuery || isAdjectiveQuery ) )
  {
    delete results;
    results = m_deinflection->search( query, preliminaryResults );
    QString *label = m_deinflection->getDeinflectionLabel();
    if( ! label->isEmpty() && ! m_hasDeinflection )
    {
      deinflectionLabel = label;
      m_hasDeinflection = true;
      wordType = m_deinflection->getWordType();
    }
  }
  else
  {
    deinflectionLabel = nullptr;
    wordType = nullptr;
    m_hasDeinflection = false;
  }

  if( results )
  {
    EntryList *common   = new EntryList();
    EntryList *uncommon = new EntryList();
    EntryList::EntryIterator i( *results );
    while( i.hasNext() )
    {
      EntryEdict *entry = static_cast<EntryEdict*>( i.next() );
      if( entry->isCommon() )
      {
        common->append( entry );
      }
      else
      {
        uncommon->append( entry );
      }
    }

    delete results;
    results = new EntryList();
    results->appendList( common );
    results->appendList( uncommon );
    delete common;
    delete uncommon;

    EntryList *exact     = new EntryList();
    EntryList *beginning = new EntryList();
    EntryList *ending    = new EntryList();
    EntryList *anywhere  = new EntryList();
    EntryList::EntryIterator it( *results );
    while( it.hasNext() )
    {
      Entry *entry = it.next();

      if( entry->getWord() == query.getWord() )
      {
        exact->append( entry );
      }
      else if( entry->getWord().startsWith( query.getWord() ) )
      {
        beginning->append( entry );
      }
      else if( entry->getWord().endsWith( query.getWord() ) )
      {
        ending->append( entry );
      }
      else
      {
        anywhere->append( entry );
      }
    }

    delete results;
    results = new EntryList();
    results->appendList( exact );
    results->appendList( beginning );
    results->appendList( ending );
    results->appendList( anywhere );
    delete exact;
    delete beginning;
    delete ending;
    delete anywhere;
  }

  return results;
}

/**
 * Make a list of all the extra fields in our db.. Entry uses this to decide
 * what goes in the interpretations it gives.
 */
QStringList DictFileEdict::listDictDisplayOptions( QStringList x ) const
{
  x += displayOptions().keys();
  return x;
}

/**
 * Load up the dictionary
 */
bool DictFileEdict::loadDictionary( const QString &fileName, const QString &dictName )
{
  if( m_edictFile.valid() )
  {
    return false; //Already loaded
  }

  if( m_edictFile.loadFile( fileName ) )
  {
    m_dictionaryName = dictName;
    m_dictionaryFile = fileName;

    m_deinflection = new Deinflection( m_dictionaryName );
    m_deinflection->load();

    return true;
  }

  return false;
}

QMap<QString,QString> DictFileEdict::loadDisplayOptions() const
{
  QMap<QString,QString> list = displayOptions();
  list[ QStringLiteral("Word/Kanji") ]  = QStringLiteral("Word/Kanji");
  list[ QStringLiteral("Reading") ]     = QStringLiteral("Reading");
  list[ QStringLiteral("Meaning") ]     = QStringLiteral("Meaning");
  list[ QStringLiteral("--Newline--") ] = QStringLiteral("--Newline--");

  return list;
}

void DictFileEdict::loadSettings()
{
  this->displayFields = new QStringList( loadDisplayOptions().values() );
}

inline Entry* DictFileEdict::makeEntry( const QString &entry )
{
  return new EntryEdict( getName(), entry );
}


/**
 * Scan a potential file for the correct format, remembering to skip comment
 * characters. This is not a foolproof scan, but it should be checked before adding
 * a new dictionary.
 * Valid EDICT format is considered:
 * \<kanji or kana\>+ [\<kana\>] /latin characters & symbols/separated with slashes/
 * Comment lines start with... something... not remembering now.
 */
bool DictFileEdict::validDictionaryFile( const QString &filename )
{
  QFile file( filename );
  bool returnFlag = true;

  if( ! file.exists() || ! file.open( QIODevice::ReadOnly ) )
  {
    return false;
  }

  //Now we can actually check the file
  QTextStream fileStream( &file );
  fileStream.setCodec( QTextCodec::codecForName( "eucJP" ) );
  QString commentMarker( QStringLiteral("？？？？") ); //Note: Don't touch this! vim seems to have
                                      //An odd text codec error here too :(
  QRegExp formattedLine( "^\\S+\\s+(\\[\\S+\\]\\s+)?/.*/$" );
  while( ! fileStream.atEnd() )
  {
    QString line = fileStream.readLine();

    if( line.left( 4 ) == commentMarker )
    {
      continue;
    }
    if( line.contains( formattedLine ) ) //If it matches our regex
    {
      continue;
    }

    returnFlag = false;
    break;
  }

  file.close();
  return returnFlag;
}

/**
 * Reject queries that specify anything we don't understand
 */
//TODO: Actually write this method (validQuery)
bool DictFileEdict::validQuery( const DictQuery &query )
{
  return true;
}
