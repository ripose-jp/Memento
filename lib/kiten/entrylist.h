/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
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

#ifndef KITEN_ENTRYLIST_H
#define KITEN_ENTRYLIST_H

#include <QList>
#include <QString>
#include <QStringList>

#include "dictquery.h"
#include "entry.h"

/**
 * EntryList is a simple container for Entry objects, and is-a QList<Entry*>
 * A few simple overrides allow you to deal with sorting and translating.
 */
class EntryList : public QList<Entry*>
{
  public:
    /**
     * A simple overridden iterator for working with the Entries
     */
    typedef QListIterator<Entry*> EntryIterator;

    /**
     * Basic constructor, create an empty EntryList
     */
    EntryList();
    /**
     * Copy constructor
     */
    EntryList( const EntryList &old );
    /**
     * Basic Destructor, does not delete Entry* objects. Please remember to call
     * deleteAll() before deleting an EntryList.
     */
    virtual ~EntryList();
    /**
     * Delete all Entry objects in our list. In the future, we'll switch to a reference
     * counting system, and this will be deprecated.
     */
    void deleteAll();

    /**
     * Convert every element of the EntryList to a QString and return it
     */
    QString toString() const;
    /**
     * Convert every element of the EntryList to a QString in HTML form and return it
     */
    QString toHTML() const;

    /**
     * Convert a given range of the EntryList to a QString and return it
     * @param start the location in the list where we should start
     * @param length the length of the list we should generate
     */
    QString toString( unsigned int start, unsigned int length ) const;
    /**
     * Convert a given range of the EntryList to a QString in HTML form and return it
     * @param start the location in the list where we should start
     * @param length the length of the list we should generate
     */
    QString toHTML( unsigned int start, unsigned int length ) const;
    /**
     * Convert the entire list to KVTML for export to a flashcard app
     * @param start the location in the list where we should start
     * @param length the length of the list we should generate
     */
    QString toKVTML( unsigned int start, unsigned int length ) const;

    /**
     * Sort the list according to the given fields in sortOrder, if dictionaryOrder
     * is blank, don't order the list by dictionary, otherwise items are sorted by dictionary
     * then by sortOrder aspects
     * @param sortOrder the keys to sort by, this should be a list of fields to sort by, this should
     *        be the same as the fields that are returned from dictFile::listDictDisplayOptions().
     *        "--NewLine--" entries will be ignored, "Word/Kanji", "Meaning", and "Reading" entries will
     *        be accepted. An entry which has an extended attribute is considered higher ranking (sorted to
     *        a higher position) than an entry which does not have such an attribute.
     * @param dictionaryOrder the order for the Entry objects to be sorted in, dictionary-wise. This should
     *        match the names of the dictionary objects, passed to the DictionaryManager.
     */
    void sort( QStringList &sortOrder, QStringList &dictionaryOrder );

    /**
     * Append another EntryList onto this one
     */
    const EntryList& operator+=( const EntryList &other );
    /**
     * Copy an entry list
     */
    const EntryList& operator=( const EntryList &other );
    /**
     * Append another EntryList onto this one
     */
    void appendList( const EntryList *other );
    /**
     * Get the query that generated this list, note that if you have appended EntryLists from
     * two different queries, the resulting DictQuery from this is undefined.
     */
    DictQuery getQuery() const;
    /**
     * Set the query for this list.  See getQuery() for a potential problem with this
     */
    void setQuery( const DictQuery &newQuery );

    int scrollValue() const;
    void setScrollValue( int val );

  private:
    class Private;
    Private* const d;
};

#endif
