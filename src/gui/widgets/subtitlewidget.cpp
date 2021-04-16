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
#include "../../util/constants.h"

#include <QApplication>
#include <QClipboard>
#include <QThreadPool>
#include <QDebug>
#include <QScrollBar>
#include <QSettings>

#define MAX_QUERY_LENGTH        37
#define DOUBLE_DELTA            0.05

SubtitleWidget::SubtitleWidget(QWidget *parent) : QTextEdit(parent),
                                                  m_dictionary(new Dictionary),
                                                  m_currentIndex(-1),
                                                  m_findDelay(new QTimer(this)),
                                                  m_paused(true),
                                                  m_fullscreen(false)
{
    setTheme();

    setFixedHeight(0);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setFocusPolicy(Qt::FocusPolicy::NoFocus);
    setAcceptDrops(false);
    setFrameShape(QFrame::Shape::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setLineWrapMode(QTextEdit::NoWrap);
    setReadOnly(true);
    setAcceptRichText(false);
    setTextInteractionFlags(Qt::NoTextInteraction);
    hide();
    setCursor(Qt::ArrowCursor);

    m_findDelay->setSingleShot(true);

    loadSettings();

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Slots */
    connect(m_findDelay, &QTimer::timeout,                          this, &SubtitleWidget::findTerms);
    connect(mediator,    &GlobalMediator::searchSettingsChanged,    this, &SubtitleWidget::loadSettings);
    connect(mediator,    &GlobalMediator::interfaceSettingsChanged, this, &SubtitleWidget::setTheme);
    connect(mediator,    &GlobalMediator::definitionsHidden,        this, &SubtitleWidget::deselectText);
    connect(mediator,    &GlobalMediator::definitionsShown,         this, &SubtitleWidget::setSelectedText);
    connect(mediator,    &GlobalMediator::playerSubtitleChanged,    this, &SubtitleWidget::setSubtitle);
    connect(mediator,    &GlobalMediator::playerPositionChanged,    this, &SubtitleWidget::postitionChanged);
    connect(mediator,    &GlobalMediator::playerFullscreenChanged,  this, 
        [=] (const bool full) {
            m_fullscreen = full;
        }
    );
    connect(mediator,    &GlobalMediator::playerPauseStateChanged, this, 
        [=] (const bool paused) {
            m_paused = paused;
            if (m_paused && !m_fullscreen)
            {
                showIfNeeded();
            }
            else if (!m_paused && m_hideOnPlay)
            {
                hide();
            }
        }
    );
}

SubtitleWidget::~SubtitleWidget()
{
    delete m_findDelay;
}

void SubtitleWidget::setTheme()
{
    setStyleSheet(
        "QTextEdit { font-size: 20pt; color: white; background: rgba(0, 0, 0, 0); font-weight: bold; }"
    );
    /*
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    if (settings.value(SETTINGS_INTERFACE_STYLESHEETS, SETTINGS_INTERFACE_STYLESHEETS_DEFAULT).toBool())
    {
        setStyleSheet(
            settings.value(
                SETTINGS_INTERFACE_SUBTITLE_SEARCH_STYLE,
                SETTINGS_INTERFACE_SUBTITLE_SEARCH_STYLE_DEFAULT
            ).toString()
        );
    }
    else
    {
        setStyleSheet(SETTINGS_INTERFACE_SUBTITLE_SEARCH_STYLE_DEFAULT);
    }
    settings.endGroup();
    initializeSize();
    setSubtitle(m_rawText, m_startTime, m_endTime, 0);
    */
}

void SubtitleWidget::showIfNeeded()
{
    if (!m_hideOnPlay)
    {
        show();
    }
    else if (toPlainText().isEmpty())
    {
        hide();
    }
    else
    {
        setVisible(m_paused);
    }
}

void SubtitleWidget::setSubtitle(QString subtitle,
                                 const double start, 
                                 const double end,
                                 const double delay)
{
    m_rawText = subtitle;

    /* Process the subtitle */
    if (m_replaceNewLines)
    {
        subtitle.replace('\n', m_replaceStr);
    }
        
    /* Add it to the text edit */
    clear();
    QStringList subList = subtitle.split('\n');
    for (const QString &text : subList)
    {
        if (text.isEmpty())
            continue;
        
        append(text);
        setAlignment(Qt::AlignHCenter);
    }

    /* Update Size */
    updateGeometry();
    int height = document()->size().toSize().height();
    if (horizontalScrollBar()->isVisible())
    {
        height += horizontalScrollBar()->height();
    }
    setFixedHeight(height);
    
    /* Keep track of when to delete the subtitle */
    m_startTime = start + delay;
    m_endTime = end + delay;
    m_currentIndex = -1;

    showIfNeeded();
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
        setSubtitle("", 0, std::numeric_limits<double>::infinity(), 0);
        Q_EMIT GlobalMediator::getGlobalMediator()->subtitleExpired();
    }
}

void SubtitleWidget::loadSettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);
    m_delay = settings.value(SETTINGS_SEARCH_DELAY, DEFAULT_DELAY).toInt();
    if (m_delay < 0)
    {
        m_delay = DEFAULT_DELAY;
    }

    QString modifier = settings.value(SETTINGS_SEARCH_MODIFIER, DEFAULT_MODIFIER).toString();
    if (modifier == MODIFIER_ALT)
    {
        m_modifier = Qt::Modifier::ALT;
    }
    else if (modifier == MODIFIER_CTRL)
    {
        m_modifier = Qt::Modifier::CTRL;
    }
    else if (modifier == MODIFIER_SHIFT)
    {
        m_modifier = Qt::Modifier::SHIFT;
    }
    else if (modifier == MODIFIER_SUPER)
    {
        m_modifier = Qt::Modifier::META;
    }
    else
    {
        m_modifier = Qt::Modifier::SHIFT;
    }

    QString method = settings.value(SETTINGS_SEARCH_METHOD, DEFAULT_METHOD).toString();
    if (method == SEARCH_METHOD_HOVER)
    {
        m_method = SearchMethod::Hover;
    }
    else if (method == SEARCH_METHOD_MODIFIER)
    {
        m_method = SearchMethod::Modifier;
    }
    else
    {
        m_method = SearchMethod::Hover;
    }

    m_hideOnPlay = settings.value(SETTINGS_SEARCH_HIDE_BAR, DEFAULT_HIDE_BAR).toBool();
    if (m_paused)
    {
        showIfNeeded();
    }
    else if (m_hideOnPlay)
    {
        hide();
    }
    
    m_replaceNewLines = settings.value(SETTINGS_SEARCH_REPLACE_LINES, DEFAULT_REPLACE_LINES).toBool();
    m_replaceStr      = settings.value(SETTINGS_SERACH_REPLACE_WITH,  DEFAULT_REPLACE_WITH).toString();
    setSubtitle(m_rawText, m_startTime, m_endTime, 0);
    settings.endGroup();
}

void SubtitleWidget::findTerms()
{
    if (!m_paused)
        return;

    int index = m_currentIndex;
    QString queryStr = m_rawText;
    queryStr.remove(0, index);
    queryStr.truncate(MAX_QUERY_LENGTH);
    if (queryStr.isEmpty())
    {
        return;
    }

    QThreadPool::globalInstance()->start([=] {
        QList<Term *> *terms = m_dictionary->searchTerms(queryStr, m_rawText, index, &m_currentIndex);

        if (terms == nullptr)
        {
            return;
        }
        else if (!m_paused || index != m_currentIndex)
        {
            deleteTerms(terms);
        }
        else if (terms->isEmpty())
        {
            delete terms;
        }
        else
        {
            Q_EMIT GlobalMediator::getGlobalMediator()->termsChanged(terms);
            m_lastEmittedIndex = index;
            m_lastEmittedSize  = terms->first()->clozeBody.size();
        }
    });
}

void SubtitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
    int position = cursorForPosition(event->pos()).position();

    if (m_paused && position != m_currentIndex)
    {
        switch (m_method)
        {
        case SearchMethod::Hover:
            m_currentIndex = position;
            m_findDelay->start(m_delay);
            break;
        case SearchMethod::Modifier:
            if (QGuiApplication::keyboardModifiers() & m_modifier)
            {
                m_currentIndex = position;
                findTerms();
            }
            break;
        }
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
    setAlignment(Qt::AlignHCenter);
    int height = document()->size().toSize().height();
    if (horizontalScrollBar()->isVisible())
    {
        height += horizontalScrollBar()->height();
    }
    setFixedHeight(height);
    updateGeometry();
    QTextEdit::resizeEvent(event);
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

void SubtitleWidget::paintEvent(QPaintEvent *event)
{
    QTextCharFormat format;
    format.setTextOutline (QPen (QColor(0, 0, 0), 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)); // Color and width of outline
    QTextCursor cursor (this->document());
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(format);
    QTextEdit::paintEvent(event);
    format.setTextOutline(QPen (Qt::transparent));
    cursor.mergeCharFormat(format);
    QTextEdit::paintEvent(event);
}

QSize SubtitleWidget::sizeHint() const
{
    QSize size = document()->size().toSize();
    size.rwidth()  = size.width();
    size.rheight() = size.height();
    return size;
}