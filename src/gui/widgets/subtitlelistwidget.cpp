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

#include "../../util/directoryutils.h"
#include "../../util/globalmediator.h"
#include "../../util/constants.h"
#include "../playeradapter.h"

#include <iterator>
#include <QMultiMap>
#include <QApplication>
#include <QSettings>

SubtitleListWidget::SubtitleListWidget(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::SubtitleListWidget)
{
    m_ui->setupUi(this);
    setTheme();
    hideSecondarySubs();

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Signals */
    connect(m_ui->tablePrim, &QTableWidget::itemDoubleClicked, this, &SubtitleListWidget::seekToPrimarySubtitle);
    connect(m_ui->tableSec,  &QTableWidget::itemDoubleClicked, this, &SubtitleListWidget::seekToSecondarySubtitle);

    connect(mediator, &GlobalMediator::interfaceSettingsChanged, this, &SubtitleListWidget::setTheme);

    connect(mediator, &GlobalMediator::playerSubDelayChanged, this, &SubtitleListWidget::updateTimestamps);

    connect(mediator, &GlobalMediator::playerSubtitleChanged,      this, &SubtitleListWidget::addPrimarySubtitle);
    connect(mediator, &GlobalMediator::playerSubtitleTrackChanged, this, &SubtitleListWidget::clearPrimarySubtitles);
    connect(mediator, &GlobalMediator::playerSubtitlesDisabled,    this, &SubtitleListWidget::clearPrimarySubtitles);
    connect(mediator, &GlobalMediator::playerTracksChanged,        this, &SubtitleListWidget::clearPrimarySubtitles);

    connect(mediator, &GlobalMediator::playerSecSubtitleChanged,         this, &SubtitleListWidget::addSecondarySubtitle);
    connect(mediator, &GlobalMediator::playerSecondSubtitleTrackChanged, this, &SubtitleListWidget::clearSecondarySubtitles);
    connect(mediator, &GlobalMediator::playerSecondSubtitlesDisabled,    this, &SubtitleListWidget::clearSecondarySubtitles);
    connect(mediator, &GlobalMediator::playerTracksChanged,              this, &SubtitleListWidget::clearSecondarySubtitles);

    connect(mediator, &GlobalMediator::playerSecondSubtitleTrackChanged, this, &SubtitleListWidget::showSecondarySubs);
    connect(mediator, &GlobalMediator::playerSecondSubtitlesDisabled,    this, &SubtitleListWidget::hideSecondarySubs);

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

void SubtitleListWidget::setTheme()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    if (settings.value(SETTINGS_INTERFACE_STYLESHEETS, SETTINGS_INTERFACE_STYLESHEETS_DEFAULT).toBool())
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

void SubtitleListWidget::hideSecondarySubs()
{
    m_ui->tabWidget->setTabVisible(1, false);
    m_ui->tabWidget->tabBar()->hide();
}

void SubtitleListWidget::showSecondarySubs()
{
    if (GlobalMediator::getGlobalMediator()->getPlayerAdapter()->canGetSecondarySubText())
    {
        m_ui->tabWidget->setTabVisible(1, true);
        m_ui->tabWidget->tabBar()->show();
    }
}

QString SubtitleListWidget::getContext(const QTableWidget *table, 
                                       const QString      &seperator) const
{
    QList<QTableWidgetItem *> items = table->selectedItems();
    QString context;
    for (const QTableWidgetItem *item : items)
    {
        context += item->text().replace('\n', seperator) + seperator;
    }
    return context;
}

QString SubtitleListWidget::getPrimaryContext(const QString &seperator) const
{
    return getContext(m_ui->tablePrim, seperator);
}

QString SubtitleListWidget::getSecondaryContext(const QString &seperator) const
{
    return getContext(m_ui->tableSec, seperator);
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

void SubtitleListWidget::addSubtitle(QTableWidget                      *table,
                                     QMap<double, QTableWidgetItem *>  &seenSubs,
                                     QHash<QTableWidgetItem *, double> &startTimes,
                                     const QString                     &subtitle,
                                     const double                       start,
                                     const double                       delay)
{
    size_t i;
    auto it = seenSubs.constFind(start);
    if (it == seenSubs.constEnd() || (*it)->text() != subtitle)
    {
        QTableWidgetItem *subtitleItem = new QTableWidgetItem(subtitle);
        table->setWordWrap(true);
        startTimes.insert(subtitleItem, start);
        auto end = seenSubs.insert(start, subtitleItem);
        i = std::distance(seenSubs.begin(), end);

        QTableWidgetItem *timecodeItem = new QTableWidgetItem(formatTimecode(start + delay));
        timecodeItem->setFlags(Qt::NoItemFlags);

        table->insertRow(i);
        table->setItem(i, 1, subtitleItem);
        table->setItem(i, 0, timecodeItem);
        table->resizeRowToContents(i);
    }
    else
    {
        i = std::distance(seenSubs.constBegin(), it);
    }

    table->clearSelection();
    table->setCurrentCell(i, 1);
}

void SubtitleListWidget::addPrimarySubtitle(const QString &subtitle,
                                            const double   start,
                                            const double   end, 
                                            const double   delay)
{
    addSubtitle(m_ui->tablePrim, m_seenPrimarySubs, m_timesPrimarySubs, subtitle, start, delay);
}

#define TIME_DELTA 5

void SubtitleListWidget::addSecondarySubtitle(const QString &subtitle,
                                                    double   start,
                                              const double   delay)
{
    /* There is no secondary-sub-start property, so start times are approximate.
     * Checking that this subtitle is not duplicated is neccessary
     */
    QList<double> times = m_subSecondaryToTime.values(subtitle);
    for (const double time : times)
    {
        const double low  = time - TIME_DELTA;
        const double high = time + TIME_DELTA;
        if (low < start && start < high)
        {
            /* Assume this subtitle has been seen */
            start = time;
            break; 
        }
    }

    /* Add the subtitle to the seen list */
    if (times.isEmpty())
    {
        m_subSecondaryToTime.insert(subtitle, start);
    }

    addSubtitle(m_ui->tableSec, m_seenSecondarySubs, m_timesSecondarySubs, subtitle, start, delay);
}

#undef TIME_DELTA

void SubtitleListWidget::updateTimestampsHelper(QTableWidget                           *table,
                                                const QMap<double, QTableWidgetItem *> &seenSubs,
                                                const double                            delay)
{
    size_t i = 0;
    for (auto it = seenSubs.keyBegin(); it != seenSubs.keyEnd(); ++it)
    {
        const double time = *it + delay < 0 ? 0 : *it + delay;
        QTableWidgetItem *timecodeItem = new QTableWidgetItem(formatTimecode(time));
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

void SubtitleListWidget::clearSubtitles(QTableWidget                      *table,
                                        QMap<double, QTableWidgetItem *>  &seenSubs,
                                        QHash<QTableWidgetItem *, double> &startTimes)
{
    table->clearContents();
    table->setRowCount(0);
    seenSubs.clear();
    startTimes.clear();
}

void SubtitleListWidget::clearPrimarySubtitles()
{
    clearSubtitles(m_ui->tablePrim, m_seenPrimarySubs, m_timesPrimarySubs);
}

void SubtitleListWidget::clearSecondarySubtitles()
{
    m_subSecondaryToTime.clear();
    clearSubtitles(m_ui->tableSec, m_seenSecondarySubs, m_timesSecondarySubs);
}

void SubtitleListWidget::seekToSubtitle(QTableWidgetItem                        *item,
                                        const QHash<QTableWidgetItem *, double> &startTimes) const
{
    PlayerAdapter *player = GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    double         pos    = startTimes[item] + player->getSubDelay();
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

void SubtitleListWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    QApplication::processEvents();
    m_ui->tablePrim->scrollToItem(m_ui->tablePrim->currentItem());
    m_ui->tableSec->scrollToItem(m_ui->tableSec->currentItem());

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