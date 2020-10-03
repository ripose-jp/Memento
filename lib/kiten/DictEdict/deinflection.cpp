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

#include "deinflection.h"

#include "dictfileedict.h"
#include "../dictquery.h"
#include "entryedict.h"
#include "../entrylist.h"

#include <QDebug>
#include <QFile>
#include <QHash>
#include <QList>
#include <QStandardPaths>
#include <QString>
#include <QTextCodec>
#include <QTextStream>
#include <QVector>

//This is a very primative form of information hiding
//But C++ can get stupid with static QT objects...
//So this turns out to be much, much easier
//TODO: Fix this for thread safety/functionality (I'm presuming it's broken atm)

//Declare our constants
QList<Deinflection::Conjugation> *Deinflection::conjugationList = nullptr;

Deinflection::Deinflection( const QString &name )
: m_deinflectionLabel( QString() )
, m_wordType( QString() )
, m_dictionaryName( name )
{
}

QString* Deinflection::getDeinflectionLabel()
{
  return &m_deinflectionLabel;
}

QString* Deinflection::getWordType()
{
  return &m_wordType;
}

EntryList* Deinflection::search( const DictQuery &query, const QVector<QString> &preliminaryResults )
{
  if( conjugationList == nullptr )
  {
    return NULL;
  }

  m_deinflectionLabel = QString();
  m_wordType = QString();

  EntryList *entries = new EntryList();

  QStringList edictTypesList;
  edictTypesList.append( EdictFormatting::Adjectives );
  edictTypesList.append( EdictFormatting::Verbs      );

  QString edictTypes = edictTypesList.join(QLatin1Char(',') );

  foreach( const QString &item, preliminaryResults )
  {
    EntryEdict *entry = makeEntry( item );
    QStringListIterator it( entry->getTypesList() );
    bool matched = false;
    while( it.hasNext() && ! matched )
    {
      if( edictTypes.contains( it.next() ) )
      {
        entries->append( entry );
        matched = true;
      }
    }
    if (! matched) delete entry;
  }

  EntryList *results = new EntryList();
  EntryList::EntryIterator it( *entries );
  while( it.hasNext() )
  {
    EntryEdict *entry = static_cast<EntryEdict*>( it.next() );

    QString text = query.getWord();
    if( text.isEmpty() )
    {
      text = query.getPronunciation();

      if( text.isEmpty() )
      {
        entries->deleteAll();
        delete entries;
        delete results;
        return NULL;
      }
    }

    QString word = entry->getWord();
    foreach( const Deinflection::Conjugation &conj, *conjugationList )
    {
      if(    text.endsWith( conj.ending )
          && word.endsWith( conj.replace )
          && text.startsWith( word.left( word.length() - conj.replace.length() ) ) )
      {
        QString replacement = text;
        replacement.truncate( text.length() - conj.ending.length() );
        replacement += conj.replace;

        if( word == replacement )
        {
          if( m_deinflectionLabel.isEmpty() )
          {
            m_deinflectionLabel = conj.label;
          }

          if( m_wordType.isEmpty() )
          {
            if( entry->isVerb() )
            {
              m_wordType = "verb";
            }
            else if( entry->isAdjective() )
            {
              m_wordType = "adjective";
            }
          }

          results->append( entry );
          break;
        }
      }
    }
  }
  delete entries;
  return results;
}

bool Deinflection::load()
{
  if ( conjugationList != nullptr )
  {
    return true;
  }

  conjugationList = new QList<Conjugation>;

  QString vconj = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kiten/vconj"));
  
  //Find the file
  if ( vconj.isEmpty() )
  {
    qDebug() << "Verb deinflection information not found, so verb deinflection cannot be used.";
    return false;
  }

  QHash<unsigned long,QString> names;
  //Open the file
  QFile f( vconj );
  if ( ! f.open( QIODevice::ReadOnly ) )
  {
    qDebug() << "Verb deinflection information could not be loaded, so verb deinflection cannot be used.";
    return false;
  }

  QTextStream t( &f );
  t.setCodec( QTextCodec::codecForName( "eucJP" ) );

  //The file starts out with a number -> name list of the conjugation types
  //In the format "#[#]  NAME\n"
  //The next section beginning is flagged with a $ at the beginning of the line
  for( QString text = t.readLine(); ! t.atEnd() && text.at( 0 ) != '$';
            text = t.readLine() )
  {
    if( text.at( 0 ) != '#' )
    {
      unsigned long number = text.left( 2 ).trimmed().toULong();
      QString name = text.right( text.length() - 2 ).trimmed();
      names[ number ] = name;
    }
  }

  //Now for the actual conjugation data
  //Format is "NUMBER_FROM_LIST_ABOVE  ENDING_TO_REPLACE\n"
  QString replacement = QString();
  for( QString text = t.readLine(); ! t.atEnd(); text = t.readLine() )
  {
    if( ! text.isEmpty() && text.at( 0 ) == '$' )
    {
      replacement = text.right( 1 ).trimmed();
    }
    else if( ! text.trimmed().isEmpty() && text.at( 0 ) != '#' )
    {
      unsigned long labelIndex = text.section( ' ', 0, 1 ).trimmed().toULong();

      Conjugation conj;
      conj.label   = names.value( labelIndex );
      conj.ending  = text.section( ' ', 2 ).trimmed();
      conj.replace = replacement;

      conjugationList->append( conj );
    }
  }

  f.close();

  return true;
}

inline EntryEdict* Deinflection::makeEntry( const QString &entry )
{
  return new EntryEdict( m_dictionaryName, entry );
}
