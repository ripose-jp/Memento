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

#include "entrykanjidic.h"

#include "dictfilekanjidic.h"
#include "../kitenmacros.h"

#include <QDebug>

#define QSTRINGLISTCHECK(x) (x==NULL?QStringList():*x)

EntryKanjidic::EntryKanjidic( const EntryKanjidic &dict )
: Entry( dict )
{
}

EntryKanjidic::EntryKanjidic( const QString &dict )
: Entry( dict )
{
}

EntryKanjidic::EntryKanjidic( const QString &dict, const QString &entry )
: Entry( dict )
{
  loadEntry( entry );
}

QString EntryKanjidic::addReadings( const QStringList &list ) const
{
  QString readings;
  foreach( const QString &reading, list )
  {
    readings.append( makeReadingLink( reading ) + outputListDelimiter );
  }

  return readings;
}

Entry* EntryKanjidic::clone() const
{
  return new EntryKanjidic( *this );
}

/**
 * This reproduces a kanjidic-formatted line from the Entry.
 *  Look at the above parser to see how the format works.
 */
QString EntryKanjidic::dumpEntry() const
{
  /* Loop over the ExtendedInfo to add it to the line we produce */
  QString dumpExtendedInfo;
  QHash<QString,QString>::const_iterator it;
  for( it = ExtendedInfo.constBegin(); it != ExtendedInfo.constEnd(); ++it )
  {
    dumpExtendedInfo += ' ' + it.key() + it.value();
  }

  return QStringLiteral( "%1 %2%3" ).arg( Word )
                             .arg( Readings.join( QLatin1Char(' ') ) )
                             .arg( dumpExtendedInfo );
}

bool EntryKanjidic::extendedItemCheck( const QString &key, const QString &value ) const
{
  if( key == QLatin1String("common") )
  {
    return ! getExtendedInfoItem( QStringLiteral("G") ).isEmpty();
  }

  return Entry::extendedItemCheck( key, value );
}

QString EntryKanjidic::getAsRadicalReadings() const
{
  return AsRadicalReadings.join( outputListDelimiter );
}

QStringList EntryKanjidic::getAsRadicalReadingsList() const
{
  return AsRadicalReadings;
}

QString EntryKanjidic::getDictionaryType() const
{
  return KANJIDIC;
}

QString EntryKanjidic::getInNamesReadings() const
{
  return InNamesReadings.join( outputListDelimiter );
}

QStringList EntryKanjidic::getInNamesReadingsList() const
{
  return InNamesReadings;
}

QString EntryKanjidic::getKanjiGrade() const
{
  return getExtendedInfoItem( QStringLiteral("G") );
}

QString EntryKanjidic::getKunyomiReadings() const
{
  return KunyomiReadings.join( outputListDelimiter );
}

QStringList EntryKanjidic::getKunyomiReadingsList() const
{
  return KunyomiReadings;
}

QString EntryKanjidic::getOnyomiReadings() const
{
  return OnyomiReadings.join( outputListDelimiter );
}

QStringList EntryKanjidic::getOnyomiReadingsList() const
{
  return OnyomiReadings;
}

QString EntryKanjidic::getStrokesCount() const
{
  return getExtendedInfoItem( QStringLiteral("S") );
}

QString EntryKanjidic::HTMLExtendedInfo( const QString &field ) const
{
  //qDebug() << field;
  return QStringLiteral( "<span class=\"ExtendedInfo\">%1: %2</span>" )
             .arg( field )
             .arg( ExtendedInfo[ field ] );
}

/**
 * Prepares Readings for output as HTML
 */
QString EntryKanjidic::HTMLReadings() const
{
  QString htmlReadings;
  htmlReadings += addReadings( originalReadings );

  if( InNamesReadings.count() > 0 )
  {
    htmlReadings += "In names: ";
    htmlReadings += addReadings( InNamesReadings );
  }

  if( AsRadicalReadings.count() > 0 )
  {
    htmlReadings += "As radical: ";
    htmlReadings += addReadings( AsRadicalReadings );
  }

  // get rid of last ,
  htmlReadings.truncate( htmlReadings.length() - outputListDelimiter.length() );
  return QStringLiteral( "<span class=\"Readings\">%1</span>" ).arg( htmlReadings );
}

QString EntryKanjidic::HTMLWord() const
{
  return QStringLiteral( "<span class=\"Word\">%1</span>" ).arg( makeLink( Word ) );
}

/**
 * Fill the fields of our Entry object appropriate to the given
 * entry line from Kanjidic.
 */
/* TODO: Error checking */
bool EntryKanjidic::loadEntry( const QString &entryLine )
{
  unsigned int length = entryLine.length();

  /* The loop would be a bit faster if we first grabbed the kanji (2 bytes) and then the
     space that follows, etc. for the fixed-space portion of the entries let's try that.
     First the first 2 bytes are guaranteed to be our kanji.  The 3rd byte is a space.
     The 4th through 7th are an ascii representation of the JIS code.  One more space
     Currently, kana are not detected so readings are anything that is not otherwise
     in the 8th position. */
  Word = entryLine.left( 1 );
  //	QString strjis = raw.mid( 2, 4 );

  /* variables for the loop */
  QChar ichar;
  QString curString;

  /* we would need to do these exact things ... many times so here now. */
  #define INCI if(i < length) \
  { \
    i++; \
    ichar = entryLine.at(i); \
  }
  #define LOADSTRING(stringToLoad) while(entryLine.at(i) != ' ') \
  { \
    stringToLoad += entryLine.at(i); \
    if(i < length) i++; \
    else break; \
  }

  //	qDebug() << "LOADSTRING: '" << stringToLoad << "'";

  /* We can start looping at 8 because we have guarantees about the initial
     data.  This loop is used because the kanjidic format allows the data
     to be in any order until the end of the line.  The format was designed
     such that the data can be identified by the first byte. */
  for ( unsigned int i = 7; i < length - 1; i++ )
  {
      ichar = entryLine.at( i );

      curString = QLatin1String("");
      switch( ichar.unicode() )
      {
        case ' ':
          /* as far as I can tell, there is no real rule forcing only 1 space so
                  there's not really any significance to them.  This block is not
                  reached in kanjidic itself. */
          break;
        case 'B':
          /* the radical, or busyu, number */
        case 'C':
          /* the classical radical number, usually doesn't differ from busyu number */
        case 'E':
          /* Henshell's "A Guide To Remembering Japanese Characters" index number */
        case 'F':
          /* frequency ranking */
        case 'G':
          /* grade level Jouyou 1 - 6 or 8 for common use or 9 for Jinmeiyou */
        case 'H':
          /* number from Halpern's New Japanese-English Character Dictionary */
        case 'K':
          /* Gakken Kanji Dictionary index */
        case 'L':
          /* Heisig's "Remembering The Kanji" index */
        case 'N':
          /* number from Nelson's Modern Reader's Japanese-English Character Dictionary */
        case 'O':
          /* O'Neill's "Japanese Names" index number */
        case 'P':
          /* SKIP code ... #-#-# format */
        case 'Q':
          /* Four Corner codes, it seems, can be multiple though I'm tempted just to take the last one. */
        case 'U':
          /* unicode which we are ignoring as it is found in another way */
        case 'V':
          /* number from Haig's New Nelson Japanese-English Character Dictionary */
        case 'W':
          /* korean reading */
        case 'X':
          /* I don't entirely understand this field. */
        case 'Y':
          /* Pinyin reading */
        case 'Z':
          /* SKIP misclassifications */

          /* All of the above are of the format <Char><Data> where <Char> is
                  exactly 1 character. */
          i++;
          LOADSTRING( curString );
          ExtendedInfo.insert( QString( ichar ), curString );
          break;
        case 'I':
          /* index codes for Spahn & Hadamitzky reference books we need the next
                  char to know what to do with it. */
          INCI
          if( ichar == 'N' )
          {
            /* a Kanji & Kana book number */
            LOADSTRING( curString )
          }
          else
          {
            /* The Kanji Dictionary number, we need the current ichar. */
            LOADSTRING( curString )
          }
          ExtendedInfo.insert( 'I' + QString( ichar ), curString );
          break;
        case 'M':
          /* index and page numbers for Morohashi's Daikanwajiten 2 fields possible */
          INCI
          if( ichar == 'N' )
          {
            LOADSTRING( curString )
            /* index number */
          }
          else if( ichar == 'P' )
          {
            LOADSTRING( curString )
            /* page number in volume.page format */
          }
          ExtendedInfo.insert( 'M' + QString( ichar ), curString );
          break;
        case 'S':
          /* stroke count: may be multiple.  In that case, first is actual, others common
                  miscounts */
          i++;
          if( ! ExtendedInfo.contains(QStringLiteral("S") ) )
          {
            LOADSTRING( curString )
            ExtendedInfo.insert( QString( ichar ), curString );
          }
          else
          {
            LOADSTRING( curString )
            ExtendedInfo.insert( '_' + QString( ichar ), curString );
          }
          break;
        case 'D':
          /* dictionary codes */
          INCI
          LOADSTRING( curString )
          ExtendedInfo.insert( 'D' + QString( ichar ), curString );
          break;
        case '{':
          /* This should be starting with the first '{' character of a meaning section.
                  Let us get take it to the last. */
          INCI
          while( ichar != '}' )
          {
            curString += ichar;
            /* sanity */
            if( i < length )
            {
              i++;
            }
            else
            {
              break;
            }
            ichar = entryLine.at( i );
          }
          INCI
//           qDebug() << "Meaning's curString: '" << curString << "'";
          Meanings.append( curString );
          break;
        case 'T': /* a reading that is used in names for T1, radical names for T2 */
        {
          i++;
          LOADSTRING( curString )
          // Get the type number (1 for T1, 2 for T2).
          int type = curString.toInt();
          bool finished = false;
          while( ! finished )
          {
            // Skip all whitespaces.
            INCI
            while( ichar == ' ' )
            {
              INCI
            }
            // Check if the current character is Kana.
            if( 0x3040 <= ichar.unicode() && ichar.unicode() <= 0x30FF )
            {
              // Reset our variable and load it with
              // all available kana until we find a whitespace.
              curString = QLatin1String("");
              LOADSTRING( curString );
              switch( type )
              {
                case 1: // Special reading used in names.
                  InNamesReadings.append( curString );
                  break;
                case 2: // Reading as radical.
                  AsRadicalReadings.append( curString );
                  break;
              }
            }
            else
            {
              // There are not more kana characters,
              // so we finish this loop for now.
              finished = true;
            }
          }
          // Now 'i' points to a '{' character. We decrease its value
          // so in the next loop we can reach the "case '{'" section.
          i--;
        }
        break;
        case '-':
          /* a reading that is only in postposition */
          /* any of those 2 signals a reading is to ensue. */
          LOADSTRING( curString )
          originalReadings.append( curString );

          // If it is Hiragana (Kunyomi)
          if( 0x3040 <= ichar.unicode() && ichar.unicode() <= 0x309F )
          {
            KunyomiReadings.append( curString );
          }
          // If it is Katakana (Onyomi)
          else if( 0x30A0 <= ichar.unicode() && ichar.unicode() <= 0x30FF )
          {
            OnyomiReadings.append( curString );
          }

          curString = curString.remove( '-' ).remove( '.' );
          Readings.append( curString );
          break;
        default:
          /* either a character we don't address or a problem...we should ignore it */
// 	  qDebug() << "hit default in kanji parser.  Unicode: '" << ichar.unicode() << "'";

          /* This should detect unicode kana */
          // Hiragana 0x3040 - 0x309F, Katakana: 0x30A0 - 0x30FF
          if( 0x3040 <= ichar.unicode() && ichar.unicode() <= 0x30FF )
          {
            LOADSTRING( curString )
            originalReadings.append( curString );

            // If it is Hiragana (Kunyomi)
            if( 0x3040 <=ichar.unicode() && ichar.unicode() <= 0x309F )
            {
              KunyomiReadings.append( curString );
            }
            // If it is Katakana (Onyomi)
            else if( 0x30A0 <= ichar.unicode() && ichar.unicode() <= 0x30FF )
            {
              OnyomiReadings.append( curString );
            }

            curString = curString.remove( '-' ).remove( '.' );
            Readings.append( curString );
            break;
          }
          /* if it's not a kana reading ... it is something unhandled ...
             possibly a new field in kanjidic.  Let's treat it as the
             oh-so-common <char><data> type of entry.  It could be hotly
             debated what we should actually do about these. */
          i++;
          LOADSTRING( curString );
          ExtendedInfo.insert( QString( ichar ), curString );

          break;
      }
  }
//   qDebug() << "Parsed: '"<<Word<<"' ("<<Readings.join("^")<<") \""<<
//   Meanings.join("|")<<"\ and " <<ExtendedInfo.keys() << " from :"<<entryLine<<endl;

  return true;
}

QString EntryKanjidic::makeReadingLink( const QString &inReading ) const
{
  QString reading = inReading;
  return QStringLiteral( "<a href=\"%1\">%2</a>" ).arg( reading.remove( '.' ).remove( '-' ) )
                                           .arg( inReading );
}

/**
 * Returns a HTML version of an Entry
 */
QString EntryKanjidic::toHTML() const
{
  QString result = QStringLiteral("<div class=\"KanjidicBrief\">");

  foreach( const QString &field, QSTRINGLISTCHECK( DictFileKanjidic::displayFields ) )
  {
    //qDebug() << "Display: "<<field;
    if( field == QLatin1String("--NewLine--") )              result += QLatin1String("<br>");
    else if( field == QLatin1String("Word/Kanji") )          result += HTMLWord() + ' ';
    else if( field == QLatin1String("Meaning") )             result += HTMLMeanings() + ' ';
    else if( field == QLatin1String("Reading") )             result += HTMLReadings() + ' ';
    else if( ExtendedInfo.contains( field ) ) result += HTMLExtendedInfo( field ) + ' ';
  }

  result += QLatin1String("</div>");
  return result;
}
