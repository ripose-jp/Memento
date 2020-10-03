/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
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

#include "historyptrlist.h"

#include "entrylist.h"

#include <QList>

class HistoryPtrList::Private
{
  public:
    Private() : index( -1 ) {}

    static const int  max_size = 20;
    int               index;
    QList<EntryList*> list;
};

HistoryPtrList::HistoryPtrList()
: d( new Private )
{
}

HistoryPtrList::~HistoryPtrList()
{
  for( int i = d->list.size() - 1; i >= 0; i-- )
  {
    d->list.at( i )->deleteAll();
    delete d->list.at( i );
  }

  delete d;
}

void HistoryPtrList::addItem( EntryList *newItem )
{
  if( ! newItem ) return;
  //If we're currently looking at something prior to the end of the list
  //Remove everything in the list up to this point.
  int currentPosition = d->index + 1;
  EntryList *temp;
  while( currentPosition < count() )
  {
    temp = d->list.takeLast();
    temp->deleteAll();
    delete temp;
  }

  //Now... check to make sure our history isn't 'fat'
  while( count() >= d->max_size )
  {
    temp = d->list.takeFirst();
    temp->deleteAll();
    delete temp;
  }
  d->index = count()-1; //Since we have trimmed down to the current position

  //One other odd case... if this query is a repeat of the last query
  //replace the current one with the new one
  if( d->list.size() > 0 )
  {
    if( current()->getQuery() == newItem->getQuery() )
    {
      temp = d->list.takeLast();
      temp->deleteAll();
      delete temp;
    }
  }
  //Now add the item
  d->list.append( newItem );
  d->index = count() - 1;
}

int HistoryPtrList::count()
{
  return d->list.size();
}

EntryList* HistoryPtrList::current()
{
  if( d->index == -1 )
  {
    return NULL;
  }

  return d->list.at( d->index );
}

int HistoryPtrList::index()
{
  return d->index;
}

void HistoryPtrList::next( int distance )
{
  if( distance + d->index > count() - 1 )
  {
    d->index = count() - 1;
  }
  else
  {
    d->index += distance;
  }
}

void HistoryPtrList::prev(int distance)
{
  if( d->index - distance < 0 )
  {
    d->index = 0;
  }
  else
  {
    d->index -= distance;
  }
}

void HistoryPtrList::setCurrent( int i )
{
  if( i<count() && i >= 0 )
  {
    d->index = i;
  }
}

//Get a StringList of the History Items
QStringList HistoryPtrList::toStringList()
{
  QStringList result;

  foreach( const EntryList *p, d->list )
  {
    result.append( p->getQuery().toString() );
  }

  return result;
}

QStringList HistoryPtrList::toStringListNext()
{
  HistoryPtrList localCopy( *this );

  int currentPosition = d->index + 1;
  while( currentPosition-- )
  {
    localCopy.d->list.removeFirst();
  }

  return localCopy.toStringList();
}

QStringList HistoryPtrList::toStringListPrev()
{
  QStringList result;

  for( int i = 0; i < d->index; i++ )
  {
    result.append( d->list.at( i )->getQuery().toString() );
  }

  return result;
}
