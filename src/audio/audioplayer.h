////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QObject>

#include <QHash>
#include <QMutex>
#include <QTemporaryFile>

struct mpv_handle;
class QNetworkAccessManager;

/**
 * Returns an asynchronous replies from the AudioPlayer.
 */
class AudioPlayerReply : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

Q_SIGNALS:
    /**
     * Emitted when a result is ready.
     * @param success true on success, false otherwise.
     */
    void result(const bool success);
};

/**
 * Plays audio files from over the network.
 */
class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    /**
     * Clears all files in the cache.
     */
    void clearFiles();

    /**
     * Plays the audio at the URL.
     * @param url  The url of the audio.
     * @param hash If the audio file matches this MD5, it is not played.
     * @return AnkiPlayerReply that emits the result() signal with true on
     *         success or false if the audio file could not be played/matches
     *         the hash. The caller does not own this object. Returns nullptr if
     *         the file is already cached.
     */
    AudioPlayerReply *playAudio(QString url, QString hash = QString());

private:
    /**
     * Plays a file in mpv.
     * @param file The file for mpv to play.
     * @return true if it was played, false on error.
     */
    bool playFile(const QTemporaryFile *file);

    /* The mpv context. Used for playing audio. */
    mpv_handle *m_mpv;

    /* The network access manager used for fetching audio files. */
    QNetworkAccessManager *m_manager;

    /* Maps urls to cached files. */
    QHash<QString, QTemporaryFile *> m_files;

    /* Mutex that prevents the file cache from being modified concurrently. */
    QMutex m_fileLock;
};

#endif // AUDIOPLAYER_H
