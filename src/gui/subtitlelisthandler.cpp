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

#include "subtitlelisthandler.h"

#include "../util/directoryutils.h"

#include <iterator>
#include <QMultiMap>

SubtitleListHandler::SubtitleListHandler(QListWidget *list,
                                         PlayerAdapter *player,
                                         QObject *parent)
    : m_list(list), m_player(player), QObject(parent),
      m_seenSubtitles(new QMultiMap<double, QString>),
      m_subStartTimes(new QMultiHash<QString, double>)
{
    connect(m_player, &PlayerAdapter::subtitleChanged,
        this, &SubtitleListHandler::addSubtitle);
    connect(m_player, &PlayerAdapter::subtitleTrackChanged,
        this, &SubtitleListHandler::clearSubtitles);
    connect(m_player, &PlayerAdapter::subtitleDisabled,
        this, &SubtitleListHandler::clearSubtitles);
    
    connect(m_list, &QListWidget::itemDoubleClicked,
        this, &SubtitleListHandler::seekToSubtitle);
}

SubtitleListHandler::~SubtitleListHandler()
{
    delete m_seenSubtitles;
    delete m_subStartTimes;
}

void SubtitleListHandler::addSubtitle(const QString &subtitle,
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
        m_list->insertItem(i, subtitle);
    }
    else
    {
        i = std::distance(m_seenSubtitles->constBegin(), it);
    }

    m_list->setCurrentRow(i);
}

void SubtitleListHandler::clearSubtitles()
{
    m_list->clear();
    m_seenSubtitles->clear();
    m_subStartTimes->clear();
}

void SubtitleListHandler::seekToSubtitle(const QListWidgetItem *item)
{
    double pos = m_subStartTimes->value(item->text()) + m_player->getSubDelay();
    m_player->seek(pos);
}