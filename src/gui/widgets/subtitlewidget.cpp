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
#include <QScrollBar>

#define TEXT_PADDING_HEIGHT 0
#define TIMER_DELAY         250
#define MAX_QUERY_LENGTH    37
#define DOUBLE_DELTA        0.05

SubtitleWidget::SubtitleWidget(QWidget *parent) : QTextEdit(parent),
                                                  m_dictionary(new Dictionary),
                                                  m_currentIndex(-1),
                                                  m_findDelay(new QTimer(this))
{
    setStyleSheet(
        "QTextEdit {"
            "color : white;"
            "background-color : black;"
        "}"
    );

    QFont font;
    font.setFamily(QString::fromUtf8("Noto Sans CJK JP"));
    font.setPointSize(20);
    setFont(font);
    m_fontHeight = QFontMetrics(font).height() + TEXT_PADDING_HEIGHT;

    setFixedHeight(m_fontHeight);

    m_findDelay->setSingleShot(true);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Slots */
    connect(mediator,    &GlobalMediator::definitionsHidden,     this, &SubtitleWidget::deselectText);
    connect(mediator,    &GlobalMediator::definitionsShown,      this, &SubtitleWidget::setSelectedText);
    connect(mediator,    &GlobalMediator::playerSubtitleChanged, this, &SubtitleWidget::setSubtitle);
    connect(mediator,    &GlobalMediator::playerPositionChanged, this, &SubtitleWidget::postitionChanged);
    connect(m_findDelay, &QTimer::timeout,                       this, &SubtitleWidget::findTerms);
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
    setPlainText(subtitle.replace(QChar::fromLatin1('\n'), "/"));
    m_startTime = start + delay;
    m_endTime = end + delay;
    m_currentIndex = -1;
    resizeToContents();
}

void SubtitleWidget::setSelectedText()
{
    QTextCursor q = textCursor();
    q.setPosition(m_lastEmittedIndex);
    q.setPosition(m_lastEmittedIndex + m_lastEmittedSize, QTextCursor::KeepAnchor);
    setTextCursor(q);
}

void SubtitleWidget::deselectText()
{
    QTextCursor q = textCursor();
    q.clearSelection();
    setTextCursor(q);
}

void SubtitleWidget::postitionChanged(const double value)
{
    if (value < m_startTime - DOUBLE_DELTA || value > m_endTime + DOUBLE_DELTA)
    {
        setSubtitle("", 0, 0, 0);
        Q_EMIT GlobalMediator::getGlobalMediator()->subtitleExpired();
    }
}

void SubtitleWidget::resizeToContents()
{
    setAlignment(Qt::AlignCenter);
    if (horizontalScrollBar()->isVisible())
    {
        setFixedHeight(m_fontHeight + horizontalScrollBar()->height());
    }
    else
    {
        setFixedHeight(m_fontHeight);
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
            m_dictionary->searchTerms(queryStr, m_rawText, index, &m_currentIndex);

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
    event->ignore();
    int position = cursorForPosition(event->pos()).position();
    if (position != m_currentIndex)
    {
        m_currentIndex = position;
        m_findDelay->start(TIMER_DELAY);
    }
}

void SubtitleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QApplication::clipboard()->setText(m_rawText);
}

void SubtitleWidget::leaveEvent(QEvent *event)
{
    m_findDelay->stop();
    m_currentIndex = -1;
}

void SubtitleWidget::resizeEvent(QResizeEvent *event)
{
    event->ignore();
    resizeToContents();
    Q_EMIT GlobalMediator::getGlobalMediator()->requestDefinitionDelete();
    Q_EMIT GlobalMediator::getGlobalMediator()->requestFullscreenResize();
}

void SubtitleWidget::deleteTerms(QList<Term *> *terms)
{
    for (Term *term : *terms)
    {
        delete term;
    }
    delete terms;
}