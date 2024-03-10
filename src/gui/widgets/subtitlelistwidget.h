////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#ifndef SUBTITLELISTWIDGET_H
#define SUBTITLELISTWIDGET_H

#include <QWidget>

#include <memory>
#include <vector>

#include <QHash>
#include <QMultiHash>
#include <QMultiMap>
#include <QMutex>
#include <QRegularExpression>

#include "anki/ankiclient.h"
#include "player/playeradapter.h"

class QShortcut;
class QTableWidget;
class QTableWidgetItem;

struct SubtitleInfo;

namespace Ui
{
    class SubtitleListWidget;
}

/**
 * A widget that displays a list of primary and secondary subtitles as reported
 * by the player.
 */
class SubtitleListWidget : public QWidget
{
    Q_OBJECT

public:
    SubtitleListWidget(QWidget *parent = nullptr);
    ~SubtitleListWidget();

    /**
     * Returns a string containing all the selected rows in the primary subtitle
     * list.
     * @param separator String to replace newlines with and place inbetween
     *                  subtitles.
     * @return String containing the selected rows with the separator inbetween
     *         them.
     */
    QString getPrimaryContext(const QString &separator) const;

    /**
     * Returns a string containing all the selected rows in the secondary
     * subtitle list.
     * @param separator String to replace newlines with and place inbetween
     *                  subtitles.
     * @return String containing the selected rows with the separator inbetween
     *         them.
     */
    QString getSecondaryContext(const QString &separator) const;

    /**
     * Returns a pair containing the earliest start time and latest end time of
     * the subtitles selected.
     *
     * @return The start time in the first element, the end time in the second
     *         element. Both elements are 0.0 if nothing is selected.
     */
    QPair<double, double> getPrimaryContextTime() const;

protected:
    /**
     * Scrolls to the current selected row on show.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Scrolls to the last item in the subtitle list on hide.
     * @param event The hide event, not used.
     */
    void hideEvent(QHideEvent *event) override;

    /**
     * Maintains well sized rows and avoids the need for horizontal scrolling or
     * wasted space.
     * @param event The resize event, not used.
     */
    void resizeEvent(QResizeEvent *event) override;

Q_SIGNALS:
    /**
     * Requests that the subtitle list be refreshed.
     */
    void requestRefresh();

    /**
     * Emitted when this widget is shown.
     */
    void widgetShown() const;

    /**
     * Emitted when the widget is hidden.
     */
    void widgetHidden() const;

private Q_SLOTS:
    /**
     * Initializes the style sheets for the widget.
     */
    void initTheme();

    /**
     * Initializes the subtitle filter regex.
     */
    void initRegex();

    /**
     * Handles the track list changing.
     */
    void handleTracklistChange(const QList<const Track *> &tracks);

    /**
     * Refreshes the contents of the subtitle list.
     */
    void handleRefresh();

    /**
     * Handles a change in the sid of the primary subtitle track.
     * @param sid The sid of the new primary subtitle track.
     */
    void handlePrimaryTrackChange(int64_t sid);

    /**
     * Handles a change in the sid of the secondary subtitle track.
     * @param sid The sid of the secondary subtitle track.
     */
    void handleSecondaryTrackChange(int64_t sid);

    /**
     * Updates the current primary subtitle
     * if the primary subtitles are not populated from
     * an external track, add the current subtitle
     * @param subtitle The subtitle to add.
     * @param start    The start time of the subtitle.
     * @param end      The end time of the subtitle.
     * @param delay    The signed delay of the subtitle.
     */
    void updatePrimarySubtitle(const QString &subtitle,
                               double start,
                               double end,
                               double delay);

    /**
     * Updates the current secondary subtitle
     * if the secondary subtitles are not populated from
     * an external track, add the current subtitle
     * @param subtitle The subtitle to add.
     * @param start    The start time of the subtitle.
     * @param end      The end time of the subtitle.
     * @param delay    The signed delay of the subtitle.
     */
    void updateSecondarySubtitle(const QString &subtitle,
                                 double start,
                                 double end,
                                 double delay);

    /**
     * Updates the primary timestamps of the subtitles with a new delay.
     * Subtitles with a negative timestamp will be rounded to 0.
     * @param delay The signed delay.
     */
    void updatePrimaryTimestamps(const double delay);

    /**
     * Updates the secondary timestamps of the subtitles with a new delay.
     * Subtitles with a negative timestamp will be rounded to 0.
     * @param delay The signed delay.
     */
    void updateSecondaryTimestamps(const double delay);

    /**
     * Resizes rows to contents.
     * @param index The index of the table to update. 0 for primary,
     *              1 for secondary.
     */
    void fixTableDimensions(const int index);

    /**
     * Removes all the subtitles in the primary subtitle list.
     */
    void clearPrimarySubtitles();

    /**
     * Removes all the subtitles in the secondary subtitle list.
     */
    void clearSecondarySubtitles();

    /**
     * Clears all cached subtitles.
     */
    void clearCachedSubtitles();

    /**
     * Seeks to the primary subtitle belonging to the item.
     * @param item The item belonging to the subtitle to seek to. Not the
     *             timestamp item.
     */
    void seekToPrimarySubtitle(QTableWidgetItem *item) const;

    /**
     * Seeks to the secondary subtitle belonging to the item.
     * @param item The item belonging to the subtitle to seek to. Not the
     *             timestamp item.
     */
    void seekToSecondarySubtitle(QTableWidgetItem *item) const;

    /**
     * Copys the currently selected context to clipboard.
     */
    void copyContext() const;

    /**
     * Copies the current audio context to clipboard.
     */
    void copyAudioContext() const;

    /**
     * Find the text in the current subtitle list.
     * @param text The text to search for.
     */
    void findText(const QString &text);

    /**
     * Seeks the the previous row of the current search.
     */
    void findPrev();

    /**
     * Seeks the the next row of the current search.
     */
    void findNext();

private:
    /* Holds all structures relating to a subtitle list. */
    struct SubtitleList
    {
        /* The table to populate subtitles with */
        QTableWidget *table = nullptr;

        /* Locks all structures related to the subtitle. */
        QMutex lock;

        /* Holds subtitle info for the current track. */
        std::shared_ptr<std::vector<std::shared_ptr<SubtitleInfo>>> subList;

        /* true if subtitles were parsed, false otherwise */
        std::shared_ptr<bool> subsParsed = nullptr;

        /* Maps timecodes to table widget items for the subtitles. */
        QMultiMap<double, QTableWidgetItem *> startToItem;

        /* Maps table widget items to subtitle infos. */
        QHash<
            QTableWidgetItem *,
            std::shared_ptr<const SubtitleInfo>
        > itemToSub;

        /* Maps subtitle lines to table widget items for parsed subtitles. */
        QMultiHash<QString, QTableWidgetItem *> lineToItem;

        /* Begin Search Values */

        /* true if the widget has been modified since the last search */
        bool modified = true;

        /* all the found rows */
        QList<int> foundRows;

        /* the currently found row */
        int currentFind;
    };

    /**
     * Hides the secondary subtitle list and tabs.
     */
    void hideSecondarySubs();

    /**
     * Shows the secondary subtitle list and tabs.
     */
    void showSecondarySubs();

    /**
     * Selects all the subtitles contained in the subtitle.
     * @param list     The list to select subtitles from.
     * @param subtitle The non-regex filtered subtitle.
     * @param start    The start time of the subtitle.
     * @param end      The end time of the subtitle.
     * @param delay    The current subtitle delay.
     */
    void selectSubtitles(SubtitleList &list,
                         const QString &subtitle,
                         double delay);

    /**
     * Helper method for getting a context string from a subtitle list.
     * @param list      The list to get selected rows from.
     * @param separator The separator to place between subtitles and replace
     *                  newlines with.
     * @return A string containing all the selected rows in the table.
     */
    QString getContext(const SubtitleList *list,
                       const QString      &separator) const;

    /**
     * Gets the earliest start and latest end time of the selected subtitles in
     * the table.
     * @param table The table to get the start and end times from.
     * @return A pair containing the start time and end time.
     */
    QPair<double, double> getContextTime(const SubtitleList &list) const;

    /**
     * Selects parsed subtitles in the subtitle list.
     *
     * @param list
     * @param subtitle
     * @param start
     * @param end
     * @param delay
     */
    void selectParsedSubtitles(SubtitleList &list,
                               const QString &subtitle,
                               double start,
                               double end,
                               double delay);

    /**
     * Helper method for adding a subtitle to a table.
     * @param list     The subtitle list to operate on.
     * @param subtitle The subtitle to add.
     * @param start    The start time of the subtitle.
     * @param end      The end time of the subtitle.
     * @param delay    The signed delay of the subtitle.
     * @param regex    True if regex should be used to filter the subtitle,
     *                 false otherwise.
     */
    void addSubtitle(SubtitleList &list,
                     const QString &subtitle,
                     double start,
                     double end,
                     double delay,
                     bool regex = false);

    /**
     * Adds an item to a subtitle table. Assumes the subtitle is not already in
     * the table.
     * @param list       The subtitle list to add the item to.
     * @param info       The subtitle information.
     * @param delay      The signed delay of the subtitle.
     * @param regex      Filter the subtitle with the subtitle regex.
     * @return The constructed QTableWidgetItem. Belongs to table. nullptr if
     *         regex is true and the text is empty.
     */
    QTableWidgetItem *addTableItem(SubtitleList &list,
                                   const std::shared_ptr<SubtitleInfo> &info,
                                   double delay,
                                   bool regex = false);

    /**
     * Helper method that converts a time in seconds to a timecode string of the
     * form HH:MM:SS.
     * @param time The time in seconds.
     * @returns A timecode fo the format HH:MM:SS.
     */
    QString formatTimecode(const int time);

    /**
     * Updates the timestamps in a table with a new delay.
     * @param list     The list to update the timestamps of.
     * @param delay    The signed delay of the subtitle.
     */
    void updateTimestampsHelper(SubtitleList &list, double delay);

    /**
     * Removes all the information in the table and cleans up metadata.
     * @param list The list to remove items from.
     */
    void clearSubtitles(SubtitleList &list);

    /**
     * Seeks to the subtitle belonging to the item.
     * @param item       The item belonging to the subtitle to seek to. Not a
     *                   timestamp item.
     * @param startTimes Maps an item to a timecode.
     */
    void seekToSubtitle(QTableWidgetItem *item, const SubtitleList &list) const;

    /**
     * Finds the text in the list without locking the list.
     * @param list The list to find the text in.
     * @param text The text to search for.
     */
    void findTextHelper(SubtitleList &list, const QString &text);

    /**
     * Selects the row of the current table with the offset from the currently
     * currently found row.
     * @param offset The offset from the currently found row.
     */
    void findRowHelper(int offset);

    /* The UI item containing the widgets. */
    Ui::SubtitleListWidget *m_ui;

    /* Shortcut for copying the current context */
    QShortcut *m_copyShortcut;

    /* Shortcut for copying the current context's audio */
    QShortcut *m_copyAudioShortcut;

    /* Shortcut for opening the find widget */
    QShortcut *m_findShortcut;

    /* Regular expression for filtering subtitles */
    QRegularExpression m_subRegex;

    /* Lock for the regular expression */
    QMutex m_subRegexLock;

    /* Maps sid to a list of subtitles */
    QHash<
        int64_t,
        std::shared_ptr<std::vector<std::shared_ptr<SubtitleInfo>>>
    > m_subtitleMap;

    /* Maps sid to whether or not the subtitle was parsed. */
    QHash<int64_t, std::shared_ptr<bool>> m_subtitleParsed;

    /* The primary subtitle list */
    SubtitleList m_primary;

    /* The secondary subtitle list */
    SubtitleList m_secondary;

    /* Saved pointer to the global AnkiClient. */
    AnkiClient *m_client;
};

#endif // SUBTITLELISTWIDGET_H
