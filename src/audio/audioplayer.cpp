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

#include "audioplayer.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QThreadPool>

#include <mpv/client.h>

#include "state/context.h"
#include "util/utils.h"

/* Begin Constructor/Destructor */

AudioPlayer::AudioPlayer(Context *context, QObject *parent) :
    QObject(parent),
    m_context(std::move(context)),
    m_manager(this)
{
    m_mpv = mpv_create();
    if (!m_mpv)
    {
        emit m_context->showCritical(
            "Could not start mpv",
            "AudioPlayer: Could not create mpv context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }

    mpv_set_option_string(m_mpv, "config",         "no");
    mpv_set_option_string(m_mpv, "terminal",       "no");
    mpv_set_option_string(m_mpv, "force-window",   "no");
    mpv_set_option_string(m_mpv, "input-terminal", "no");
    mpv_set_option_string(m_mpv, "cover-art-auto", "no");
    mpv_set_option_string(m_mpv, "vid",            "no");

    if (mpv_initialize(m_mpv) < 0)
    {
        emit m_context->showCritical(
            "Could not start mpv",
            "AudioPlayer: Failed to initialize mpv context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }
}

AudioPlayer::~AudioPlayer()
{
    mpv_terminate_destroy(m_mpv);
    clearFiles();
}

void AudioPlayer::clearFiles()
{
    QMutexLocker fileLock(&m_fileLock);
    m_files.clear();
}

/* End Constructor/Destructor */
/* Begin Implementations */

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
        NULL
    };

    if (mpv_command(m_mpv, args) < 0)
    {
        return false;
    }

    return true;
}

QCoro::Task<bool> AudioPlayer::playAudio(QString url, QString hash)
{
    {
        /* Check if the file exists */
        QMutexLocker fileLock(&m_fileLock);
        auto fileIt = m_files.find(url);
        if (fileIt != std::end(m_files))
        {
            if (FileUtils::calculateMd5(fileIt->get()) != hash)
            {
                co_return playFile(fileIt->get());
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
        qDebug() << reply->errorString();
        co_return false;
    }

    /* Put the audio in a new temp file */
    std::shared_ptr<QTemporaryFile> file = std::make_shared<QTemporaryFile>();
    if (!file->open())
    {
        qDebug() << "Could not open temp file";
        co_return false;
    }
    file->write(reply->readAll());
    file->close();

    {
        /* Add the file to the cache */
        QMutexLocker fileLock(&m_fileLock);
        m_files[url] = std::move(file);
    }

    co_return co_await playAudio(url, hash);
}

/* End Implementations */
