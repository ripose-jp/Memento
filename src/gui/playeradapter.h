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

#ifndef PLAYERADAPTER_H
#define PLAYERADAPTER_H

#include <QObject>
#include <QKeyEvent>
#include <QWheelEvent>

struct Track
    {
    enum track_type
        {
        audio,
        video,
        subtitle
    };
    int64_t id;
    track_type type;
    int64_t src_id;
    QString title;
    QString lang;
    bool albumart;
    bool def;
    bool selected;
    bool external;
    QString external_filename;
    QString codec;
} typedef Track;

class PlayerAdapter : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    virtual ~PlayerAdapter() {}

    virtual int64_t getMaxVolume() const = 0;

    virtual double getSubStart() const = 0;
    virtual double getSubEnd()   const = 0;
    virtual double getSubDelay() const = 0;

    virtual QString getSecondarySubtitle() const = 0;

    virtual bool getSubVisibility() const = 0;

    virtual double getAudioDelay() const = 0;

    virtual QList<const Track *> getTracks()      = 0;
    virtual int64_t getAudioTrack()         const = 0;
    virtual int64_t getSubtitleTrack()      const = 0;

    virtual QString getPath()  const = 0;
    virtual QString getTitle() const = 0;

    virtual bool isFullScreen() const = 0;

    virtual bool canGetSecondarySubText() const = 0;

public Q_SLOTS:
    virtual void open(const QString     &file,
                      const bool         append = false)  = 0;
    virtual void open(const QList<QUrl> &files)          = 0;

    virtual void addSubtitle(const QString &file) = 0;

    virtual void seek(const int64_t time) = 0;

    virtual void play()  = 0;
    virtual void pause() = 0;
    virtual void stop()  = 0;

    virtual void seekForward()  = 0;
    virtual void seekBackward() = 0;

    virtual void skipForward()  = 0;
    virtual void skipBackward() = 0;

    virtual void disableAudio()       = 0;
    virtual void disableVideo()       = 0;
    virtual void disableSubtitles()   = 0;
    virtual void disableSubtitleTwo() = 0;

    virtual void setSubVisiblity(const bool visible) = 0;

    virtual void setAudioTrack(int64_t id)       = 0;
    virtual void setVideoTrack(int64_t id)       = 0;
    virtual void setSubtitleTrack(int64_t id)    = 0;
    virtual void setSubtitleTwoTrack(int64_t id) = 0;

    virtual void setFullscreen(const bool value) = 0;
    virtual void setVolume(const int64_t value)  = 0;

    virtual QString tempScreenshot(const bool subtitles,
                                   const QString &ext = ".jpg") = 0;

    virtual void keyPressed(const QKeyEvent *event)        = 0;
    virtual void mouseWheelMoved(const QWheelEvent *event) = 0;
};

#endif // PLAYERADAPTER_H