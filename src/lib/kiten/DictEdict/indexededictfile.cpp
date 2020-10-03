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

#include "indexededictfile.h"

#include "../xjdxgen.h"

#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QString>
#include <QTextCodec>
#include <QVector>

#include <sys/mman.h>

IndexedEdictFile::IndexedEdictFile()
: m_valid( false )
, m_dictPtr( static_cast<unsigned char*>( MAP_FAILED ) )
, m_indexPtr( static_cast<uint32_t*>( MAP_FAILED ) )
{
}

IndexedEdictFile::~IndexedEdictFile()
{
  if( m_valid )
  {
    munmap( static_cast<void*>( m_dictPtr ), m_dictFile.size() );
    munmap( static_cast<void*>( m_indexPtr ), m_indexFile.size() );
    m_dictFile.close();
    m_indexFile.close();
  }
}

//Warning: This assumes that both files are CLOSED
bool IndexedEdictFile::buildIndex()
{
  QByteArray dictFileName = m_dictFile.fileName().toLocal8Bit();
  QByteArray cacheFileName = m_indexFile.fileName().toLocal8Bit();
  char *argv[2];
  argv[0] = dictFileName.data();
  argv[1] = cacheFileName.data();
  int exitVal = __indexer_start(2, argv);
  return exitVal;
}

//Warning: This assumes that both files have already been opened
bool IndexedEdictFile::checkIndex() const
{
  //Verify the index file version and size
  uint32_t dictionaryLength = static_cast<uint32_t>( m_dictFile.size() );
  dictionaryLength++;
  uint32_t indexVersionTest;

  if( 4 == m_indexFile.read( reinterpret_cast<char*>( &indexVersionTest ), 4 ) )
  {
    if( indexVersionTest == dictionaryLength + indexFileVersion )
    {
      return true;
    }
  }

  return false;
}

/**
 * A small set of EUC formatted string comparison functions
 * that will handle katakana and hiragana as the same, and compare both
 * kanji and latin chars in the same function. Handy for a few sections
 * of the doSearch() routine.
 * This version returns 0 if the first string is equal to or the same
 * as the beginning of the second string.
 */
int IndexedEdictFile::equalOrSubstring( const char *str1, const char *str2 ) const
{
  for(unsigned i=0; ; ++i)
  {
    unsigned char c1 = static_cast<unsigned char>( str1[ i ] );
    unsigned char c2 = static_cast<unsigned char>( str2[ i ] );

    if( c1 == '\0' )
    {
      return 0;
    }

    if( ( i % 2 ) == 0 )
    {
      //on the highbyte (of kana)
      if( c2 == 0xA5 ) //Make katakana and hiragana equivalent
      {
        c2 = 0xA4;
      }
      if( c1 == 0xA5 )
      {
        c1 = 0xA4;
      }
    }

    if( ( 'A' <= c1 ) && ( c1 <= 'Z' ) )
    {
      c1 |= 0x20; // 'fix' uppercase
    }
    if( ( 'A' <= c2 ) && ( c2 <= 'Z' ) )
    {
      c2 |= 0x20;
    }

    if( c1 != c2 )
    {
      return (int)c2 - (int)c1;
    }
  }

  return 0; //silly compiler requirements
}

/**
 * Simple binary search through the index to find the query
 * Returns 0 on failure.
 */
uint32_t IndexedEdictFile::findFirstMatch( const QByteArray &query ) const
{
  int low = 0;
  int high = m_indexFile.size() / sizeof( uint32_t ) - 1;
  int cur;
  int comp = 0;

  do
  {
    cur = ( high + low ) / 2;
    comp = equalOrSubstring( query, lookupDictLine( cur ) );
    if( comp < 0 )
    {
      low = cur + 1;
    }
    if( comp > 0 )
    {
      high = cur - 1;
    }
  } while( high >= low && comp != 0 && ! ( high == 0 && low == 0 ) );

  if( comp != 0 )
  {
    return 0;
  }

  while( cur - 1 && 0 == equalOrSubstring( query,lookupDictLine( cur ) ) )
  {
    --cur;
  }

  return cur;
}

QVector<QString> IndexedEdictFile::findMatches( const QString &query ) const
{
  QVector<QString> results;
  if( ! m_valid )
  {
    return results;
  }

  QTextCodec *codec = QTextCodec::codecForName( "eucJP" );
  if( ! codec )
  {
    return results;
  }

  QByteArray searchString = codec->fromUnicode( query );
  int indexSize = m_indexFile.size() / sizeof( uint32_t );

  int matchLocation = findFirstMatch( searchString );
  QByteArray currentWord = lookupDictLine( ++matchLocation );
  if( matchLocation == 0 )
  {
    return results;
  }

  QVector<uint32_t> possibleHits;

  do
  {
    currentWord = lookupDictLine( ++matchLocation );
    int i = 0;
    while( lookupDictChar( m_indexPtr[ matchLocation - 1 ] + i - 2 ) != 0x0A )
    {
      --i;
    }
    possibleHits.push_back( m_indexPtr[ matchLocation - 1 ] + i - 1 );
  } while( matchLocation < indexSize && 0 == equalOrSubstring( searchString, currentWord ) );

  if( possibleHits.size() <= 0 )
  {
    return results;
  }

  std::sort(possibleHits.begin(), possibleHits.end());
  uint32_t last = 0;

  foreach( uint32_t it, possibleHits )
  {
    if(last != it)
    {
      last = it;
      results.push_back( codec->toUnicode( lookupFullLine( it ) ) );
    }
  }

  return results;
}

/**
 * A small set of EUC formatted string comparison functions
 * that will handle katakana and hiragana as the same, and compare both
 * kanji and latin chars in the same function. Handy for a few sections
 * of the doSearch() routine.
 * findMatches() is another string comparer, but unlike the other one
 * compares strictly (with an exepmption for punctuation).
 */
int IndexedEdictFile::findMatches( const char *str1, const char *str2 ) const
{
#define EUC_LATIN_CHARACTER(x) (('a'<=x && x<='z')||(x==0xA4)||(x==0x80))

  for(unsigned i=0; ; ++i)
  {
    unsigned char c1 = static_cast<unsigned char>( str1[ i ] );
    unsigned char c2 = static_cast<unsigned char>( str2[ i ] );

    if( ( i % 2 ) == 0 )
    {
      //on the highbyte (of kana)
      if( c2 == 0xA5 ) //Make katakana and hiragana equivalent
      {
        c2 = 0xA4;
      }

      if( c1 == 0xA5 )
      {
        c1 = 0xA4;
      }
    }

    if( ( 'A' <= c1 ) && ( c1 <= 'Z' ) )
    {
      c1 |= 0x20; // 'fix' uppercase
    }
    if( ( 'A' <= c2 ) && ( c2 <= 'Z' ) )
    {
      c2 |= 0x20;
    }

    if( c1 == '\0' )
    {
      if( ! EUC_LATIN_CHARACTER( c2 ) )
      {
        return 0;
      }

      return c2;
    }

    if( c1 != c2 )
    {
      return (int)c2 - (int)c1;
    }
  }

  return 0; //shouldn't happen... but gcc will warn if this isn't here
}

bool IndexedEdictFile::loadFile( const QString &fileName )
{
  if( m_valid )
  {
    return false;
  }

  m_dictFile.setFileName( fileName );
  if( ! m_dictFile.exists() )
  {
    return false;		//Bail if the file doesn't exist
  }

  m_dictPtr = static_cast<unsigned char*>( MAP_FAILED );
  m_indexFile.setFileName( QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QLatin1Char('/') + "kiten/xjdx/"
                        + QFileInfo( fileName ).baseName() + ".xjdx" );
  m_indexPtr = static_cast<uint32_t*>( MAP_FAILED );
  if( ! m_indexFile.exists() )
  {
    //If the index file isn't there, build it
    //TODO: Verify the format if the index doesn't exist?
    if( ! buildIndex() ) //If we can't build the file, bail
    {
      return false;
    }
  }

  if( ! m_dictFile.open( QIODevice::ReadOnly ) )
  {
    return false;
  }

  if( m_indexFile.open( QIODevice::ReadOnly ) )
  {
    if( checkIndex() )
    {
      if( loadmmaps() )
      {
        m_valid = true;
        return true;
      }
    }

    m_indexFile.close();
  }

  //Success is actually in the middle of that if statement, so if we get here
  //something failed and we need to clean up
  m_dictFile.close();
  return false;
}

//Warning: This assumes that both files have already been opened
bool IndexedEdictFile::loadmmaps()
{
  m_indexPtr = static_cast<uint32_t*>(
                  mmap(nullptr, m_indexFile.size(), PROT_READ, MAP_SHARED, m_indexFile.handle(), 0));
  if( m_indexPtr == static_cast<uint32_t*>( MAP_FAILED ) )
  {
    return false;
  }

  m_dictPtr = static_cast<unsigned char*>( mmap(  nullptr
                                                , m_dictFile.size()
                                                , PROT_READ
                                                , MAP_SHARED
                                                , m_dictFile.handle()
                                                , 0 ) );
  if( m_dictPtr == static_cast<unsigned char*>( MAP_FAILED ) )
  {
    munmap( static_cast<void*>( m_indexPtr ), m_indexFile.size() );
    m_indexPtr = static_cast<uint32_t*>( MAP_FAILED );
    return false;
  }

  return true;
}

/**
 * Similar to it's larger cousin below, this scans the dictionary at
 * a particular location and extracts the unsigned char at a particular
 * location within the file/memorymap, offset from the start of the
 * dictionary.
 */
inline unsigned char IndexedEdictFile::lookupDictChar( uint32_t i ) const
{
  if( i > static_cast<uint32_t>( m_dictFile.size() ) /*|| i < 0*/ )
  {
    return 0x0A; //If out of bounds, return endl
  }

  return m_dictPtr[i];
}

/**
 * This quick utility method looks in index at location i and pulls,
 * the corresponding line from the dictionary returning it as an euc
 * formatted QCString. i=1 is the first entry that the index points to.
 */
QByteArray IndexedEdictFile::lookupDictLine( uint32_t i ) const
{
  if( i > static_cast<uint32_t>( m_dictFile.size()) /*|| i < 0*/ )
  {
    return QByteArray( "" );
  }

  uint32_t start = m_indexPtr[ i ] - 1;
  uint32_t pos = start;
  const unsigned size = m_dictFile.size();
  //Grab the whole word
  //As long as we don't get EOF, null or newline... keep going forward
  while( pos<=size && m_dictPtr[ pos ] != 0 && m_dictPtr[ pos ] != 0x0A )
  {
    ++pos;
  }

  //Copy the word to a QCString
  QByteArray retval(  (const char*)( m_dictPtr + start )
                    , 1 + pos - start );
  //and away we go
  return retval;
}

/**
 * This is just like lookupDictChar, but grabs till EOL
 */
QByteArray IndexedEdictFile::lookupFullLine( uint32_t i ) const
{
  if( i > static_cast<uint32_t>( m_dictFile.size() ) /*|| i < 0*/ )
  {
    return QByteArray (0x0A, 1 );	//If out of bounds, return endl
  }

  uint32_t start = i;
  uint32_t pos = start;
  const unsigned max = m_dictFile.size();
  while( pos <= max && m_dictPtr[ pos ] != 0 && m_dictPtr[ pos ] != 0x0A )
  {
    ++pos;
  }

  QByteArray retval(  (const char*)( m_dictPtr + start )
                    , 1 + pos - start );
  //and away we go
  return retval;
}

bool IndexedEdictFile::valid() const
{
  return m_valid;
}
