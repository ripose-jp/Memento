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

#include "subtitle/subtitlelists.h"

SubtitleLists::SubtitleLists(QObject *parent) : QObject(parent)
{

}

SubtitleListModel *SubtitleLists::primary() const noexcept
{
    return m_primary;
}

void SubtitleLists::setPrimary(SubtitleListModel *value)
{
    if (m_primary == value)
    {
        return;
    }
    m_primary = value;
    emit primaryChanged(m_primary);
}

SubtitleListModel *SubtitleLists::secondary() const noexcept
{
    return m_secondary;
}

void SubtitleLists::setSecondary(SubtitleListModel *value)
{
    if (m_secondary == value)
    {
        return;
    }
    m_secondary = value;
    emit secondaryChanged(m_secondary);
}
