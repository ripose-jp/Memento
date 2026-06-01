////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
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

#include "quick/fileopenhandler.h"

#include <QFileOpenEvent>
#include <QUrl>

FileOpenHandler::FileOpenHandler(QObject *parent) : QObject(parent)
{
    m_debounceTimer.setInterval(50);
    m_debounceTimer.setSingleShot(true);
    connect(
        &m_debounceTimer, &QTimer::timeout,
        this, &FileOpenHandler::sendFiles,
        Qt::QueuedConnection
    );
}

QStringList FileOpenHandler::takePendingFiles()
{
    m_ready = true;
    QStringList files = std::move(m_pendingFiles);
    return files;
}

bool FileOpenHandler::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() != QEvent::FileOpen)
    {
        return QObject::eventFilter(watched, event);
    }

    QFileOpenEvent *fileOpenEvent = static_cast<QFileOpenEvent *>(event);
    QString file = fileOpenEvent->file();
    if (file.isEmpty())
    {
        QUrl url = fileOpenEvent->url();
        file = url.isLocalFile() ? url.toLocalFile() : url.toString();
    }
    openFile(file);
    event->accept();
    return true;
}

void FileOpenHandler::openFile(const QString &file)
{
    if (file.isEmpty())
    {
        return;
    }
    m_pendingFiles.emplaceBack(file);
    m_debounceTimer.start();
}

void FileOpenHandler::sendFiles()
{
    if (!m_ready || m_pendingFiles.isEmpty())
    {
        return;
    }
    emit filesOpened(m_pendingFiles);
    m_pendingFiles.clear();
}
