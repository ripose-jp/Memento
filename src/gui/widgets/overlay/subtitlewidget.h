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

#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include "gui/widgets/common/strokelabel.h"

#include <QMouseEvent>
#include <QTimer>

#include "dict/dictionary.h"

/**
 * Widget used to display subtitle text and initiate searches.
 */
class SubtitleWidget : public StrokeLabel
{
    Q_OBJECT

public:
    SubtitleWidget(QWidget *parent = 0);
    ~SubtitleWidget();

protected:
    /**
     * Used for starting searches.
     * If m_settings.method is Hover, starts the timer.
     * If m_settings.method is Modifier, starts a direct search.
     * @param event The mouse move event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * Copys the current subtitle to clipboard.
     * @param event Mouse event, not used.
     */
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    /**
     * Prevents accidental searches from triggering when moving mouse across
     * the widget.
     * @param event The leave event, not used.
     */
    void leaveEvent(QEvent *event) override;

    /**
     * Realigns the subtitle and prevents graphical jank by emitting signals.
     * @param event The resize event, not used.
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * Hides the player subtitles when visible if set.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Shows the player subtitles when not visible if set.
     * @param event The show event, not used.
     */
    void hideEvent(QHideEvent *event) override;

private Q_SLOTS:
    /**
     * Initializes the subtitle theme.
     * Called when the theme is updated.
     */
    void initTheme();

    /**
     * Initializes settings.
     * Called whenever relevant settings change.
     */
    void initSettings();

    /**
     * Searches the current index for terms. Emits termsChanged() if found.
     */
    void findTerms();

    /**
     * Adjusts the visibility according to the settings.
     * Called at various points when state might change.
     */
    void adjustVisibility();

    /**
     * Event handler for when the player position is changed.
     * Hides the subtitle if needed.
     * @param value The current time of the player in seconds.
     */
    void positionChanged(const double value);

    /**
     * Sets the text to the current subtitle.
     * @param subtitle The subtitle text.
     * @param start    The start time code of the subtitle in seconds.
     * @param end      The end time code of the subtitle in seconds.
     * @param delay    The signed subtitle delay in seconds.
     */
    void setSubtitle(QString subtitle,
                     const double start,
                     const double end,
                     const double delay);

    /**
     * Selects the text described by m_lastEmittedIndex and m_lastEmittedSize.
     */
    void selectText();

private:
    /* The dictionary object, used for query for terms. */
    Dictionary *m_dictionary;

    /* Timer object used for starting searches when hover is enabled. */
    QTimer *m_findDelay;

    /* The current index the cursor is over. -1 if not over anything. */
    int m_currentIndex;

    /* The index of the last emitted term list. */
    int m_lastEmittedIndex;

    /* The size of the last emitted term match. */
    int m_lastEmittedSize;

    /* True if definitions are visible, false otherwise */
    bool m_definitionsVisible{false};

    /* Saved pause state of the player. */
    bool m_paused;

    /* True if playback has already been paused for the current subtitle, false
     * otherwise. */
    bool m_pausedForCurrentSubtitle{false};

    /* Contains information about the current subtitle. */
    struct Subtitle
    {
        /* The raw subtitle text as returned by the player. */
        QString rawText;

        /* The start time of the current subtitle. */
        double startTime;

        /* The end time of the current subtitle. */
        double endTime;
    } m_subtitle;

    /* Saved setting relevant to the widget. */
    struct Settings
    {
        /* Possible search methods. */
        enum SearchMethod
        {
            /* Search is set to trigger when hovered. */
            Hover,

            /* Search is triggered when a modifier is held. */
            Modifier
        };

        /* The currently set search method. */
        SearchMethod method;

        /* The hover delay, before a search is triggered. */
        int delay;

        /* The modifier key that needs to be held. */
        Qt::Modifier modifier;

        /* True if the player subtitle should be hidden when the widget is
         * visible, false otherwise.
         */
        bool hideSubsWhenVisible;

        /* True if the widget should be hidden the player is unpaused, false
         * otherwise.
         */
        bool hideOnPlay;

        /* True if the subtitle should be visible if possible, false otherwise
         */
        bool requestedVisibility{true};

        /* true if playback should pause when the mouse moves over the
         * subtitle false otherwise */
        bool pauseOnHover;

        /* True if newlines should be replaced before being displayed, false
         * otherwise.
         */
        bool replaceNewLines;

        /* The string to replace newlines with if replaceNewLines is true. */
        QString replaceStr;

        /* true if subtitles should be shown when needed, false otherwise. */
        bool showSubtitles;

        /* True if playback should be paused for the current subtitle, false
         * otherwise. */
        bool pauseOnSubtitleEnd;
    } m_settings;
};

#endif // SUBTITLEWIDGET_H
