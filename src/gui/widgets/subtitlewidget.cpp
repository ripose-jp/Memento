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

#include "../playeradapter.h"

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
                                                  m_paused(true)
{
    setTheme();

    setFixedHeight(0);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    setFocusPolicy(Qt::FocusPolicy::NoFocus);
    setAcceptDrops(false);
    setFrameShape(QFrame::Shape::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
    connect(mediator,    &GlobalMediator::playerResized,            this, &SubtitleWidget::setTheme);
    connect(mediator,    &GlobalMediator::interfaceSettingsChanged, this, &SubtitleWidget::setTheme);
    connect(mediator,    &GlobalMediator::definitionsHidden,        this, &SubtitleWidget::deselectText);
    connect(mediator,    &GlobalMediator::definitionsShown,         this, &SubtitleWidget::setSelectedText);
    connect(mediator,    &GlobalMediator::playerSubtitleChanged,    this, &SubtitleWidget::setSubtitle);
    connect(mediator,    &GlobalMediator::playerPositionChanged,    this, &SubtitleWidget::postitionChanged);
    connect(mediator,    &GlobalMediator::playerPauseStateChanged,  this, 
        [=] (const bool paused) {
            m_paused = paused;
            adjustVisibility();
        }
    );
}

SubtitleWidget::~SubtitleWidget()
{
    delete m_findDelay;
}

void SubtitleWidget::setTheme()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);

    QFont font(
        settings.value(
            SETTINGS_INTERFACE_SUB_FONT,
            SETTINGS_INTERFACE_SUB_FONT_DEFAULT
        ).toString()
    );
    font.setBold(
        settings.value(
            SETTINGS_INTERFACE_SUB_FONT_BOLD,
            SETTINGS_INTERFACE_SUB_FONT_BOLD_DEFAULT
        ).toBool()
    );
    font.setItalic(
        settings.value(
            SETTINGS_INTERFACE_SUB_FONT_ITALICS,
            SETTINGS_INTERFACE_SUB_FONT_ITALICS_DEFAULT
        ).toBool()
    );
    font.setStyleStrategy(QFont::PreferAntialias);
    setFont(font);

    QString stylesheetFormat = 
        "QTextEdit {"
            "font-size: %1px;"
            "color: rgba(%2, %3, %4, %5);"
            "background: rgba(%6, %7, %8, %9);"
        "}";

    int fontSize = (int)
        GlobalMediator::getGlobalMediator()->getPlayerWidget()->height() * 
        settings.value(SETTINGS_INTERFACE_SUB_SCALE, SETTINGS_INTERFACE_SUB_SCALE_DEFAULT).toDouble();
    QColor fontColor(
        settings.value(
            SETTINGS_INTERFACE_SUB_TEXT_COLOR, 
            SETTINGS_INTERFACE_SUB_TEXT_COLOR_DEFAULT
        ).toString()
    );
    QColor bgColor(
        settings.value(
            SETTINGS_INTERFACE_SUB_BG_COLOR, 
            SETTINGS_INTERFACE_SUB_BG_COLOR_DEFAULT
        ).toString()
    );

    setStyleSheet(
        stylesheetFormat.arg(QString::number(fontSize))
                        .arg(QString::number(fontColor.red()))
                        .arg(QString::number(fontColor.green()))
                        .arg(QString::number(fontColor.blue()))
                        .arg(QString::number(fontColor.alpha()))
                        .arg(QString::number(bgColor.red()))
                        .arg(QString::number(bgColor.green()))
                        .arg(QString::number(bgColor.blue()))
                        .arg(QString::number(bgColor.alpha()))

    );

    m_strokeColor.setNamedColor(
        settings.value(
            SETTINGS_INTERFACE_SUB_STROKE_COLOR,
            SETTINGS_INTERFACE_SUB_STROKE_COLOR_DEFAULT
        ).toString()
    );
    m_strokeSize = settings.value(
        SETTINGS_INTERFACE_SUB_STROKE,
        SETTINGS_INTERFACE_SUB_STROKE_DEFAULT
    ).toDouble();

    settings.endGroup();

    setSubtitle(m_rawText, m_startTime, m_endTime, 0);
}

void SubtitleWidget::adjustVisibility()
{
    if (toPlainText().isEmpty())
    {
        hide();
    }
    else if (m_paused)
    {
        show();
    }
    else if (m_hideOnPlay)
    {
        hide();
    }
    else
    {
        show();
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
    if (m_rawText.isEmpty())
    {
        setFixedSize(QSize(0, 0));
    }
    else
    {
        fitToContents();
    }
    
    /* Keep track of when to delete the subtitle */
    m_startTime = start + delay;
    m_endTime = end + delay;
    m_currentIndex = -1;

    adjustVisibility();
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

    m_hideSubsWhenVisible = settings.value(SETTINGS_SEARCH_HIDE_SUBS, DEFAULT_HIDE_SUBS).toBool();
    if (m_hideSubsWhenVisible)
    {
        GlobalMediator::getGlobalMediator()->getPlayerAdapter()->setSubVisiblity(!isVisible());
    }
    else
    {
        GlobalMediator::getGlobalMediator()->getPlayerAdapter()->setSubVisiblity(true);
    }

    m_hideOnPlay = settings.value(SETTINGS_SEARCH_HIDE_BAR, DEFAULT_HIDE_BAR).toBool();
    adjustVisibility();
    
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
    if (!m_rawText.isEmpty())
        fitToContents();
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

void SubtitleWidget::fitToContents()
{
    updateGeometry();
    int width = document()->idealWidth() + 4;
    if (width > GlobalMediator::getGlobalMediator()->getPlayerWidget()->width())
    {
        width = GlobalMediator::getGlobalMediator()->getPlayerWidget()->width();
    }
    setFixedWidth(width);
    int height = document()->size().toSize().height();
    setFixedHeight(height);
    updateGeometry();
}

void SubtitleWidget::paintEvent(QPaintEvent *event)
{
    QTextCharFormat format;
    format.setTextOutline(QPen(m_strokeColor, m_strokeSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QTextCursor cursor(document());
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(format);
    QTextEdit::paintEvent(event);
    
    format = QTextCharFormat();
    format.setTextOutline(QPen(Qt::transparent)); // Potential SIGSEGV
    cursor.mergeCharFormat(format);
    QTextEdit::paintEvent(event);
}

void SubtitleWidget::showEvent(QShowEvent *event)
{
    if (m_hideSubsWhenVisible)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->requestSetSubtitleVisibility(false);
    }
}

void SubtitleWidget::hideEvent(QHideEvent *event)
{
    if (m_hideSubsWhenVisible && m_hideOnPlay)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->requestSetSubtitleVisibility(true);
    }
}