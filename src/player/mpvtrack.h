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

/**
 * @brief Holds information on a player track.
 */
class MpvTrack : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        Type m_type
        READ type
        WRITE setType
        NOTIFY typeChanged
    )

    Q_PROPERTY(
        int64_t id
        READ id
        WRITE setId
        NOTIFY idChanged
    )

    Q_PROPERTY(
        int64_t sourceId
        READ sourceId
        WRITE setSourceId
        NOTIFY sourceIdChanged
    )

    Q_PROPERTY(
        QString title
        READ title
        WRITE setTitle
        NOTIFY titleChanged
    )

    Q_PROPERTY(
        QString language
        READ language
        WRITE setLanguage
        NOTIFY languageChanged
    )

    Q_PROPERTY(
        bool albumArt
        READ albumArt
        WRITE setAlbumArt
        NOTIFY albumArtChanged
    )

    Q_PROPERTY(
        bool defaultTrack
        READ defaultTrack
        WRITE setDefaultTrack
        NOTIFY defaultTrackChanged
    )

    Q_PROPERTY(
        bool selected
        READ selected
        WRITE setSelected
        NOTIFY selectedChanged
    )

    Q_PROPERTY(
        int64_t mainSelection
        READ mainSelection
        WRITE setMainSelection
        NOTIFY mainSelectionChanged
    )

    Q_PROPERTY(
        bool external
        READ external
        WRITE setExternal
        NOTIFY externalChanged
    )

    Q_PROPERTY(
        QString externalFilename
        READ externalFilename
        WRITE setExternalFilename
        NOTIFY externalFilenameChanged
    )

    Q_PROPERTY(
        QString codec
        READ codec
        WRITE setCodec
        NOTIFY codecChanged
    )

public:
    MpvTrack(QObject *parent = nullptr);
    virtual ~MpvTrack() = default;

    /**
     * @brief The type of track this is.
     */
    enum Type
    {
        None = 0,
        Audio,
        Video,
        Subtitle,
    };
    Q_ENUM(Type)

    /**
     * @brief The type of the track.
     *
     * @return The type of the track.
     */
    [[nodiscard]]
    Type type() const noexcept;

    /**
     * @brief Sets the type of the track.
     *
     * @param value The type of the track.
     */
    void setType(Type value);

    /**
     * @brief The ID of the track.
     * Starts at 1.
     * Unique within its own type.
     *
     * @return The ID of the track.
     */
    [[nodiscard]]
    int64_t id() const noexcept;

    /**
     * @brief Sets the ID of the track.
     *
     * @param value The ID of the track.
     */
    void setId(int64_t value);

    /**
     * @brief The track ID as used in the source file.
     * Not always available.
     *
     * @return The track ID as used in the source file.
     */
    [[nodiscard]]
    int64_t sourceId() const noexcept;

    /**
     * @brief Sets the source ID of the track.
     *
     * @param value The source ID of the track.
     */
    void setSourceId(int64_t value);

    /**
     * @brief The title of the track.
     * Not always available.
     *
     * @return The title of the track.
     */
    [[nodiscard]]
    const QString &title() const noexcept;

    /**
     * @brief Sets the title of the track.
     *
     * @param value The title of the track.
     */
    void setTitle(QString value);

    /**
     * @brief The language of the track.
     * Not always available.
     *
     * @return The language of the track.
     */
    [[nodiscard]]
    const QString &language() const noexcept;

    /**
     * @brief Sets the language of the track.
     *
     * @param value The language of the track.
     */
    void setLanguage(QString value);

    /**
     * @brief True if the track has album art.
     *
     * @return true if the track has album art,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool albumArt() const noexcept;

    /**
     * @brief Sets if the track has album art.
     *
     * @param value True if the track has album art.
     */
    void setAlbumArt(bool value);

    /**
     * @brief True if this track is a default track.
     *
     * @return true if the track is a default,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool defaultTrack() const noexcept;

    /**
     * @brief Sets if this track is a default track.
     *
     * @param value True if this is a default track.
     */
    void setDefaultTrack(bool value);

    /**
     * @brief True if the track is selected.
     *
     * @return true if the track is selected,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool selected() const noexcept;

    /**
     * @brief Sets if the track is selected.
     *
     * @param value True if the track is selected.
     */
    void setSelected(bool value);

    /**
     * @brief 0 if this track is the primary selection, 1 if it is the
     * secondary selection.
     *
     * @return The selection of the track.
     */
    [[nodiscard]]
    int64_t mainSelection() const noexcept;

    /**
     * @brief Sets the main selection of the track.
     *
     * @param value The selection index of the track.
     */
    void setMainSelection(int64_t value);

    /**
     * @brief If this track is from an external file.
     *
     * @return true if the track is from an external file,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool external() const noexcept;

    /**
     * @brief Sets if this track is an external track.
     *
     * @param value True if the track is external, false otherwise.
     */
    void setExternal(bool value);

    /**
     * @brief The filename of the track if it's external.
     *
     * @return The filename of the track it it's external.
     */
    [[nodiscard]]
    const QString &externalFilename() const noexcept;

    /**
     * @brief Sets the filename of the track.
     *
     * @param value The filename of the external track.
     */
    void setExternalFilename(QString value);

    /**
     * @brief The codec of the track.
     * Unavailable in some rare cases.
     *
     * @return The codec of the track.
     */
    [[nodiscard]]
    const QString &codec() const noexcept;

    /**
     * @brief Sets the codec of the track.
     *
     * @param value The codec of the track.
     */
    void setCodec(QString value);

signals:
    /**
     * @brief Emitted when the type is changed.
     *
     * @param value The type of the track.
     */
    void typeChanged(Type value);

    /**
     * @brief Emitted when the ID is changed.
     *
     * @param value The ID of the track.
     */
    void idChanged(int64_t value);

    /**
     * @brief Emitted when the source ID is changed.
     *
     * @param value The source ID of the track.
     */
    void sourceIdChanged(int64_t value);

    /**
     * @brief Emitted when the title of the track is changed.
     *
     * @param value The title of the track.
     */
    void titleChanged(const QString &value);

    /**
     * @brief Emitted when the language of the track is changed.
     *
     * @param value The language of the track.
     */
    void languageChanged(const QString &value);

    /**
     * @brief Emitted when the album art availability of the track changes.
     *
     * @param value True if the track has album art, false otherwise.
     */
    void albumArtChanged(bool value);

    /**
     * @brief Emitted when the default status of this track changes.
     *
     * @param value True if this is a default track, false otherwise.
     */
    void defaultTrackChanged(bool value);

    /**
     * @brief Emitted when the selected property changes.
     *
     * @param value True if the track is selected, false otherwise.
     */
    void selectedChanged(bool value);

    /**
     * @brief Emitted when the main selection property changes.
     *
     * @param value 0 if the track is primary, 1 if the track is secondary.
     */
    void mainSelectionChanged(int64_t value);

    /**
     * @brief Emitted when the external property is changed.
     *
     * @param value True if this track is external, false otherwise.
     */
    void externalChanged(bool value);

    /**
     * @brief The external filename of the track.
     *
     * @param value The filename of the track.
     */
    void externalFilenameChanged(const QString &value);

    /**
     * @brief Emitted when the codec name of the track changes.
     *
     * @param value The name of the codec.
     */
    void codecChanged(const QString &value);

private:
    /* The type of track */
    Type m_type{Type::None};

    /* The ID of the track. Starts at 1. Unique within its type. */
    int64_t m_id{0};

    /* The ID of the track in the source file. Not always available. */
    int64_t m_sourceId{0};

    /* The title of the track in the file. Not always available. */
    QString m_title;

    /* The language of the track in the file. Not always available. */
    QString m_language;

    /* True if this track has album art, false otherwise. */
    bool m_albumArt{false};

    /* True if this is a default track, false otherwise. */
    bool m_defaultTrack{false};

    /* True if this track is selected, false otherwise. */
    bool m_selected{false};

    /* 0 if this track is a primary selection, 1 if it's a secondary
     * selection */
    int64_t m_mainSelection{0};

    /* True if this track is from an external file */
    bool m_external{false};

    /* The filename of the track if it is external */
    QString m_externalFilename;

    /* The codec of the track. Unavailable in some cases. */
    QString m_codec;
};
