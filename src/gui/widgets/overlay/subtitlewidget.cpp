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

#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QClipboard>
#include <QDebug>
#include <QScrollBar>
#include <QSettings>
#include <QTextEdit>
#include <QThreadPool>

#include "../../../util/constants.h"
#include "../../../util/globalmediator.h"
#include "../../../util/utils.h"
#include "../../playeradapter.h"

/* The maximum length of text that can be searched. */
#define MAX_QUERY_LENGTH 37

/* Prevents valid subtitles from being hidden due to double precision errors. */
#define DOUBLE_DELTA 0.001

/* Begin Constructor/Destructor */

SubtitleWidget::SubtitleWidget(QWidget *parent)
    : StrokeLabel(parent),
      m_dictionary(GlobalMediator::getGlobalMediator()->getDictionary()),
      m_currentIndex(-1),
      m_findDelay(new QTimer(this)),
      m_paused(true)
{
    if (m_dictionary == nullptr)
    {
        m_dictionary = new Dictionary;
    }
    m_settings.showSubtitles = true;

    initTheme();

    setMouseTracking(true);
    setCursor(Qt::ArrowCursor);
    hide();

    m_findDelay->setSingleShot(true);

    initSettings();

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Slots */
    connect(m_findDelay, &QTimer::timeout, this, &SubtitleWidget::findTerms);
    connect(
        mediator, &GlobalMediator::behaviorSettingsChanged,
        this,     &SubtitleWidget::initSettings
    );
    connect(
        mediator, &GlobalMediator::searchSettingsChanged,
        this,     &SubtitleWidget::initSettings
    );
    connect(
        mediator, &GlobalMediator::playerResized,
        this,     &SubtitleWidget::initTheme
    );
    connect(
        mediator, &GlobalMediator::interfaceSettingsChanged,
        this,     &SubtitleWidget::initTheme
    );
    connect(
        mediator, &GlobalMediator::definitionsHidden,
        this,     &StrokeLabel::deselectText
    );
    connect(
        mediator, &GlobalMediator::definitionsShown,
        this,     &SubtitleWidget::selectText
    );
    connect(
        mediator, &GlobalMediator::playerSubtitleChanged,
        this,     &SubtitleWidget::setSubtitle
    );
    connect(
        mediator, &GlobalMediator::playerPositionChanged,
        this,     &SubtitleWidget::positionChanged
    );
    connect(
        mediator, &GlobalMediator::playerSubtitlesDisabled,
        this,     [=] { positionChanged(-1); }
    );
    connect(
        mediator, &GlobalMediator::playerSubtitleTrackChanged,
        this,     [=] { positionChanged(-1); }
    );
    connect(
        mediator, &GlobalMediator::menuSubtitleVisibilityToggled, this,
        [=] (bool visible) {
            m_settings.showSubtitles = visible;
            adjustVisibility();
        }
    );
    connect(mediator, &GlobalMediator::playerPauseStateChanged, this,
        [=] (const bool paused) {
            m_paused = paused;
            adjustVisibility();
        }
    );
}

SubtitleWidget::~SubtitleWidget()
{
    disconnect();
    delete m_findDelay;
}

void SubtitleWidget::deleteTerms(QList<Term *> *terms)
{
    for (Term *term : *terms)
    {
        delete term;
    }
    delete terms;
}

/* End Constructor/Destructor */
/* Begin Intializers */

void SubtitleWidget::initTheme()
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
    font.setPixelSize(
        GlobalMediator::getGlobalMediator()->getPlayerWidget()->height() *
        settings.value(
            SETTINGS_INTERFACE_SUB_SCALE,
            SETTINGS_INTERFACE_SUB_SCALE_DEFAULT
        ).toDouble()
    );
    setTextFont(font);

    QColor fontColor(
        settings.value(
            SETTINGS_INTERFACE_SUB_TEXT_COLOR,
            SETTINGS_INTERFACE_SUB_TEXT_COLOR_DEFAULT
        ).toString()
    );
    setTextColor(fontColor);

    QColor bgColor(
        settings.value(
            SETTINGS_INTERFACE_SUB_BG_COLOR,
            SETTINGS_INTERFACE_SUB_BG_COLOR_DEFAULT
        ).toString()
    );
    setBackgroundColor(bgColor);

    QColor strokeColor(
        settings.value(
            SETTINGS_INTERFACE_SUB_STROKE_COLOR,
            SETTINGS_INTERFACE_SUB_STROKE_COLOR_DEFAULT
        ).toString()
    );
    setStrokeColor(strokeColor);

    double strokeSize = settings.value(
        SETTINGS_INTERFACE_SUB_STROKE,
        SETTINGS_INTERFACE_SUB_STROKE_DEFAULT
    ).toDouble();
    setStrokeSize(strokeSize);

    settings.endGroup();

    setSubtitle(
        m_subtitle.rawText, m_subtitle.startTime, m_subtitle.endTime, 0
    );
}

void SubtitleWidget::initSettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);
    m_settings.delay = settings.value(
            SETTINGS_SEARCH_DELAY,
            DEFAULT_DELAY
        ).toInt();
    if (m_settings.delay < 0)
    {
        m_settings.delay = DEFAULT_DELAY;
    }

    QString modifier = settings.value(
            SETTINGS_SEARCH_MODIFIER,
            DEFAULT_MODIFIER
        ).toString();
    if (modifier == MODIFIER_ALT)
    {
        m_settings.modifier = Qt::Modifier::ALT;
    }
    else if (modifier == MODIFIER_CTRL)
    {
        m_settings.modifier = Qt::Modifier::CTRL;
    }
    else if (modifier == MODIFIER_SHIFT)
    {
        m_settings.modifier = Qt::Modifier::SHIFT;
    }
    else if (modifier == MODIFIER_SUPER)
    {
        m_settings.modifier = Qt::Modifier::META;
    }
    else
    {
        m_settings.modifier = Qt::Modifier::SHIFT;
    }

    QString method = settings.value(
            SETTINGS_SEARCH_METHOD,
            DEFAULT_METHOD
        ).toString();
    if (method == SEARCH_METHOD_HOVER)
    {
        m_settings.method = Settings::SearchMethod::Hover;
    }
    else if (method == SEARCH_METHOD_MODIFIER)
    {
        m_settings.method = Settings::SearchMethod::Modifier;
    }
    else
    {
        m_settings.method = Settings::SearchMethod::Hover;
    }

    m_settings.hideSubsWhenVisible = settings.value(
            SETTINGS_SEARCH_HIDE_SUBS,
            DEFAULT_HIDE_SUBS
        ).toBool();
    if (m_settings.hideSubsWhenVisible)
    {
        GlobalMediator::getGlobalMediator()
            ->getPlayerAdapter()->setSubVisiblity(!isVisible());
    }
    else
    {
        GlobalMediator::getGlobalMediator()
            ->getPlayerAdapter()->setSubVisiblity(true);
    }

    m_settings.hideOnPlay = settings.value(
            SETTINGS_SEARCH_HIDE_BAR,
            DEFAULT_HIDE_BAR
        ).toBool();
    adjustVisibility();

    m_settings.replaceNewLines = settings.value(
            SETTINGS_SEARCH_REPLACE_LINES,
            DEFAULT_REPLACE_LINES
        ).toBool();
    m_settings.replaceStr = settings.value(
            SETTINGS_SERACH_REPLACE_WITH,
            DEFAULT_REPLACE_WITH
        ).toString();
    setSubtitle(
        m_subtitle.rawText, m_subtitle.startTime, m_subtitle.endTime, 0
    );
    settings.endGroup();

    settings.beginGroup(SETTINGS_BEHAVIOR);
    m_settings.pauseOnSubtitleEnd = settings.value(
            SETTINGS_BEHAVIOR_SUBTITLE_PAUSE,
            SETTINGS_BEHAVIOR_SUBTITLE_PAUSE_DEFAULT
        ).toBool();
    settings.endGroup();
}

/* End Intializers */
/* Begin Event Handlers */

void SubtitleWidget::showEvent(QShowEvent *event)
{
    StrokeLabel::showEvent(event);

    if (m_settings.hideSubsWhenVisible)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()
            ->requestSetSubtitleVisibility(false);
    }
}

void SubtitleWidget::hideEvent(QHideEvent *event)
{
    StrokeLabel::hideEvent(event);

    if (m_settings.hideSubsWhenVisible && m_settings.hideOnPlay)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()
            ->requestSetSubtitleVisibility(true);
    }
    Q_EMIT GlobalMediator::getGlobalMediator()->subtitleHidden();
}

void SubtitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    StrokeLabel::mouseMoveEvent(event);

    int position = getPosition(event->pos());
    if (!m_paused || position == m_currentIndex || position == -1)
    {
        return;
    }

    switch (m_settings.method)
    {
    case Settings::SearchMethod::Hover:
        m_currentIndex = position;
        m_findDelay->start(m_settings.delay);
        break;

    case Settings::SearchMethod::Modifier:
        if (QGuiApplication::keyboardModifiers() & m_settings.modifier)
        {
            m_currentIndex = position;
            findTerms();
        }
        break;
    }

    event->ignore();
}

void SubtitleWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QApplication::clipboard()->setText(m_subtitle.rawText);
}

void SubtitleWidget::leaveEvent(QEvent *event)
{
    StrokeLabel::leaveEvent(event);

    m_findDelay->stop();
    m_currentIndex = -1;
}

void SubtitleWidget::resizeEvent(QResizeEvent *event)
{
    StrokeLabel::resizeEvent(event);

    Q_EMIT GlobalMediator::getGlobalMediator()->requestDefinitionDelete();
}

/* End Event Handlers */
/* Begin General Slots */

void SubtitleWidget::findTerms()
{
    if (!m_paused)
        return;

    int index = m_currentIndex;
    QString queryStr = getText();
    queryStr.remove(0, index);
    queryStr.truncate(MAX_QUERY_LENGTH);
    if (queryStr.isEmpty() || queryStr[0].isSpace())
    {
        return;
    }

    /* Get common fields. */
    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    QString title = mediator->getPlayerAdapter()->getTitle();

    double delay =
        mediator->getPlayerAdapter()->getSubDelay() -
        mediator->getPlayerAdapter()->getAudioDelay();
    double startTime = m_subtitle.startTime + delay;
    double endTime = m_subtitle.endTime + delay;

    QString sentence2 = mediator->getPlayerAdapter()->getSecondarySubtitle();

    QThreadPool::globalInstance()->start(
        [=] {
            QList<Term *> *terms = m_dictionary->searchTerms(
                    queryStr, m_subtitle.rawText, index, &m_currentIndex
                );

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
                /* Populate the missing fields */
                for (Term *term : *terms)
                {
                    term->title = title;
                    term->startTime = startTime;
                    term->endTime = endTime;
                    term->sentence2 = sentence2;
                }

                Q_EMIT GlobalMediator::getGlobalMediator()->termsChanged(terms);

                m_lastEmittedIndex = index;
                m_lastEmittedSize  = terms->first()->clozeBody.size();
                m_lastEmittedSize += getText()
                    .midRef(m_lastEmittedIndex, m_lastEmittedSize)
                    .count('\n');
            }
        }
    );
}

void SubtitleWidget::adjustVisibility()
{
    if (!m_settings.showSubtitles)
    {
        hide();
    }
    else if (getText().isEmpty())
    {
        hide();
    }
    else if (m_paused)
    {
        show();
    }
    else if (m_settings.hideOnPlay)
    {
        hide();
    }
    else
    {
        show();
    }
}

/* If pausing on subtitle end is enabled, prevents pausing on the next
 * subtitle. */
#define PAUSE_DELTA 0.028

void SubtitleWidget::positionChanged(const double value)
{
    const double timeToSubtitleEnd = m_subtitle.endTime - value;
    if (m_settings.pauseOnSubtitleEnd &&
        !m_pausedForCurrentSubtitle &&
        timeToSubtitleEnd >= -PAUSE_DELTA &&
        timeToSubtitleEnd <= PAUSE_DELTA &&
        value > PAUSE_DELTA)
    {
         GlobalMediator::getGlobalMediator()->getPlayerAdapter()->pause();
         m_pausedForCurrentSubtitle = true;
    }

    if (value < m_subtitle.startTime - DOUBLE_DELTA ||
        value > m_subtitle.endTime + DOUBLE_DELTA)
    {
        m_subtitle.rawText.clear();
        clearText();
        hide();
        Q_EMIT GlobalMediator::getGlobalMediator()->subtitleExpired();
    }
}

#undef PAUSE_DELTA

void SubtitleWidget::setSubtitle(QString subtitle,
                                 const double start,
                                 const double end,
                                 const double delay)
{
    m_subtitle.rawText = subtitle;

    /* Process the subtitle */
    subtitle = subtitle.trimmed();
    if (m_settings.replaceNewLines)
    {
        subtitle.replace('\n', m_settings.replaceStr);
    }

    /* Add it to the text edit */
    setText(subtitle);

    /* Keep track of when to delete the subtitle */
    m_subtitle.startTime = start + delay;
    m_subtitle.endTime = end + delay;
    m_currentIndex = -1;

    adjustVisibility();

    /* Reset auto pause state */
    m_pausedForCurrentSubtitle = false;
}

void SubtitleWidget::selectText()
{
    StrokeLabel::selectText(m_lastEmittedIndex, m_lastEmittedSize);
}

/* End General Slots */
