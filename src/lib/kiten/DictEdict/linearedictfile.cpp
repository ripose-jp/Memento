/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2008 Joseph Kerian <jkerian@gmail.com>                      *
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

#include "linearedictfile.h"


#include <QFile>
#include <QDebug>
#include <QTextCodec>

LinearEdictFile::LinearEdictFile()
: m_properlyLoaded( false )
{
}

LinearEdictFile::~LinearEdictFile()
{
}

/**
 * Get everything that looks remotely like a given search string
 */
QVector<QString> LinearEdictFile::findMatches( const QString &searchString ) const
{
  QVector<QString> matches;
  foreach( const QString &it, m_edict )
  {
    if ( it.contains( searchString ) )
    {
      matches.append( it );
    }
  }

  return matches;
}

bool LinearEdictFile::loadFile( const QString& filename )
{
  qDebug() << "Loading edict from " << filename;

  //if already loaded
  if ( ! m_edict.isEmpty() )
  {
    return true;
  }

  QFile file( filename );
  if ( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    return false;
  }

  QTextStream fileStream( &file );
  fileStream.setCodec( QTextCodec::codecForName( "eucJP" ) );

  QString lastLine;
  while ( ! fileStream.atEnd() )
  {
    lastLine = fileStream.readLine();
    if ( lastLine[ 0 ] != '#' ) m_edict << lastLine;
  }

  file.close();
  m_properlyLoaded = true;

  return true;
}

bool LinearEdictFile::valid() const
{
  return m_properlyLoaded;
}
