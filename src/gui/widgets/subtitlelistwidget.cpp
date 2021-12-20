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
#include <QThreadPool>

#include "../../util/constants.h"
#include "../../util/globalmediator.h"
#include "../../util/subtitleparser.h"
#include "../../util/utils.h"

/* Begin Constructor/Destructors */

SubtitleListWidget::SubtitleListWidget(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::SubtitleListWidget)
{
    m_ui->setupUi(this);

    m_primary.table = m_ui->tablePrim;
    m_secondary.table = m_ui->tableSec;

    initTheme();
    initRegex();
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
        mediator, &GlobalMediator::searchSettingsChanged,
        this,     &SubtitleListWidget::initRegex
    );
    connect(
        mediator, &GlobalMediator::playerSubDelayChanged,
        this,     &SubtitleListWidget::updateTimestamps
    );
    connect(
        this, &SubtitleListWidget::requestRefresh,
        this, &SubtitleListWidget::handleRefresh
    );

    connect(
        mediator, &GlobalMediator::playerSubtitleChangedRaw,
        this,     &SubtitleListWidget::updatePrimarySubtitle
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
        this,     &SubtitleListWidget::updateSecondarySubtitle
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

void SubtitleListWidget::initRegex()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);
    m_subRegexLock.lock();
    m_subRegex.setPattern(
        settings.value(
            SETTINGS_SEARCH_REMOVE_REGEX,
            DEFAULT_REMOVE_REGEX
        ).toString()
    );
    m_subRegexLock.unlock();
    settings.endGroup();

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    if (player)
    {
        QList<const Track *> tracks = player->getTracks();
        handleTracklistChange(tracks);
        for (const Track *track : tracks)
        {
            delete track;
        }
    }
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
    QStringList extTracks;
    QList<int64_t> extSids;
    for (const Track *track : tracks)
    {
        if (track->type == Track::subtitle)
        {
            if (track->selected)
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

            if (track->external)
            {
                extTracks << track->externalFilename;
                extSids << track->id;
            }
        }
    }

    QThreadPool::globalInstance()->start([=] {
        SubtitleParser parser;

        m_primary.lock.lock();
        m_secondary.lock.lock();

        for (size_t i = 0; i < extTracks.size(); ++i)
        {
            m_subtitleMap[extSids[i]] =
                parser.parseSubtitles(extTracks[i], false);
            m_subtitleParsed[extSids[i]] = true;
        }

        m_secondary.lock.unlock();
        m_primary.lock.unlock();

        Q_EMIT requestRefresh();
    });

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

void SubtitleListWidget::handleRefresh()
{
    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    int64_t sid = player->getSubtitleTrack();
    if (sid > 0)
    {
        handlePrimaryTrackChange(sid);
    }

    sid = player->getSecondarySubtitleTrack();
    if (sid > 0)
    {
        handleSecondaryTrackChange(sid);
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
    SubtitleList &list,
    const SubtitleInfo &info,
    double delay,
    bool regex)
{
    QTableWidgetItem *subtitleItem;
    if (regex)
    {
        QString text = QString(info.text).remove(m_subRegex);
        if (text.isEmpty())
        {
            return nullptr;
        }
        subtitleItem = new QTableWidgetItem(text);
    }
    else
    {
        subtitleItem = new QTableWidgetItem(info.text);
    }
    list.itemToSub.insert(subtitleItem, &info);
    QStringList lines = info.text.split('\n');
    for (const QString &line : lines)
    {
        list.lineToItem.insert(line, subtitleItem);
    }
    auto endIt = list.startToItem.insert(info.start, subtitleItem);

    /* This is a bit of a hack to get the subtitles to appear in order.
     * If a subtitle starts at the same start time as another, the subtitle
     * that appears later will be closer to seenSubs.begin() than the
     * earlier subtitle. To get around this, we assume the subtitle being
     * added came after all subtitles with the same start time. This isn't
     * going to work 100% of the time, but it's right enough of the time to
     * be worth it.
     */
    size_t i = std::distance(list.startToItem.begin(), endIt);
    for (endIt += 1;
         endIt != list.startToItem.end() && endIt.key() == info.start;
         ++endIt)
    {
        ++i;
    }

    QTableWidgetItem *timecodeItem =
        new QTableWidgetItem(formatTimecode(info.start + delay));
    timecodeItem->setFlags(Qt::NoItemFlags);

    list.table->insertRow(i);
    list.table->setItem(i, 1, subtitleItem);
    list.table->setItem(i, 0, timecodeItem);
    list.table->resizeRowToContents(i);

    return subtitleItem;
}

#define TIME_DELTA 0.0001

void SubtitleListWidget::addSubtitle(
    SubtitleList &list,
    const QString &subtitle,
    const double start,
    const double end,
    const double delay)
{
    /* Check if we have already seen this subtitle. Finds it if we have. */
    auto it = list.startToItem.lowerBound(start - TIME_DELTA);
    while (it != list.startToItem.end() && it.key() - start <= TIME_DELTA)
    {
        if ((*it)->text() == subtitle)
        {
            break;
        }
        ++it;
    }

    QTableWidgetItem *subtitleItem = nullptr;
    if (it == list.startToItem.end() || it.key() - start > TIME_DELTA)
    {
        list.subList->append(SubtitleInfo{
            .text = subtitle,
            .start = start,
            .end = end
        });

        subtitleItem = addTableItem(list, list.subList->last(), delay);
    }
    else
    {
        subtitleItem = *it;
    }

    list.table->clearSelection();
    list.table->setCurrentItem(subtitleItem);
}

#undef TIME_DELTA

void SubtitleListWidget::handlePrimaryTrackChange(int64_t sid)
{
    m_primary.lock.lock();

    clearSubtitles(m_primary);
    m_primary.subList = &m_subtitleMap[sid];
    m_primary.subsParsed = &m_subtitleParsed[sid];

    double delay =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter()->getSubDelay();
    m_subRegexLock.lock();
    for (const SubtitleInfo &info : *m_primary.subList)
    {
        addTableItem(m_primary, info, delay, true);
    }
    m_subRegexLock.unlock();

    m_primary.lock.unlock();
}

void SubtitleListWidget::handleSecondaryTrackChange(int64_t sid)
{
    m_secondary.lock.lock();

    clearSubtitles(m_secondary);
    m_secondary.subList = &m_subtitleMap[sid];
    m_secondary.subsParsed = &m_subtitleParsed[sid];

    double delay =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter()->getSubDelay();
    for (const SubtitleInfo &info : *m_secondary.subList)
    {
        addTableItem(m_secondary, info, delay);
    }

    m_secondary.lock.unlock();
}

#define TIME_DELTA 0.001

void SubtitleListWidget::selectSubtitles(SubtitleList &list,
                                         const QString &subtitle,
                                         double delay)
{
    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    double time = player->getTimePos() - delay;

    list.table->clearSelection();

    QStringList lines = subtitle.split('\n');
    for (const QString &line : lines)
    {
        QList<QTableWidgetItem *> items = list.lineToItem.values(line);
        for (QTableWidgetItem *item : items)
        {
            const SubtitleInfo *info = list.itemToSub[item];
            if (info->start <= time + TIME_DELTA &&
                info->end >= time - TIME_DELTA)
            {
                item->setSelected(true);
                list.table->scrollToItem(item);
            }
        }
    }
}

#undef TIME_DELTA

void SubtitleListWidget::updatePrimarySubtitle(QString subtitle,
                                               double start,
                                               double end,
                                               double delay)
{
    if (!m_primary.lock.try_lock())
    {
        return;
    }
    if (*m_primary.subsParsed)
    {
        selectSubtitles(m_primary, subtitle, delay);
    }
    else
    {
        m_subRegexLock.lock();
        subtitle.remove(m_subRegex);
        m_subRegexLock.unlock();
        addSubtitle(m_primary, subtitle, start, end, delay);
    }
    m_primary.lock.unlock();
}

void SubtitleListWidget::updateSecondarySubtitle(const QString &subtitle,
                                                 double start,
                                                 double end,
                                                 double delay)
{
    if (!m_secondary.lock.try_lock())
    {
        return;
    }
    if (*m_secondary.subsParsed)
    {
        selectSubtitles(m_secondary, subtitle, delay);
    }
    else
    {
        addSubtitle(m_secondary, subtitle, start, end, delay);
    }
    m_secondary.lock.unlock();
}

void SubtitleListWidget::updateTimestampsHelper(SubtitleList &list,
                                                double delay)
{
    size_t i = 0;
    for (auto it = list.startToItem.keyBegin();
         it != list.startToItem.keyEnd();
         ++it)
    {
        const double time = *it + delay < 0 ? 0 : *it + delay;
        QTableWidgetItem *timecodeItem =
            new QTableWidgetItem(formatTimecode(time));
        timecodeItem->setFlags(Qt::NoItemFlags);
        list.table->setItem(i, 0, timecodeItem);

        i++;
    }
    list.table->resizeRowsToContents();
}

void SubtitleListWidget::updateTimestamps(const double delay)
{
    updateTimestampsHelper(m_primary, delay);
    updateTimestampsHelper(m_secondary, delay);
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

inline QPair<double, double> SubtitleListWidget::getContextTime(
    const SubtitleList &list) const
{
    double start = 0.0;
    double end = 0.0;

    QList<QTableWidgetItem *> items = list.table->selectedItems();
    if (!items.isEmpty())
    {
        start = list.itemToSub[items.first()]->start;
        end = list.itemToSub[items.last()]->end;
    }

    return QPair<double, double>(start, end);
}

QPair<double, double> SubtitleListWidget::getPrimaryContextTime() const
{
    return getContextTime(m_primary);
}

/* End Context Methods */
/* Begin Seek Methods */

#define SEEK_ERROR 0.028

void SubtitleListWidget::seekToSubtitle(QTableWidgetItem *item,
                                        const SubtitleList &list) const
{
    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    double pos =
        list.itemToSub[item]->start +
        player->getSubDelay() +
        SEEK_ERROR;
    if (pos < 0)
    {
        pos = 0;
    }
    player->seek(pos);
}

#undef SEEK_ERROR

void SubtitleListWidget::seekToPrimarySubtitle(QTableWidgetItem *item) const
{
    seekToSubtitle(item, m_primary);
}

void SubtitleListWidget::seekToSecondarySubtitle(QTableWidgetItem *item) const
{
    seekToSubtitle(item, m_secondary);
}

/* End Seek Methods */
/* Begin Clear Methods */

void SubtitleListWidget::clearSubtitles(SubtitleList &list)
{
    list.table->clearContents();
    list.table->setRowCount(0);
    list.subList = nullptr;
    list.subsParsed = nullptr;
    list.startToItem.clear();
    list.itemToSub.clear();
    list.lineToItem.clear();
}

void SubtitleListWidget::clearPrimarySubtitles()
{
    m_primary.lock.lock();
    clearSubtitles(m_primary);
    m_primary.lock.unlock();
}

void SubtitleListWidget::clearSecondarySubtitles()
{
    m_secondary.lock.lock();
    clearSubtitles(m_secondary);
    m_secondary.lock.unlock();
}

void SubtitleListWidget::clearCachedSubtitles()
{
    clearPrimarySubtitles();
    clearSecondarySubtitles();
    m_subtitleMap.clear();
    m_subtitleParsed.clear();
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
