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

#include <QHash>
#include <QMultiMap>
#include <QMutex>
#include <QRegularExpression>

#include "../playeradapter.h"

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
                               const double   start,
                               const double   end,
                               const double   delay);

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
                                 const double   start,
                                 const double   end,
                                 const double   delay);

    /**
     * Updates the timestamps of the subtitles with a new delay.
     * Subtitles with a negative timestamp will be rounded to 0.
     * @param delay The signed delay.
     */
    void updateTimestamps(const double delay);

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

private:
    /**
     * Hides the secondary subtitle list and tabs.
     */
    void hideSecondarySubs();

    /**
     * Shows the secondary subtitle list and tabs.
     */
    void showSecondarySubs();

    /**
     * Helper method for getting a context string from a subtitle list.
     * @param table     The table to get selected rows from.
     * @param separator The separator to place between subtitles and replace
     *                  newlines with.
     * @return A string containing all the selected rows in the table.
     */
    QString getContext(const QTableWidget *table,
                       const QString      &separator) const;

    /**
     * Helper method for adding a subtitle to a table.
     * @param table      The table to add the subtitle to.
     * @param subInfos   List of the SubtitleInfos for the current track.
     * @param seenSubs   Map used for mapping timecodes to table items.
     * @param startTimes Map used for mapping table items to timecodes.
     * @param subtitle   The subtitle to add.
     * @param start      The start time of the subtitle.
     * @param end        The end time of the subtitle.
     * @param delay      The signed delay of the subtitle.
     */
    void addSubtitle(
        QTableWidget *table,
        QList<SubtitleInfo> *subInfos,
        QMultiMap<double, QTableWidgetItem *> &seenSubs,
        QHash<QTableWidgetItem *, const SubtitleInfo *> &startTimes,
        const QString &subtitle,
        const double start,
        const double end,
        const double delay);

    /**
     * Adds an item to a subtitle table. Assumes the subtitle is not already in
     * the table.
     * @param table      The table to add the subtitle to.
     * @param seenSubs   Map used for mapping timecodes to table items.
     * @param startTimes Map used for mapping table items to timecodes.
     * @param info       The subtitle information.
     * @param delay      The signed delay of the subtitle.
     * @return The constructed QTableWidgetItem. Belongs to table.
     */
    QTableWidgetItem *addTableItem(
        QTableWidget *table,
        QMultiMap<double, QTableWidgetItem *> &seenSubs,
        QHash<QTableWidgetItem *, const SubtitleInfo *> &startTimes,
        const SubtitleInfo &info,
        const double delay);

    /**
     * Helper method that converts a time in seconds to a timecode string of the
     * form HH:MM:SS.
     * @param time The time in seconds.
     * @returns A timecode fo the format HH:MM:SS.
     */
    QString formatTimecode(const int time);

    /**
     * Updates the timestamps in a table with a new delay.
     * @param table    The table to change the timestamps on.
     * @param seenSubs Map for mapping timecodes to table items.
     * @param delay    The signed delay of the subtitle.
     */
    void updateTimestampsHelper(
        QTableWidget *table,
        const QMultiMap<double, QTableWidgetItem *> &seenSubs,
        const double delay);

    /**
     * Removes all the information in the table and cleans up metadata.
     * @param table      The table to remove information from.
     * @param seenSubs   The timecode to item map remove information from.
     * @param startTimes The item top timecode map remove information from.
     */
    void clearSubtitles(
        QTableWidget *table,
        QMultiMap<double, QTableWidgetItem *> &seenSubs,
        QHash<QTableWidgetItem *, const SubtitleInfo *> &startTimes);

    /**
     * Seeks to the subtitle belonging to the item.
     * @param item       The item belonging to the subtitle to seek to. Not a
     *                   timestamp item.
     * @param startTimes Maps an item to a timecode.
     */
    void seekToSubtitle(
        QTableWidgetItem *item,
        const QHash<QTableWidgetItem *, const SubtitleInfo *> &startTimes) const;

    /* The UI item containing the widgets. */
    Ui::SubtitleListWidget *m_ui;

    /* Regular expression for filtering subtitles */
    QRegularExpression m_subRegex;

    /* Lock for the regular expression */
    QMutex m_subRegexLock;

    /* Maps sid to a list of subtitles */
    QHash<int64_t, QList<SubtitleInfo>> m_subtitleMap;

    /* Locks all structures related to the primary subtitle. */
    QMutex m_primaryLock;

    /* Maps timecodes to table widget items for the primary subtitles. */
    QMultiMap<double, QTableWidgetItem *> m_seenPrimarySubs;

    /* Maps table widget items to subtitle info for the primary subtitles. */
    QHash<QTableWidgetItem *, const SubtitleInfo *> m_timesPrimarySubs;

    /* Holds subtitle info for the current primary track. */
    QList<SubtitleInfo> *m_primarySubInfoList;

    /* Locks all structures related to the secondary subtitle. */
    QMutex m_secondaryLock;

    /* Maps timecodes to table widget items for the secondary subtitles. */
    QMultiMap<double, QTableWidgetItem *>  m_seenSecondarySubs;

    /* Maps table widget items to subtitle info for the secondary subtitles. */
    QHash<QTableWidgetItem *, const SubtitleInfo *> m_timesSecondarySubs;

    /* Holds subtitle info for the current secondary track. */
    QList<SubtitleInfo> *m_secondarySubInfoList;

    /* Flag for whether the primary subs have been parsed & populated */
    bool m_didParsePrimarySubs = false;

    /* Flag for whether the secondary subs have been parsed & populated */
    bool m_didParseSecondarySubs = false;
};

#endif // SUBTITLELISTWIDGET_H