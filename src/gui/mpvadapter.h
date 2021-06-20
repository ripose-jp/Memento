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
    virtual ~MpvAdapter() { disconnect(); }

    int64_t getMaxVolume() const override;

    double getSubStart() const override;
    double getSubEnd()   const override;
    double getSubDelay() const override;

    QString getSecondarySubtitle() const override;

    bool getSubVisibility() const override;
    
    double getAudioDelay() const override;

    QList<const Track *> getTracks() override;
    int64_t getAudioTrack()    const override;
    int64_t getSubtitleTrack() const override;

    QString getPath()  const override;
    QString getTitle() const override;

    bool isFullScreen() const override;

    bool canGetSecondarySubText() const override;

public Q_SLOTS:
    void open(const QString     &file, 
              const bool         append = false) override;
    void open(const QList<QUrl> &files)          override;

    void addSubtitle(const QString &file) override;

    void seek(const int64_t time) override;

    void play()  override;
    void pause() override;
    void stop()  override;

    void seekForward()  override;
    void seekBackward() override;

    void skipForward()  override;
    void skipBackward() override;

    void disableAudio()       override;
    void disableVideo()       override;
    void disableSubtitles()   override;
    void disableSubtitleTwo() override;

    void setSubVisiblity(const bool visible) override;

    void setAudioTrack(int64_t id)       override;
    void setVideoTrack(int64_t id)       override;
    void setSubtitleTrack(int64_t id)    override;
    void setSubtitleTwoTrack(int64_t id) override;

    void setFullscreen(const bool value) override;
    void setVolume(const int64_t value)  override;

    QString tempScreenshot(const bool subtitles,
                           const QString &ext = ".jpg") override;

    void keyPressed(const QKeyEvent *event)        override;
    void mouseWheelMoved(const QWheelEvent *event) override;

private Q_SLOTS:
    QList<const Track *> processTracks(const mpv_node *node);

private:
    MpvWidget *m_mpv;
    mpv_handle *m_handle;
};

#endif // MPVADAPTER