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

#include "../../util/directoryutils.h"

#include <QApplication>
#include <QClipboard>
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
    connect(m_findDelay, &QTimer::timeout, this, &SubtitleWidget::findTerms);
}

SubtitleWidget::~SubtitleWidget()
{
    delete m_findDelay;
}

void SubtitleWidget::updateText(const QString &text)
{
    m_rawText = text;
    setText(m_rawText.replace(QChar::fromLatin1('\n'), "/"));
    setAlignment(Qt::AlignCenter);
    m_currentIndex = -1;
}

void SubtitleWidget::deselectText()
{
    m_currentIndex = -1;
    deselect();
}

void SubtitleWidget::findTerms()
{
    int index = m_currentIndex;
    QString queryStr = m_rawText;
    queryStr.remove(0, index);
    queryStr.truncate(MAX_QUERY_LENGTH);
    if (queryStr.isEmpty())
    {
        return;
    }

    QThreadPool::globalInstance()->start([=] {
        QList<Term *> *terms = 
            m_dictionary->search(queryStr, m_rawText, index, &m_currentIndex);

        if (index != m_currentIndex)
        {
            deleteTerms(terms);
        }
        else
        {
            if (terms->isEmpty())
            {
                delete terms;
            }
            else
            {
                setSelection(index, terms->first()->clozeBody.size());
                Q_EMIT termsChanged(terms);
            }
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

void SubtitleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QApplication::clipboard()->setText(m_rawText);
}

void SubtitleWidget::leaveEvent(QEvent *event)
{
    m_findDelay->stop();
}

void SubtitleWidget::deleteTerms(QList<Term *> *terms)
{
    for (Term *term : *terms)
    {
        delete term;
    }
    delete terms;
}