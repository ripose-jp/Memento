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

#include "player/mpvtrack.h"

MpvTrack::MpvTrack(QObject *parent) : QObject(parent)
{

}

MpvTrack::Type MpvTrack::type() const noexcept
{
    return m_type;
}

void MpvTrack::setType(MpvTrack::Type value)
{
    if (m_type == value)
    {
        return;
    }
    m_type = value;
    emit typeChanged(m_type);
}

int64_t MpvTrack::id() const noexcept
{
    return m_id;
}

void MpvTrack::setId(int64_t value)
{
    if (m_id == value)
    {
        return;
    }
    m_id = value;
    emit idChanged(m_type);
}

int64_t MpvTrack::sourceId() const noexcept
{
    return m_sourceId;
}

void MpvTrack::setSourceId(int64_t value)
{
    if (m_sourceId == value)
    {
        return;
    }
    m_sourceId = value;
    emit sourceIdChanged(m_sourceId);
}

const QString &MpvTrack::title() const noexcept
{
    return m_title;
}

void MpvTrack::setTitle(QString value)
{
    if (m_title == value)
    {
        return;
    }
    m_title = std::move(value);
    emit titleChanged(m_title);
}

const QString &MpvTrack::language() const noexcept
{
    return m_language;
}

void MpvTrack::setLanguage(QString value)
{
    if (m_language == value)
    {
        return;
    }
    m_language = std::move(value);
    emit languageChanged(m_language);
}

bool MpvTrack::albumArt() const noexcept
{
    return m_albumArt;
}

void MpvTrack::setAlbumArt(bool value)
{
    if (m_albumArt == value)
    {
        return;
    }
    m_albumArt = value;
    emit albumArtChanged(m_albumArt);
}

bool MpvTrack::defaultTrack() const noexcept
{
    return m_defaultTrack;
}

void MpvTrack::setDefaultTrack(bool value)
{
    if (m_defaultTrack == value)
    {
        return;
    }
    m_defaultTrack = value;
    emit defaultTrackChanged(m_defaultTrack);
}

bool MpvTrack::selected() const noexcept
{
    return m_selected;
}

void MpvTrack::setSelected(bool value)
{
    if (m_selected == value)
    {
        return;
    }
    m_selected = value;
    emit selectedChanged(m_selected);
}

int64_t MpvTrack::mainSelection() const noexcept
{
    return m_mainSelection;
}

void MpvTrack::setMainSelection(int64_t value)
{
    if (m_mainSelection == value)
    {
        return;
    }
    m_mainSelection = value;
    emit mainSelectionChanged(m_mainSelection);
}

bool MpvTrack::external() const noexcept
{
    return m_external;
}

void MpvTrack::setExternal(bool value)
{
    if (m_external == value)
    {
        return;
    }
    m_external = value;
    emit externalChanged(m_external);
}

const QString &MpvTrack::externalFilename() const noexcept
{
    return m_externalFilename;
}

void MpvTrack::setExternalFilename(QString value)
{
    if (m_externalFilename == value)
    {
        return;
    }
    m_externalFilename = std::move(value);
    emit externalFilenameChanged(m_externalFilename);
}

const QString &MpvTrack::codec() const noexcept
{
    return m_codec;
}

void MpvTrack::setCodec(QString value)
{
    if (m_codec == value)
    {
        return;
    }
    m_codec = std::move(value);
    emit codecChanged(m_language);
}
