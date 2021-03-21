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
#include "../../util/globalmediator.h"

#include <QApplication>
#include <QClipboard>
#include <QThreadPool>
#include <QDebug>

#define TIMER_DELAY         250
#define MAX_QUERY_LENGTH    37
#define DOUBLE_DELTA        0.05

SubtitleWidget::SubtitleWidget(QWidget *parent) : QLineEdit(parent),
                                                  m_dictionary(new Dictionary),
                                                  m_currentIndex(-1),
                                                  m_findDelay(new QTimer(this))
{
    setStyleSheet("QLineEdit { color : white; background-color : black; }");

    m_findDelay->setSingleShot(true);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Slots */
    connect(mediator,    &GlobalMediator::definitionsHidden,     this, &QLineEdit::deselect);
    connect(mediator,    &GlobalMediator::definitionsShown,      this, 
        [=] { setSelection(m_lastEmittedIndex, m_lastEmittedSize); }
    );
    connect(mediator,    &GlobalMediator::playerSubtitleChanged, this, &SubtitleWidget::setSubtitle);
    connect(mediator,    &GlobalMediator::playerPositionChanged, this, &SubtitleWidget::postitionChanged);
    connect(m_findDelay, &QTimer::timeout,                       this, &SubtitleWidget::findTerms);

    setAlignment(Qt::AlignCenter);
}

SubtitleWidget::~SubtitleWidget()
{
    delete m_findDelay;
}

void SubtitleWidget::setSubtitle(QString subtitle,
                                 const double start, 
                                 const double end,
                                 const double delay)
{
    m_rawText = subtitle;
    setText(subtitle.replace(QChar::fromLatin1('\n'), "/"));
    m_startTime = start + delay;
    m_endTime = end + delay;
    m_currentIndex = -1;
}

void SubtitleWidget::postitionChanged(const double value)
{
    if (value < m_startTime - DOUBLE_DELTA || value > m_endTime + DOUBLE_DELTA)
    {
        clear();
        m_currentIndex = -1;
        Q_EMIT GlobalMediator::getGlobalMediator()->subtitleExpired();
    }
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
                Q_EMIT GlobalMediator::getGlobalMediator()->termsChanged(terms);
                m_lastEmittedIndex = index;
                m_lastEmittedSize  = terms->first()->clozeBody.size();
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