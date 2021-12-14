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

#include "subtitlelistwidget.h"
#include "ui_subtitlelistwidget.h"

#include <iterator>
#include <QApplication>
#include <QMultiMap>
#include <QSettings>

#include "../../util/constants.h"
#include "../../util/globalmediator.h"
#include "../../util/utils.h"

/**
 * Information about a subtitle.
 */
struct SubtitleInfo
{
    /* The text of the subtitle */
    QString text;

    /* The start time of the subtitle in seconds */
    double start;

    /* The end time of the subtitle in seconds */
    double end;

    SubtitleInfo &operator=(const SubtitleInfo &rhs)
    {
        text = rhs.text;
        start = rhs.start;
        end = rhs.end;

        return *this;
    }
} typedef SubtitleInfo;

/* Begin Constructor/Destructors */

SubtitleListWidget::SubtitleListWidget(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::SubtitleListWidget)
{
    m_ui->setupUi(this);
    initTheme();
    hideSecondarySubs();

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Signals */
    connect(
        m_ui->tablePrim, &QTableWidget::itemDoubleClicked,
        this,            &SubtitleListWidget::seekToPrimarySubtitle
    );
    connect(
        m_ui->tableSec, &QTableWidget::itemDoubleClicked,
        this,           &SubtitleListWidget::seekToSecondarySubtitle
    );
    connect(
        m_ui->tabWidget, &QTabWidget::currentChanged,
        this,            &SubtitleListWidget::fixTableDimensions
    );

    connect(
        mediator, &GlobalMediator::interfaceSettingsChanged,
        this,     &SubtitleListWidget::initTheme
    );
    connect(
        mediator, &GlobalMediator::playerSubDelayChanged,
        this,     &SubtitleListWidget::updateTimestamps
    );

    connect(
        mediator, &GlobalMediator::playerSubtitleChanged,
        this,     &SubtitleListWidget::addPrimarySubtitle
    );
    connect(
        mediator, &GlobalMediator::playerSubtitleTrackChanged,
        this,     &SubtitleListWidget::handlePrimaryTrackChange
    );
    connect(
        mediator, &GlobalMediator::playerSubtitlesDisabled,
        this,     &SubtitleListWidget::clearPrimarySubtitles
    );
    connect(
        mediator, &GlobalMediator::searchSettingsChanged,
        this,     &SubtitleListWidget::clearPrimarySubtitles
    );

    connect(
        mediator, &GlobalMediator::playerSecSubtitleChanged,
        this,     &SubtitleListWidget::addSecondarySubtitle
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitleTrackChanged,
        this,     &SubtitleListWidget::handleSecondaryTrackChange
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitlesDisabled,
        this,     &SubtitleListWidget::clearSecondarySubtitles
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitleTrackChanged,
        this,     &SubtitleListWidget::showSecondarySubs
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitlesDisabled,
        this,     &SubtitleListWidget::hideSecondarySubs
    );

    connect(
        mediator, &GlobalMediator::playerFileChanged,
        this,     &SubtitleListWidget::clearCachedSubtitles
    );
    connect(
        mediator, &GlobalMediator::playerTracksChanged,
        this,     &SubtitleListWidget::handleTracklistChange
    );

    connect(mediator, &GlobalMediator::controlsSubtitleListToggled, this,
        [=] {
            setVisible(!isVisible());
        }
    );
}

SubtitleListWidget::~SubtitleListWidget()
{
    disconnect();
    delete m_ui;
}

/* End Constructor/Destructors */
/* Begin Initializers */

void SubtitleListWidget::initTheme()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    const bool customStylesheets = settings.value(
            SETTINGS_INTERFACE_STYLESHEETS,
            SETTINGS_INTERFACE_STYLESHEETS_DEFAULT
        ).toBool();
    if (customStylesheets)
    {
        setStyleSheet(settings.value(
                SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE,
                SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT
            ).toString()
        );
    }
    else
    {
        setStyleSheet(SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT);
    }

    const bool showTimestamps = !settings.value(
        SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS,
        SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS_DEFAULT
    ).toBool();
    m_ui->tablePrim->setColumnHidden(0, showTimestamps);
    m_ui->tableSec->setColumnHidden (0, showTimestamps);
    settings.endGroup();
}

/* End Initializers */
/* Begin Event Handlers */

void SubtitleListWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    QApplication::processEvents();
    m_ui->tablePrim->scrollToItem(m_ui->tablePrim->currentItem());
    m_ui->tableSec->scrollToItem(m_ui->tableSec->currentItem());
    m_ui->tablePrim->resizeRowsToContents();
    m_ui->tableSec->resizeRowsToContents();

    Q_EMIT GlobalMediator::getGlobalMediator()->subtitleListShown();
}

void SubtitleListWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);

    QList<QTableWidgetItem *> items = m_ui->tablePrim->selectedItems();
    if (!items.isEmpty())
    {
        QApplication::processEvents();
        m_ui->tablePrim->scrollToItem(items.last());
    }

    items = m_ui->tableSec->selectedItems();
    if (!items.isEmpty())
    {
        QApplication::processEvents();
        m_ui->tableSec->scrollToItem(items.last());
    }

    Q_EMIT GlobalMediator::getGlobalMediator()->subtitleListHidden();
}

void SubtitleListWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_ui->tablePrim->resizeRowsToContents();
    m_ui->tableSec->resizeRowsToContents();
}

/* End Event Handlers */
/* Begin Adder Methods */

void SubtitleListWidget::handleTracklistChange(
    const QList<const Track *> &tracks)
{
    int64_t primarySid = -1;
    int64_t secondarySid = -1;
    for (const Track *track : tracks)
    {
        if (track->type == Track::subtitle && track->selected)
        {
            if (track->mainSelection == 0)
            {
                primarySid = track->id;
            }
            else
            {
                secondarySid = track->id;
            }
        }
    }

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    if (primarySid != -1)
    {
        handlePrimaryTrackChange(primarySid);
    }
    if (secondarySid != -1)
    {
        handleSecondaryTrackChange(secondarySid);
    }
}

QString SubtitleListWidget::formatTimecode(const int time)
{
    const int SECONDS_IN_HOUR = 3600;
    const int SECONDS_IN_MINUTE = 60;

    const int hours   = time / SECONDS_IN_HOUR;
    const int minutes = (time % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
    const int seconds = time % SECONDS_IN_MINUTE;

    QString timeStr("%1:%2:%3");
    return timeStr.arg(hours,   2, 10, QLatin1Char('0'))
                  .arg(minutes, 2, 10, QLatin1Char('0'))
                  .arg(seconds, 2, 10, QLatin1Char('0'));
}

QTableWidgetItem *SubtitleListWidget::addTableItem(
    QTableWidget *table,
    QMultiMap<double, QTableWidgetItem *> &seenSubs,
    QHash<QTableWidgetItem *, const SubtitleInfo *> &startTimes,
    const SubtitleInfo &info,
    const double delay)
{
    QTableWidgetItem *subtitleItem = new QTableWidgetItem(info.text);
    startTimes.insert(subtitleItem, &info);
    auto endIt = seenSubs.insert(info.start, subtitleItem);

    /* This is a bit of a hack to get the subtitles to appear in order.
     * If a subtitle starts at the same start time as another, the subtitle
     * that appears later will be closer to seenSubs.begin() than the
     * earlier subtitle. To get around this, we assume the subtitle being
     * added came after all subtitles with the same start time. This isn't
     * going to work 100% of the time, but it's right enough of the time to
     * be worth it.
     */
    size_t i = std::distance(seenSubs.begin(), endIt);
    for (endIt += 1;
         endIt != seenSubs.end() && endIt.key() == info.start;
         ++endIt)
    {
        ++i;
    }

    QTableWidgetItem *timecodeItem =
        new QTableWidgetItem(formatTimecode(info.start + delay));
    timecodeItem->setFlags(Qt::NoItemFlags);

    table->insertRow(i);
    table->setItem(i, 1, subtitleItem);
    table->setItem(i, 0, timecodeItem);
    table->resizeRowToContents(i);

    return subtitleItem;
}

void SubtitleListWidget::addSubtitle(
    QTableWidget *table,
    QList<SubtitleInfo> *subInfos,
    QMultiMap<double, QTableWidgetItem *> &seenSubs,
    QHash<QTableWidgetItem *, const SubtitleInfo *> &startTimes,
    const QString &subtitle,
    const double start,
    const double end,
    const double delay)
{
    /* Check if we have already seen this subtitle. Finds it if we have. */
    auto it = seenSubs.find(start);
    while (it != seenSubs.end() && it.key() == start)
    {
        if ((*it)->text() == subtitle)
        {
            break;
        }
        ++it;
    }

    QTableWidgetItem *subtitleItem = nullptr;
    if (it == seenSubs.end() || it.key() != start)
    {
        subInfos->append(SubtitleInfo{
            .text = subtitle,
            .start = start,
            .end = end
        });

        subtitleItem = addTableItem(
            table, seenSubs, startTimes, subInfos->last(), delay
        );
    }
    else
    {
        subtitleItem = *it;
    }

    table->clearSelection();
    table->setCurrentItem(subtitleItem);
}

void SubtitleListWidget::handlePrimaryTrackChange(int64_t sid)
{
    m_primaryLock.lock();
    clearPrimarySubtitles();
    m_primarySubInfoList = &m_subtitleMap[sid];
    double delay =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter()->getSubDelay();
    for (const SubtitleInfo &info : *m_primarySubInfoList)
    {
        addTableItem(
            m_ui->tablePrim,
            m_seenPrimarySubs,
            m_timesPrimarySubs,
            info,
            delay
        );
    }
    m_primaryLock.unlock();
}

void SubtitleListWidget::handleSecondaryTrackChange(int64_t sid)
{
    m_secondaryLock.lock();
    clearSecondarySubtitles();
    m_secondarySubInfoList = &m_subtitleMap[sid];
    double delay =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter()->getSubDelay();
    for (const SubtitleInfo &info : *m_secondarySubInfoList)
    {
        addTableItem(
            m_ui->tableSec,
            m_seenSecondarySubs,
            m_timesPrimarySubs,
            info,
            delay
        );
    }
    m_secondaryLock.unlock();
}

void SubtitleListWidget::addPrimarySubtitle(const QString &subtitle,
                                            const double   start,
                                            const double   end,
                                            const double   delay)
{
    m_primaryLock.lock();
    addSubtitle(
        m_ui->tablePrim,
        m_primarySubInfoList,
        m_seenPrimarySubs,
        m_timesPrimarySubs,
        subtitle,
        start,
        end,
        delay
    );
    m_primaryLock.unlock();
}

void SubtitleListWidget::addSecondarySubtitle(const QString &subtitle,
                                              const double   start,
                                              const double   end,
                                              const double   delay)
{
    m_secondaryLock.lock();
    addSubtitle(
        m_ui->tableSec,
        m_secondarySubInfoList,
        m_seenSecondarySubs,
        m_timesSecondarySubs,
        subtitle,
        start,
        end,
        delay
    );
    m_secondaryLock.unlock();
}

void SubtitleListWidget::updateTimestampsHelper(
    QTableWidget *table,
    const QMultiMap<double, QTableWidgetItem *> &seenSubs,
    const double delay)
{
    size_t i = 0;
    for (auto it = seenSubs.keyBegin(); it != seenSubs.keyEnd(); ++it)
    {
        const double time = *it + delay < 0 ? 0 : *it + delay;
        QTableWidgetItem *timecodeItem =
            new QTableWidgetItem(formatTimecode(time));
        timecodeItem->setFlags(Qt::NoItemFlags);
        table->setItem(i, 0, timecodeItem);

        i++;
    }
    table->resizeRowsToContents();
}

void SubtitleListWidget::updateTimestamps(const double delay)
{
    updateTimestampsHelper(m_ui->tablePrim, m_seenPrimarySubs,   delay);
    updateTimestampsHelper(m_ui->tableSec,  m_seenSecondarySubs, delay);
}

/* End Adder Methods */
/* Begin Context Methods */

QString SubtitleListWidget::getContext(const QTableWidget *table,
                                       const QString      &separator) const
{
    QList<QTableWidgetItem *> items = table->selectedItems();
    QString context;
    for (const QTableWidgetItem *item : items)
    {
        context += item->text().replace('\n', separator) + separator;
    }
    return context;
}

QString SubtitleListWidget::getPrimaryContext(const QString &separator) const
{
    return getContext(m_ui->tablePrim, separator);
}

QString SubtitleListWidget::getSecondaryContext(const QString &separator) const
{
    return getContext(m_ui->tableSec, separator);
}

QPair<double, double> SubtitleListWidget::getPrimaryContextTime() const
{
    double start = 0.0;
    double end = 0.0;

    QList<QTableWidgetItem *> items = m_ui->tablePrim->selectedItems();
    if (!items.isEmpty())
    {
        start = m_timesPrimarySubs[items.first()]->start;
        end = m_timesPrimarySubs[items.last()]->end;
    }

    return QPair<double, double>(start, end);
}

/* End Context Methods */
/* Begin Seek Methods */

void SubtitleListWidget::seekToSubtitle(
    QTableWidgetItem *item,
    const QHash<QTableWidgetItem *, const SubtitleInfo *> &startTimes) const
{
    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    double pos = startTimes[item]->start + player->getSubDelay();
    if (pos < 0)
    {
        pos = 0;
    }
    player->seek(pos);
}

void SubtitleListWidget::seekToPrimarySubtitle(QTableWidgetItem *item) const
{
    seekToSubtitle(item, m_timesPrimarySubs);
}

void SubtitleListWidget::seekToSecondarySubtitle(QTableWidgetItem *item) const
{
    seekToSubtitle(item, m_timesSecondarySubs);
}

/* End Seek Methods */
/* Begin Clear Methods */

void SubtitleListWidget::clearSubtitles(
    QTableWidget *table,
    QMultiMap<double, QTableWidgetItem *> &seenSubs,
    QHash<QTableWidgetItem *, const SubtitleInfo *> &startTimes)
{
    table->clearContents();
    table->setRowCount(0);
    seenSubs.clear();
    startTimes.clear();
}

void SubtitleListWidget::clearPrimarySubtitles()
{
    clearSubtitles(m_ui->tablePrim, m_seenPrimarySubs, m_timesPrimarySubs);
    m_primarySubInfoList = nullptr;
}

void SubtitleListWidget::clearSecondarySubtitles()
{
    clearSubtitles(m_ui->tableSec, m_seenSecondarySubs, m_timesSecondarySubs);
    m_secondarySubInfoList = nullptr;
}

void SubtitleListWidget::clearCachedSubtitles()
{
    clearPrimarySubtitles();
    clearSecondarySubtitles();
    m_subtitleMap.clear();
}

/* End Clear Methods */
/* Begin Helper Slots */

void SubtitleListWidget::hideSecondarySubs()
{
    m_ui->tabWidget->setTabVisible(1, false);
    m_ui->tabWidget->tabBar()->hide();
}

void SubtitleListWidget::showSecondarySubs()
{
    if (GlobalMediator::getGlobalMediator()
        ->getPlayerAdapter()->canGetSecondarySubText())
    {
        m_ui->tabWidget->setTabVisible(1, true);
        m_ui->tabWidget->tabBar()->show();
    }
}

void SubtitleListWidget::fixTableDimensions(const int index)
{
    switch(index)
    {
    case 0:
        m_ui->tablePrim->resizeRowsToContents();
        break;
    case 1:
        m_ui->tableSec->resizeRowsToContents();
        break;
    }
}

/* End Helper Slots */
