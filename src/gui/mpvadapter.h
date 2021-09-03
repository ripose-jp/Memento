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

#include <QSet>

#include "widgets/mpvwidget.h"

/**
 * Implements the features described in PlayerAdapter for mpv and MpvWidget.
 */
class MpvAdapter : public PlayerAdapter
{
public:
    MpvAdapter(MpvWidget *mpv, QObject *parent = 0);
    virtual ~MpvAdapter() { disconnect(); }

    void loadCommandLineArgs() override;

    int64_t getMaxVolume() const override;

    double getSubStart() const override;

    double getSubEnd() const override;

    double getSubDelay() const override;

    QString getSecondarySubtitle() const override;

    bool getSubVisibility() const override;

    double getAudioDelay() const override;

    QList<const Track *> getTracks() override;

    int64_t getAudioTrack() const override;

    QString getPath() const override;

    QString getTitle() const override;

    bool isFullscreen() const override;

    bool isPaused() const override;

    bool canGetSecondarySubText() const override;

public Q_SLOTS:
    void open(const QString     &file,
              const bool         append = false,
              const QStringList &options = QStringList()) override;
    void open(const QList<QUrl> &files) override;

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
    QString tempAudioClip(double start,
                          double end,
                          const QString &ext = ".aac") override;

    void keyPressed(const QKeyEvent *event) override;
    void mouseWheelMoved(const QWheelEvent *event) override;

private Q_SLOTS:
    /**
     * Processes the mpv_node into a list of tracks.
     * @param node The mpv_node containing the raw track-list.
     * @return A list containing the tracks.
     */
    QList<const Track *> processTracks(const mpv_node *node);

private:
    /* Node in the command line argument multi-tree. */
    struct LoadFileNode
    {
        /* List of files specific to this level. */
        QStringList files;

        /* All the options specific to this level. */
        QStringList options;

        /* Children of this node. */
        QList<struct LoadFileNode> children;
    };

    /**
     * Error checks that command line args are well-formatted.
     * @return true if they are valid, false otherwise.
     */
    bool commandLineArgsValid(const QStringList &args) const;

    /**
     * Parses command line arguments recursively to build a multi-tree.
     * @param      args   An array of command line arguments.
     * @param      index  The index to start processing arguments at.
     * @param[out] parent The parent node to build a tree below.
     * @param      depth  The maximum number of times to recurse.
     * @return The index of the argument array buildArgsTree stopped at. -1 on
     *         error.
     */
    int buildArgsTree(const QStringList &args,
                      int index,
                      struct LoadFileNode &parent,
                      int depth = 10) const;

    /**
     * Loads files from a tree of LoadFileNodes.
     * @param parent  The parent to load files from.
     * @param options Options inherited by files at this level.
     */
    void loadFilesFromTree(const struct LoadFileNode &parent,
                                        QStringList  &options);

    /* The MpvWidget */
    MpvWidget *m_mpv;

    /* The mpv context. Used for interacting with the mpv api. */
    mpv_handle *m_handle;

    /* A set containing all subtitle filetype extensions. */
    QSet<QString> m_subExts;
};

#endif // MPVADAPTER