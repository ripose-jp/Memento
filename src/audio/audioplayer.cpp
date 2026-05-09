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

#include "audio/audioplayer.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <mpv/client.h>

#include "util/utils.h"

/* Begin Constructor/Destructor */

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent)
{
    m_mpv = ::mpv_create();
    if (!m_mpv)
    {
        qCritical("AudioPlayer: Could not create mpv context");
        QCoreApplication::exit(-1);
    }

    ::mpv_set_option_string(m_mpv, "config",         "no");
    ::mpv_set_option_string(m_mpv, "terminal",       "no");
    ::mpv_set_option_string(m_mpv, "force-window",   "no");
    ::mpv_set_option_string(m_mpv, "input-terminal", "no");
    ::mpv_set_option_string(m_mpv, "cover-art-auto", "no");
    ::mpv_set_option_string(m_mpv, "vid",            "no");

    if (::mpv_initialize(m_mpv) < 0)
    {
        qCritical("AudioPlayer: Failed to initialize mpv context");
        QCoreApplication::exit(-1);
    }
}

AudioPlayer::~AudioPlayer()
{
    ::mpv_terminate_destroy(m_mpv);
    clearFiles();
}

void AudioPlayer::clearFiles()
{
    QMutexLocker lock{&m_fileMutex};
    qDeleteAll(m_files);
    m_files.clear();
}

/* End Constructor/Destructor */
/* Begin Implementations */

QCoro::QmlTask AudioPlayer::play(QString url, QString hash)
{
    return playAsync(std::move(url), std::move(hash));
}

QCoro::Task<bool> AudioPlayer::playAsync(QString url, QString hash)
{
    constexpr qsizetype MAX_CACHE_SIZE{10};

    if (m_mpv == nullptr)
    {
        co_return false;
    }

    {
        /* Check if the file exists */
        QMutexLocker lock{&m_fileMutex};
        auto fileIt = m_files.find(url);
        if (fileIt != std::end(m_files))
        {
            if (FileUtils::calculateMd5(*fileIt) != hash)
            {
                co_return playFile(*fileIt);
            }
            co_return false;
        }
    }

    /* File does not exist so fetch it */
    QNetworkRequest req{QUrl(url)};
    req.setAttribute(
        QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::UserVerifiedRedirectPolicy
    );
    std::unique_ptr<QNetworkReply> reply{m_manager.get(std::move(req))};
    connect(
        reply.get(), &QNetworkReply::redirected,
        reply.get(), &QNetworkReply::redirectAllowed
    );
    co_await reply.get();

    if (reply->error() != QNetworkReply::NetworkError::NoError)
    {
        qDebug("AudioPlayer: %s", qUtf8Printable(reply->errorString()));
        co_return false;
    }

    /* Put the audio in a new temp file */
    QTemporaryFile *file = new QTemporaryFile(this);
    if (!file->open())
    {
        qDebug("AudioPlayer: Could not open temp file");
        co_return false;
    }
    file->write(reply->readAll());
    file->close();

    {
        /* Add the file to the cache */
        QMutexLocker lock{&m_fileMutex};
        if (m_files.size() >= MAX_CACHE_SIZE)
        {
            qDeleteAll(m_files);
            m_files.clear();
        }
        m_files[url] = std::move(file);
    }

    co_return co_await playAsync(url, hash);
}

bool AudioPlayer::playFile(const QTemporaryFile *file)
{
    if (file == nullptr)
    {
        return false;
    }

    QByteArray fileName = QFileInfo(*file).absoluteFilePath().toUtf8();
    const char *args[] = {
        "loadfile",
        fileName,
        nullptr
    };

    if (::mpv_command(m_mpv, args) < 0)
    {
        return false;
    }

    return true;
}

/* End Implementations */
