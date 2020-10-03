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

#ifndef KITEN_DICTQUERY_H
#define KITEN_DICTQUERY_H

#include <QHash>
#include <QHashIterator>
#include <QString>
#include <QStringList>

class QChar;

 /**
  * @short A class to allow users of libkiten to properly setup a database
  * query.
  *
  * In general, you either pass or parse in parameters from users or automated
  * programs to be later sent to the dictionary manager.
  *
  * This class is one of the three critical classes (along with
  * dictionary and EntryList) that are needed to use libkiten. Essentially...
  * you feed the dictionary class a DictQuery, and dictionary will return an
  * EntryList that matches the query.
  *
  * @code
  * dictionary dictManager();
  * //Load some dictionaries via dictionary class methods
  * EntryList *results;
  * DictQuery myQuery("kanji");
  * results = dictManager.doSearch(myQuery);
  * //Print results (if any)
  * @endcode
  *
  * The internal implementation understands four distinct types of data:
  * Japanese Kanji
  * Japanese Kana
  * English Characters
  * Property Pairs of the form \<i\>name\</i\>:\<i\>value\</i\>
  *
  * It is left up to the individual dictionary types to parse these values
  * for matching and appropriateness to each dictionary.
  * You can use the setDictionaries() method to narrow the range of the
  * dictionaries that it will apply to.
  *
  * A DictQuery object can be considered an "unordered set" of values.
  * When testing for equality or comparison, each property or text entry above
  * is seen as a unique item in a set. Order is not important for these
  * operations. The object will attempt to preserve the order from parsed
  * strings, but it does not consider this order to be important.
  *
  * In general, application level programs are expected to use the
  * QString based interfaces, and dictionary implementations and other
  * parts of libkiten are expected to use the direct accessors and mutators,
  * although specialized interfaces (such as kitenradselect) may use property
  * mutators for a limited set of properties. (in this case, radicals)
  *
  * The query string input is of the following format:
  * &lt;QS&gt; ::= &lt;M&gt;DictQuery::mainDelimiter&lt;QS&gt;|&lt;R&gt;DictQuery::mainDelimiter&lt;QS&gt;|
  *       &lt;O&gt;DictQuery::mainDelimiter&lt;QS&gt;|NULL
  * &lt;M&gt;  ::= kana&lt;M&gt;|kana
  * &lt;R&gt;  ::= character&lt;R&gt;|character
  * &lt;O&gt;  ::= &lt;C&gt;DictQuery::propertySeparator&lt;D&gt;
  * &lt;C&gt;  ::= character&lt;C&gt;|character
  * &lt;D&gt;  ::= character&lt;D&gt;|character
  *
  * @author Joseph Kerian \<jkerian@gmail.com>
  */
class DictQuery
{
public:
  /**
   * This is the main delimiter that the DictQuery uses when parsing strings.
   * It is set to "space" at the moment.
   */
  static const QString mainDelimiter;
  /**
   * This is the delimiter that DictQuery uses when parsing property strings
   * of the form <i>strokes:4</i>. It is set to ":" at the moment.
   */
  static const QString propertySeperator;
  /**
    * Normal constructor.
    *
    * This will create an empty query object.
    */
  DictQuery();
  /**
    * Constructor with a given QString.
    *
    * @param str the QString will be parsed as described below in operator=(const QString&)
    */
  explicit DictQuery( const QString& str );
  /**
   * Copy constructor
   *
   * @param orig the original DictQuery to be copied
   */
  DictQuery( const DictQuery& orig );
  /**
   * Destructor
   */
  ~DictQuery();

  /**
   * @return true if the DictQuery is completely empty
   */
  bool isEmpty() const;
  /**
   * Removes all text/entries from the DictQuery
   */
  void clear();
  /**
   * The assignment copy operator
   */
  DictQuery &operator=( const DictQuery &old );
  /**
   * The clone method
   */
  DictQuery *clone() const;
  /**
   * This returns a QString that represents the query. This may be the same
   * as the original string, but some slight changes may have occurred if you
   * have done any manipulations on the DictQuery.
   */
  const QString toString() const;
  /**
   * This is a conversion to a QString... useful in a surprising
   * number of cases.
   */
  operator QString() const;

  /**
   * Use this to get a list of all the property keys in the query
   */
  const QList<QString> listPropertyKeys() const;
  /**
   * Returns a given extended attribute
   */
  const QString operator[]( const QString &key ) const;
  /**
   * Sets a given extended attribute
   */
  QString operator[]( const QString &key );
  /**
   * Get a specific property by key (is the same as using operator[] const)
   */
  QString getProperty( const QString &key ) const;
  /**
   * Verify if a given DictQuery object has a search parameter of a
   * particular property.
   */
  bool hasProperty( const QString &key ) const;
  /**
   * Set a particular property... this does significantly more error checking
   * than the operator[] version, and will return false if there was a
   * problem (an empty value or bad key)
   *
   * @param key the key for this entry
   * @param value the value to set this to, will overwrite the current contents of this location
   *
   * @returns false on failure
   */
  bool setProperty( const QString& key, const QString& value );
  /**
   * Remove all instances of a property.
   *
   * @returns true if the DictQuery had properties of the given type
   */
  bool removeProperty( const QString &key );
  /**
   * Returns and removes the property
   */
  QString takeProperty( const QString &key );

  /**
   * Returns a list of the dictionaries that this particular query
   * will target. An empty list (the default) will search all dictionaries
   * that the user has selected.
   */
  QStringList getDictionaries() const;
  /**
   * Set the list of dictionaries to search. This will be read and used
   * by the dictionary manager.
   */
  void setDictionaries( const QStringList &newDictionaries );

  /**
   * Accessor for the non-japanese meaning field
   */
  QString getMeaning() const;
  /**
   * Mutator for the Meaning field
   */
  bool setMeaning( const QString &newMeaning );
  /**
   * Accessor for the Pronunciation field (generally kana)
   */
  QString getPronunciation() const;
  /**
   * Mutator for the Pronunciation field
   */
  bool setPronunciation( const QString &newPronunciation );
  /**
   * Accessor for the Word/Kanji field (this is usually used for anything
   * containing kanji).
   */
  QString getWord() const;
  /**
   * Mutator for the Word/Kanji field. If what you are setting contains
   * only kana, consider using the setPronunciation instead.
   */
  bool setWord( const QString &newWord );

  /**
   * A simple setwise comparison of two DictQuery objects
   * Note that order is not important here... only each element
   * that is one of the DictQuery objects appears in the other.
   */
  friend bool operator==( const DictQuery &a, const DictQuery &b );
  /**
   * Convenient inverted override of operator==( DictQuery, DictQuery )
   */
  friend bool operator!=( const DictQuery &other, const DictQuery &query );
  /**
   * Set-wise strictly less than. A better way to think of this
   * might be the "subset" operator
   */
  friend bool operator<( const DictQuery &a, const DictQuery &b );
  /**
   * Convenient override of operator<( DictQuery, DictQuery ) and operator==
   */
  friend bool operator<=( const DictQuery &a, const DictQuery &b );
  /**
   * This will append the properties and other elements of the added kanji
   * onto the elements of the current element. If regenerated as a string,
   * it should look something like concatenation
   */
  DictQuery &operator+=( const DictQuery &old );
  /**
   * A simple string parser, look above for examples and explanations
   */
  DictQuery &operator=( const QString &old );
  /**
   * A simple override of operator+=( const DictQuery& )
   */
  DictQuery &operator+=( const QString &old );
  /**
   * Simple addition... similar to operator+=
   */
  friend DictQuery operator+( const DictQuery &a, const DictQuery &b );
#ifndef QT_NO_CAST_ASCII
  /**
   * An ascii cast variant of the operator=
   * Only available if QT_NO_CAST_ASCII is not defined on lib compilation
   */
  DictQuery &operator=( const char* );
#endif

   //Specify the type of matching
  /**
   * This enum is used to define the type of matching this query is supposed
   * to do. The names are fairly self-explanatory
   */
  enum MatchType
  {
    Exact,
    Beginning,
    Ending,
    Anywhere
  };
  /**
   * Get which match type is currently set on the DictQuery.
   */
  MatchType getMatchType() const;
  /**
   * Set a match type. If this is not called, the default is matchExact.
   */
  void setMatchType( MatchType newType );

  /**
   * This enum is used to define the type of matching this query is supposed
   * to do.
   */
  enum MatchWordType
  {
    Any,
    Verb,
    Noun,
    Adjective,
    Adverb,
    Prefix,
    Suffix,
    Expression
  };
  /**
   * Get which word type is currently set on the DictQuery.
   */
  MatchWordType getMatchWordType() const;
  /**
   * Set a word type. If this is not called, the default value is 'Any'.
   */
  void setMatchWordType( MatchWordType newType );

  enum FilterType
  {
    NoFilter,
    Rare,
    CommonUncommon
  };
  /**
   * Get which filter is currently set on the DictQuery.
   */
  FilterType getFilterType() const;
  /**
   * Set whether or not the query should output results separated in
   * common and uncommon sections.
   */
  void setFilterType( FilterType newType );

  /**
   * This enum is used as the return type for the two utility functions,
   * stringTypeCheck and charTypeCheck.
   */
  enum StringTypeEnum
  {
    Kanji,
    Kana,
    Latin,
    Mixed,
    ParseError
  };
  /**
   * A simple utility routine to tell us what sort of string we have
   * If the string contains only kanji, kana or non-kanji/kana characters, the result is strTypeKanji,
   * strTypeKana or strTypeLatin (perhaps a misnomer... but so far it's valid).
   * If the string contains both kanji and kana, the type returned is strTypeKanji
   * If the string contains any other combination, the return type is mixed.
   */
  static StringTypeEnum stringTypeCheck( const QString &in );
  /**
   * This utility does the same thing for QChar as stringTypeCheck does for QString. At the moment
   * the implementation is rather simple, and it assumes that anything that is not latin1 or kana is
   * a kanji.
   */
  static StringTypeEnum charTypeCheck( const QChar &ch );

private:
  class Private;
  Private* const d;
};

//Currently... KDE doesn't seem to want to use exceptions
#ifdef LIBKITEN_USING_EXCEPTIONS
class InvalidQueryException
{
  public:
    InvalidQueryException( QString x ) { m_val = x; }
    InvalidQueryException( QString m = "Invalid Query String", QString x ) { m_val = x; m_msg = m; }
    QString value() { return m_val; }
    QString message() { return m_msg; }

  protected:
    QString m_val;
    QString m_msg;
};
#endif

#endif
