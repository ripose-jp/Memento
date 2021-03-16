////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <QString>
#include <QStringList>

struct Tag
{
    QString name;
    QString category;
    QString notes;
    int     order;
    int     score;
} typedef Tag;

struct TermFrequency
{
    QString         dictionary;
    uint64_t        freq;
} typedef TermFrequency;

struct Definition
{
    QString     dictionary;
    QList<Tag>  tags;
    QStringList glossary;
} typedef Definition;

struct Term
{
    QString                 expression;
    QString                 reading;
    QList<Tag>              tags;
    QList<Definition>       definitions;
    QList<TermFrequency>    frequencies;

    /* Cloze values will be set outside of database manager */
    QString                 sentence;
    QString                 clozeBody;
    QString                 clozePrefix;
    QString                 clozeSuffix;
} typedef Term;

#endif // EXPRESSION_H