////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 Ripose
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

#include "cursortimer.h"

#include <QSettings>

#include "util/constants.h"

OSCTimer::OSCTimer(QObject *parent) : CursorTimer(parent)
{
    m_timer.setSingleShot(true);
    QSettings settings;
    settings.beginGroup(Constants::Settings::Behavior::GROUP);
    int timeout = settings.value(
        Constants::Settings::Behavior::OSC_DURATION,
        Constants::Settings::Behavior::OSC_DURATION_DEFAULT
    ).toInt();
    timeout += settings.value(
        Constants::Settings::Behavior::OSC_FADE,
        Constants::Settings::Behavior::OSC_FADE_DEFAULT
    ).toInt();
    m_timer.setInterval(timeout);
    settings.endGroup();

    connect(&m_timer, &QTimer::timeout, this, &CursorTimer::hideCursor);
}

OSCTimer::~OSCTimer()
{
    disconnect();
}

void OSCTimer::start()
{
    m_timer.start();
    Q_EMIT showCursor();
}

void OSCTimer::stop()
{
    m_timer.stop();
}

void OSCTimer::forceTimeout()
{
    m_timer.stop();
    Q_EMIT hideCursor();
}
