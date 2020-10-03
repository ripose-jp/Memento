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

#ifndef KITEN_INDEXEDEDICTFILE_H
#define KITEN_INDEXEDEDICTFILE_H

#include <QByteArray>
#include <QFile>
#include <QString>
#include <QVector>

#ifdef HAVE_INTTYPES_H
  #include <inttypes.h>
#else
  typedef unsigned int uint32_t;
#endif

/**
 * A class for managing the EDICT formatted dictionaries with their
 * dictionary files. This class is not really designed for subclassing.
 */
class /* NO_EXPORT */ IndexedEdictFile
{
  public:
    /**
     * Create and initialize this object
     */
    IndexedEdictFile();
    ~IndexedEdictFile();

    /**
     * Get everything that looks remotely like a given search string
     */
    QVector<QString> findMatches( const QString &query ) const;

    /**
     * Load a file, generate the index if it doesn't already exist
     */
    bool loadFile( const QString &fileName );

    /**
     * Test if the file was properly loaded
     */
    bool valid() const;

  private:
    bool          buildIndex();
    bool          checkIndex() const;
    int           equalOrSubstring( const char* str1, const char* str2 ) const;
    uint32_t      findFirstMatch( const QByteArray &query ) const;
    int           findMatches( const char* str1, const char* str2 ) const;
    bool          loadmmaps();
    unsigned char lookupDictChar( uint32_t i ) const;
    QByteArray    lookupDictLine( uint32_t i ) const;
    QByteArray    lookupFullLine( uint32_t i ) const;

    bool              m_valid;
    QFile             m_dictFile;
    unsigned char    *m_dictPtr;
    mutable QFile     m_indexFile;
    uint32_t         *m_indexPtr;

    static const int  indexFileVersion = 14;
};

#endif
