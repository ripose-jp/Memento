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

#include <QList>
#include <QQmlListProperty>

#include <mpv/client.h>

#include "player/mpvsubtitle.h"
#include "player/mpvtrack.h"

class MpvPlayer;

/**
 * @brief Holds the state of the current mpv instance.
 */
class MpvState : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        int videoWidth
        READ videoWidth
        NOTIFY videoWidthChanged
    )

    Q_PROPERTY(
        int videoHeight
        READ videoHeight
        NOTIFY videoHeightChanged
    )

    Q_PROPERTY(
        QList<double> chapters
        READ chapters
        NOTIFY chaptersChanged
    )

    Q_PROPERTY(
        double duration
        READ duration
        NOTIFY durationChanged
    )

    Q_PROPERTY(
        bool fullscreen
        READ fullscreen
        NOTIFY fullscreenChanged
    )

    Q_PROPERTY(
        QString path
        READ path
        NOTIFY pathChanged
    )

    Q_PROPERTY(
        QString title
        READ title
        NOTIFY titleChanged
    )

    Q_PROPERTY(
        bool pause
        READ pause
        NOTIFY pauseChanged
    )

    Q_PROPERTY(
        double timePosition
        READ timePosition
        NOTIFY timePositionChanged
    )

    Q_PROPERTY(
        QQmlListProperty<MpvTrack> audioTracks
        READ audioTracksQml
        NOTIFY audioTracksChanged
    )

    Q_PROPERTY(
        QQmlListProperty<MpvTrack> videoTracks
        READ videoTracksQml
        NOTIFY videoTracksChanged
    )

    Q_PROPERTY(
        QQmlListProperty<MpvTrack> subtitleTracks
        READ subtitleTracksQml
        NOTIFY subtitleTracksChanged
    )

    Q_PROPERTY(
        int64_t volume
        READ volume
        NOTIFY volumeChanged
    )

    Q_PROPERTY(
        int64_t maxVolume
        READ maxVolume
        NOTIFY maxVolumeChanged
    )

    Q_PROPERTY(
        int64_t aid
        READ aid
        NOTIFY aidChanged
    )

    Q_PROPERTY(
        int64_t sid
        READ sid
        NOTIFY sidChanged
    )

    Q_PROPERTY(
        int64_t secondarySid
        READ secondarySid
        NOTIFY secondarySidChanged
    )

    Q_PROPERTY(
        int64_t vid
        READ vid
        NOTIFY vidChanged
    )

    Q_PROPERTY(
        MpvSubtitle *subtitle
        READ subtitle
        NOTIFY subtitleChanged
    )

    Q_PROPERTY(
        MpvSubtitle *secondarySubtitle
        READ secondarySubtitle
        NOTIFY secondarySubtitleChanged
    )

    Q_PROPERTY(
        AutoHideType cursorAutoHideType
        READ cursorAutoHideType
        NOTIFY cursorAutoHideTypeChanged
    )

    Q_PROPERTY(
        int64_t cursorAutoHideTime
        READ cursorAutoHideTime
        NOTIFY cursorAutoHideTimeChanged
    )

    Q_PROPERTY(
        bool cursorAutoHideFullscreenOnly
        READ cursorAutoHideFullscreenOnly
        NOTIFY cursorAutoHideFullscreenOnlyChanged
    )

public:
    MpvState(MpvPlayer *parent = nullptr);
    virtual ~MpvState() = default;

    /**
     * @brief Describes internal state of mpv's cursor-autohide property.
     */
    enum AutoHideType
    {
        AutoHideNumber,
        AutoHideAlways,
        AutoHideNever,
    };
    Q_ENUM(AutoHideType)

    /**
     * @brief The player this state object is tracking.
     * This property is NOT exposed to QML.
     *
     * @return A pointer to the mpv player.
     */
    [[nodiscard]]
    MpvPlayer *player() const noexcept;

    /**
     * @brief Sets the player this state is bound to.
     *
     * @param value The player this state is bound to.
     */
    void setPlayer(MpvPlayer *value);

    /**
     * @brief The width of the video in pixels.
     *
     * @return The width of the video in pixels.
     */
    [[nodiscard]]
    int64_t videoWidth() const noexcept;

    /**
     * @brief Sets the video width in pixels.
     *
     * @param value The video width in pixels.
     */
    void setVideoWidth(int64_t value);

    /**
     * @brief The height of the video in pixels.
     *
     * @return The height of the video in pixels.
     */
    [[nodiscard]]
    int64_t videoHeight() const noexcept;

    /**
     * @brief Sets the video height in pixels.
     *
     * @param value The video height in pixels.
     */
    void setVideoHeight(int64_t value);

    /**
     * @brief Returns a list of chapters for the currently playing content.
     *
     * @return A reference to the chapters timestamps.
     */
    [[nodiscard]]
    const QList<double> &chapters() const noexcept;

    /**
     * @brief Set the chapter timestamps.
     *
     * @param values The value to set as the timestamps.
     */
    void setChapters(QList<double> values);

    /**
     * @brief Returns the duration of the content.
     *
     * @return The duration of the content in seconds.
     */
    [[nodiscard]]
    double duration() const noexcept;

    /**
     * @brief Sets the duration of the content.
     *
     * @param value The duration of the content.
     */
    void setDuration(double value);

    /**
     * @brief The mpv fullscreen property.
     *
     * @return true mpv is fullscreen.
     * @return false mpv is not fullscreen.
     */
    [[nodiscard]]
    bool fullscreen() const noexcept;

    /**
     * @brief Sets the mpv fullscreen property.
     *
     * @param value True if fullscreen, false otherwise.
     */
    void setFullscreen(bool value);

    /**
     * @brief The title of the currently playing content.
     *
     * @return The title of the currently playing content.
     */
    [[nodiscard]]
    const QString &title() const noexcept;

    /**
     * @brief Sets the title of the currently playing content.
     *
     * @param title The title of the currently playing content.
     */
    void setTitle(QString title);

    /**
     * @brief Sets the path of the currently playing content.
     *
     * @return The path of the currently playing content.
     */
    [[nodiscard]]
    const QString &path() const noexcept;

    /**
     * @brief Sets the path of the currently playing content.
     *
     * @param value The path of the currently playing content.
     */
    void setPath(QString value);

    /**
     * @brief The mpv paused property.
     *
     * @return true if the player is paused.
     * @return false if the player is playing.
     */
    [[nodiscard]]
    bool pause() const noexcept;

    /**
     * @brief Sets the paused state of the player.
     *
     * @param value True if the player is paused, false otherwise.
     */
    void setPause(bool value);

    /**
     * @brief The time position of the player in seconds.
     *
     * @return The time position of the player in seconds.
     */
    [[nodiscard]]
    double timePosition() const noexcept;

    /**
     * @brief Sets the time position of the player.
     *
     * @param value The time position of the player.
     */
    void setTimePosition(double value);

    /**
     * @brief Gets the audio tracks in a QQmlPropertyList.
     *
     * @return A QQmlPropertyList referencing the audio tracks.
     */
    [[nodiscard]]
    QQmlListProperty<MpvTrack> audioTracksQml();

    /**
     * @brief The list of audio tracks.
     * Owned by MpvState.
     *
     * @return The list of audio tracks.
     */
    [[nodiscard]]
    const QList<MpvTrack *> &audioTracks() const noexcept;

    /**
     * @brief Clears existing audio tracks and sets new tracks.
     * MpvState takes ownership.
     *
     * @param tracks The tracks to set.
     */
    void setAudioTracks(const QList<MpvTrack *> &tracks);

    /**
     * @brief Gets the video tracks in a QQmlPropertyList.
     *
     * @return A QQmlPropertyList referencing the video tracks.
     */
    [[nodiscard]]
    QQmlListProperty<MpvTrack> videoTracksQml();

    /**
     * @brief The list of video tracks.
     * Owned by MpvState.
     *
     * @return The list of video tracks.
     */
    [[nodiscard]]
    const QList<MpvTrack *> &videoTracks() const noexcept;

    /**
     * @brief Clears existing video tracks and sets new tracks.
     * MpvState takes ownership.
     *
     * @param tracks The tracks to set.
     */
    void setVideoTracks(const QList<MpvTrack *> &tracks);

    /**
     * @brief Gets the subtitle tracks in a QQmlPropertyList.
     *
     * @return A QQmlPropertyList referencing the video tracks.
     */
    [[nodiscard]]
    QQmlListProperty<MpvTrack> subtitleTracksQml();

    /**
     * @brief The list of subtitle tracks.
     * Owned by MpvState.
     *
     * @return The list of subtitle tracks.
     */
    [[nodiscard]]
    const QList<MpvTrack *> &subtitleTracks() const noexcept;

    /**
     * @brief Clears existing subtitle tracks and sets new tracks.
     * MpvState takes ownership.
     *
     * @param tracks The tracks to set.
     */
    void setSubtitleTracks(const QList<MpvTrack *> &tracks);

    /**
     * @brief Sets audioTracks, videoTracks, and subtitleTracks.
     *
     * @param node The mpv_node containing the results of track-list.
     */
    void setTracks(const mpv_node *node);

    /**
     * @brief The current volume of the player.
     *
     * @return The current volume of the player.
     */
    [[nodiscard]]
    int64_t volume() const noexcept;

    /**
     * @brief Sets the current volume of the player.
     *
     * @param value The current volume of the player.
     */
    void setVolume(int64_t value);

    /**
     * @brief The current max volume of the player.
     *
     * @return The current max volume of the player.
     */
    [[nodiscard]]
    int64_t maxVolume() const noexcept;

    /**
     * @brief Sets the current max volume of the player.
     *
     * @param value The current max volume of the player.
     */
    void setMaxVolume(int64_t value);

    /**
     * @brief The index of the currently selected audio track.
     * 0 if none is selected.
     *
     * @return The index of the currently selected audio track.
     */
    [[nodiscard]]
    int64_t aid() const noexcept;

    /**
     * @brief Sets the index of the currently selected audio track.
     *
     * @param value The value of the currently selected audio track.
     */
    void setAid(int64_t value);

    /**
     * @brief The index of the currently selected subtitle track.
     * 0 if none is selected.
     *
     * @return The index of the currently selected subtitle track.
     */
    [[nodiscard]]
    int64_t sid() const noexcept;

    /**
     * @brief Sets the index of the currently selected subtitle track.
     *
     * @param value The value of the currently selected subtitle track.
     */
    void setSid(int64_t value);

    /**
     * @brief The index of the currently selected secondary subtitle track.
     * 0 if none is selected.
     *
     * @return The index of the currently selected secondary subtitle track.
     */
    [[nodiscard]]
    int64_t secondarySid() const noexcept;

    /**
     * @brief Sets the index of the currently selected secondary subtitle track.
     *
     * @param value The value of the currently selected secondary subtitle
     * track.
     */
    void setSecondarySid(int64_t value);

    /**
     * @brief The index of the currently selected video track.
     * 0 if none is selected.
     *
     * @return The index of the currently selected video track.
     */
    [[nodiscard]]
    int64_t vid() const noexcept;

    /**
     * @brief Sets the index of the currently selected video track.
     *
     * @param value The value of the currently selected video track.
     */
    void setVid(int64_t value);

    /**
     * @brief The current subtitle information.
     *
     * @return The current subtitle information.
     */
    [[nodiscard]]
    MpvSubtitle *subtitle() const noexcept;

    /**
     * @brief Sets the subtitle object. Takes ownership.
     *
     * @param value The subtitle object.
     */
    void setSubtitle(MpvSubtitle *value);

    /**
     * @brief The current secondary subtitle information.
     *
     * @return The current secondary subtitle information.
     */
    [[nodiscard]]
    MpvSubtitle *secondarySubtitle() const noexcept;

    /**
     * @brief Sets the secondary subtitle object.
     *
     * @param value The secondary subtitle object.
     */
    void setSecondarySubtitle(MpvSubtitle *value);

    /**
     * @brief Gets the type of property cursor-autohide.
     *
     * @return The mpv cursor-autohide type.
     */
    [[nodiscard]]
    AutoHideType cursorAutoHideType() const noexcept;

    /**
     * @brief Sets the cursor-autohide type.
     *
     * @param value The cursor-autohide type.
     */
    void setCursorAutoHideType(AutoHideType value);

    /**
     * @brief Sets the milliseconds of inactivity before the cursor is hidden
     * if cursorAutoHideType is number.
     *
     * @return The milliseconds of inactivity before the cursor is hidden.
     */
    [[nodiscard]]
    int64_t cursorAutoHideTime() const noexcept;

    /**
     * @brief Sets the milliseconds of inactivity before the cursor is hidden.
     *
     * @param value The milliseconds of inactivity before the cursor is hidden.
     */
    void setCursorAutoHideTime(int64_t value);

    /**
     * @brief Gets if the cursor should only be hidden in fullscreen.
     *
     * @return true if the cursor should only be hidden in fullscreen,
     * @return false if the cursor should always be hidden.
     */
    [[nodiscard]]
    bool cursorAutoHideFullscreenOnly() const noexcept;

    /**
     * @brief Sets if the cursor should only be hidden in fullscreen.
     *
     * @param value true if the cursor should only be hidden in fullscreen,
     * false if the cursor should always be hidden.
     */
    void setCursorAutoHideFullscreenOnly(bool value);

signals:
    /**
     * @brief Emitted when the player is changed.
     * This property is NOT exposed to QML.
     */
    void playerChanged();

    /**
     * @brief Emitted when the video width is changed.
     *
     * @param value The video width in pixels.
     */
    void videoWidthChanged(int64_t value);

    /**
     * @brief Emitted when the video height is changed.
     *
     * @param value The video height in pixels.
     */
    void videoHeightChanged(int64_t value);

    /**
     * @brief Emitted when the player is changed.
     *
     * @param values The chapter timestamps.
     */
    void chaptersChanged(const QList<double> &values);

    /**
     * @brief Emitted when the duration of the content is changed.
     *
     * @param value The duration of the content.
     */
    void durationChanged(double value);

    /**
     * @brief Emitted when mpv's fullscreen property is changed.
     *
     * @param value True if fullscreen, false otherwise.
     */
    void fullscreenChanged(bool value);

    /**
     * @brief Emitted when the title of the content is changed.
     *
     * @param value The title of the media.
     */
    void titleChanged(const QString &value);

    /**
     * @brief Emitted when the path of the content changes.
     *
     * @param value The path to the content.
     */
    void pathChanged(const QString &value);

    /**
     * @brief Emitted when the paused state of the player is changed.
     *
     * @param value True if the player is paused, false otherwise.
     */
    void pauseChanged(bool value);

    /**
     * @brief Emitted when the time position is changed.
     *
     * @param value The time position of the player in seconds.
     */
    void timePositionChanged(double value);

    /**
     * @brief Emitted when the audio tracks are changed.
     */
    void audioTracksChanged();

    /**
     * @brief Emitted when the video tracks are changed.
     */
    void videoTracksChanged();

    /**
     * @brief Emitted when the subtitle tracks are changed.
     */
    void subtitleTracksChanged();

    /**
     * @brief Emitted when the volume of the player is changed.
     *
     * @param value The current volume of the player.
     */
    void volumeChanged(int64_t value);

    /**
     * @brief Emitted when the max volume of the player changes.
     *
     * @param value The max volume of the player.
     */
    void maxVolumeChanged(int64_t value);

    /**
     * @brief Emitted when the audio track is changed.
     *
     * @param value The ID of the current audio track. 0 if none.
     */
    void aidChanged(int64_t value);

    /**
     * @brief Emitted when the subtitle track is changed.
     *
     * @param value The ID of the current subtitle track. 0 if none.
     */
    void sidChanged(int64_t value);

    /**
     * @brief Emitted when the secondary subtitle track is changed.
     *
     * @param value The ID of the current secondary subtitle track. 0 if none.
     */
    void secondarySidChanged(int64_t value);

    /**
     * @brief Emitted when the video track is changed.
     *
     * @param value The ID of the current video track. 0 if none.
     */
    void vidChanged(int64_t value);

    /**
     * @brief Emitted when the subtitle object is changed.
     *
     * @param value The subtitle object.
     */
    void subtitleChanged(MpvSubtitle *value);

    /**
     * @brief Emitted when the secondary subtitle object is changed.
     *
     * @param value The secondary subtitle object.
     */
    void secondarySubtitleChanged(MpvSubtitle *value);

    /**
     * @brief Emitted when cursor-autohide is changed.
     *
     * @param value The cursor-autohide type.
     */
    void cursorAutoHideTypeChanged(AutoHideType value);

    /**
     * @brief Emitted when cursor-autohide time is changed.
     *
     * @param value The milliseconds of inactivity before the cursor is hidden.
     */
    void cursorAutoHideTimeChanged(int64_t value);

    /**
     * @brief Emitted when the auto hide fullscreen only is changed.
     *
     * @param value true if the cursor should only be hidden in fullscreen,
     * false if the cursor should always be hidden.
     */
    void cursorAutoHideFullscreenOnlyChanged(bool value);

private:
    /* The player this state is tracking */
    MpvPlayer *m_player{nullptr};

    /* The width of the video in pixels */
    int64_t m_videoWidth{0};

    /* The height of the video in pixels */
    int64_t m_videoHeight{0};

    /* Chapter timestamps */
    QList<double> m_chapters;

    /* The duration of the content */
    double m_duration{0};

    /* True if mpv is fullscreen, false otherwise */
    bool m_fullscreen{false};

    /* The title of the currently playing media */
    QString m_title;

    /* The path to the currently playing content */
    QString m_path;

    /* The mpv pause property */
    bool m_pause{false};

    /* The time position of the player */
    double m_timePosition{0};

    /* The current audio track information */
    QList<MpvTrack *> m_audioTracks;

    /* The current video track information */
    QList<MpvTrack *> m_videoTracks;

    /* The subtitle video track information */
    QList<MpvTrack *> m_subtitleTracks;

    /* The current volume of the player */
    int64_t m_volume{0};

    /* The maximum volume of the player */
    int64_t m_maxVolume{0};

    /* The ID of the currently selected audio track. 0 if none */
    int64_t m_aid{0};

    /* The ID of the currently selected subtitle track. 0 if none */
    int64_t m_sid{0};

    /* The ID of the currently selected secondary subtitle track. 0 if none */
    int64_t m_secondarySid{0};

    /* The ID of the currently selected video track. 0 if none*/
    int64_t m_vid{0};

    /* The primary subtitle of the player */
    MpvSubtitle *m_subtitle{new MpvSubtitle(this)};

    /* The secondary subtitle of the player */
    MpvSubtitle *m_secondarySubtitle{new MpvSubtitle(this)};

    /* The mpv auto hide type */
    AutoHideType m_cursorAutoHideType{AutoHideType::AutoHideNumber};

    /* The millisecond timeout before the cursor should be hidden if
     * AutoHideNumber */
    int64_t m_cursorAutoHideTime{0};

    /* true if the cursor should only be hidden in fullscreen */
    bool m_cursorAutoHideFullscreenOnly{false};
};
