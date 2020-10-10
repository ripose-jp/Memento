////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#include "subtitlewidget.h"

#include "definitionwidget.h"
#include "../../util/directoryutils.h"

#include <algorithm>
#include <mecab.h>
#include <QThreadPool>
#include <QDebug>
#include <QSet>
#include <QPair>

#define MAX_QUERY_LENGTH 37
#define WORD_INDEX 6
#define UNICODE_LENGTH 3

SubtitleWidget::SubtitleWidget(QWidget *parent) : QLineEdit(parent),
                                                  m_currentIndex(-1)
{
    setStyleSheet("QLineEdit { color : white; background-color : black; }");

    QString path = DirectoryUtils::getDictionaryDir() + JMDICT_DB_NAME;
    m_dictionary = new JMDict(path);
}

SubtitleWidget::~SubtitleWidget()
{
    delete m_dictionary;
}

void SubtitleWidget::updateText(const QString &text)
{
    setText(text);
    setAlignment(Qt::AlignCenter);
    m_currentIndex = -1;
}

void SubtitleWidget::deselectText()
{
    m_currentIndex = -1;
    deselect();
}

void SubtitleWidget::findEntry()
{
    QString queryStr = text().remove(0, m_currentIndex);
    queryStr.truncate(MAX_QUERY_LENGTH);
    QueryThread *queryThread = 
        new QueryThread(this, queryStr, text(), m_currentIndex);
    QThreadPool::globalInstance()->start(queryThread);
}

void SubtitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (cursorPositionAt(event->pos()) != m_currentIndex)
    {
        m_currentIndex = cursorPositionAt(event->pos());
        findEntry();
    }
    event->ignore();
}

void SubtitleWidget::QueryThread::run()
{
    if (m_query.isEmpty())
        return;
    
    // Lemmatization of the string
    MeCab::Model *model = MeCab::createModel("");
    MeCab::Tagger *tagger = model->createTagger();
    MeCab::Lattice *lattice = model->createLattice();
    char buffer[BUFSIZ];
    strncpy(buffer, m_query.toUtf8().data(), BUFSIZ);
    lattice->set_sentence(buffer);
    if (!tagger->parse(lattice))
    {
        qDebug() << "Cannot access MeCab";
        delete lattice;
        delete tagger;
        delete model;
        return;
    }
    
    // Match the longest exact entry in JMDict
    QList<const Entry *> *exact_entries = 0;
    while (!m_query.isEmpty())
    {
        exact_entries = 
            m_parent->m_dictionary->query(m_query, JMDict::FULLTEXT);
        if (!exact_entries->isEmpty())
        {
            break;
        }
        deleteEntries(exact_entries);
        exact_entries = 0;
        m_query.chop(1);
    }
    if (exact_entries == 0)
    {
        exact_entries = new QList<const Entry *>;
    }

    // Generate Lemmenized queries
    const MeCab::Node *node = lattice->bos_node()->next;
    QString match = QString::fromUtf8(node->surface, node->length);

    QSet<QString> *duplicates = new QSet<QString>;
    duplicates->insert(m_query);
    QList<QPair<QString, unsigned int>> queries;
    do
    {
        QString str = QString(node->feature).split(',')[WORD_INDEX];
        if (str != '*' && !duplicates->contains(str))
        {
            duplicates->insert(str);
            queries.push_back(QPair<QString, unsigned int>(str, match.size()));
        }
    }
    while (node = node->bnext);

    delete lattice;
    delete tagger;
    delete model;
    delete duplicates;

    // Query for the lemmenized entries
    unsigned int maxLen = 0;
    QList<const Entry *> *lem_entires = new QList<const Entry *>;
    for (auto it = queries.begin(); it != queries.end(); ++it)
    {
        QPair<QString, unsigned int> pair = *it;
        QList<const Entry *> *query_results = 
            m_parent->m_dictionary->query(pair.first, JMDict::FULLTEXT);

        if (!query_results->isEmpty())
        {
            lem_entires->append(*query_results);
            maxLen = pair.second > maxLen ? pair.second : maxLen;
        }
        
        delete query_results;
    }

    // Merge query results
    QList<const Entry *> *entries;
    if (maxLen > m_query.length())
    {
        entries = lem_entires;
        entries->append(*exact_entries);
        delete exact_entries;
    }
    else
    {
        entries = exact_entries;
        entries->append(*lem_entires);
        delete lem_entires;
        maxLen = m_query.length();
    }

    // Emit the results if still relavent
    if (!entries->isEmpty() && 
        m_currentIndex == m_parent->m_currentIndex &&
        m_currentSubtitle == m_parent->text())
    {
        Q_EMIT m_parent->entriesChanged(entries);
        m_parent->setSelection(m_parent->m_currentIndex, maxLen);
    }
    else 
    {
        deleteEntries(entries);
    }
}

void SubtitleWidget::QueryThread::deleteEntries(QList<const Entry *> *entries)
{
    for (auto it = entries->begin(); it != entries->end(); ++it)
    {
        delete *it;
    }
    delete entries;
}