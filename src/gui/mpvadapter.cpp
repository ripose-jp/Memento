////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#include "mpvadapter.h"

#include <QDebug>

MpvAdapter::MpvAdapter(MpvWidget *mpv, QObject *parent) : m_mpv(mpv),
                                                          PlayerAdapter(parent)
{
    connect(m_mpv, &MpvWidget::tracklistChanged, 
        this, &MpvAdapter::processTracks);
    connect(m_mpv, &MpvWidget::audioTrackChanged, 
        this, &MpvAdapter::audioTrackChanged);
    connect(m_mpv, &MpvWidget::videoTrackChanged, 
        this, &MpvAdapter::videoTrackChanged);
    connect(m_mpv, &MpvWidget::subtitleTrackChanged,
        this, &MpvAdapter::subtitleTrackChanged);

    connect(m_mpv, &MpvWidget::audioDisabled,
        this, &MpvAdapter::audioDisabled);
    connect(m_mpv, &MpvWidget::videoDisabled,
        this, &MpvAdapter::videoDisabled);
    connect(m_mpv, &MpvWidget::subtitleDisabled,
        this, &MpvAdapter::subtitleDisabled);

    connect(m_mpv, &MpvWidget::subtitleChanged,
        this, &MpvAdapter::processSubtitle);
    connect(m_mpv, &MpvWidget::durationChanged,
        this, &MpvAdapter::durationChanged);
    connect(m_mpv, &MpvWidget::positionChanged,
        this, &MpvAdapter::positionChanged);
    connect(m_mpv, &MpvWidget::stateChanged,
        this, &MpvAdapter::stateChanged);
    connect(m_mpv, &MpvWidget::fullscreenChanged,
        this, &MpvAdapter::fullscreenChanged);
    connect(m_mpv, &MpvWidget::volumeChanged,
        this, &MpvAdapter::volumeChanged);
    connect(m_mpv, &MpvWidget::titleChanged,
        [=] (const char **name) { Q_EMIT titleChanged(QString(*name)); } );

    connect(m_mpv, &MpvWidget::hideCursor, this, &MpvAdapter::hideCursor);
    connect(m_mpv, &MpvWidget::shutdown, this, &MpvAdapter::close);
}

void MpvAdapter::open(const QString &file, const bool append)
{
    if (file.isEmpty())
        return;
    
    QStringList command;
    command << "loadfile" << file;
    if (append)
        command << "append";
    m_mpv->command(command);
}

void MpvAdapter::open(const QList<QUrl> &files)
{
    if (files.isEmpty())
        return;

    open(files.first().toLocalFile()); // mpv won't start with loadfile append
    for (auto it = files.begin() + 1; it != files.end(); ++it)
    {
        if (!(*it).toLocalFile().isEmpty())
            open((*it).toLocalFile(), true);
    }
}

void MpvAdapter::seek(const int time)
{
    m_mpv->asyncCommand(QVariantList() << "seek" << time << "absolute");
}

void MpvAdapter::play()
{
    m_mpv->setProperty("pause", false);
}

void MpvAdapter::pause()
{
    m_mpv->setProperty("pause", true);
}

void MpvAdapter::stop()
{
    m_mpv->asyncCommand(QVariantList() << "stop");
}

void MpvAdapter::seekForward()
{
    m_mpv->asyncCommand(QVariantList() << "sub-seek" << 1);
}

void MpvAdapter::seekBackward()
{
    m_mpv->asyncCommand(QVariantList() << "sub-seek" << -1);
}

void MpvAdapter::skipForward()
{
    m_mpv->asyncCommand(QVariantList() << "playlist-next");
}

void MpvAdapter::skipBackward()
{
    m_mpv->asyncCommand(QVariantList() << "playlist-prev");
}

void MpvAdapter::disableAudio()
{
    m_mpv->setProperty("aid", "no");
}

void MpvAdapter::disableVideo()
{
    m_mpv->setProperty("vid", "no");
}

void MpvAdapter::disableSubtitles()
{
    m_mpv->setProperty("sid", "no");
}

void MpvAdapter::setAudioTrack(const int id)
{
    m_mpv->setProperty("aid", id);
}

void MpvAdapter::setVideoTrack(const int id)
{
    m_mpv->setProperty("vid", id);
}

void MpvAdapter::setSubtitleTrack(const int id)
{
    m_mpv->setProperty("sid", id);
}

void MpvAdapter::setFullscreen(const bool value)
{
    m_mpv->setProperty("fullscreen", value);
}

void MpvAdapter::setVolume(const int value)
{
    m_mpv->setProperty("volume", value);
}

void MpvAdapter::keyPressed(const QKeyEvent *event)
{
    QString key = "";
    if (event->modifiers() & Qt::ShiftModifier)
    {
        key += "Shift+";
    }
    if (event->modifiers() & Qt::ControlModifier)
    {
        key += "Ctrl+";
    }
    if (event->modifiers() & Qt::AltModifier)
    {
        key += "Alt+";
    }
    if (event->modifiers() & Qt::MetaModifier)
    {
        key += "Meta+";
    }
    if (event->modifiers() & Qt::KeypadModifier)
    {
        key += "KP";
    }
    switch (event->key())
    {
    case Qt::Key::Key_Shift:
    case Qt::Key::Key_Control:
    case Qt::Key::Key_Alt:
    case Qt::Key::Key_Meta:
        return;
    case Qt::Key::Key_Left:
        key += "LEFT";
        break;
    case Qt::Key::Key_Right:
        key += "RIGHT";
        break;
    case Qt::Key::Key_Up:
        key += "UP";
        break;
    case Qt::Key::Key_Down:
        key += "DOWN";
        break;
    case Qt::Key::Key_Enter:
        key += "ENTER";
        break;
    case Qt::Key::Key_Escape:
        key += "ESC";
        break;
    case Qt::Key::Key_PageUp:
        key += "PGUP";
        break;
    case Qt::Key::Key_PageDown:
        key += "PGDWN";
        break;
    case Qt::Key::Key_Backspace:
        key += "BS";
        break;
    default:
    {
        if (event->modifiers() & Qt::ControlModifier)
        {
            if (event->modifiers() & Qt::ShiftModifier)
                key += QKeySequence(event->key()).toString();
            else
                key += QKeySequence(event->key()).toString().toLower();
        }
        else
            key = event->text();
    }
    }
    m_mpv->command(QVariantList() << "keypress" << key);
}

void MpvAdapter::mouseWheelMoved(const QWheelEvent *event)
{
    QString direction = "WHEEL_";
    if (event->angleDelta().y() > 0)
    {
        direction += "UP";
    }
    else if (event->angleDelta().y() < 0)
    {
        direction += "DOWN";
    }
    else if (event->angleDelta().x() > 0)
    {
        direction += "RIGHT";
    }
    else if (event->angleDelta().x() < 0)
    {
        direction += "LEFT";
    }
    m_mpv->command(QVariantList() << "keypress" << direction);
}

int MpvAdapter::getMaxVolume() const
{
    return m_mpv->getProperty("volume-max").toInt();
}

void MpvAdapter::processSubtitle(const char **subtitle,
                                 const int64_t start,
                                 const int64_t end)
{
    QString formatted = *subtitle;
    formatted = formatted.replace(QChar::fromLatin1('\n'), " / ");
    Q_EMIT subtitleChanged(formatted, start, end);
}

// Code modified from loadTracks()
// https://github.com/u8sand/Baka-MPlayer/blob/master/src/mpvhandler.cpp
void MpvAdapter::processTracks(const mpv_node *node)
{
    QList<const PlayerAdapter::Track *> tracks;
    if (node->format == MPV_FORMAT_NODE_ARRAY)
    {
        for (size_t i = 0; i < node->u.list->num; i++)
        {
            PlayerAdapter::Track *track = new PlayerAdapter::Track;
            if (node->u.list->values[i].format == MPV_FORMAT_NODE_MAP)
            {
                for (size_t n = 0; n < node->u.list->values[i].u.list->num; n++)
                {
                    if (QString(node->u.list->values[i].u.list->keys[n]) == "id")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                            track->id = node->u.list->values[i].u.list->values[n].u.int64;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "type")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                        {
                            QString type = node->u.list->values[i].u.list->values[n].u.string;
                            if (type == "audio")
                                track->type = Track::track_type::audio;
                            else if (type == "video")
                                track->type = Track::track_type::video;
                            else if (type == "sub")
                                track->type = Track::track_type::subtitle;
                        }
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "src-id")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                            track->src_id = node->u.list->values[i].u.list->values[n].u.int64;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "title")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track->title = node->u.list->values[i].u.list->values[n].u.string;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "lang")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track->lang = node->u.list->values[i].u.list->values[n].u.string;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "albumart")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track->albumart = node->u.list->values[i].u.list->values[n].u.flag != 0;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "default")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track->def = node->u.list->values[i].u.list->values[n].u.flag != 0;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "selected")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track->selected = node->u.list->values[i].u.list->values[n].u.flag != 0;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "external")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track->external = node->u.list->values[i].u.list->values[n].u.flag != 0;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "external-filename")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track->external_filename = node->u.list->values[i].u.list->values[n].u.string;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "codec")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track->codec = node->u.list->values[i].u.list->values[n].u.string;
                    }
                }
                tracks.push_back(track);
            }
        }
    }
    Q_EMIT tracksChanged(tracks);
}