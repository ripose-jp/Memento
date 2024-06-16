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

#include <mpv/client.h>
#include <QCoreApplication>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QThreadPool>
#include <stdlib.h>

#include "util/globalmediator.h"
#include "util/utils.h"

/* Begin Constructor/Destructor */

AudioPlayer::AudioPlayer(QObject *parent) : QObject(parent)
{
    m_mpv = mpv_create();
    if (!m_mpv)
    {
        Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
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
        Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
            "Could not start mpv",
            "AudioPlayer: Failed to initialize mpv context"
        );
        QCoreApplication::exit(EXIT_FAILURE);
    }

    /* Initialize other values */
    m_manager = new QNetworkAccessManager;
}

AudioPlayer::~AudioPlayer()
{
    mpv_terminate_destroy(m_mpv);
    delete m_manager;
    clearFiles();
}

void AudioPlayer::clearFiles()
{
    m_fileLock.lock();
    for (QTemporaryFile *file : m_files)
    {
        delete file;
    }
    m_files.clear();
    m_fileLock.unlock();
}

/* End Constructor/Destructor */
/* Begin Implementations */

bool AudioPlayer::playFile(const QTemporaryFile *file)
{
    if (file == nullptr)
        return false;

    QByteArray fileName = QFileInfo(*file).absoluteFilePath().toUtf8();
    const char *args[3] = {
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

AudioPlayerReply *AudioPlayer::playAudio(QString url, QString hash)
{
    /* Check if the file exists */
    m_fileLock.lock();
    QTemporaryFile *file = m_files[url];
    if (file)
    {
        if (FileUtils::calculateMd5(file) != hash)
        {
            playFile(file);
        }
        m_fileLock.unlock();
        return nullptr;
    }
    m_fileLock.unlock();

    /* File does not exist so fetch it */
    AudioPlayerReply *audioReply = new AudioPlayerReply;
    QNetworkRequest req{QUrl(url)};
    req.setAttribute(
        QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::UserVerifiedRedirectPolicy
    );
    QNetworkReply *reply = m_manager->get(std::move(req));
    connect(
        reply, &QNetworkReply::redirected,
        reply, &QNetworkReply::redirectAllowed
    );
    connect(reply, &QNetworkReply::finished,  this,
        [=] {
            QTemporaryFile *file = nullptr;
            bool res = false;

            if (reply->error() != QNetworkReply::NetworkError::NoError)
            {
                qDebug() << reply->errorString();
                goto cleanup;
            }

            /* Put the audio in a new temp file */
            file = new QTemporaryFile;
            if (!file->open())
            {
                qDebug() << "Could not open temp file";
                goto cleanup;
            }
            file->write(reply->readAll());
            file->close();

            /* Add the file to the cache */
            m_fileLock.lock();
            delete m_files[url];
            m_files[url] = file;

            /* Check the hash */
            if (FileUtils::calculateMd5(file) == hash)
            {
                m_fileLock.unlock();
                goto cleanup;
            }

            /* Play the file */
            if (!playFile(file))
            {
                qDebug() << "Could not play audio file";
                m_fileLock.unlock();
                goto cleanup;
            }

            m_fileLock.unlock();

            res = true;
        cleanup:
            Q_EMIT audioReply->result(res);
            reply->deleteLater();
            audioReply->deleteLater();
        }
    );

    return audioReply;
}

/* End Implementations */
