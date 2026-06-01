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

#pragma once

#include <QObject>

#include <QEvent>
#include <QStringList>
#include <QTimer>

/**
 * @brief Receives platform file-open events and exposes them to QML.
 */
class FileOpenHandler : public QObject
{
    Q_OBJECT

public:
    explicit FileOpenHandler(QObject *parent = nullptr);
    virtual ~FileOpenHandler() = default;

    /**
     * @brief Gets queued files and marks the player as ready for future events.
     *
     * @return Files received before QML was ready to handle them.
     */
    [[nodiscard]]
    Q_INVOKABLE QStringList takePendingFiles();

signals:
    /**
     * @brief Emitted when the platform asks Memento to open files.
     *
     * @param files The files or URLs to open.
     */
    void filesOpened(const QStringList &files);

protected:
    /**
     * @brief Handles platform file open events.
     *
     * @param watched The watched object.
     * @param event The event to handle.
     * @return true if event was handled, false otherwise.
     */
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    /**
     * @brief Handles the debounce timeout and emits files.
     */
    void sendFiles();

private:
    /**
     * @brief Opens or queues a file.
     *
     * @param file The file to open.
     */
    void openFile(const QString &file);

    /* Files received before QML is ready. */
    QStringList m_pendingFiles;

    /* Timer that correlates related QFileOpenEvents */
    QTimer m_debounceTimer{this};

    /* True if QML is ready to receive file open events. */
    bool m_ready = false;
};
