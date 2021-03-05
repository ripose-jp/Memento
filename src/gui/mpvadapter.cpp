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
#include <QTemporaryFile>

MpvAdapter::MpvAdapter(MpvWidget *mpv, QObject *parent)
    : m_mpv(mpv), m_handle(mpv->get_handle()), PlayerAdapter(parent)
{
    connect(m_mpv, &MpvWidget::tracklistChanged, [=] (const mpv_node *node) {
        Q_EMIT tracksChanged(processTracks(node));
    });
    connect(m_mpv, &MpvWidget::audioTrackChanged, 
        this, &MpvAdapter::audioTrackChanged);
    connect(m_mpv, &MpvWidget::videoTrackChanged, 
        this, &MpvAdapter::videoTrackChanged);
    connect(m_mpv, &MpvWidget::subtitleTrackChanged,
        this, &MpvAdapter::subtitleTrackChanged);
    connect(m_mpv, &MpvWidget::subtitleTwoTrackChanged,
        this, &MpvAdapter::subtitleTwoTrackChanged);

    connect(m_mpv, &MpvWidget::audioDisabled,
        this, &MpvAdapter::audioDisabled);
    connect(m_mpv, &MpvWidget::videoDisabled,
        this, &MpvAdapter::videoDisabled);
    connect(m_mpv, &MpvWidget::subtitleDisabled,
        this, &MpvAdapter::subtitleDisabled);
    connect(m_mpv, &MpvWidget::subtitleTwoDisabled,
        this, &MpvAdapter::subtitleTwoDisabled);

    connect(m_mpv, &MpvWidget::subtitleChanged,
        this, &MpvAdapter::subtitleChanged);
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
        this, &MpvAdapter::titleChanged);
    connect(m_mpv, &MpvWidget::fileChanged,
        this, &MpvAdapter::fileChanged);

    connect(m_mpv, &MpvWidget::hideCursor, this, &MpvAdapter::hideCursor);
    connect(m_mpv, &MpvWidget::shutdown, this, &MpvAdapter::close);
}

int64_t MpvAdapter::getMaxVolume() const
{
    int64_t max;
    if (mpv_get_property(m_handle, "volume-max", MPV_FORMAT_INT64, &max) < 0)
    {
        qDebug() << "Could not get volume-max property";
        return 0;
    }
    return max;
}

double MpvAdapter::getSubStart() const
{
    double start;
    if (mpv_get_property(m_handle, "sub-start", MPV_FORMAT_DOUBLE, &start) < 0)
    {
        qDebug() << "Could not get sub-start property";
        return 0;
    }
    return start;
}

double MpvAdapter::getSubEnd() const
{
    double end;
    if (mpv_get_property(m_handle, "sub-end", MPV_FORMAT_DOUBLE, &end) < 0)
    {
        qDebug() << "Could not get sub-end property";
        return 0;
    }
    return end;
}

double MpvAdapter::getSubDelay() const
{
    double delay;
    if (mpv_get_property(m_handle, "sub-delay", MPV_FORMAT_DOUBLE, &delay) < 0)
    {
        qDebug() << "Could not get sub-delay property";
        return 0;
    }
    return delay;
}
    
double MpvAdapter::getAudioDelay() const
{
    double delay;
    if (mpv_get_property(m_handle, "audio-delay", MPV_FORMAT_DOUBLE, &delay) < 0)
    {
        qDebug() << "Could not get mpv delay property";
        return 0;
    }
    return delay;
}

QList<const PlayerAdapter::Track *> MpvAdapter::getTracks()
{
    mpv_node node;
    if (mpv_get_property(m_handle, "track-list", MPV_FORMAT_NODE, &node) < 0)
    {
        qDebug() << "Could not get track-list property";
        return QList<const PlayerAdapter::Track *>();
    }
    return processTracks(&node);
}

int64_t MpvAdapter::getAudioTrack() const
{
    int64_t track;
    if (mpv_get_property(m_handle, "aid", MPV_FORMAT_INT64, &track) < 0)
    {
        qDebug() << "Could not get mpv aid property";
        return -1;
    }
    return track;
}

int64_t MpvAdapter::getSubtitleTrack() const
{
    int64_t track;
    if (mpv_get_property(m_handle, "sid", MPV_FORMAT_INT64, &track) < 0)
    {
        qDebug() << "Could not get mpv sid property";
        return -1;
    }
    return track;
}

QString MpvAdapter::getPath() const
{
    char *path = NULL;
    if (mpv_get_property(m_handle, "path", MPV_FORMAT_STRING, &path) < 0)
    {
        qDebug() << "Could not get mpv path property";
        return "";
    }
    QString path_str(path);
    mpv_free(path);
    return path_str;
}

void MpvAdapter::open(const QString &file, const bool append)
{
    if (file.isEmpty())
        return;
    
    const char *args[4] = {
        "loadfile",
        file.toLatin1().data(),
        append ? "append" : NULL,
        NULL
    };
    
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not open file" << file;
    }
}

void MpvAdapter::open(const QList<QUrl> &files)
{
    if (files.isEmpty())
        return;

    open(files.first().toLocalFile()); // mpv won't start with loadfile append
    for (auto it = files.begin() + 1; it != files.end(); ++it)
    {
        if (!it->toLocalFile().isEmpty())
            open(it->toLocalFile(), true);
    }
}

void MpvAdapter::addSubtitle(const QString &file)
{
    const char *args[3] = {
        "sub-add",
        file.toLatin1().data(),
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not add subtitle file" << file;
    }
}

void MpvAdapter::seek(const int64_t time)
{
    QString timestr = QString::number(time);
    const char *args[4] = {
        "seek",
        timestr.toLatin1().data(),
        "absolute",
        NULL
    };
    if (mpv_command_async(m_handle, -1, args) < 0)
    {
        qDebug() << "Seeking failed";
    }
}

void MpvAdapter::play()
{
    int flag = 0;
    if (mpv_set_property(m_handle, "pause", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qDebug() << "Could not set mpv property pause to false";
    }
}

void MpvAdapter::pause()
{
    int flag = 1;
    if (mpv_set_property(m_handle, "pause", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qDebug() << "Could not set mpv property pause to true";
    }
}

void MpvAdapter::stop()
{
    const char *args[2] = {
        "stop",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not stop mpv";
    }
}

void MpvAdapter::seekForward()
{
    const char *args[3] = {
        "sub-seek",
        "1",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not seek the next subtitle";
    }
}

void MpvAdapter::seekBackward()
{
    const char *args[3] = {
        "sub-seek",
        "-1",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not seek the last subtitle";
    }
}

void MpvAdapter::skipForward()
{
    const char *args[2] = {
        "playlist-next",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not skip to the next file in the playlist";
    }
}

void MpvAdapter::skipBackward()
{
    const char *args[2] = {
        "playlist-prev",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not skip to the next file in the playlist";
    }
}

void MpvAdapter::disableAudio()
{
    const char *value = "no";
    if (mpv_set_property(m_handle, "aid", MPV_FORMAT_STRING, &value) < 0)
    {
        qDebug() << "Could not set mpv property aid to no";
    }
}

void MpvAdapter::disableVideo()
{
    const char *value = "no";
    if (mpv_set_property(m_handle, "vid", MPV_FORMAT_STRING, &value) < 0)
    {
        qDebug() << "Could not set mpv property vid to no";
    }
}

void MpvAdapter::disableSubtitles()
{
    const char *value = "no";
    if (mpv_set_property(m_handle, "sid", MPV_FORMAT_STRING, &value) < 0)
    {
        qDebug() << "Could not set mpv property sid to no";
    }
}

void MpvAdapter::disableSubtitleTwo()
{
    const char *value = "no";
    if (mpv_set_property(m_handle, "secondary-sid", MPV_FORMAT_STRING, &value) < 0)
    {
        qDebug() << "Could not set mpv property secondary-sid to no";
    }
}

void MpvAdapter::setAudioTrack(int64_t id)
{
    if (mpv_set_property(m_handle, "aid", MPV_FORMAT_INT64, &id) < 0)
    {
        qDebug() << "Could not set mpv property aid";
    }
}

void MpvAdapter::setVideoTrack(int64_t id)
{
    if (mpv_set_property(m_handle, "vid", MPV_FORMAT_INT64, &id) < 0)
    {
        qDebug() << "Could not set mpv property vid";
    }
}

void MpvAdapter::setSubtitleTrack(int64_t id)
{
    if (mpv_set_property(m_handle, "sid", MPV_FORMAT_INT64, &id) < 0)
    {
        qDebug() << "Could not set mpv property sid";
    }
}

void MpvAdapter::setSubtitleTwoTrack(int64_t id)
{
    if (mpv_set_property(m_handle, "secondary-sid", MPV_FORMAT_INT64, &id) < 0)
    {
        qDebug() << "Could not set mpv property secondary-sid";
    }
}

void MpvAdapter::setFullscreen(bool value)
{
    if (mpv_set_property(m_handle, "fullscreen", MPV_FORMAT_FLAG, &value) < 0)
    {
        qDebug() << "Could not set mpv property fullscreen";
    }
}

void MpvAdapter::setVolume(int64_t value)
{
    if (mpv_set_property(m_handle, "volume", MPV_FORMAT_INT64, &value) < 0)
    {
        qDebug() << "Could not set mpv property volume";
    }
}

QString MpvAdapter::tempScreenshot(const bool subtitles, const QString &ext)
{
    // Get a temporary file name
    QTemporaryFile file;
    if (!file.open())
        return "";
    QString filename = file.fileName() + ext;
    file.close();

    const char *args[4] = {
        "screenshot-to-file",
        filename.toLatin1().data(),
        subtitles ? NULL : "video",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not take temporary screenshot";
        return "";
    }

    return filename;
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

    const char *args[3] = {
        "keypress",
        key.toLatin1().data(),
        NULL
    };
    if (mpv_command_async(m_handle, -1, args) < 0)
    {
        qDebug() << "Could not send keypress command for key" << key;
    }
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

    const char *args[3] = {
        "keypress",
        direction.toLatin1().data(),
        NULL
    };
    if (mpv_command_async(m_handle, -1, args) < 0)
    {
        qDebug() << "Could not send keypress command for direction"
                 << direction;
    }
}

// Code modified from loadTracks()
// https://github.com/u8sand/Baka-MPlayer/blob/master/src/mpvhandler.cpp
QList<const PlayerAdapter::Track *> MpvAdapter::processTracks(const mpv_node *node)
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
    return tracks;
}