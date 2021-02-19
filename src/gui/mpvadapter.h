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

#ifndef MPVADAPTER_H
#define MPVADAPTER_H

#include "playeradapter.h"
#include "widgets/mpvwidget.h"

class MpvAdapter : public PlayerAdapter
{
public:
    MpvAdapter(MpvWidget *mpv, QObject *parent = 0);
    virtual ~MpvAdapter() {}

    int getMaxVolume() const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void open(const QString &file, const bool append = false) Q_DECL_OVERRIDE;
    void open(const QList<QUrl> &files) Q_DECL_OVERRIDE;
    void seek(const int time) Q_DECL_OVERRIDE;
    void play() Q_DECL_OVERRIDE;
    void pause() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;
    void seekForward() Q_DECL_OVERRIDE;
    void seekBackward() Q_DECL_OVERRIDE;
    void skipForward() Q_DECL_OVERRIDE;
    void skipBackward() Q_DECL_OVERRIDE;
    void disableAudio() Q_DECL_OVERRIDE;
    void disableVideo() Q_DECL_OVERRIDE;
    void disableSubtitles() Q_DECL_OVERRIDE;
    void setAudioTrack(const int id) Q_DECL_OVERRIDE;
    void setVideoTrack(const int id) Q_DECL_OVERRIDE;
    void setSubtitleTrack(const int id) Q_DECL_OVERRIDE;
    void setFullscreen(const bool value) Q_DECL_OVERRIDE;
    void setVolume(const int value) Q_DECL_OVERRIDE;
    void keyPressed(const QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseWheelMoved(const QWheelEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void processTracks(const mpv_node *node);
    void processSubtitle(const char **subtitle,
                         const int64_t start,
                         const int64_t end);

private:
    MpvWidget *m_mpv;
};

#endif // MPVADAPTER