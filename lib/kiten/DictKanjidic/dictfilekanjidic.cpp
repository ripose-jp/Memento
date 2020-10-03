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

#include "dictfilekanjidic.h"

#include "../dictquery.h"
#include "entrykanjidic.h"
#include "../entrylist.h"
#include "../kitenmacros.h"

#include <QDebug>
#include <QFile>
#include <QTextCodec>

QStringList *DictFileKanjidic::displayFields = nullptr;

DictFileKanjidic::DictFileKanjidic()
: DictFile( KANJIDIC )
{
  m_dictionaryType = KANJIDIC;
  m_searchableAttributes.clear();
  m_searchableAttributes.insert( QStringLiteral("bushu"),      QStringLiteral("B") );
  m_searchableAttributes.insert( QStringLiteral("classical"),  QStringLiteral("C") );
  m_searchableAttributes.insert( QStringLiteral("henshall"),   QStringLiteral("E") );
  m_searchableAttributes.insert( QStringLiteral("frequency"),  QStringLiteral("F") );
  m_searchableAttributes.insert( QStringLiteral("grade"),      QStringLiteral("G") );
  m_searchableAttributes.insert( QStringLiteral("halpern"),    QStringLiteral("H") );
  m_searchableAttributes.insert( QStringLiteral("spahn"),      QStringLiteral("I") );
  m_searchableAttributes.insert( QStringLiteral("hadamitzky"), QStringLiteral("I") );
  m_searchableAttributes.insert( QStringLiteral("gakken"),     QStringLiteral("K") );
  m_searchableAttributes.insert( QStringLiteral("heisig"),     QStringLiteral("L") );
  m_searchableAttributes.insert( QStringLiteral("morohashi"),  QStringLiteral("M") );
  m_searchableAttributes.insert( QStringLiteral("nelson"),     QStringLiteral("N") );
  m_searchableAttributes.insert( QStringLiteral("oneill"),     QStringLiteral("O") );
  m_searchableAttributes.insert( QStringLiteral("skip"),       QStringLiteral("P") );
  m_searchableAttributes.insert( QStringLiteral("4cc"),        QStringLiteral("Q") );
  m_searchableAttributes.insert( QStringLiteral("stroke"),     QStringLiteral("S") );
  m_searchableAttributes.insert( QStringLiteral("strokes"),    QStringLiteral("S") );
  m_searchableAttributes.insert( QStringLiteral("unicode"),    QStringLiteral("U") );
  m_searchableAttributes.insert( QStringLiteral("haig"),       QStringLiteral("V") );
  m_searchableAttributes.insert( QStringLiteral("korean"),     QStringLiteral("W") );
  m_searchableAttributes.insert( QStringLiteral("pinyin"),     QStringLiteral("Y") );
  m_searchableAttributes.insert( QStringLiteral("other"),      QStringLiteral("D") );
}

DictFileKanjidic::~DictFileKanjidic()
{
}

QMap<QString,QString> DictFileKanjidic::displayOptions() const
{
  // Enumerate the fields in our dict.... there are a rather lot of them here
  // It will be useful for a few things to have the full list generated on it's own
  QMap<QString,QString> list;
  // TODO: Figure out how to internationalize these easily
  list.insert( QStringLiteral("Bushu Number(B)"),                      QStringLiteral("B"));
  list.insert( QStringLiteral("Classical Radical Number(C)"),          QStringLiteral("C"));
  list.insert( QStringLiteral("Henshall's Index Number(E)"),           QStringLiteral("E"));
  list.insert( QStringLiteral("Frequency Ranking(F)"),                 QStringLiteral("F"));
  list.insert( QStringLiteral("Grade Level(G)"),                       QStringLiteral("G"));
  list.insert( QStringLiteral("Halpern's New J-E Char Dictionary(H)"), QStringLiteral("H"));
  list.insert( QStringLiteral("Spahn & Hadamitzky Reference(I)"),      QStringLiteral("I"));
  list.insert( QStringLiteral("Gakken Kanji Dictionary Index(K)"),     QStringLiteral("K"));
  list.insert( QStringLiteral("Heisig's Index(L)"),                    QStringLiteral("L"));
  list.insert( QStringLiteral("Morohashi's Daikanwajiten(M)"),         QStringLiteral("M"));
  list.insert( QStringLiteral("Nelsons Modern Reader's J-E Index(N)"), QStringLiteral("N"));
  list.insert( QStringLiteral("O'Neill's 'Japanese Names' Index(O)"),  QStringLiteral("O"));
  list.insert( QStringLiteral("SKIP Code(P)"),                         QStringLiteral("P"));
  list.insert( QStringLiteral("Four Corner codes(Q)"),                 QStringLiteral("Q"));
  list.insert( QStringLiteral("Stroke Count(S)"),                      QStringLiteral("S"));
  list.insert( QStringLiteral("Unicode Value(U)"),                     QStringLiteral("U"));
  list.insert( QStringLiteral("Haig's New Nelson J-E Dict(V)"),        QStringLiteral("V"));
  list.insert( QStringLiteral("Korean Reading(W)"),                    QStringLiteral("W"));
  list.insert( QStringLiteral("kanjidic field: X"),                    QStringLiteral("X"));
  list.insert( QStringLiteral("Pinyin Reading(Y)"),                    QStringLiteral("Y"));
  list.insert( QStringLiteral("Common SKIP Misclassifications(Z)"),    QStringLiteral("Z"));
  list.insert( QStringLiteral("Misc Dictionary Codes (D)"),            QStringLiteral("D"));
  return list;
}

EntryList* DictFileKanjidic::doSearch( const DictQuery &query )
{
  if( query.isEmpty() || ! m_validKanjidic )
  {
    return new EntryList();
  }

  qDebug() << "Search from:" << getName();
  QString searchQuery = query.getWord();
  if( searchQuery.length() == 0 )
  {
    searchQuery = query.getPronunciation();
    if( searchQuery.length() == 0 )
    {
      searchQuery = query.getMeaning().split( ' ' ).first().toLower();
      if( searchQuery.length() == 0 )
      {
        QList<QString> keys = query.listPropertyKeys();
        if( keys.size() == 0 )
        {
          return new EntryList();
        }
        searchQuery = keys[ 0 ];
        searchQuery = searchQuery + query.getProperty( searchQuery );
      }
    }
  }

  EntryList *results = new EntryList();
  foreach( const QString &line, m_kanjidic )
  {
    if( line.contains( searchQuery ) )
    {
      Entry *entry = makeEntry( line );
      if( entry->matchesQuery( query ) )
      {
        results->append( entry );
      }
      else delete entry;
    }
  }

  return results;
}

QStringList DictFileKanjidic::dumpDictionary()
{
  if( ! m_validKanjidic )
  {
    return QStringList();
  }

  return m_kanjidic;
}

QStringList DictFileKanjidic::listDictDisplayOptions( QStringList list ) const
{
  list += displayOptions().keys();
  return list;
}

bool DictFileKanjidic::loadDictionary( const QString &file, const QString &name )
{
  if( ! m_kanjidic.isEmpty() )
  {
    return true;
  }

  QFile dictionary( file );
  if( ! dictionary.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    return false;
  }

  qDebug() << "Loading kanjidic from:" << file;

  QTextStream fileStream( &dictionary );
  fileStream.setCodec( QTextCodec::codecForName( "eucJP" ) );

  QString currentLine;
  while( ! fileStream.atEnd() )
  {
    currentLine = fileStream.readLine();
    if( currentLine[ 0 ] != '#' )
    {
      m_kanjidic << currentLine;
    }
  }

  dictionary.close();

  if( ! validDictionaryFile( file ) )
  {
    return false;
  }

  m_dictionaryName = name;
  m_dictionaryFile = file;

  return true;
}

QMap<QString,QString> DictFileKanjidic::loadDisplayOptions() const
{
  QMap<QString,QString> list = displayOptions();
  list[ QStringLiteral("Word/Kanji") ]  = QStringLiteral("Word/Kanji");
  list[ QStringLiteral("Reading") ]     = QStringLiteral("Reading");
  list[ QStringLiteral("Meaning") ]     = QStringLiteral("Meaning");
  list[ QStringLiteral("--Newline--") ] = QStringLiteral("--Newline--");

  return list;
}

void DictFileKanjidic::loadSettings()
{
  this->displayFields = new QStringList( loadDisplayOptions().values() );
}

inline Entry* DictFileKanjidic::makeEntry( const QString &entry )
{
  return new EntryKanjidic( getName(), entry );
}

/**
 * Scan a potential file for the correct format, remembering to skip comment
 * characters. This is not a foolproof scan, but it should be checked before adding
 * a new dictionary.
 */
bool DictFileKanjidic::validDictionaryFile( const QString &filename )
{
  QFile file( filename );
  if( ! file.exists() || ! file.open( QIODevice::ReadOnly ) )
  {
    return false;
  }

  QTextStream fileStream( &file );
  fileStream.setCodec( QTextCodec::codecForName( "eucJP" ) );

  QRegExp format( "^\\S\\s+(\\S+\\s+)+(\\{(\\S+\\s?)+\\})+" );
  m_validKanjidic = true;
  while( ! fileStream.atEnd() )
  {
    QString currentLine = fileStream.readLine();

    if( currentLine[ 0 ] == '#' )
    {
      continue;
    }
    else if( currentLine.contains( format ) )
    {
      continue;
    }

    m_validKanjidic = false;
    break;
  }

  file.close();
  return m_validKanjidic;
}

/**
 * Reject queries that specify anything we don't understand
 */
bool DictFileKanjidic::validQuery( const DictQuery &query )
{
  //Multi kanji searches don't apply to this file
  if( query.getWord().length() > 1 )
  {
    return false;
  }

  //Now check if we have any properties specified that we don't understand
  QStringList propertiesWeHandle = m_searchableAttributes.values() + m_searchableAttributes.keys();
  propertiesWeHandle += QStringLiteral("common"); // We map this to be (has a G value)

  const QStringList properties = query.listPropertyKeys();
  for( QStringList::const_iterator it = properties.constBegin(); it != properties.constEnd(); ++it )
  {
    if( ! propertiesWeHandle.contains( *it ) )
    {
      return false;
    }
  }

  return true;
}
