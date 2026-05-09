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

#pragma once

#include <QObject>

#include <QHash>
#include <QMutex>
#include <QNetworkAccessManager>
#include <QTemporaryFile>

#ifdef MEMENTO_SYSTEM_QCORO
#include <QCoroNetworkReply>
#include <QCoroQmlTask>
#include <QCoroTask>
#else
#include <qcoro/network/qcoronetworkreply.h>
#include <qcoro/qcorotask.h>
#include <qcoro/qml/qcoroqmltask.h>
#endif // MEMENTO_SYSTEM_QCORO

struct mpv_handle;

/**
 * Plays audio files from over the network.
 */
class AudioPlayer : public QObject
{
    Q_OBJECT

public:
    AudioPlayer(QObject *parent = nullptr);
    virtual ~AudioPlayer();

    /**
     * Clears all files in the cache.
     */
    Q_INVOKABLE void clearFiles();

    /**
     * Plays the audio at the URL.
     * @param url The url of the audio.
     * @param hash If the audio file matches this MD5, it is not played.
     * @return Returns true on a file successfully player, false otherwise.
     */
    [[nodiscard]]
    Q_INVOKABLE QCoro::QmlTask play(QString url, QString hash = QString());

    /**
     * Plays the audio at the URL.
     * @param url The url of the audio.
     * @param hash If the audio file matches this MD5, it is not played.
     * @return Returns true on a file successfully player, false otherwise.
     */
    [[nodiscard]]
    QCoro::Task<bool> playAsync(QString url, QString hash = QString());

private:
    /**
     * Plays a file in mpv.
     * @param file The file for mpv to play.
     * @return true if it was played, false on error.
     */
    [[nodiscard]]
    bool playFile(const QTemporaryFile *file);

    /* The network access manager used for fetching audio files */
    QNetworkAccessManager m_manager{this};

    /* The mpv context. Used for playing audio */
    mpv_handle *m_mpv{nullptr};

    /* Mutex that prevents the file cache from being modified concurrently */
    QMutex m_fileMutex;

    /* Maps urls to cached files */
    QHash<QString, QTemporaryFile *> m_files;
};
