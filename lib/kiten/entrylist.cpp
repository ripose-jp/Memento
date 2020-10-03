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

#include "entrylist.h"

#include <sys/mman.h>

#include "DictEdict/dictfileedict.h"
#include "DictEdict/entryedict.h"
#include "kitenmacros.h"

class EntryList::Private
{
  public:
    Private() : storedScrollValue( 0 )
              , sorted( false )
              , sortedByDictionary( false ) {}

    Private( const Private &other ) = default;
    Private &operator=( const Private &other )  = default;

    int       storedScrollValue;
    bool      sorted;
    bool      sortedByDictionary;
    DictQuery query;
};

/* sorts the EntryList in a C++ish, thread-safe manner. */
class SortFunctor
{
  public:
    QStringList *dictionary_order;
    QStringList *sort_order;

    bool operator()( const Entry *n1, const Entry *n2 ) const
    {
      return n1->sort( *n2, *dictionary_order, *sort_order );
    }
};

EntryList::EntryList()
: QList<Entry*>()
, d( new Private )
{
}

EntryList::EntryList( const EntryList &old )
: QList<Entry*> ( old )
, d( new Private( *( old.d ) ) )
{
}

EntryList::~EntryList()
{
  delete d;
//   kdDebug() << "A copy of EntryList is being deleted... watch your memory!" << endl;
}

int EntryList::scrollValue() const
{
  return d->storedScrollValue;
}

void EntryList::setScrollValue( int val )
{
  d->storedScrollValue = val;
}

void EntryList::deleteAll()
{
  while( ! this->isEmpty() )
  {
    delete this->takeFirst();
  }

  d->sorted = false;
}

/* Returns the EntryList as HTML */
//TODO: Some intelligent decision making regarding when to print what when AutoPrinting is on
QString EntryList::toHTML( unsigned int start, unsigned int length ) const
{
  unsigned int max = count();
  if( start > max )
  {
    return QString();
  }
  if( start + length > max )
  {
    length = max-start;
  }

  QString result;
  QString temp;
  QString &lastDictionary = temp;
  const QString fromDictionary = "From Dictionary:";
  QString query( getQuery() );

  bool firstTimeDeinflection  = true;
  bool firstTimeCommon = true;
  bool firstTimeUncommon = true;
  for( unsigned int i = 0; i < max; ++i )
  {
    Entry *entry = at( i );
    if( d->sortedByDictionary )
    {
      const QString &newDictionary = entry->getDictName();
      if( firstTimeDeinflection && newDictionary == EDICT
          && DictFileEdict::deinflectionLabel )
      {
        const QString &label = *DictFileEdict::deinflectionLabel;
        const QString &type  = *DictFileEdict::wordType;
        const QString &message = QString( "%1 is a word type (verb or adjective)."
                                          " %2 is a verb or adjective tense." 
                                          " Example: 'Entered verb in past tense'."
                                          "Entered %1 in %2 form" ).arg(type, label);

        result += QStringLiteral( "<div style=\"font-style:italic\">%1</div>" ).arg( message );

        firstTimeDeinflection = false;
      }

      if( lastDictionary != newDictionary )
      {
        lastDictionary = newDictionary;
        result += QStringLiteral( "<div class=\"DictionaryHeader\">%1 %2</div>" )
                      .arg( fromDictionary )
                      .arg( newDictionary );
        firstTimeCommon = true;
        firstTimeUncommon = true;
      }
    }

    if( getQuery().getFilterType() == DictQuery::CommonUncommon )
    {
      if( entry->getDictionaryType() == EDICT )
      {
        EntryEdict *entryEdict = static_cast<EntryEdict*>( entry );
        if( entryEdict->isCommon() && firstTimeCommon )
        {
          result += QStringLiteral( "<div class=\"CommonHeader\">%1</div>" ).arg( "Common" );
          firstTimeCommon = false;
        }
        else if( ! entryEdict->isCommon() && firstTimeUncommon )
        {
          result += QStringLiteral( "<div class=\"UncommonHeader\">%1</div>" ).arg( "Uncommon" );
          firstTimeUncommon = false;
        }
      }
    }

    if( length-- > 0 )
    {
      result += QStringLiteral( "<div class=\"%1\" index=\"%2\" dict=\"%3\">%4</div>" )
                    .arg( i % 2 == 0 ? "Entry" : "Entry odd" )
                    .arg( QString::number( i ) )
                    .arg( entry->getDictName() )
                    .arg( entry->toHTML() );
    }
    else
    {
      break;
    }
  }
  //result.replace( query, "<query>" + query + "</query>" );
  return result;
}

QString EntryList::toKVTML( unsigned int start, unsigned int length ) const
{
  unsigned int max = count();
  if( start > max )
  {
    return QString();
  }
  if( start + length > max )
  {
    length = max - start;
  }

  QString result = "<?xml version=\"1.0\"?>\n<!DOCTYPE kvtml SYSTEM \"kvoctrain.dtd\">\n"
          "<kvtml encoding=\"UTF-8\" "
          " generator=\"kiten v42.0\""
          " title=\"To be determined\">\n";
  foreach( Entry *it, *this )
  {
    if( length-- > 0 )
    {
      result = result + it->toKVTML() + '\n';
    }
    else
    {
      break;
    }
  }
  return result + "</kvtml>\n";
}

QString EntryList::toHTML() const
{
  return toHTML( 0, count() );
}

/* Returns the EntryList as HTML */
//TODO: Some intelligent decision making... regarding the output format (differ for
//different types of searches?
QString EntryList::toString( unsigned int start, unsigned int length ) const
{
  unsigned int max = count();
  if( start > max )
  {
    return QString();
  }
  if( start + length > max )
  {
    length = max-start;
  }

  QString result;
  foreach( Entry *it, *this )
  {
    if( length-- > 0 )
    {
      result = result + it->toString();
    }
    else
    {
      break;
    }
  }

  return result;
}

QString EntryList::toString() const
{
  return toString( 0, count() );
}

void EntryList::sort( QStringList &sortOrder, QStringList &dictionaryOrder )
{
  //Don't shortcut sorting, unless we start to keep track of the last sorting order,
  //Otherwise we won't respond when the user changes the sorting order
  SortFunctor sorter;
  sorter.dictionary_order = &dictionaryOrder;
  sorter.sort_order = &sortOrder;

  std::stable_sort( this->begin(), this->end(), sorter );
  d->sorted = true;
  d->sortedByDictionary = dictionaryOrder.size() > 0;
}

const EntryList& EntryList::operator+=( const EntryList &other )
{
  foreach( Entry *it, other )
  {
    this->append( it );
  }
  if( other.size() > 0 )
  {
    d->sorted = false;
  }

  return *this;
}

const EntryList& EntryList::operator=( const EntryList &other )
{
  QList<Entry*>::operator=( other );
  *d = *( other.d );

  return *this;
}

void EntryList::appendList( const EntryList *other )
{
  foreach( Entry *it, *other )
  {
    append( it );
  }

  if( other->size() > 0 )
  {
    d->sorted = false;
  }
}

/**
 * This method retrieves an earlier saved query in the EntryList,
 * this should be the query that generated the list.
 */
DictQuery EntryList::getQuery() const
{
  return d->query;
}

/**
 * This allows us to save a query in the EntryList for later retrieval
 */
void EntryList::setQuery( const DictQuery &newQuery )
{
  d->query = newQuery;
}
