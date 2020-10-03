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

/*
TODO: Add features to limit the number of hits on a per-search basis.

	Add a mechanism (either through subclassing, or directly) for use
		for marking "requested" fields for the dcop system.
*/

#include "dictquery.h"

#include <QDebug>

#include <QString>
#include <QStringList>

class DictQuery::Private
{
  public:
    Private() : matchType( DictQuery::Exact )
              , matchWordType( DictQuery::Any )
              , filterType( DictQuery::NoFilter ) { }

    /** Stores the (english or otherwise non-japanese) meaning */
    QString meaning;
    /** Stores the pronunciation in kana */
    QString pronunciation;
    /** The main word, this usually contains kanji */
    QString word;
    /** Any amount of extended attributes, grade level, heisig/henshall/etc index numbers, whatever you want */
    QHash<QString,QString> extendedAttributes;
    /** The order that various attributes, meanings, and pronunciations were entered, so we can
      * regenerate the list for the user if they need them again */
    QStringList entryOrder;
    /** A list of dictionaries to limit the search to, and empty list implies "all loaded dictionaries" */
    QStringList targetDictionaries;
    /** What MatchType is this set to */
    MatchType matchType;
    /** What MatchWordType is this set to */
    MatchWordType matchWordType;
    /** What FilterType is this set to */
    FilterType filterType;

    /** Marker in the m_entryOrder for the location of the pronunciation element */
    static const QString pronunciationMarker;
    /** Marker in the m_entryOrder for the location of the translated meaning element */
    static const QString meaningMarker;
    /** Marker in the m_entryOrder for the location of the word (kanji) element */
    static const QString wordMarker;
};

const QString DictQuery::Private::pronunciationMarker( QStringLiteral("__@\\p") );
const QString DictQuery::Private::meaningMarker( QStringLiteral("__@\\m") );
const QString DictQuery::Private::wordMarker( QStringLiteral("_@\\w") );

/*****************************************************************************
*	Constructors, Destructors, Initializers, and
*	Global Status Indicators.
*****************************************************************************/
DictQuery::DictQuery()
: d( new Private )
{ }

DictQuery::DictQuery( const QString& str )
: d( new Private )
{
  this->operator=( (QString)str );
}

DictQuery::DictQuery( const DictQuery& orig )
: d( new Private )
{
  this->operator=( (DictQuery&)orig );
}

DictQuery *DictQuery::clone() const
{
  return new DictQuery( *this );
}

DictQuery::operator QString() const
{
  //kDebug() << "DictQuery toString operator called!";
  return toString();
}

DictQuery::~DictQuery()
{
  delete d;
}

bool DictQuery::isEmpty() const
{
  // We're only empty if the two strings are empty too
  return d->extendedAttributes.isEmpty() && d->meaning.isEmpty()
           && d->pronunciation.isEmpty() && d->word.isEmpty();
}

void DictQuery::clear()
{
  d->extendedAttributes.clear();
  d->meaning = QLatin1String("");
  d->pronunciation = QLatin1String("");
  d->word = QLatin1String("");
  d->entryOrder.clear();
}

/*****************************************************************************
*	Methods that involve multiple instances of the class
*	(comparison, copy etc)
*****************************************************************************/
DictQuery &DictQuery::operator=( const DictQuery &old )
{
  if ( &old == this )
  {
    return *this;
  }

  clear();
  d->matchType          = old.d->matchType;
  d->matchWordType      = old.d->matchWordType;
  d->filterType         = old.d->filterType;
  d->extendedAttributes = old.d->extendedAttributes;
  d->meaning            = old.d->meaning;
  d->pronunciation      = old.d->pronunciation;
  d->word               = old.d->word;
  d->entryOrder         = old.d->entryOrder;
  return *this;
}

DictQuery &DictQuery::operator+=( const DictQuery &old )
{
  foreach( const QString &item, old.d->entryOrder )
  {
    if( item == d->meaningMarker )
    {
      if( d->entryOrder.removeAll( d->meaningMarker ) > 0 )
      {
        setMeaning( getMeaning() + mainDelimiter + old.getMeaning() );
      }
      else
      {
        setMeaning( old.getMeaning() );
      }
    }
    else if( item == d->pronunciationMarker )
    {
      if( d->entryOrder.removeAll( d->pronunciationMarker ) > 0 )
      {
        setPronunciation( getPronunciation() + mainDelimiter + old.getPronunciation() );
      }
      else
      {
        setPronunciation( old.getPronunciation() );
      }
    }
    else if( item == d->wordMarker )
    {
      d->entryOrder.removeAll( d->wordMarker );
      //Only one of these allowed
      setWord( old.getWord() );
    }
    else
    {
      setProperty( item, old.getProperty( item ) );
    }
  }

  return *this;
}

DictQuery operator+( const DictQuery &a, const DictQuery &b )
{
  DictQuery val( a );
  val += b;
  return val;
}

bool operator==( const DictQuery &a, const DictQuery &b )
{
  if(   ( a.d->pronunciation      != b.d->pronunciation )
     || ( a.d->meaning            != b.d->meaning )
     || ( a.d->word               != b.d->word )
     || ( a.d->entryOrder         != b.d->entryOrder )
     || ( a.d->extendedAttributes != b.d->extendedAttributes )
     || ( a.d->matchType          != b.d->matchType )
     || ( a.d->matchWordType      != b.d->matchWordType )
     || ( a.d->filterType         != b.d->filterType ) )
  {
    return false;
  }

  return true;
}

bool operator!=( const DictQuery &a, const DictQuery &b )
{
  return ! ( a == b );
}

bool operator<( const DictQuery &a, const DictQuery &b )
{
  QHash<QString,QString>::const_iterator it     = a.d->extendedAttributes.constBegin();
  QHash<QString,QString>::const_iterator it_end = a.d->extendedAttributes.constEnd();
  for( ; it != it_end; ++it )
  {
    QString B_version = b.d->extendedAttributes.value( it.key() );
    if( a.d->extendedAttributes[ it.key() ] != B_version )
    {
      if( ! B_version.contains( QLatin1Char(',') ) && ! B_version.contains( QLatin1Char('-') ) )
      {
        return false;
      }
      //TODO: check for multi-values or ranges in DictQuery operator<
    }
  }

  if( ! a.d->pronunciation.isEmpty() )
  {
    QStringList aList = a.d->pronunciation.split( DictQuery::mainDelimiter );
    QStringList bList = b.d->pronunciation.split( DictQuery::mainDelimiter );
    foreach( const QString &str, aList )
    {
      if( bList.contains( str ) == 0 )
      {
        return false;
      }
    }
  }

  if( ! a.d->meaning.isEmpty() )
  {
    QStringList aList = a.d->meaning.split( DictQuery::mainDelimiter );
    QStringList bList = b.d->meaning.split( DictQuery::mainDelimiter );
    foreach( const QString &str, aList )
    {
      if( bList.contains( str ) == 0 )
      {
        return false;
      }
    }
  }

  //Assume only one entry for word
  if( ! a.d->word.isEmpty() )
  {
    if( a.d->word != b.d->word )
    {
      return false;
    }
  }

  return true;
}

/*****************************************************************************
*	Methods to extract from QStrings and recreate QStrings
*
*****************************************************************************/
const QString DictQuery::toString() const
{
  if( isEmpty() )
  {
    return QString();
  }

  QString reply;
  foreach( const QString &it, d->entryOrder )
  {
    if( it == d->pronunciationMarker )
    {
      reply += d->pronunciation+mainDelimiter;
    }
    else if( it == d->meaningMarker )
    {
      reply += d->meaning+mainDelimiter;
    }
    else if( it == d->wordMarker )
    {
      reply += d->word+mainDelimiter;
    }
    else
    {
      reply += it + propertySeperator + d->extendedAttributes.value( it )
                  + mainDelimiter;
    }
  }
  reply.truncate( reply.length() - mainDelimiter.length() );

  return reply;
}

DictQuery &DictQuery::operator=( const QString &str )
{
  QStringList parts = str.split( mainDelimiter );
  DictQuery result;
  if( str.length() > 0 )
  {
    foreach( const QString &it, parts )
    {
      if( it.contains( propertySeperator ) )
      {
        QStringList prop = it.split( propertySeperator );
        if( prop.count() != 2 )
        {
          break;
        }
        result.setProperty( prop[ 0 ], prop[ 1 ] );
        //replace or throw an error with duplicates?
      }
      else
      {
        switch( stringTypeCheck( it ) )
        {
          case DictQuery::Latin:
            if( result.d->entryOrder.removeAll( d->meaningMarker ) > 0 )
            {
              result.setMeaning( result.getMeaning() + mainDelimiter + it );
            }
            else
            {
              result.setMeaning( it );
            }
            break;

          case DictQuery::Kana:
            if( result.d->entryOrder.removeAll( d->pronunciationMarker ) > 0 )
            {
              result.setPronunciation( result.getPronunciation() + mainDelimiter + it );
            }
            else
            {
              result.setPronunciation( it );
            }
            break;

          case DictQuery::Kanji:
            result.d->entryOrder.removeAll( d->wordMarker );
            result.setWord( it ); //Only one of these allowed
            break;

          case DictQuery::Mixed:
            qWarning() << "DictQuery: String parsing error - mixed type";
            break;

          case DictQuery::ParseError:
            qWarning() << "DictQuery: String parsing error";
            break;
        }
      }
    }
  }
  //kDebug() << "Query: ("<<result.getWord() << ") ["<<result.getPronunciation()<<"] :"<<
  //	result.getMeaning()<<endl;
  this->operator=( result );
  return *this;
}

/**
 * Private utility method for the above... confirms that an entire string
 * is either completely japanese or completely english
 */
DictQuery::StringTypeEnum DictQuery::stringTypeCheck( const QString &in )
{
  StringTypeEnum firstType;
  //Split into individual characters
  if( in.size() <= 0 )
  {
    return DictQuery::ParseError;
  }

  firstType = charTypeCheck( in.at( 0 ) );
  for( int i = 1; i < in.size(); i++ )
  {
    StringTypeEnum newType = charTypeCheck( in.at( i ) );
    if( newType != firstType )
    {
      if( firstType == Kana && newType == Kanji )
      {
        firstType = Kanji;
      }
      else if( firstType == Kanji && newType == Kana )
        ; //That's okay
      else
      {
        return DictQuery::Mixed;
      }
    }
  }

  return firstType;
}

/**
 * Private utility method for the stringTypeCheck
 * Just checks and returns the type of the first character in the string
 * that is passed to it.
 */
DictQuery::StringTypeEnum DictQuery::charTypeCheck( const QChar &ch )
{
  if( ch.toLatin1() )
  {
    return Latin;
  }
  //The unicode character boundaries are:
  // 3040 - 309F Hiragana
  // 30A0 - 30FF Katakana
  // 31F0 - 31FF Katakana phonetic expressions (wtf?)
  if( 0x3040 <= ch.unicode() && ch.unicode() <= 0x30FF /*|| ch.unicode() & 0x31F0*/ )
  {
    return Kana;
  }

  return Kanji;
}

/*****************************************************************************
*	An array of Property List accessors and mutators
*
*****************************************************************************/
QString DictQuery::getProperty( const QString &key ) const
{
  return ( *this )[ key ];
}

const QList<QString> DictQuery::listPropertyKeys() const
{
  return d->extendedAttributes.keys();
}

const QString DictQuery::operator[] ( const QString &key ) const
{
  return d->extendedAttributes.value( key );
}

QString DictQuery::operator[] ( const QString &key )
{
  return d->extendedAttributes[ key ];
}

bool DictQuery::hasProperty( const QString &key ) const
{
  return d->entryOrder.contains( key ) > 0;
}

//TODO: Add i18n handling and alternate versions of property names
//TODO: further break down the barrier between different types
bool DictQuery::setProperty( const QString& key, const QString& value )
{
  if( key == d->pronunciationMarker || key == d->meaningMarker
      || key.isEmpty() || value.isEmpty() )
  {
    return false;
  }

  if ( ! d->extendedAttributes.contains( key ) )
  {
    d->entryOrder.append( key );
  }

  d->extendedAttributes.insert( key, value );
  return true;
}

bool DictQuery::removeProperty( const QString &key )
{
  if( d->extendedAttributes.contains( key ) )
  {
    return d->entryOrder.removeAll( key );
  }
  return false;
}

QString DictQuery::takeProperty ( const QString & key )
{
  d->entryOrder.removeAll( key );
  return d->extendedAttributes.take( key );
}

/*****************************************************************************
*	Meaning and Pronunciation Accessors and Mutators
****************************************************************************/
QString DictQuery::getMeaning() const
{
  return d->meaning;
}

bool DictQuery::setMeaning( const QString &newMeaning )
{
  if ( newMeaning.isEmpty() )
  {
#ifdef USING_QUERY_EXCEPTIONS
    throw InvalidQueryException( newMeaning );
#else
    return false;
#endif
  }

  d->meaning = newMeaning;

  if( ! d->entryOrder.contains( d->meaningMarker ) )
  {
    d->entryOrder.append( d->meaningMarker );
  }

  return true;
}

QString DictQuery::getPronunciation() const
{
  return d->pronunciation;
}

bool DictQuery::setPronunciation( const QString &newPronunciation )
{
  if( newPronunciation.isEmpty() )
  {
#ifdef USING_QUERY_EXCEPTIONS
    throw InvalidQueryException( newPro );
#else
    return false;
#endif
  }

  d->pronunciation = newPronunciation;

  if( ! d->entryOrder.contains( d->pronunciationMarker ) )
  {
    d->entryOrder.append( d->pronunciationMarker );
  }

  return true;
}

QString DictQuery::getWord() const
{
  return d->word;
}

bool DictQuery::setWord( const QString &newWord )
{
  if( newWord.isEmpty() )
  {
#ifdef USING_QUERY_EXCEPTIONS
    throw InvalidQueryException( newWord );
#else
    return false;
#endif
  }

  d->word = newWord;

  if( ! d->entryOrder.contains( d->wordMarker ) )
  {
    d->entryOrder.append( d->wordMarker );
  }

  return true;
}

/*************************************************************
  Handlers for getting and setting dictionary types
  *************************************************************/
QStringList DictQuery::getDictionaries() const
{
  return d->targetDictionaries;
}

void DictQuery::setDictionaries( const QStringList &newDictionaries )
{
  d->targetDictionaries = newDictionaries;
}

/**************************************************************
  Match Type Accessors and Mutators
  ************************************************************/
DictQuery::FilterType DictQuery::getFilterType() const
{
  return d->filterType;
}

void DictQuery::setFilterType( FilterType newType )
{
  d->filterType = newType;
}

DictQuery::MatchType DictQuery::getMatchType() const
{
  return d->matchType;
}

void DictQuery::setMatchType( MatchType newType )
{
  d->matchType = newType;
}

DictQuery::MatchWordType DictQuery::getMatchWordType() const
{
  return d->matchWordType;
}

void DictQuery::setMatchWordType( MatchWordType newType )
{
  d->matchWordType = newType;
}

/**************************************************************
*	Aliases to handle different forms of operator arguments
*	Disabled at the moment
*************************************************************
bool operator==( const QString &other, const DictQuery &query ) {
	DictQuery x(other); return x == query;
}
bool operator==( const DictQuery &query, const QString &other ) {
	return other==query;
}
bool operator!=( const DictQuery &q1, const DictQuery &q2 ) {
	return !(q1==q2);
}
bool operator!=( const QString &other, const DictQuery &query ) {
	return !(other==query);
}
bool operator!=( const DictQuery &query, const QString &other ) {
	return !(query==other);
}
inline bool operator<=( const DictQuery &a, const DictQuery &b) {
	return (a<b || a==b);
}
bool operator>=( const DictQuery &a, const DictQuery &b) {
	return (b>a || a==b);
}
bool operator>( const DictQuery &a, const DictQuery &b) {
	return b < a;
}
DictQuery &operator+( const DictQuery &a, const QString &b) {
	return (*(new DictQuery(a))) += b;
}
DictQuery &operator+( const QString &a,   const DictQuery &b)  {
	return (*(new DictQuery(a))) += b;
}
DictQuery    &DictQuery::operator+=(const QString &str) {
	DictQuery x(str);
	return operator+=(x);
}
#ifndef QT_NO_CAST_ASCII
DictQuery    &DictQuery::operator=(const char *str) {
	QString x(str);
	return operator=(x);
}
DictQuery    &DictQuery::operator+=(const char *str) {
	DictQuery x(str);
	return operator+=(x);
}
#endif
*/
/**************************************************************
*	Set our constants declared in the class
**************************************************************/
const QString DictQuery::mainDelimiter( QStringLiteral(" ") );
const QString DictQuery::propertySeperator( QStringLiteral(":") );
