/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com             *
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

#ifndef KITEN_ENTRYEDICT_H
#define KITEN_ENTRYEDICT_H

#include "../entry.h"

#include <QHash>
#include <QList>
#include <QSet>

#ifndef KITEN_EDICTFORMATTING
#define KITEN_EDICTFORMATTING

namespace EdictFormatting
{
  extern QStringList Nouns;
  extern QStringList Verbs;
  extern QStringList Adjectives;
  extern QStringList Adverbs;
  extern QStringList IchidanVerbs;
  extern QStringList GodanVerbs;
  extern QStringList FukisokuVerbs;
  extern QStringList Expressions;
  extern QStringList Prefix;
  extern QStringList Suffix;
  extern QString     Particle;
  
  extern QMultiHash<QString, QString> PartOfSpeechCategories;

  extern QSet<QString> PartsOfSpeech;
  extern QSet<QString> MiscMarkings;
  extern QSet<QString> FieldOfApplication;
}

#endif

class EntryEdict : public Entry
{
  friend class DictFileEdict;

  public:
//     EntryEdict( const EntryEdict &x ) : Entry( x ) {} //No special members to copy in this one
                    EntryEdict( const QString &dict );
                    EntryEdict( const QString &dict, const QString &entry );

    Entry          *clone() const override;
    /**
     * Simple accessor.
     */
    QString         getTypes() const;
    /**
     * Simple accessor.
     */
    QStringList     getTypesList() const;

    bool            isAdjective() const;
    bool            isAdverb() const;
    bool            isCommon() const;
    bool            isExpression() const;
    bool            isFukisokuVerb() const;
    bool            isGodanVerb() const;
    bool            isIchidanVerb() const;
    bool            isNoun() const;
    bool            isParticle() const;
    bool            isPrefix() const;
    bool            isSuffix() const;
    bool            isVerb() const;

    bool            matchesWordType( const DictQuery &query ) const;

    QString dumpEntry() const override;
    QString getDictionaryType() const override;
    QString HTMLWord() const override;
    bool    loadEntry( const QString &entryLine ) override;
    QString toHTML() const override;

  protected:
    virtual QString kanjiLinkify( const QString &inString ) const;

  private:
    /**
     * Types that match this entry (i.e. n, adj, adv).
     */
    QStringList    m_types;

    //Field of Application goes into the hash
    QList<QString> m_miscMarkings;
};

#endif
