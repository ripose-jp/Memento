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

#include <QAbstractListModel>
#include <QStringList>

/**
 * @brief A history entry storing the state of a played video.
 */
struct HistoryEntry
{
    /* Title of the video */
    QString title;
    /* Path to the video file */
    QString videoPath;
    /* ID of the primary subtitle track */
    int primarySubtitleTrackId = 0;
    /* ID of the secondary subtitle track */
    int secondarySubtitleTrackId = 0;
    /* Paths to external subtitle files */
    QStringList externalSubtitles;
    /* Playback position in seconds */
    double playbackPosition = 0;
    /* Path to the thumbnail image */
    QString thumbnailPath;
    /* Unix timestamp of when this entry was created */
    qint64 timestamp;
};

/**
 * @brief Manages a list of history entries for played videos.
 *
 * Provides a Qt abstract list model for QML integration, with support for
 * adding, removing, and persisting history entries to disk.
 */
class HistoryManager : public QAbstractListModel
{
    Q_OBJECT

public:
    /**
     * @brief The roles for accessing entry data from QML.
     */
    enum Roles
    {
        TitleRole = Qt::UserRole + 1,
        VideoPathRole,
        PrimarySubtitleTrackIdRole,
        SecondarySubtitleTrackIdRole,
        ExternalSubtitlesRole,
        PlaybackPositionRole,
        ThumbnailRole,
        TimestampRole,
    };

    /**
     * @brief Constructs a HistoryManager and loads persisted entries.
     *
     * @param parent The parent QObject.
     */
    explicit HistoryManager(QObject *parent = nullptr);

    /**
     * @brief Returns the number of history entries.
     *
     * @param parent The parent index (unused).
     * @return The number of entries.
     */
    [[nodiscard]]
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns data for a given role at the given index.
     *
     * @param index The model index of the entry.
     * @param role The data role to retrieve.
     * @return The data, or an empty QVariant if invalid.
     */
    [[nodiscard]]
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns the role names for QML access.
     *
     * @return A hash mapping role IDs to names.
     */
    [[nodiscard]]
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Adds a new history entry.
     *
     * If an entry with the same video path already exists, it is replaced.
     * The list is capped at 50 entries.
     *
     * @param videoPath Path to the video file.
     * @param primarySubtitleTrackId The primary subtitle track ID.
     * @param secondarySubtitleTrackId The secondary subtitle track ID.
     * @param externalSubtitles List of external subtitle file paths.
     * @param playbackPosition The playback position in seconds.
     * @param title The display title of the video.
     * @param thumbnailPath Path to the thumbnail image.
     */
    Q_INVOKABLE void addEntry(
        const QString &videoPath,
        int primarySubtitleTrackId,
        int secondarySubtitleTrackId,
        const QStringList &externalSubtitles,
        double playbackPosition,
        const QString &title,
        const QString &thumbnailPath);

    /**
     * @brief Removes a history entry at the given index.
     *
     * Also removes the associated thumbnail.
     *
     * @param index The index of the entry to remove.
     */
    Q_INVOKABLE void removeEntry(int index);

    /**
     * @brief Removes all history entries and their thumbnails.
     */
    Q_INVOKABLE void clearAll();

    /**
     * @brief Checks if the files in an entry still exist.
     *
     * Emits fileError for missing files and removes missing subtitle entries.
     *
     * @param index The index of the entry to check.
     */
    Q_INVOKABLE void checkFiles(int index);

    /**
     * @brief Returns the number of history entries.
     *
     * @return The number of entries.
     */
    Q_INVOKABLE int entryCount() const;

    /**
     * @brief Finds the index of an entry by video path.
     *
     * @param videoPath The video path to search for.
     * @return The index, or -1 if not found.
     */
    Q_INVOKABLE int findEntryByVideo(const QString &videoPath) const;

signals:
    /**
     * @brief Emitted when the entry list changes.
     */
    void entriesChanged();

    /**
     * @brief Emitted when a file referenced by an entry is missing.
     *
     * @param index The index of the entry.
     * @param message A description of the error.
     */
    void fileError(int index, const QString &message);

private:
    void loadFromFile();
    void saveToFile();
    void removeThumbnail(const QString &path);

    /* List of history entries */
    QList<HistoryEntry> m_entries;
    /* Path to the history JSON file */
    QString m_historyFile;
};
