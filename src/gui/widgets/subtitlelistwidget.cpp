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

#include "../../util/directoryutils.h"

#include <iterator>
#include <QMultiMap>

SubtitleListWidget::SubtitleListWidget(QWidget *parent)
    : m_player(nullptr), QListWidget(parent),
      m_seenSubtitles(new QMultiMap<double, QString>),
      m_subStartTimes(new QMultiHash<QString, double>)
{    
    connect(this, &QListWidget::itemDoubleClicked,
        this, &SubtitleListWidget::seekToSubtitle);
}

SubtitleListWidget::~SubtitleListWidget()
{
    delete m_seenSubtitles;
    delete m_subStartTimes;
}

void SubtitleListWidget::setPlayer(PlayerAdapter *player)
{
    if (m_player)
    {
        return;
    }
    
    m_player = player;
    connect(m_player, &PlayerAdapter::subtitleChanged,
        this, &SubtitleListWidget::addSubtitle);
    connect(m_player, &PlayerAdapter::subtitleTrackChanged,
        this, &SubtitleListWidget::clearSubtitles);
    connect(m_player, &PlayerAdapter::subtitleDisabled,
        this, &SubtitleListWidget::clearSubtitles);
}

QString SubtitleListWidget::getContext()
{
    QList<QListWidgetItem *> items = selectedItems();
    QString context;
    for (auto it = items.constBegin(); it != items.constEnd(); ++it)
    {
        context += (*it)->text() + "<br/>";
    }
    return context;
}

void SubtitleListWidget::addSubtitle(const QString &subtitle,
                                      const double start,
                                      const double end,
                                      const double delay)
{
    size_t i;
    auto it = m_seenSubtitles->constFind(start);
    if (it == m_seenSubtitles->constEnd() || *it != subtitle)
    {
        auto end = m_seenSubtitles->insert(start, subtitle);
        m_subStartTimes->insert(subtitle, start);

        i = std::distance(m_seenSubtitles->begin(), end);
        insertItem(i, subtitle);
    }
    else
    {
        i = std::distance(m_seenSubtitles->constBegin(), it);
    }

    clearSelection();
    setCurrentRow(i);
}

void SubtitleListWidget::clearSubtitles()
{
    clear();
    m_seenSubtitles->clear();
    m_subStartTimes->clear();
}

void SubtitleListWidget::seekToSubtitle(const QListWidgetItem *item)
{
    double pos = m_subStartTimes->value(item->text()) + m_player->getSubDelay();
    m_player->seek(pos);
}