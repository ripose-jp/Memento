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

    connect(m_ui->tablePrim, &QTableWidget::itemDoubleClicked, this, &SubtitleListWidget::seekToSubtitle);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    connect(mediator, &GlobalMediator::interfaceSettingsChanged,    this, &SubtitleListWidget::setTheme);
    connect(mediator, &GlobalMediator::playerSubtitleChanged,       this, &SubtitleListWidget::addSubtitle);
    connect(mediator, &GlobalMediator::playerSubDelayChanged,       this, &SubtitleListWidget::updateTimestamps);
    connect(mediator, &GlobalMediator::playerSubtitleTrackChanged,  this, &SubtitleListWidget::clearSubtitles);
    connect(mediator, &GlobalMediator::playerSubtitlesDisabled,     this, &SubtitleListWidget::clearSubtitles);
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

    m_ui->tablePrim->setColumnHidden(0, 
        !settings.value(
            SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS, 
            SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS_DEFAULT
        ).toBool()
    );
    settings.endGroup();
}

QString SubtitleListWidget::getContext(const QString &seperator)
{
    QList<QTableWidgetItem *> items = m_ui->tablePrim->selectedItems();
    QString context;
    for (const QTableWidgetItem *item : items)
    {
        context += item->text().replace('\n', seperator) + seperator;
    }
    return context;
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

void SubtitleListWidget::addSubtitle(const QString &subtitle,
                                     const double start,
                                     const double end, 
                                     const double delay)
{
    size_t i;
    auto it = m_seenSubtitles.constFind(start);
    if (it == m_seenSubtitles.constEnd() || *it != subtitle)
    {
        auto end = m_seenSubtitles.insert(start, subtitle);
        m_subStartTimes.insert(subtitle, start);

        i = std::distance(m_seenSubtitles.begin(), end);

        m_ui->tablePrim->insertRow(i);

        QTableWidgetItem *timecodeItem = new QTableWidgetItem(formatTimecode(start + delay));
        timecodeItem->setFlags(Qt::NoItemFlags);
        m_ui->tablePrim->setItem(i, 0, timecodeItem);

        QTableWidgetItem *subtitleItem = new QTableWidgetItem(subtitle);
        m_ui->tablePrim->setWordWrap(true);
        m_ui->tablePrim->setItem(i, 1, subtitleItem);

        m_ui->tablePrim->resizeRowToContents(i);
    }
    else
    {
        i = std::distance(m_seenSubtitles.constBegin(), it);
    }

    m_ui->tablePrim->clearSelection();
    m_ui->tablePrim->setCurrentCell(i, 1);
}

void SubtitleListWidget::updateTimestamps(const double delay)
{
    m_ui->tablePrim->clearContents();

    size_t i = 0;
    for (auto it = m_seenSubtitles.constKeyValueBegin(); it != m_seenSubtitles.constKeyValueEnd(); ++it)
    {
        const double time = it->first + delay < 0 ? 0 : it->first + delay;
        QTableWidgetItem *timecodeItem = new QTableWidgetItem(formatTimecode(time));
        timecodeItem->setFlags(Qt::NoItemFlags);
        m_ui->tablePrim->setItem(i, 0, timecodeItem);

        QTableWidgetItem *subtitleItem = new QTableWidgetItem(it->second);
        m_ui->tablePrim->setWordWrap(true);
        m_ui->tablePrim->setItem(i, 1, subtitleItem);

        i++;
    }

    m_ui->tablePrim->resizeRowsToContents();
}

void SubtitleListWidget::clearSubtitles()
{
    m_ui->tablePrim->clearContents();
    m_ui->tablePrim->setRowCount(0);
    m_seenSubtitles.clear();
    m_subStartTimes.clear();
}

void SubtitleListWidget::seekToSubtitle(QTableWidgetItem *item)
{
    PlayerAdapter *player   = GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    QString        subtitle = item->text();
    double         pos      = m_subStartTimes.value(subtitle) + player->getSubDelay();
    if (pos < 0)
    {
        pos = 0;
    }
    player->seek(pos);
}

void SubtitleListWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    QApplication::processEvents();
    m_ui->tablePrim->scrollToItem(m_ui->tablePrim->currentItem());

    Q_EMIT GlobalMediator::getGlobalMediator()->subtitleListShown();
}

void SubtitleListWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);

    const QList<QTableWidgetItem *> &items = m_ui->tablePrim->selectedItems();
    if (!items.isEmpty())
    {
        QApplication::processEvents();
        m_ui->tablePrim->scrollToItem(items.last());
    }
    Q_EMIT GlobalMediator::getGlobalMediator()->subtitleListHidden();
}

void SubtitleListWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_ui->tablePrim->resizeRowsToContents();
}