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

#include "player/mpvstate.h"

#include <QtAlgorithms>

#include "player/mpvplayer.h"

MpvState::MpvState(MpvPlayer *parent) :
    QObject(parent),
    m_player(parent)
{

}

MpvPlayer *MpvState::player() const noexcept
{
    return m_player;
}

void MpvState::setPlayer(MpvPlayer *value)
{
    if (m_player == value)
    {
        return;
    }
    m_player = value;
    setParent(m_player);
    emit playerChanged();
}

int64_t MpvState::videoWidth() const noexcept
{
    return m_videoWidth;
}

void MpvState::setVideoWidth(int64_t value)
{
    if (m_videoWidth == value)
    {
        return;
    }
    m_videoWidth = value;
    emit videoWidthChanged(m_videoWidth);
}

int64_t MpvState::videoHeight() const noexcept
{
    return m_videoHeight;
}

void MpvState::setVideoHeight(int64_t value)
{
    if (m_videoHeight == value)
    {
        return;
    }
    m_videoHeight = value;
    emit videoHeightChanged(m_videoHeight);
}

const QList<double> &MpvState::chapters() const noexcept
{
    return m_chapters;
}

void MpvState::setChapters(QList<double> values)
{
    m_chapters = std::move(values);
    emit chaptersChanged(m_chapters);
}

double MpvState::duration() const noexcept
{
    return m_duration;
}

void MpvState::setDuration(double value)
{
    if (m_duration == value)
    {
        return;
    }
    m_duration = value;
    emit durationChanged(m_duration);
}

bool MpvState::fullscreen() const noexcept
{
    return m_fullscreen;
}

void MpvState::setFullscreen(bool value)
{
    if (m_fullscreen == value)
    {
        return;
    }
    m_fullscreen = value;
    emit fullscreenChanged(m_fullscreen);
}

const QString &MpvState::title() const noexcept
{
    return m_title;
}

void MpvState::setTitle(QString value)
{
    if (m_title == value)
    {
        return;
    }
    m_title = std::move(value);
    emit titleChanged(m_title);
}

const QString &MpvState::path() const noexcept
{
    return m_path;
}

void MpvState::setPath(QString value)
{
    if (m_path == value)
    {
        return;
    }
    m_path = std::move(value);
    emit pathChanged(m_path);
}

bool MpvState::pause() const noexcept
{
    return m_pause;
}

void MpvState::setPause(bool value)
{
    if (m_pause == value)
    {
        return;
    }
    m_pause = value;
    emit pauseChanged(m_pause);
}

double MpvState::timePosition() const noexcept
{
    return m_timePosition;
}

void MpvState::setTimePosition(double value)
{
    if (m_timePosition == value)
    {
        return;
    }
    m_timePosition = value;
    emit timePositionChanged(m_timePosition);
}

QQmlListProperty<MpvTrack> MpvState::audioTracksQml()
{
    return QQmlListProperty<MpvTrack>(this, &m_audioTracks);
}

const QList<MpvTrack *> &MpvState::audioTracks() const noexcept
{
    return m_audioTracks;
}

void MpvState::setAudioTracks(const QList<MpvTrack *> &tracks)
{
    qDeleteAll(m_audioTracks);
    m_audioTracks.clear();
    for (MpvTrack *track : tracks)
    {
        track->setParent(this);
        m_audioTracks.emplaceBack(track);
    }
    emit audioTracksChanged();
}

QQmlListProperty<MpvTrack> MpvState::videoTracksQml()
{
    return QQmlListProperty<MpvTrack>(this, &m_videoTracks);
}

const QList<MpvTrack *> &MpvState::videoTracks() const noexcept
{
    return m_videoTracks;
}

void MpvState::setVideoTracks(const QList<MpvTrack *> &tracks)
{
    qDeleteAll(m_videoTracks);
    m_videoTracks.clear();
    for (MpvTrack *track : tracks)
    {
        track->setParent(this);
        m_videoTracks.emplaceBack(track);
    }
    emit videoTracksChanged();
}

QQmlListProperty<MpvTrack> MpvState::subtitleTracksQml()
{
    return QQmlListProperty<MpvTrack>(this, &m_subtitleTracks);
}

const QList<MpvTrack *> &MpvState::subtitleTracks() const noexcept
{
    return m_subtitleTracks;
}

void MpvState::setSubtitleTracks(const QList<MpvTrack *> &tracks)
{
    qDeleteAll(m_subtitleTracks);
    m_subtitleTracks.clear();
    for (MpvTrack *track : tracks)
    {
        track->setParent(this);
        m_subtitleTracks.emplaceBack(track);
    }
    emit subtitleTracksChanged();
}

void MpvState::setTracks(const mpv_node *node)
{
    if (node->format != MPV_FORMAT_NODE_ARRAY)
    {
        return;
    }

    m_audioTracks.clear();
    m_videoTracks.clear();
    m_subtitleTracks.clear();

    for (int i = 0; i < node->u.list->num; i++)
    {
        if (node->u.list->values[i].format != MPV_FORMAT_NODE_MAP)
        {
            continue;
        }

        MpvTrack *track = new MpvTrack(this);
        for (int n = 0; n < node->u.list->values[i].u.list->num; n++)
        {
            if (QString(node->u.list->values[i].u.list->keys[n]) == "id")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                {
                    track->setId(node->u.list->values[i].u.list->values[n].u.int64);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "type")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                {
                    QString type = node->u.list->values[i].u.list->values[n].u.string;
                    if (type == "audio")
                    {
                        track->setType(MpvTrack::Type::Audio);
                    }
                    else if (type == "video")
                    {
                        track->setType(MpvTrack::Type::Video);
                    }
                    else if (type == "sub")
                    {
                        track->setType(MpvTrack::Type::Subtitle);
                    }
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "src-id")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                {
                    track->setSourceId(node->u.list->values[i].u.list->values[n].u.int64);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "title")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                {
                    track->setTitle(node->u.list->values[i].u.list->values[n].u.string);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "lang")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                {
                    track->setLanguage(node->u.list->values[i].u.list->values[n].u.string);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "albumart")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                {
                    track->setAlbumArt(node->u.list->values[i].u.list->values[n].u.flag != 0);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "default")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                {
                    track->setDefaultTrack(node->u.list->values[i].u.list->values[n].u.flag != 0);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "selected")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                {
                    track->setSelected(node->u.list->values[i].u.list->values[n].u.flag != 0);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "main-selection")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                {
                    track->setMainSelection(node->u.list->values[i].u.list->values[n].u.int64);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "external")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                {
                    track->setExternal(node->u.list->values[i].u.list->values[n].u.flag != 0);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "external-filename")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                {
                    track->setExternalFilename(node->u.list->values[i].u.list->values[n].u.string);
                }
            }
            else if (QString(node->u.list->values[i].u.list->keys[n]) == "codec")
            {
                if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                {
                    track->setCodec(node->u.list->values[i].u.list->values[n].u.string);
                }
            }
        }

        switch (track->type())
        {
            case MpvTrack::Type::Audio:
                m_audioTracks.emplaceBack(track);
                break;

            case MpvTrack::Type::Video:
                m_videoTracks.emplaceBack(track);
                break;

            case MpvTrack::Type::Subtitle:
                m_subtitleTracks.emplaceBack(track);
                break;

            case MpvTrack::Type::None:
            default:
                track->deleteLater();
                break;
        }
    }

    emit audioTracksChanged();
    emit videoTracksChanged();
    emit subtitleTracksChanged();
}

int64_t MpvState::volume() const noexcept
{
    return m_volume;
}

void MpvState::setVolume(int64_t value)
{
    if (m_volume == value)
    {
        return;
    }
    m_volume = value;
    emit volumeChanged(m_volume);
}

int64_t MpvState::maxVolume() const noexcept
{
    return m_maxVolume;
}

void MpvState::setMaxVolume(int64_t value)
{
    if (m_maxVolume == value)
    {
        return;
    }
    m_maxVolume = value;
    emit maxVolumeChanged(m_maxVolume);
}

int64_t MpvState::aid() const noexcept
{
    return m_aid;
}

void MpvState::setAid(int64_t value)
{
    if (m_aid == value)
    {
        return;
    }
    m_aid = value;
    emit aidChanged(m_aid);
}

int64_t MpvState::sid() const noexcept
{
    return m_sid;
}

void MpvState::setSid(int64_t value)
{
    if (m_sid == value)
    {
        return;
    }
    m_sid = value;
    emit sidChanged(m_sid);
}

int64_t MpvState::secondarySid() const noexcept
{
    return m_secondarySid;
}

void MpvState::setSecondarySid(int64_t value)
{
    if (m_secondarySid == value)
    {
        return;
    }
    m_secondarySid = value;
    emit secondarySidChanged(m_secondarySid);
}


int64_t MpvState::vid() const noexcept
{
    return m_vid;
}

void MpvState::setVid(int64_t value)
{
    if (m_vid == value)
    {
        return;
    }
    m_vid = value;
    emit vidChanged(m_vid);
}

MpvSubtitle *MpvState::subtitle() const noexcept
{
    return m_subtitle;
}

void MpvState::setSubtitle(MpvSubtitle *value)
{
    if (m_subtitle == value)
    {
        return;
    }

    if (value)
    {
        value->setParent(this);
    }
    if (m_subtitle)
    {
        m_subtitle->deleteLater();
    }
    m_subtitle = value;
    emit subtitleChanged(m_subtitle);
}

MpvSubtitle *MpvState::secondarySubtitle() const noexcept
{
    return m_secondarySubtitle;
}

void MpvState::setSecondarySubtitle(MpvSubtitle *value)
{
    if (m_secondarySubtitle == value)
    {
        return;
    }

    if (value)
    {
        value->setParent(this);
    }
    if (m_secondarySubtitle)
    {
        m_secondarySubtitle->deleteLater();
    }
    m_secondarySubtitle = value;
    emit subtitleChanged(m_secondarySubtitle);
}

MpvState::AutoHideType MpvState::cursorAutoHideType() const noexcept
{
    return m_cursorAutoHideType;
}

void MpvState::setCursorAutoHideType(AutoHideType value)
{
    if (value == m_cursorAutoHideType)
    {
        return;
    }
    m_cursorAutoHideType = value;
    emit cursorAutoHideTypeChanged(m_cursorAutoHideType);
}

int64_t MpvState::cursorAutoHideTime() const noexcept
{
    return m_cursorAutoHideTime;
}

void MpvState::setCursorAutoHideTime(int64_t value)
{
    if (value == m_cursorAutoHideTime)
    {
        return;
    }
    m_cursorAutoHideTime = value;
    emit cursorAutoHideTimeChanged(m_cursorAutoHideTime);
}

bool MpvState::cursorAutoHideFullscreenOnly() const noexcept
{
    return m_cursorAutoHideFullscreenOnly;
}

void MpvState::setCursorAutoHideFullscreenOnly(bool value)
{
    if (value == m_cursorAutoHideFullscreenOnly)
    {
        return;
    }
    m_cursorAutoHideFullscreenOnly = value;
    emit cursorAutoHideFullscreenOnlyChanged(m_cursorAutoHideFullscreenOnly);
}
