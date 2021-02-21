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

#include <QThreadPool>
#include <QDebug>

#define TIMER_DELAY 250
#define MAX_QUERY_LENGTH 37

SubtitleWidget::SubtitleWidget(QWidget *parent) : QLineEdit(parent),
                                                  m_dictionary(new Dictionary),
                                                  m_currentIndex(-1),
                                                  m_findDelay(new QTimer(this))
{
    setStyleSheet("QLineEdit { color : white; background-color : black; }");

    m_findDelay->setSingleShot(true);
    connect(m_findDelay, &QTimer::timeout, this, &SubtitleWidget::findEntry);
}

SubtitleWidget::~SubtitleWidget()
{
    delete m_findDelay;
}

void SubtitleWidget::jmDictUpdated()
{
    m_dictionary->reopenDictionary();
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
    int index = m_currentIndex;
    QString queryStr = text().remove(0, index);
    queryStr.truncate(MAX_QUERY_LENGTH);
    if (queryStr.isEmpty())
    {
        return;
    }

    QThreadPool::globalInstance()->start([=] () {
        QList<Entry *> *entries = 
            m_dictionary->search(queryStr, text(), index, &m_currentIndex);
        
        if (index != m_currentIndex)
        {
            deleteEntries(entries);
        }
        else
        {
            if (!entries->isEmpty())
            {
                setSelection(index, entries->first()->m_clozeBody->size());
            }
            Q_EMIT entriesChanged(entries);
        }
    });
}

void SubtitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (cursorPositionAt(event->pos()) != m_currentIndex)
    {
        m_currentIndex = cursorPositionAt(event->pos());
        m_findDelay->start(TIMER_DELAY);
    }
    event->ignore();
}

void SubtitleWidget::leaveEvent(QEvent *event)
{
    m_findDelay->stop();
}

void SubtitleWidget::deleteEntries(QList<Entry *> *entries)
{
    for (auto it = entries->begin(); it != entries->end(); ++it)
    {
        delete *it;
    }
    delete entries;
}