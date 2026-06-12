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

#include "quick/historymanager.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>

#include "util/directoryutils.h"

/* Begin Constructor */

HistoryManager::HistoryManager(QObject *parent)
    : QAbstractListModel(parent)
{
    m_historyFile = DirectoryUtils::getHistoryDir() + "history.json";
    loadFromFile();
}

/* End Constructor */
/* Begin Model Functions */

int HistoryManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_entries.size();
}

QVariant HistoryManager::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 ||
        index.row() >= m_entries.size())
    {
        return {};
    }

    const HistoryEntry &entry = m_entries[index.row()];
    switch (role)
    {
    case TitleRole:
        return entry.title;
    case VideoPathRole:
        return entry.videoPath;
    case PrimarySubtitleTrackIdRole:
        return entry.primarySubtitleTrackId;
    case SecondarySubtitleTrackIdRole:
        return entry.secondarySubtitleTrackId;
    case ExternalSubtitlesRole:
        return QVariant::fromValue(entry.externalSubtitles);
    case PlaybackPositionRole:
        return entry.playbackPosition;
    case ThumbnailRole:
        return entry.thumbnailPath;
    case TimestampRole:
        return entry.timestamp;
    default:
        return {};
    }
}

QHash<int, QByteArray> HistoryManager::roleNames() const
{
    return {
        {TitleRole, "title"},
        {VideoPathRole, "videoPath"},
        {PrimarySubtitleTrackIdRole, "primarySubtitleTrackId"},
        {SecondarySubtitleTrackIdRole, "secondarySubtitleTrackId"},
        {ExternalSubtitlesRole, "externalSubtitles"},
        {PlaybackPositionRole, "playbackPosition"},
        {ThumbnailRole, "thumbnail"},
        {TimestampRole, "timestamp"},
    };
}

/* End Model Functions */
/* Begin Public Functions */

void HistoryManager::addEntry(
    const QString &videoPath,
    int primarySubtitleTrackId,
    int secondarySubtitleTrackId,
    const QStringList &externalSubtitles,
    double playbackPosition,
    const QString &title,
    const QString &thumbnailPath)
{
    if (videoPath.isEmpty())
    {
        return;
    }

    for (int i = 0; i < m_entries.size(); ++i)
    {
        if (m_entries[i].videoPath == videoPath)
        {
            removeThumbnail(m_entries[i].thumbnailPath);
            beginRemoveRows(QModelIndex(), i, i);
            m_entries.removeAt(i);
            endRemoveRows();
            break;
        }
    }

    HistoryEntry entry;
    entry.title = title.isEmpty() ? QFileInfo(videoPath).fileName() : title;
    entry.videoPath = videoPath;
    entry.primarySubtitleTrackId = primarySubtitleTrackId;
    entry.secondarySubtitleTrackId = secondarySubtitleTrackId;
    entry.externalSubtitles = externalSubtitles;
    entry.playbackPosition = playbackPosition;
    entry.thumbnailPath = thumbnailPath;
    entry.timestamp = QDateTime::currentSecsSinceEpoch();

    beginInsertRows(QModelIndex(), 0, 0);
    m_entries.prepend(entry);
    endInsertRows();

    constexpr int MAX_ENTRIES = 50;
    if (m_entries.size() > MAX_ENTRIES)
    {
        for (int i = MAX_ENTRIES; i < m_entries.size(); ++i)
        {
            removeThumbnail(m_entries[i].thumbnailPath);
        }
        beginRemoveRows(QModelIndex(), MAX_ENTRIES, m_entries.size() - 1);
        m_entries.resize(MAX_ENTRIES);
        endRemoveRows();
    }

    saveToFile();
    emit entriesChanged();
}

void HistoryManager::removeEntry(int index)
{
    if (index < 0 || index >= m_entries.size())
    {
        return;
    }

    removeThumbnail(m_entries[index].thumbnailPath);

    beginRemoveRows(QModelIndex(), index, index);
    m_entries.removeAt(index);
    endRemoveRows();

    saveToFile();
    emit entriesChanged();
}

void HistoryManager::clearAll()
{
    if (m_entries.isEmpty())
    {
        return;
    }

    for (const auto &e : m_entries)
    {
        removeThumbnail(e.thumbnailPath);
    }

    beginResetModel();
    m_entries.clear();
    endResetModel();

    saveToFile();
    emit entriesChanged();
}

void HistoryManager::removeThumbnail(const QString &path)
{
    if (path.isEmpty())
    {
        return;
    }
    QFile file(path);
    if (file.exists())
    {
        file.remove();
    }
}

void HistoryManager::checkFiles(int index)
{
    if (index < 0 || index >= m_entries.size())
    {
        return;
    }

    const HistoryEntry &entry = m_entries[index];

    if (!QFileInfo::exists(entry.videoPath))
    {
        emit fileError(
            index,
            QString("Video file not found:\n%1").arg(entry.videoPath)
        );
    }

    QStringList &ext = m_entries[index].externalSubtitles;
    bool changed = false;
    for (int i = ext.size() - 1; i >= 0; --i)
    {
        if (!QFileInfo::exists(ext[i]))
        {
            emit fileError(
                index,
                QString(
                    "External subtitle file not found and has been removed "
                    "from history:\n%1"
                ).arg(ext[i])
            );
            ext.removeAt(i);
            changed = true;
        }
    }

    if (changed)
    {
        saveToFile();
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {ExternalSubtitlesRole});
        emit entriesChanged();
    }
}

int HistoryManager::entryCount() const
{
    return m_entries.size();
}

int HistoryManager::findEntryByVideo(const QString &videoPath) const
{
    for (int i = 0; i < m_entries.size(); ++i)
    {
        if (m_entries[i].videoPath == videoPath)
        {
            return i;
        }
    }
    return -1;
}

/* End Public Functions */
/* Begin Private Functions */

void HistoryManager::loadFromFile()
{
    QFile file(m_historyFile);
    if (!file.exists())
    {
        return;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning("Could not open history file: %s", qPrintable(m_historyFile));
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray())
    {
        qWarning("Invalid history file format. Data: %s", data.constData());
        return;
    }

    QJsonArray arr = doc.array();
    for (const QJsonValue &val : arr)
    {
        QJsonObject obj = val.toObject();
        HistoryEntry entry;
        entry.title = obj["title"].toString();
        entry.videoPath = obj["videoPath"].toString();
        entry.primarySubtitleTrackId = obj["primarySubtitleTrackId"].toInt(0);
        entry.secondarySubtitleTrackId =
            obj["secondarySubtitleTrackId"].toInt(0);

        QJsonValue extVal = obj["externalSubtitles"];
        if (extVal.isArray())
        {
            for (const QJsonValue &v : extVal.toArray())
            {
                entry.externalSubtitles.append(v.toString());
            }
        }

        entry.playbackPosition = obj["playbackPosition"].toDouble(0);
        entry.thumbnailPath = obj["thumbnail"].toString();
        entry.timestamp =
            static_cast<qint64>(obj["timestamp"].toDouble());
        m_entries.append(entry);
    }
}

void HistoryManager::saveToFile()
{
    QJsonArray arr;
    for (const HistoryEntry &entry : m_entries)
    {
        QJsonObject obj;
        obj["title"] = entry.title;
        obj["videoPath"] = entry.videoPath;
        obj["primarySubtitleTrackId"] = entry.primarySubtitleTrackId;
        obj["secondarySubtitleTrackId"] = entry.secondarySubtitleTrackId;

        QJsonArray extArr;
        for (const QString &s : entry.externalSubtitles)
        {
            extArr.append(s);
        }
        obj["externalSubtitles"] = extArr;

        obj["playbackPosition"] = entry.playbackPosition;
        obj["thumbnail"] = entry.thumbnailPath;
        obj["timestamp"] = static_cast<double>(entry.timestamp);
        arr.append(obj);
    }

    QFile file(m_historyFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning("Could not write history file: %s", qPrintable(m_historyFile));
        return;
    }

    QJsonDocument doc(arr);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

/* End Private Functions */
