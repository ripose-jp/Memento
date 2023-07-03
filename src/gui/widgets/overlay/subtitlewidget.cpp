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

#include "player/playeradapter.h"
#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/utils.h"

/* The maximum length of text that can be searched. */
#define MAX_QUERY_LENGTH 37

/* Prevents valid subtitles from being hidden due to double precision errors. */
#define DOUBLE_DELTA 0.001

/* Begin Constructor/Destructor */

SubtitleWidget::SubtitleWidget(QWidget *parent)
    : StrokeLabel(parent),
      m_dictionary(GlobalMediator::getGlobalMediator()->getDictionary()),
      m_findDelay(new QTimer(this)),
      m_currentIndex(-1),
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
        this,     &SubtitleWidget::initSettings,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::searchSettingsChanged,
        this,     &SubtitleWidget::initSettings,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerResized,
        this,     &SubtitleWidget::initTheme,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::interfaceSettingsChanged,
        this,     &SubtitleWidget::initTheme,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::definitionsHidden,
        this,     &StrokeLabel::deselectText,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::definitionsHidden,
        this,
        [this] () {
            m_definitionsVisible = false;
            adjustVisibility();
        },
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::definitionsHidden,
        this,     [this] () { m_definitionsVisible = false; },
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::definitionsShown,
        this,     &SubtitleWidget::selectText,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::definitionsShown,
        this,
        [this] () {
            m_definitionsVisible = true;
            adjustVisibility();
        },
        Qt::QueuedConnection
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
        this,     [this] { positionChanged(-1); },
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSubtitleTrackChanged,
        this,     [this] { positionChanged(-1); },
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::menuSubtitleVisibilityToggled, this,
        [this] (bool visible) {
            m_settings.showSubtitles = visible;
            adjustVisibility();
        },
        Qt::QueuedConnection
    );
    connect(mediator, &GlobalMediator::requestSubtitleWidgetVisibility, this,
        [this] (bool visible) {
            m_settings.requestedVisibility = visible;
            adjustVisibility();
        },
        Qt::QueuedConnection
    );
    connect(mediator, &GlobalMediator::playerPauseStateChanged, this,
        [this] (bool paused) {
            m_paused = paused;
            adjustVisibility();
        },
        Qt::QueuedConnection
    );
}

SubtitleWidget::~SubtitleWidget()
{
    disconnect();
    delete m_findDelay;
}

/* End Constructor/Destructor */
/* Begin Initializers */

void SubtitleWidget::initTheme()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);

    QFont font(
        settings.value(
            Constants::Settings::Interface::Subtitle::FONT,
            Constants::Settings::Interface::Subtitle::FONT_DEFAULT
        ).toString()
    );
    font.setBold(
        settings.value(
            Constants::Settings::Interface::Subtitle::FONT_BOLD,
            Constants::Settings::Interface::Subtitle::FONT_BOLD_DEFAULT
        ).toBool()
    );
    font.setItalic(
        settings.value(
            Constants::Settings::Interface::Subtitle::FONT_ITALICS,
            Constants::Settings::Interface::Subtitle::FONT_ITALICS_DEFAULT
        ).toBool()
    );
    font.setStyleStrategy(QFont::PreferAntialias);
    font.setPixelSize(
        GlobalMediator::getGlobalMediator()->getPlayerWidget()->height() *
        settings.value(
            Constants::Settings::Interface::Subtitle::SCALE,
            Constants::Settings::Interface::Subtitle::SCALE_DEFAULT
        ).toDouble()
    );
    setTextFont(font);

    QColor fontColor(
        settings.value(
            Constants::Settings::Interface::Subtitle::TEXT_COLOR,
            Constants::Settings::Interface::Subtitle::TEXT_COLOR_DEFAULT
        ).toString()
    );
    setTextColor(fontColor);

    QColor bgColor(
        settings.value(
            Constants::Settings::Interface::Subtitle::BACKGROUND_COLOR,
            Constants::Settings::Interface::Subtitle::BACKGROUND_COLOR_DEFAULT
        ).toString()
    );
    setBackgroundColor(bgColor);

    QColor strokeColor(
        settings.value(
            Constants::Settings::Interface::Subtitle::STROKE_COLOR,
            Constants::Settings::Interface::Subtitle::STROKE_COLOR_DEFAULT
        ).toString()
    );
    setStrokeColor(strokeColor);

    double strokeSize = settings.value(
        Constants::Settings::Interface::Subtitle::STROKE,
        Constants::Settings::Interface::Subtitle::STROKE_DEFAULT
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
    settings.beginGroup(Constants::Settings::Search::GROUP);
    m_settings.delay = settings.value(
            Constants::Settings::Search::DELAY,
            Constants::Settings::Search::DELAY_DEFAULT
        ).toInt();
    if (m_settings.delay < 0)
    {
        m_settings.delay = Constants::Settings::Search::DELAY_DEFAULT;
    }

    QString modifier = settings.value(
            Constants::Settings::Search::MODIFIER,
            Constants::Settings::Search::MODIFIER_DEFAULT
        ).toString();
    if (modifier == Constants::Settings::Search::Modifier::ALT)
    {
        m_settings.modifier = Qt::Modifier::ALT;
    }
    else if (modifier == Constants::Settings::Search::Modifier::CTRL)
    {
        m_settings.modifier = Qt::Modifier::CTRL;
    }
    else if (modifier == Constants::Settings::Search::Modifier::SHIFT)
    {
        m_settings.modifier = Qt::Modifier::SHIFT;
    }
    else if (modifier == Constants::Settings::Search::Modifier::SUPER)
    {
        m_settings.modifier = Qt::Modifier::META;
    }
    else
    {
        m_settings.modifier = Qt::Modifier::SHIFT;
    }

    QString method = settings.value(
            Constants::Settings::Search::METHOD,
            Constants::Settings::Search::METHOD_DEFAULT
        ).toString();
    if (method == Constants::Settings::Search::Method::HOVER)
    {
        m_settings.method = Settings::SearchMethod::Hover;
    }
    else if (method == Constants::Settings::Search::Method::MODIFIER)
    {
        m_settings.method = Settings::SearchMethod::Modifier;
    }
    else
    {
        m_settings.method = Settings::SearchMethod::Hover;
    }

    m_settings.hideSubsWhenVisible = settings.value(
            Constants::Settings::Search::HIDE_SUBS,
            Constants::Settings::Search::HIDE_SUBS_DEFAULT
        ).toBool();
    m_settings.hideOnPlay = settings.value(
            Constants::Settings::Search::HIDE_BAR,
            Constants::Settings::Search::HIDE_BAR_DEFAULT
        ).toBool();
    adjustVisibility();
    m_settings.pauseOnHover = settings.value(
            Constants::Settings::Search::HOVER_PAUSE,
            Constants::Settings::Search::HOVER_PAUSE_DEFAULT
        ).toBool();

    m_settings.replaceNewLines = settings.value(
            Constants::Settings::Search::REPLACE_LINES,
            Constants::Settings::Search::REPLACE_LINES_DEFAULT
        ).toBool();
    m_settings.replaceStr = settings.value(
            Constants::Settings::Search::REPLACE_WITH,
            Constants::Settings::Search::REPLACE_WITH_DEFAULT
        ).toString();
    setSubtitle(
        m_subtitle.rawText, m_subtitle.startTime, m_subtitle.endTime, 0
    );
    settings.endGroup();

    settings.beginGroup(Constants::Settings::Behavior::GROUP);
    m_settings.pauseOnSubtitleEnd = settings.value(
            Constants::Settings::Behavior::SUBTITLE_PAUSE,
            Constants::Settings::Behavior::SUBTITLE_PAUSE_DEFAULT
        ).toBool();
    settings.endGroup();
}

/* End Initializers */
/* Begin Event Handlers */

void SubtitleWidget::showEvent(QShowEvent *event)
{
    StrokeLabel::showEvent(event);

    if (m_settings.hideSubsWhenVisible)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()
            ->requestSubtitleVisibility(false);
    }
}

void SubtitleWidget::hideEvent(QHideEvent *event)
{
    StrokeLabel::hideEvent(event);

    if (m_settings.hideSubsWhenVisible && m_settings.hideOnPlay)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()
            ->requestSubtitleVisibility(true);
    }
    Q_EMIT GlobalMediator::getGlobalMediator()->subtitleHidden();
}

void SubtitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    StrokeLabel::mouseMoveEvent(event);

    if (m_settings.pauseOnHover && !m_paused)
    {
        GlobalMediator::getGlobalMediator()->getPlayerAdapter()->pause();
    }

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
    StrokeLabel::mouseDoubleClickEvent(event);
    QApplication::clipboard()->setText(getText());
}

void SubtitleWidget::leaveEvent(QEvent *event)
{
    StrokeLabel::leaveEvent(event);

    m_findDelay->stop();
    m_currentIndex = -1;
    adjustVisibility();
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
    {
        return;
    }

    int index = m_currentIndex;
    QString queryStr = getText();
    queryStr.remove(0, index);
    queryStr.truncate(MAX_QUERY_LENGTH);
    if (queryStr.isEmpty() || queryStr[0].isSpace())
    {
        return;
    }

    QString subtitleText = getText();
    QThreadPool::globalInstance()->start(
        [=] {
            /* Look for Terms */
            SharedTermList terms = m_dictionary->searchTerms(
                queryStr, subtitleText, index, &m_currentIndex
            );
            if (terms == nullptr)
            {
                /* noop */
            }
            else if (!m_paused || index != m_currentIndex)
            {
                /* Early Exit */
                return;
            }
            else if (terms->isEmpty())
            {
                /* No Terms */
                terms = nullptr;
            }
            else
            {
                m_lastEmittedIndex = index;
                m_lastEmittedSize = terms->first()->clozeBody.size();
            }

            /* Look for Kanji */
            SharedKanji kanji = nullptr;
            if (CharacterUtils::isKanji(queryStr[0]))
            {
                kanji = m_dictionary->searchKanji(queryStr[0]);
                if (kanji)
                {
                    kanji->sentence = subtitleText;
                    kanji->clozePrefix = subtitleText.left(index);
                    kanji->clozeBody = kanji->character;
                    kanji->clozeSuffix = subtitleText.mid(index + 1);
                    if (terms == nullptr)
                    {
                        m_lastEmittedIndex = index;
                        m_lastEmittedSize = 1;
                    }
                }
            }

            Q_EMIT GlobalMediator::getGlobalMediator()
                ->termsChanged(terms, kanji);
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
    else if (!m_paused && m_settings.hideOnPlay)
    {
        hide();
    }
    else if (
        !m_settings.requestedVisibility &&
        !m_definitionsVisible &&
        !underMouse()
    )
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
#define PAUSE_DELTA 0.04

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
