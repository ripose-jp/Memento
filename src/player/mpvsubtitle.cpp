////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
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

#include "player/mpvsubtitle.h"

MpvSubtitle::MpvSubtitle(QObject *parent) : QObject(parent)
{

}

const QString &MpvSubtitle::text() const noexcept
{
    return m_text;
}

void MpvSubtitle::setText(QString value)
{
    if (m_text == value)
    {
        return;
    }
    m_text = std::move(value);
    emit textChanged(m_text);
}

double MpvSubtitle::startTime() const noexcept
{
    return m_startTime;
}

void MpvSubtitle::setStartTime(double value)
{
    if (m_startTime == value)
    {
        return;
    }
    m_startTime = value;
    emit startTimeChanged(m_startTime);
}

double MpvSubtitle::endTime() const noexcept
{
    return m_endTime;
}

void MpvSubtitle::setEndTime(double value)
{
    if (m_endTime == value)
    {
        return;
    }
    m_endTime = value;
    emit endTimeChanged(m_endTime);
}

double MpvSubtitle::delay() const noexcept
{
    return m_delay;
}

void MpvSubtitle::setDelay(double value)
{
    if (m_delay == value)
    {
        return;
    }
    m_delay = value;
    emit delayChanged(m_delay);
}

bool MpvSubtitle::visible() const noexcept
{
    return m_visible;
}

void MpvSubtitle::setVisible(bool value)
{
    if (m_visible == value)
    {
        return;
    }
    m_visible = value;
    emit visibleChanged(m_visible);
}
