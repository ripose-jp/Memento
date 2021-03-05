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

#ifndef SUBTITLELISTHANDLER_H
#define SUBTITLELISTHANDLER_H

#include <QListWidget>

#include "playeradapter.h"

struct mpv_handle;
struct mpv_event;

class SubtitleListHandler : public QObject
{
    Q_OBJECT

public:
    SubtitleListHandler(QListWidget *list,
                        PlayerAdapter *player,
                        QObject *parent = nullptr);
    ~SubtitleListHandler();

public Q_SLOTS:
    void open(const QString &file);
    void populateList(int64_t track);

private:
    QListWidget *m_list;
    mpv_handle *m_mpv;
    PlayerAdapter *m_player;
    QList<double> m_startTimes;
};

#endif // SUBTITLELISTHANDLER_H