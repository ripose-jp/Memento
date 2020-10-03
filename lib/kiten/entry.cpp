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

#include "entry.h"
#include "dictquery.h"

#include <iostream>
#include <cassert>
#include <sys/mman.h>
#include <stdio.h>

/**
 * The default constructor, unless you really know what you're doing,
 * THIS SHOULD NOT BE USED. For general use, other entities will need
 * to have the information provided by the other constructors
 * (particularly the sourceDictionary).
 */
Entry::Entry()
{
  init();
}

Entry::Entry( const QString &sourceDictionary )
: sourceDict( sourceDictionary )
{
  init();
}

Entry::Entry( const QString &sourceDictionary, const QString &word,
	      const QStringList &reading, const QStringList &meanings )
: Word( word )
, Meanings( meanings )
, Readings( reading )
, sourceDict( sourceDictionary )
{
  init();
}

Entry::Entry( const Entry &src )
: Word( src.Word )
, Meanings( src.Meanings )
, Readings( src.Readings )
, ExtendedInfo( src.ExtendedInfo )
, sourceDict( src.sourceDict )
{
  outputListDelimiter = src.outputListDelimiter;
}

Entry::~Entry()
{
//   kdDebug() << "nuking : " << Word << endl;
}

bool Entry::extendedItemCheck( const QString& key, const QString &value ) const
{
  return getExtendedInfoItem( key ) == value;
}

/**
 * Get the dictionary name that generated this Entry. I can't think of a reason to be changing this
 */
QString Entry::getDictName() const
{
  return sourceDict;
}

/**
 * Get the word from this Entry. If the entry is of type kanji/kana/meaning/etc, this will return
 * the kanji. If it is of kana/meaning/etc, it will return kana.
 */
QString Entry::getWord() const
{
  return Word;
}

/**
 * Get a QString containing all of the meanings known, connected by the outputListDelimiter
 */
QString Entry::getMeanings() const
{
  return Meanings.join(outputListDelimiter);
}

/**
 * Simple accessor
 */
QStringList Entry::getMeaningsList() const
{
  return Meanings;
}

/**
 * Simple accessor
 */
QString Entry::getReadings() const
{
  return Readings.join( outputListDelimiter );
}

/**
 * Simple accessor
 */
QStringList Entry::getReadingsList() const
{
  return Readings;
}

/**
 * Simple accessor
 */
QHash<QString,QString> Entry::getExtendedInfo() const
{
  return ExtendedInfo;
}

/**
 * Simple accessor
 *
 * @param x the key for the extended info item to get
 */
QString Entry::getExtendedInfoItem( const QString &x ) const
{
  return ExtendedInfo[ x ];
}

/**
 * Prepares Meanings for output as HTML
 */
inline QString Entry::HTMLMeanings() const
{
  return QStringLiteral( "<span class=\"Meanings\">%1</span>" )
             .arg( Meanings.join( outputListDelimiter ) );
}

/* Prepares Readings for output as HTML */
inline QString Entry::HTMLReadings() const
{
  QStringList list;
  foreach( const QString &it, Readings )
  {
    list += makeLink( it );
  }

  return QStringLiteral( "<span class=\"Readings\">%1</span>" )
             .arg( list.join( outputListDelimiter ) );
}

/**
 * Prepares Word for output as HTML
 */
inline QString Entry::HTMLWord() const
{
  return QStringLiteral( "<span class=\"Word\">%1</span>" ).arg( Word );
}

void Entry::init()
{
  outputListDelimiter = "; "; // DANGER, possible issues
}

/**
 * Determines whether @param character is a kanji character.
 */
bool Entry::isKanji( const QChar &character ) const
{
  ushort value = character.unicode();
  if( value < 255 )
  {
    return false;
  }
  if( 0x3040 <= value && value <= 0x30FF )
  {
    return false; //Kana
  }

  return true; //Note our folly here... we assuming any non-ascii/kana is kanji
}

/**
 * Returns true if all members of test are in list
 */
bool Entry::listMatch( const QStringList &list, const QStringList &test, DictQuery::MatchType type ) const
{
  if( type == DictQuery::Exact )
  {
    foreach( const QString &it, test )
    {
      if( ! list.contains( it ) )
      {
        return false;
      }
    }
  }
  else if( type == DictQuery::Beginning )
  {
    foreach( const QString &it, test )
    {
      bool found = false;
      foreach( const QString &it2, list )
      {
        if( it2.startsWith( it ) )
        {
          found = true;
          break;
        }
      }
      if( ! found )
      {
        return false;
      }
    }
  }
  else if( type == DictQuery::Ending )
  {
    foreach( const QString &it, test )
    {
      bool found = false;
      foreach( const QString &it2, list )
      {
        if( it2.endsWith( it ) )
        {
          found = true;
          break;
        }
      }
      if( ! found )
      {
        return false;
      }
    }
  }
  else
  {
    foreach( const QString &it, test )
    {
      bool found = false;
      foreach( const QString &it2, list )
      {
        if( it2.contains( it ) )
        {
          found = true;
          break;
        }
      }
      if( ! found )
      {
        return false;
      }
    }
  }

  return true;
}

/**
 * New functions for Entry doing direct display.
 *
 * Creates a link for the given @p entryString.
 */
inline QString Entry::makeLink( const QString &entryString ) const
{
  return QStringLiteral( "<a href=\"%1\">%1</a>" ).arg( entryString );
}

bool Entry::matchesQuery( const DictQuery &query ) const
{
  if( ! query.getWord().isEmpty() )
  {
    if( query.getMatchType() == DictQuery::Exact
      && this->getWord() != query.getWord() )
    {
      return false;
    }
    if( query.getMatchType() == DictQuery::Beginning
      && ! this->getWord().startsWith( query.getWord() ) )
    {
      return false;
    }
    if( query.getMatchType() == DictQuery::Ending
      && ! this->getWord().endsWith( query.getWord() ) )
    {
      return false;
    }
    if( query.getMatchType() == DictQuery::Anywhere
      && ! this->getWord().contains( query.getWord() ) )
    {
      return false;
    }
  }

  if( ! query.getPronunciation().isEmpty() && ! getReadings().isEmpty() )
  {
    if( ! listMatch( Readings, query.getPronunciation().split( DictQuery::mainDelimiter ),
                            query.getMatchType() ) )
    {
      return false;
    }
  }

  if( ! query.getPronunciation().isEmpty() && getReadings().isEmpty() && ! getWord().isEmpty() )
  {
    switch ( query.getMatchType() )
    {
      case DictQuery::Exact:
        if ( getWord() != query.getPronunciation() )
        {
          return false;
        }
        break;
      case DictQuery::Beginning:
        if ( ! getWord().startsWith( query.getPronunciation() ) )
        {
          return false;
        }
        break;
      case DictQuery::Ending:
        if ( ! getWord().endsWith( query.getPronunciation() ) )
        {
          return false;
        } //fallthrough
      case DictQuery::Anywhere:
        if ( ! getWord().contains( query.getPronunciation() ) )
        {
          return false;
        }
        break;
    }
  }

  if( ! query.getMeaning().isEmpty() )
  {
    if( ! listMatch(   Meanings.join(QLatin1Char(' ') ).toLower().split( ' ' )
                     , query.getMeaning().toLower().split( DictQuery::mainDelimiter )
                     , query.getMatchType() ) )
    {
      return false;
    }
  }

  QList<QString> propList = query.listPropertyKeys();
  foreach( const QString &key, propList )
  {
    if( ! extendedItemCheck( key, query.getProperty( key ) ) )
    {
      return false;
    }
  }

  return true;
}

/**
 * Main switching function for displaying to the user
 */
QString Entry::toHTML() const
{
  return QStringLiteral( "<div class=\"Entry\">%1%2%3</div>" )
             .arg( HTMLWord() )
             .arg( HTMLReadings() )
             .arg( HTMLMeanings() );
}

inline QString Entry::toKVTML() const
{
        /*
   <e m="1" s="1">
           <o width="414" l="en" q="t">(eh,) excuse me</o>
           <t width="417" l="jp" q="o">(あのう、) すみません </t>
   </e>
   */
  //TODO: en should not necessarily be the language here.
  return QString( "<e>\n<o l=\"en\">%1</o>\n"
                  "<t l=\"jp-kanji\">%2</t>\n"
                  "<t l=\"jp-kana\">%3</t></e>\n\n" ).arg( getMeanings() )
                                                     .arg( getWord() )
                                                     .arg( getReadings() );
}

/**
 * This method should return the entry object in a simple QString format
 * Brief form should be usable in quick summaries, for example
 * Verbose form might be used to save a complete list of entries to a file, for example.
 */
QString Entry::toString() const
{
  return QStringLiteral( "%1 (%2) %3" ).arg( Word )
                                .arg( getReadings() )
                                .arg( getMeanings() );
}

/**
 * This version of sort only sorts dictionaries...
 * This is a replacement for an operator\< function... so we return true if
 * "this" should show up first on the list.
 */
bool Entry::sort( const Entry &that, const QStringList &dictOrder, const QStringList &fields ) const
{
  if( this->sourceDict != that.sourceDict )
  {
    foreach( const QString &dict, dictOrder )
    {
      if( dict == that.sourceDict )
      {
        return false;
      }
      if( dict == this->sourceDict )
      {
        return true;
      }
    }
  }
  else
  {
    foreach( const QString &field, fields )
    {
      if( field == QLatin1String( "Word/Kanji" ) )
      {
        return this->getWord() < that.getWord();
      }
      else if( field == QLatin1String( "Meaning" ) )
      {
        return listMatch( that.getMeaningsList(), this->getMeaningsList(), DictQuery::Exact )
               && ( that.getMeaningsList().count() != this->getMeaningsList().count() );
      }
      else if( field == QLatin1String( "Reading" ) )
      {
        return listMatch( that.getReadingsList(), this->getReadingsList(), DictQuery::Exact )
               && ( that.getReadingsList().count() != this->getReadingsList().count() );
      }
      else
      {
        const QString thisOne = this->getExtendedInfoItem( field );
        const QString thatOne = that.getExtendedInfoItem( field );
        //Only sort by this field if the values differ, otherwise move to the next field
        if( thisOne != thatOne )
        {
          //If the second item does not have this field, sort this one first
          if( thatOne.isEmpty() )
          {
            return true;
          }
          //If we don't have this field, sort "this" to second
          if( thisOne.isEmpty() )
          {
            return false;
          }
          //Otherwise, send it to a virtual function (to allow dictionaries to override sorting)
          return this->sortByField( that, field );
        }
      }
    }
  }
  return false; //If we reach here, they match as much as possible
}

bool Entry::sortByField( const Entry &that, const QString &field ) const
{
  return this->getExtendedInfoItem( field ) < that.getExtendedInfoItem( field );
}
