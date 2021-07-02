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

#include <QKeyEvent>
#include <QObject>
#include <QWheelEvent>

/**
 * Struct containing track information.
 */
struct Track
{
    /**
     * The type of track.
     */
    enum Type
    {
        audio,
        video,
        subtitle
    };

    /* Type of track. */
    Type type;

    /* Id of the track start at 1. Unique within its type. */
    int64_t id;

    /* Track ID as used in the source file. Not always available. */
    int64_t srcId;

    /* Track title as it is stored in the file. Not always available. */
    QString title;

    /* Track language as identified by the file. Not always available. */
    QString lang;

    /* true if the track has album art. */
    bool albumart;

    /* true if the track is a default track. */
    bool def;

    /* true if the track is currently selected. */
    bool selected;

    /* true if the track is from an external file. */
    bool external;

    /* The filename if the track is from an external file, unavailable 
     * otherwise. 
     */
    QString externalFilename;

    /* The codec name used by this track, for example h264. Unavailable in some
     * rare cases.
     */
    QString codec;
} typedef Track;

/**
 * Adapter for interacting with a media player backend.
 */
class PlayerAdapter : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    virtual ~PlayerAdapter() {}

    /**
     * Gets the maximum volume value.
     * @return The maximum volume value.
     */
    virtual int64_t getMaxVolume() const = 0;

    /**
     * Get the start time of the current subtitle without delay.
     * @return The start time of the current subtitle in seconds.
     */
    virtual double getSubStart() const = 0;

    /**
     * Get the end time of the current subtitle without delay.
     * @return The end time of the current subtitle in seconds.
     */
    virtual double getSubEnd() const = 0;

    /**
     * Get the subtitle delay.
     * @return The subtitle delay in seconds.
     */
    virtual double getSubDelay() const = 0;

    /**
     * Get the secondary subtitle text if supported.
     * @return The secondary subtitle text. Empty string if no subtitle or 
     *         unsupported operation.
     */
    virtual QString getSecondarySubtitle() const = 0;

    /**
     * Get the subtitle visibility.
     * @return true if subtitles are visible, false otherwise.
     */
    virtual bool getSubVisibility() const = 0;

    /**
     * Get the audio delay.
     * @return The audio delay in seconds.
     */
    virtual double getAudioDelay() const = 0;

    /**
     * Returns a list of the current tracks.
     * @return A list of the current tracks. Tracks belong to the callee.
     */
    virtual QList<const Track *> getTracks() = 0;

    /**
     * Get the id of the current audio track starting at 1.
     * @return The id of the current audio track.
     */
    virtual int64_t getAudioTrack() const = 0;

    /**
     * Get the id of the current subtitle track starting at 1.
     * @return The id of the current subtitle track.
     */
    virtual int64_t getSubtitleTrack() const = 0;

    /**
     * Get the path of the current playing file.
     * @return The path of the currently playing file.
     */
    virtual QString getPath() const = 0;

    /**
     * Get the title of the currently playing file.
     * @return Title of the currently playing file, filename if it doesn't 
     *         exist.
     */
    virtual QString getTitle() const = 0;

    /**
     * Returns if the player is fullscreened.
     * @return true if the player is fullscreened, false otherwise.
     */
    virtual bool isFullscreen() const = 0;

    /**
     * Returns if the secondary subtitle text can be accessed.
     * @return true if secondary subtitles can be accessed, false otherwise.
     */
    virtual bool canGetSecondarySubText() const = 0;

public Q_SLOTS:
    /**
     * Opens the file.
     * @param file   The path of the file to open.
     * @param append If true, append to the current playlist, otherwise 
     *               overwrite the current playlist.
     */
    virtual void open(const QString &file, const bool append = false) = 0;

    /**
     * Opens the first file and adds subsequent files to a playlist.
     * @param files The list of files to open.
     */
    virtual void open(const QList<QUrl> &files) = 0;

    /**
     * Adds the subtitle file to add to the playback.
     * @param file The subtitle file to add.
     */
    virtual void addSubtitle(const QString &file) = 0;

    /**
     * Seeks to the time in seconds.
     * @param time The time in seconds to seek to.
     */
    virtual void seek(const int64_t time) = 0;

    /**
     * Resumes playback.
     */
    virtual void play() = 0;

    /**
     * Pauses playback.
     */
    virtual void pause() = 0;

    /**
     * Stops playback and clears the playlist.
     */
    virtual void stop() = 0;

    /**
     * Seeks to the next subtitle.
     */
    virtual void seekForward() = 0;

    /**
     * Seeks to the last subtitle.
     */
    virtual void seekBackward() = 0;

    /**
     * Skip to the next video in the playlist.
     */
    virtual void skipForward()  = 0;

    /**
     * Skip to the last video in the playlist.
     */
    virtual void skipBackward() = 0;

    /**
     * Disables player audio.
     */
    virtual void disableAudio() = 0;

    /**
     * Disables player video.
     */
    virtual void disableVideo() = 0;

    /**
     * Disables player subtitles.
     */
    virtual void disableSubtitles() = 0;

    /**
     * Disables secondary player subtitles.
     */
    virtual void disableSubtitleTwo() = 0;

    /**
     * Sets the subtitle visbility.
     * @param visible true to make subtitle visible, false otherwise.
     */
    virtual void setSubVisiblity(const bool visible) = 0;

    /**
     * Changes the audio track to the one with the same id.
     * @param id The id of the audio track.
     */
    virtual void setAudioTrack(int64_t id) = 0;

    /**
     * Changes the video track to the one with the same id.
     * @param id The id of the video track.
     */
    virtual void setVideoTrack(int64_t id) = 0;

    /**
     * Changes the subtitle track to the one with the same id.
     * @param id The id of the subtitle track.
     */
    virtual void setSubtitleTrack(int64_t id) = 0;

    /**
     * Changes the secondary subtitle track to the one with the same id.
     * @param id The id of the secondary subtitle track.
     */
    virtual void setSubtitleTwoTrack(int64_t id) = 0;

    /**
     * Sets the player's fullscreen value.
     * @param value true for fullscreen, false otherwise.
     */
    virtual void setFullscreen(const bool value) = 0;

    /**
     * Sets the current volume of the player.
     * @param value The volume to set.
     */
    virtual void setVolume(const int64_t value) = 0;

    /**
     * Takes a screenshot of the player contents and stores the files in the
     * temp directory.
     * @param subtitles true to include the subtitles in the images, false 
     *                  otherwise.
     * @param ext       The image format to use. '.jpg' by default.
     * @return Path to the file.
     */
    virtual QString tempScreenshot(const bool subtitles,
                                   const QString &ext = ".jpg") = 0;

    /**
     * Passes a keypress event to the player.
     * @param event The key press event.
     */
    virtual void keyPressed(const QKeyEvent *event) = 0;

    /**
     * Passes a mouse wheel event to the player.
     * @param event The mouse wheel event.
     */
    virtual void mouseWheelMoved(const QWheelEvent *event) = 0;
};

#endif // PLAYERADAPTER_H