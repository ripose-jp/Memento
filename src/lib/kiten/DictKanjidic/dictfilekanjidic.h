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

#ifndef KITEN_DICTFILEKANJIDIC_H
#define KITEN_DICTFILEKANJIDIC_H

#include "../dictfile.h"

#include <QStringList>

class DictQuery;
class Entry;
class EntryList;
class KConfigSkeleton;
class KConfigSkeletonItem;

class DictFileKanjidic : public DictFile
{
  friend class EntryKanjidic;

  public:
                          DictFileKanjidic();
    virtual              ~DictFileKanjidic();

    QMap<QString,QString> displayOptions() const;
    EntryList    *doSearch( const DictQuery &query ) override;
    QStringList           dumpDictionary();
    QStringList   listDictDisplayOptions( QStringList list ) const override;
    bool                  loadDictionary(  const QString &file
                                         , const QString &name ) override;
    void                  loadSettings();
    bool                  validDictionaryFile( const QString &filename ) override;
    bool                  validQuery( const DictQuery &query ) override;

  protected:
    virtual inline Entry *makeEntry( const QString &entry );

    static QStringList *displayFields;

  private:
    QMap<QString,QString> loadDisplayOptions() const;

    QStringList m_kanjidic;
    bool        m_validKanjidic;
};

#endif
