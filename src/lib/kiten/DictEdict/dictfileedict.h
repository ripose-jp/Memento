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

#ifndef KITEN_DICTFILEEDICT_H
#define KITEN_DICTFILEEDICT_H

#include "../dictfile.h"
#include "indexededictfile.h"
#include "linearedictfile.h"

#include <QMap>

#include <sys/types.h>

class Deinflection;
class DictQuery;
class DictionaryPreferenceDialog;
class EntryEdict;
class KConfigSkeleton;
class KConfigSkeletonItem;
class QString;
class QStringList;

class DictFileEdict : public DictFile
{
  friend class EntryEdict;

  public:
                                        DictFileEdict();
    virtual                            ~DictFileEdict();

    EntryList                  *doSearch( const DictQuery &query ) override;
    QStringList                 listDictDisplayOptions( QStringList x ) const override;
    bool                                loadDictionary(  const QString &file
                                                       , const QString &name ) override;
    void                                loadSettings();
    bool                        validDictionaryFile( const QString &filename ) override;
    bool                                validQuery( const DictQuery &query ) override;

    static QString                     *deinflectionLabel;
    static QString                     *wordType;

  protected:
    virtual QMap<QString,QString> displayOptions() const;
    //This is a blatant abuse of protected methods to make the kanji subclass easy
    virtual Entry                *makeEntry( const QString &entry );

    LinearEdictFile     m_edictFile;

    static QStringList *displayFields;

  private:
    QMap<QString,QString> loadDisplayOptions() const;

    Deinflection *m_deinflection;
    bool          m_hasDeinflection;
};

#endif
