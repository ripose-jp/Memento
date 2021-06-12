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

#include <QTemporaryFile>
#include <QMutex>
#include <QHash>

struct mpv_handle;
class QNetworkAccessManager;

class AudioPlayerReply : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

Q_SIGNALS:
    void result(const bool success);
};

class AudioPlayer : public QObject
{
    Q_OBJECT
public:
    AudioPlayer(QObject *parent = nullptr);
    ~AudioPlayer();

    void clearFiles();

    AudioPlayerReply *playAudio(QString url, QString hash = QString());

private:
    mpv_handle                       *m_mpv;
    QNetworkAccessManager            *m_manager;
    QHash<QString, QTemporaryFile *>  m_files;
    QMutex                            m_fileLock;

    bool playFile(const QTemporaryFile *file);
};

#endif // AUDIOPLAYER_H