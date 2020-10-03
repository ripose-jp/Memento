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

#ifndef KITEN_HISTORYPTRLIST_H
#define KITEN_HISTORYPTRLIST_H

#include <QStringList>

class EntryList;

class HistoryPtrList
{
  public:
    /**
     * Construct a HistoryPtrList, this should be done early on
     */
    HistoryPtrList();
    /**
     * Destructor... kill all lists before going
     */
    virtual ~HistoryPtrList();
    /**
     * Add an item to the end of the history list and set it as
     * the current displayed item.
     */
    void addItem( EntryList *newItem );
    /**
     * Return a list of the entries. Note that this is usually
     * just a QStringList of all of the EntryList's DictQuery->toString() calls.
     */
    QStringList toStringList();
    /**
     * Return a list of the entries prior to the current one (not including
     * the current entry.
     */
    QStringList toStringListPrev();
    /**
     * Return a summary list that only includes those after the current
     */
    QStringList toStringListNext();
    /**
     * Add one to the current location, convenient for 'forward' buttons
     */
    void next( int distance = 1 );
    /**
     * Sub one from the current location, the counterpart to next()
     */
    void prev( int distance = 1 );
    /**
     * Return the current numerical 0-based location
     */
    int index();
    /**
     * Return the item at the location given by the parameter, and set it
     * to be the current history list item.
     */
    /**
     * Return the current item
     */
    EntryList *current();
    /**
     * Set the current item
     */
    void setCurrent( int i );
    /**
     * Return the total number of items in the list
     */
    int count();

  private:
    class Private;
    Private* const d;
};

#endif
