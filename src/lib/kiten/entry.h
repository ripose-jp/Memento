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

#ifndef KITEN_ENTRY_H
#define KITEN_ENTRY_H

#include <QHash>
#include <QStringList>

#include "dictquery.h"

class Entry;
class EntryList;
class QString;

/**
 * The Entry class is a generic base class for each particular entry in a given dictionary.
 * It's used as the basic class to ferry information back to the user application.
 * It also handles some of the display aspects.
 */
class Entry
{
  friend class EntryListModel;

  private:
    /**
      * Default constructor, should not be used. Made private to serve as a warning
      * that you're doing something wrong if you try to call this.
      */
    Entry();

  protected:
    /**
     * Copy constructor
     */
    Entry( const Entry& );
    /**
     * Constructor that includes the dictionary source. This does not need to be overridden by
     * subclasses, but you might find it to be convenient as the superclass constructor to call
     * from your constructors.
     * @param sourceDictionary the dictionary name (not fileName) that this entry originated with
     */
    Entry( const QString &sourceDictionary );
    /**
     * A constructor that includes the basic information, nicely separated
     * @param sourceDictionary the dictionary name (not fileName) that this entry originated with
     * @param word the word entry of this dictionary entry (normally kanji/kana)
     * @param readings a list of possible pronunciations for this result (kana)
     * @param meanings a list of possible meanings for this word
     */
    Entry( const QString &sourceDictionary, const QString &word,
           const QStringList &readings, const QStringList &meanings );

  public:
    /**
     * Generic Destructor
     */
    virtual ~Entry();
    /**
     * A clone method, this should just implement "return new EntrySubClass(*this)"
     */
    virtual Entry *clone() const = 0;

    /**
     * Fairly important method, this tests if this particular entry matches a query. The
     * EDICT and Kanjidic doSearch methods do an approximate match, load an Entry, and then
     * check more carefully by calling this method. This works nicely for handling searchWithinResults
     * cleanly.
     */
    virtual bool matchesQuery( const DictQuery& ) const;

    /**
     * Get the dictionary name that generated this Entry. I can't think of a reason to be changing this
     */
    QString getDictName() const;
    /**
     * Get the dictionary type (e.g. edict, kanjidic).
     */
    virtual QString getDictionaryType() const = 0;
    /**
     * Get the word from this Entry. If the entry is of type kanji/kana/meaning/etc, this will
     * return the kanji. If it is of kana/meaning/etc, it will return kana.
     */
    QString getWord() const;
    /**
     * Get a QString containing all of the meanings known, connected by the outputListDelimiter
     */
    QString getMeanings() const;
    /**
     * Simple accessor
     */
    QStringList getMeaningsList() const;
    /**
     * Simple accessor
     */
    QString getReadings() const;
    /**
     * Simple accessor
     */
    QStringList getReadingsList() const;
    /**
     * Simple accessor
     */
    QHash<QString,QString> getExtendedInfo() const;
    /**
     * Simple accessor
     * @param x the key for the extended info item to get
     */
    QString getExtendedInfoItem( const QString &x ) const;
    /**
     * Simple accessor
     * @param key the key for the extended item that is being verified
     * @param value the value it is supposed to have
     * @returns true if the key has that value, false if it is different or does not exist
     */
    virtual bool extendedItemCheck( const QString &key, const QString &value ) const;

    /**
     * An entry should be able to generate a representation of itself in (valid) HTML
     */
    virtual QString toHTML() const;
    /**
     * KVTML format for exporting
     */
    virtual QString toKVTML() const;
    /**
     * This will return a pure text interpretation of the Entry
     */
    virtual QString toString() const;

    /**
     * An entry should be able to parse an in-file representation of an entry
     * as a QString and put it back.  The latter will be useful for writing
     * to dictionaries on disk at some point.
     */
    virtual bool loadEntry( const QString& ) = 0;
    /**
     * Return a QString of an entry, as if it were dumped back into it's source file
     */
    virtual QString dumpEntry() const = 0;

    /**
     * An overrideable sorting function, similar to operator< in most contexts
     * The default version will sort by dictionary, then by fields
     *
     * @param that the second item we are comparing (this) with
     * @param dictOrder the list of dictionaries (in order) to sort
     *			     If this list is empty, the entries will not be sorted in order
     * @param fields the list of fields to sort in, uses special codes of
     *		        Reading, Meaning, Word/Kanji for those elements, all others by their
     *		        extended attribute keys.
     */
    virtual bool sort( const Entry &that, const QStringList &dictOrder,
                       const QStringList &fields ) const;
    /**
     * Overrideable sorting mechanism for sorting by individual fields.
     * The sort routine checks if the given field is equal, before calling this virtual function
     * So if this is called, you can assume that this->extendedItem(field) != that.extendedItem(field)
     *
     * @param that the second item we are comparing (this) with
     * @param field the specific extended item field that is being compared
     */
    virtual bool sortByField( const Entry &that, const QString &field ) const;

  protected:
    /**
     * The Word (usually containing kanji) that matches this entry. If you override the accessors
     * above, this has no use.
     */
    QString Word;
    /**
     * The Meanings that match this entry. If you override the accessors
     * above, this has no use.
     */
    QStringList Meanings;
    /**
     * The Readings (usually kana) that match this entry. If you override the accessors
     * above, this has no use.
     */
    QStringList Readings;
    /**
     * A hash of extended information. You may find it useful to store all sorts of details here
     */
    QHash<QString,QString> ExtendedInfo;

    /**
     * The dictionary that this entry originated at
     */
    QString sourceDict;
    /**
     * The delimiter for lists... usually space
     */
    QString outputListDelimiter;

    /**
     * This is used by the constructors to set some default values
     */
    void init();

    /**
     * Handy function for generating a link from a given QString
     */
    virtual QString makeLink( const QString &entryString ) const;
    /**
     * Return and HTML version of a word
     */
    virtual QString HTMLWord() const;
    /**
     * Return and HTML version of a reading list
     */
    virtual QString HTMLReadings() const;
    /**
     * Return and HTML version of a meaning list
     */
    virtual QString HTMLMeanings() const;

    /**
     * Handy Utility functions for matching to lists and identifying char types
     */
    bool listMatch( const QStringList &list, const QStringList &test, DictQuery::MatchType type ) const;
    /**
     * Handy Utility functions for matching to lists and identifying char types
     */
    bool isKanji( const QChar &character ) const;
};

#endif
