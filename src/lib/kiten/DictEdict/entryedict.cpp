/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
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

#include "entryedict.h"

#include "dictfileedict.h"
#include "../kitenmacros.h"

#include <QDebug>

#define QSTRINGLISTCHECK(x) (x==NULL?QStringList():*x)

EntryEdict::EntryEdict( const QString &dict )
: Entry( dict )
{
}

EntryEdict::EntryEdict( const QString &dict, const QString &entry )
: Entry( dict )
{
  loadEntry( entry );
}

Entry* EntryEdict::clone() const
{
  return new EntryEdict( *this );
}

/**
 * Regenerate a QString like the one we got in loadEntry()
 */
QString EntryEdict::dumpEntry() const
{
  QString readings = QString( Readings.isEmpty() ? QStringLiteral(" ") : " [" + Readings.first() + "] " );

  return QStringLiteral( "%1%2/%3/" ).arg( Word )
                              .arg( readings )
                              .arg( Meanings.join( QLatin1Char('/') ) );
}

QString EntryEdict::getDictionaryType() const
{
  return EDICT;
}

QString EntryEdict::getTypes() const
{
  return m_types.join( outputListDelimiter );
}

QStringList EntryEdict::getTypesList() const
{
  return m_types;
}

bool EntryEdict::isAdjective() const
{
  foreach( const QString &type, EdictFormatting::Adjectives )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isAdverb() const
{
  foreach( const QString &type, EdictFormatting::Adverbs )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isCommon() const
{
  return getExtendedInfoItem( QStringLiteral( "common" ) ) == QLatin1Char('1');
}

bool EntryEdict::isExpression() const
{
  foreach( const QString &type, EdictFormatting::Expressions )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isFukisokuVerb() const
{
  foreach( const QString &type, EdictFormatting::FukisokuVerbs )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isGodanVerb() const
{
  foreach( const QString &type, EdictFormatting::GodanVerbs )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isIchidanVerb() const
{
  foreach( const QString &type, EdictFormatting::IchidanVerbs )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isNoun() const
{
  foreach( const QString &type, EdictFormatting::Nouns )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isParticle() const
{
  return m_types.contains( EdictFormatting::Particle );
}

bool EntryEdict::isPrefix() const
{
  foreach( const QString &type, EdictFormatting::Prefix )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isSuffix() const
{
  foreach( const QString &type, EdictFormatting::Suffix )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

bool EntryEdict::isVerb() const
{
  foreach( const QString &type, EdictFormatting::Verbs )
  {
    if( m_types.contains( type ) )
    {
      return true;
    }
  }

  return false;
}

QString EntryEdict::HTMLWord() const
{
  return QStringLiteral( "<span class=\"Word\">%1</span>" )
             .arg( Word.isEmpty() ? kanjiLinkify( Meanings.first() ) : kanjiLinkify( Word ) );
}

/**
 * Makes a link out of each kanji in @param inString
 */
QString EntryEdict::kanjiLinkify( const QString &inString ) const
{
  QString outString;

  for( int i = 0; i < inString.length(); i++ )
  {
    if( isKanji( inString.at( i ) ) )
    {
      outString += makeLink( QString( inString.at( i ) ) );
    }
    else
    {
      outString += inString.at( i );
    }
  }

  return outString;
}

/**
 * Take a QString and load it into the Entry as appropriate
 * The format is basically: KANJI [KANA] /(general information) gloss/gloss/.../
 * Note that they can rudely place more (general information) in gloss's that are
 * not the first one.
 */
bool EntryEdict::loadEntry( const QString &entryLine )
{
  /* Set tempQString to be the reading and word portion of the entryLine */
  int endOfKanjiAndKanaSection = entryLine.indexOf( '/' );
  if( endOfKanjiAndKanaSection == -1 )
  {
    return false;
  }
  QString tempQString = entryLine.left( endOfKanjiAndKanaSection );
  /* The actual Word is the beginning of the line */
  int endOfKanji = tempQString.indexOf( ' ' );
  if( endOfKanji == -1 )
  {
    return false;
  }
  Word = tempQString.left( endOfKanji );

  /* The Reading is either Word or encased in '[' */
  Readings.clear();
  int startOfReading = tempQString.indexOf( '[' );
  if( startOfReading != -1 )  // This field is optional for EDICT (and kiten)
  {
    Readings.append( tempQString.left( tempQString.lastIndexOf( ']' ) ).mid( startOfReading + 1 ) );
  }
  /* TODO: use this code or not?
  * app does not handle only reading and no word entries
  * very well so far
  else
  {
    Readings.append(Word);
    Word.clear();
  }
  */

  /* set Meanings to be all of the meanings in the definition */
  QString remainingLine = entryLine.mid( endOfKanjiAndKanaSection );
  //Trim to last '/'
  remainingLine = remainingLine.left( remainingLine.lastIndexOf( '/' ) );
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
  Meanings = remainingLine.split( '/', QString::SkipEmptyParts );
#else
  Meanings = remainingLine.split( '/', Qt::SkipEmptyParts );
#endif

  if( Meanings.size() == 0 )
  {
    return false;
  }

  if( Meanings.last() == QLatin1String("(P)") )
  {
    ExtendedInfo[ QStringLiteral( "common" ) ] = QStringLiteral("1");
    Meanings.removeLast();
  }

  QString firstWord = Meanings.first();
  QStringList stringTypes;

  //Pulls the various types out
  //TODO: Remove them from the original string
  for ( int i = firstWord.indexOf( QLatin1Char('(') );
        i != -1;
        i = firstWord.indexOf( QLatin1Char('('), i + 1 ) )
  {
    QString parentheses = firstWord.mid( i + 1, firstWord.indexOf( QLatin1Char(')'), i ) - i - 1 );
    stringTypes += parentheses.split( ',' );
  }

  foreach( const QString &str, stringTypes )
  {
    if( EdictFormatting::PartsOfSpeech.contains( str ) )
    {
      m_types += str;
    }
    else if( EdictFormatting::FieldOfApplication.contains( str ) )
    {
      ExtendedInfo[ QStringLiteral("field") ] = str;
    }
    else if( EdictFormatting::MiscMarkings.contains( str ) )
    {
      m_miscMarkings += str;
    }
  }

  return true;
}

bool EntryEdict::matchesWordType( const DictQuery &query ) const
{
  if( ! query.isEmpty() )
  {
    if( query.getMatchWordType() == DictQuery::Verb
        && isVerb() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Noun
        && isNoun() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Adjective
        && isAdjective() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Adverb
        && isAdverb() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Expression
        && isExpression() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Prefix
        && isPrefix() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Suffix
        && isSuffix() )
    {
      return true;
    }
    if( query.getMatchWordType() == DictQuery::Any )
    {
      return true;
    }
  }

  return false;
}

/**
 * Returns a HTML version of an Entry
 */
QString EntryEdict::toHTML() const
{
  QString result = QStringLiteral( "<div class=\"%1\">" ).arg( EDICT.toUpper() );
  if( isCommon() )
  {
    result += QLatin1String("<div class=\"Common\">");
  }

  foreach( const QString &field, QSTRINGLISTCHECK( DictFileEdict::displayFields ) )
  {
    if( field == QLatin1String("--NewLine--") )		 result += QLatin1String("<br>");
    else if( field == QLatin1String("Word/Kanji") ) result += HTMLWord()+' ';
    else if( field == QLatin1String("Meaning") )		 result += HTMLMeanings()+' ';
    else if( field == QLatin1String("Reading") )		 result += HTMLReadings()+' ';
    else qDebug() << "Unknown field: " << field;
  }

  if( isCommon() )
  {
    result += QLatin1String("</div>");
  }

  result += QLatin1String("</div>");
  return result;
}



#ifdef KITEN_EDICTFORMATTING

/**
 * The basic idea of this is to provide a mapping from possible entry types to
 * possible things the user could enter. Then our code for the matching entry can simply
 * use this mapping to determine if a given entry could be understood to match the user's input.
 *
 * There are two basic approaches we could take:
 *   Convert the user's entry into a list of types, see if the Entry type matches any of
 *           the conversions from this list (the list comparisons will be MANY enums).
 *   Convert our Entry types to a list of acceptable string aliases. Then compare the
 *           user's input to this list (the list will be a relatively small list of strings).
 *
 * My gut instinct is that the first case (comparison of a largish list of ints) will be
 * faster, and so that's the one that's implemented here.
 *
 * The following are the minimum list of case-insensitive aliases that the user could enter:
 *   noun
 *   verb:
 *     ichidan
 *     godan
 *   adjective
 *   adverb
 *   particle
 *
 * Note that our File Parser will also expand to general cases, if not included already:
 * For Example: v5aru -> v5aru,v5 (so that a search for "godan" will find it)
 */
namespace EdictFormatting
{
  // Forward declarations of our functions to be used.
  QMultiHash<QString, QString> createPartOfSpeechCategories();
  QSet<QString>                createPartsOfSpeech();
  QSet<QString>                createMiscMarkings();
  QSet<QString>                createFieldOfApplication();
  QStringList                  createNounsList();
  QStringList                  createVerbsList();
  QStringList                  createExpressionsList();
  QStringList                  createPrefixesList();
  QStringList                  createSuffixesList();
 
  // Private variables.
  QString noun      = "Noun";
  QString verb      = "Verb";
  QString adjective = "Adjective";
  QString adverb    = "Adverb";
  QString particle  = "Particle";
  QString ichidanVerb   = "Ichidan";
  QString godanVerb     = "Godan";
  QString fukisokuVerb  = "Fukisoku";
  QString expression = QString( "Expression" );
  QString idiomaticExpression = QString( "Idiomatic expression" );
  QString prefix = QString( "Prefix" );
  QString suffix = QString( "Suffix" );
  QString nounPrefix = QString( "Noun (used as a prefix)" );
  QString nounSuffix = QString( "Noun (used as a suffix)" );


  // Define our public variables.
  QMultiHash<QString, QString> PartOfSpeechCategories = createPartOfSpeechCategories();
  QSet<QString> PartsOfSpeech      = createPartsOfSpeech();
  QSet<QString> MiscMarkings       = createMiscMarkings();
  QSet<QString> FieldOfApplication = createFieldOfApplication();

  // PartOfSpeechCategories needs to has some values before this line.
  QStringList Nouns         = createNounsList();
  QStringList Adjectives    = PartOfSpeechCategories.values( adjective );
  QStringList Adverbs       = PartOfSpeechCategories.values( adverb );
  QStringList IchidanVerbs  = PartOfSpeechCategories.values( ichidanVerb );
  QStringList GodanVerbs    = PartOfSpeechCategories.values( godanVerb );
  QStringList FukisokuVerbs = PartOfSpeechCategories.values( fukisokuVerb );
  QStringList Verbs         = createVerbsList();
  QStringList Expressions   = createExpressionsList();
  QStringList Prefix        = createPrefixesList();
  QStringList Suffix        = createSuffixesList();
  QString     Particle      = PartOfSpeechCategories.value( particle );



  QStringList createNounsList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( noun ) );
    list.append( PartOfSpeechCategories.values( nounPrefix ) );
    list.append( PartOfSpeechCategories.values( nounSuffix ) );
    return list;
  }

  QStringList createVerbsList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( verb ) );
    list.append( IchidanVerbs );
    list.append( GodanVerbs );
    list.append( FukisokuVerbs );
    return list;
  }

  QStringList createExpressionsList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( expression ) );
    list.append( PartOfSpeechCategories.values( idiomaticExpression ) );
    return list;
  }

  QStringList createPrefixesList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( prefix ) );
    list.append( PartOfSpeechCategories.values( nounPrefix ) );
    return list;
  }

  QStringList createSuffixesList()
  {
    QStringList list;
    list.append( PartOfSpeechCategories.values( suffix ) );
    list.append( PartOfSpeechCategories.values( nounSuffix ) );
    return list;
  }

  QMultiHash<QString, QString> createPartOfSpeechCategories()
  { 
    QMultiHash<QString, QString> categories;

    // Nouns
    categories.insert( noun, QStringLiteral("n") );
    categories.insert( noun, QStringLiteral("n-adv") );
    categories.insert( noun, QStringLiteral("n-t") );
    categories.insert( noun, QStringLiteral("adv-n") );

    // Noun (used as a prefix)
    categories.insert( nounPrefix, QStringLiteral("n-pref") );

    // Noun (used as a suffix)
    categories.insert( nounSuffix, QStringLiteral("n-suf") );

    // Ichidan Verbs
    categories.insert( ichidanVerb, QStringLiteral("v1") );
    categories.insert( ichidanVerb, QStringLiteral("vz") );

    // Godan Verbs
    categories.insert( godanVerb, QStringLiteral("v5") );
    categories.insert( godanVerb, QStringLiteral("v5aru") );
    categories.insert( godanVerb, QStringLiteral("v5b") );
    categories.insert( godanVerb, QStringLiteral("v5g") );
    categories.insert( godanVerb, QStringLiteral("v5k") );
    categories.insert( godanVerb, QStringLiteral("v5k-s") );
    categories.insert( godanVerb, QStringLiteral("v5m") );
    categories.insert( godanVerb, QStringLiteral("v5n") );
    categories.insert( godanVerb, QStringLiteral("v5r") );
    categories.insert( godanVerb, QStringLiteral("v5r-i") );
    categories.insert( godanVerb, QStringLiteral("v5s") );
    categories.insert( godanVerb, QStringLiteral("v5t") );
    categories.insert( godanVerb, QStringLiteral("v5u") );
    categories.insert( godanVerb, QStringLiteral("v5u-s") );
    categories.insert( godanVerb, QStringLiteral("v5uru") );
    categories.insert( godanVerb, QStringLiteral("v5z") );

    // Fukisoku verbs
    categories.insert( fukisokuVerb, QStringLiteral("iv") );
    categories.insert( fukisokuVerb, QStringLiteral("vk") );
    categories.insert( fukisokuVerb, QStringLiteral("vn") );
    categories.insert( fukisokuVerb, QStringLiteral("vs-i") );
    categories.insert( fukisokuVerb, QStringLiteral("vs-s") );

    // Other Verbs
    categories.insert( verb, QStringLiteral("vi") );
    categories.insert( verb, QStringLiteral("vs") );
    categories.insert( verb, QStringLiteral("vt") );
    categories.insert( verb, QStringLiteral("aux-v") );

    // Adjectives
    categories.insert( adjective, QStringLiteral("adj-i") );
    categories.insert( adjective, QStringLiteral("adj-na") );
    categories.insert( adjective, QStringLiteral("adj-no") );
    categories.insert( adjective, QStringLiteral("adj-pn") );
    categories.insert( adjective, QStringLiteral("adj-t") );
    categories.insert( adjective, QStringLiteral("adj-f") );
    categories.insert( adjective, QStringLiteral("adj") );
    categories.insert( adjective, QStringLiteral("aux-adj") );

    // Adverbs
    categories.insert( adverb, QStringLiteral("adv") );
    categories.insert( adverb, QStringLiteral("adv-n") );
    categories.insert( adverb, QStringLiteral("adv-to") );

    // Particle
    categories.insert( particle, QStringLiteral("prt") );

    // Expression
    categories.insert( expression, QStringLiteral("exp") );

    // Idiomatic expression
    categories.insert( idiomaticExpression, QStringLiteral("id") );

    // Prefix
    categories.insert( prefix, QStringLiteral("pref") );

    // Suffix
    categories.insert( suffix, QStringLiteral("suf") );

    return categories;
  }

  QSet<QString> createPartsOfSpeech()
  {
    QSet<QString> category;

    category << QStringLiteral("adj-i") << QStringLiteral("adj-na") << QStringLiteral("adj-no") << QStringLiteral("adj-pn") << QStringLiteral("adj-t") << QStringLiteral("adj-f")
             << QStringLiteral("adj") << QStringLiteral("adv") << QStringLiteral("adv-n") << QStringLiteral("adv-to") << QStringLiteral("aux") << QStringLiteral("aux-v")
             << QStringLiteral("aux-adj") << QStringLiteral("conj") << QStringLiteral("ctr") << QStringLiteral("exp") << QStringLiteral("id") << QStringLiteral("int")
             << QStringLiteral("iv") << QStringLiteral("n") << QStringLiteral("n-adv") << QStringLiteral("n-pref") << QStringLiteral("n-suf") << QStringLiteral("n-t")
             << QStringLiteral("num") << QStringLiteral("pn") << QStringLiteral("pref") << QStringLiteral("prt") << QStringLiteral("suf") << QStringLiteral("v1")
             << QStringLiteral("v5") << QStringLiteral("v5aru") << QStringLiteral("v5b") << QStringLiteral("v5g") << QStringLiteral("v5k") << QStringLiteral("v5k-s")
             << QStringLiteral("v5m") << QStringLiteral("v5n") << QStringLiteral("v5r") << QStringLiteral("v5r-i") <<  QStringLiteral("v5s") << QStringLiteral("v5t")
             << QStringLiteral("v5u") << QStringLiteral("v5u-s") << QStringLiteral("v5uru") << QStringLiteral("v5z") << QStringLiteral("vz") << QStringLiteral("vi")
             << QStringLiteral("vk") << QStringLiteral("vn") << QStringLiteral("vs") << QStringLiteral("vs-i") << QStringLiteral("vs-s") << QStringLiteral("vt");

    return category;
  }

  QSet<QString> createFieldOfApplication()
  {
    QSet<QString> category;

    // Field of Application terms
    category << QStringLiteral("Buddh") << QStringLiteral("MA")   << QStringLiteral("comp") << QStringLiteral("food") << QStringLiteral("geom")
             << QStringLiteral("ling")  << QStringLiteral("math") << QStringLiteral("mil")  << QStringLiteral("physics");

    return category;
  }

  QSet<QString> createMiscMarkings()
  {
    QSet<QString> category;

    // Miscellaneous Markings (in EDICT terms)
    category << QStringLiteral("X")    << QStringLiteral("abbr") << QStringLiteral("arch") << QStringLiteral("ateji")   << QStringLiteral("chn")   << QStringLiteral("col") << QStringLiteral("derog")
             << QStringLiteral("eK")   << QStringLiteral("ek")   << QStringLiteral("fam")  << QStringLiteral("fem")     << QStringLiteral("gikun") << QStringLiteral("hon") << QStringLiteral("hum") << QStringLiteral("iK")   << QStringLiteral("id")
             << QStringLiteral("io")   << QStringLiteral("m-sl") << QStringLiteral("male") << QStringLiteral("male-sl") << QStringLiteral("ng")    << QStringLiteral("oK")  << QStringLiteral("obs") << QStringLiteral("obsc") << QStringLiteral("ok")
             << QStringLiteral("poet") << QStringLiteral("pol")  << QStringLiteral("rare") << QStringLiteral("sens")    << QStringLiteral("sl")    << QStringLiteral("uK")  << QStringLiteral("uk")  << QStringLiteral("vulg");

    return category;
  }
}

#endif
