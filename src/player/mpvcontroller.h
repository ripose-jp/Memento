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

#include <optional>

#include <QImage>
#include <QPoint>
#include <QSet>

#include <mpv/client.h>

class MpvPlayer;

/**
 * @brief Arguments for tempAudioClip.
 */
struct MpvAudioClipArgs
{
    Q_GADGET
    Q_PROPERTY(double start MEMBER start)
    Q_PROPERTY(double end MEMBER end)
    Q_PROPERTY(bool normalize MEMBER normalize)
    Q_PROPERTY(double db MEMBER db)
    Q_PROPERTY(QString extension MEMBER extension)

public:
    /* The start time of the clip */
    double start = 0.0;

    /* The end time of the clip */
    double end = 0.0;

    /* True to normalize audio, false otherwise */
    bool normalize = false;

    /* The decibels to normalize to */
    double db = -20.0;

    /* The file extension to write to */
    QString extension = ".aac";
};

/**
 * @brief Arguments for tempVideoClip.
 */
struct MpvVideoClipArgs
{
    Q_GADGET
    Q_PROPERTY(double start MEMBER start)
    Q_PROPERTY(double end MEMBER end)
    Q_PROPERTY(bool audio MEMBER audio)
    Q_PROPERTY(bool subtitles MEMBER subtitles)
    Q_PROPERTY(bool normalize MEMBER normalize)
    Q_PROPERTY(double db MEMBER db)

public:
    /* The start time of the clip */
    double start = 0.0;

    /* The end time of the clip */
    double end = 0.0;

    /* True to include audio in the clip, false otherwise */
    bool audio = true;

    /* True to include subtitles, false otherwise */
    bool subtitles = true;

    /* True to normalize audio, false otherwise */
    bool normalize = false;

    /* The decibels to normalize to */
    double db = -20.0;
};

/**
 * @brief Issue commands to an mpv instance.
 */
class MpvController : public QObject
{
    Q_OBJECT

public:
    MpvController(MpvPlayer *parent = nullptr);
    virtual ~MpvController() = default;

    /**
     * @brief The player this controller object is controlling.
     * This property is NOT exposed to QML.
     *
     * @return A pointer to the mpv player.
     */
    [[nodiscard]]
    MpvPlayer *player() const noexcept;

    /**
     * @brief Sets the player this controller is bound to.
     *
     * @param value The player this controller is bound to.
     */
    void setPlayer(MpvPlayer *value);

    /**
     * @brief Load files and options from the commandline.
     *
     * @param args The list of commandline arguments.
     * @return true if something was loaded,
     * @return false if there was no arguments.
     */
    Q_INVOKABLE bool loadArgs(const QStringList &args);

    /**
     * @brief Loads a file into mpv for playback.
     *
     * @param file The path of the file to load.
     * @param append True to append to the playlist, false to replace the
     * playlist.
     * @param options Options to pass to loadfile.
     * @return true if the file was loaded,
     * @return false otherwise.
     */
    Q_INVOKABLE bool loadFile(
        const QString &file,
        bool append = false,
        const QStringList &options = {});

    /**
     * @brief Loads multiple files into a playlist.
     *
     * @param files The list of files to load.
     * @param append True to append to the playlist, false to replace the
     * playlist.
     */
    Q_INVOKABLE void loadFile(const QStringList &files, bool append = false);

    /**
     * @brief Loads a subtitle file for the currently playing media.
     *
     * @param file The path to the subtitle file.
     * @param true if the subtitle was loaded,
     * @param false otherwise.
     */
    Q_INVOKABLE bool loadSubtitle(const QString &file);

    /**
     * @brief Seeks to the specified time.
     *
     * @param time The time in seconds.
     */
    Q_INVOKABLE void seek(double time);

    /**
     * @brief Pauses playback.
     */
    Q_INVOKABLE void pause();

    /**
     * @brief Resumes playback.
     */
    Q_INVOKABLE void play();

    /**
     * @brief Stops playback and unloads all files.
     */
    Q_INVOKABLE void stop();

    /**
     * @brief Seeks count subtitle away.
     *
     * @param count The number of subtitle to seek.
     * @param primary true to seek the primary subtitle track, false to seek the
     * secondary track.
     */
    Q_INVOKABLE void subtitleSeek(int count, bool primary);

    /**
     * @brief Loads the next file in the playlist.
     */
    Q_INVOKABLE void playlistNext();

    /**
     * @brief Loads the previous file in the playlist.
     */
    Q_INVOKABLE void playlistPrev();

    /**
     * @brief Sets the auto track.
     *
     * @param id The ID of the audio track. 0 to disable.
     */
    Q_INVOKABLE void setAid(int64_t id);

    /**
     * @brief Sets the subtitle track.
     *
     * @param id The ID of the subtitle track. 0 to disable.
     */
    Q_INVOKABLE void setSid(int64_t id);

    /**
     * @brief Sets the secondary subtitle track.
     *
     * @param id The ID of the subtitle track. 0 to disable.
     */
    Q_INVOKABLE void setSecondarySid(int64_t id);

    /**
     * @brief Sets the video track.
     *
     * @param id The ID of the video track. 0 to disable.
     */
    Q_INVOKABLE void setVid(int64_t id);

    /**
     * @brief Sets the subtitle visibility.
     *
     * @param visible true for visible subtitles, false otherwise.
     */
    Q_INVOKABLE void setSubtitleVisibility(bool visible);

    /**
     * @brief Sets the secondary subtitle visibility.
     *
     * @param visible true for visible subtitles, false otherwise.
     */
    Q_INVOKABLE void setSecondarySubtitleVisibility(bool visible);

    /**
     * @brief Sets the subtitle delay.
     *
     * @param delay The delay to add to subtitles in seconds.
     */
    Q_INVOKABLE void setSubtitleDelay(double delay);

    /**
     * @brief Sets the secondary subtitle delay.
     *
     * @param delay The delay to add to secondary subtitles in seconds.
     */
    Q_INVOKABLE void setSecondarySubtitleDelay(double delay);

    /**
     * @brief Sets the fullscreen property of the player.
     *
     * @param value true if fullscreen, false otherwise.
     */
    Q_INVOKABLE void setFullscreen(bool value);

    /**
     * @brief Sets the volume of the player.
     *
     * @param value The volume of the player.
     */
    Q_INVOKABLE void setVolume(int64_t value);

    /**
     * @brief Shows text temporarily on the player.
     *
     * @param text The text to show.
     */
    Q_INVOKABLE void showText(const QString &text);

    /**
     * @brief Sends a keypress to the player.
     *
     * @param key The Qt key pressed.
     * @param modifiers The modifiers held.
     */
    Q_INVOKABLE void sendKeyPress(int key, int modifiers);

    /**
     * @brief Sends a mouse position to the player.
     *
     * @param x The x-coordinate of the cursor.
     * @param y The y-coordinate of the cursor.
     */
    Q_INVOKABLE void sendMouse(double x, double y);

    /**
     * @brief Sends a mouse button to the player.
     *
     * @param x The x-coordinate of the cursor.
     * @param y The y-coordinate of the cursor.
     * @param button The button pressed.
     * @param single true for a single click, false for a double.
     */
    Q_INVOKABLE void sendMouseButton(
        double x, double y, Qt::MouseButton button, bool single);

    /**
     * @brief Sends a mouse wheel event to the player.
     *
     * @param x The x-coordinate of the cursor.
     * @param y The y-coordinate of the cursor.
     * @param angleDelta The angle difference of the wheel.
     */
    Q_INVOKABLE void sendWheel(double x, double y, QPoint angleDelta);

    /**
     * @brief Take a screenshot of the player contents and stores the files in
     * the temp directory.
     *
     * @param subtitles true to include the subtitles in the images, false
     * otherwise.
     * @param ext The image format to use. '.jpg' by default.
     * @return Path to the file.
     */
    [[nodiscard]]
    Q_INVOKABLE QString tempScreenshot(
        bool subtitles, const QString &ext = ".jpg");

    /**
     * @brief Take a screenshot of the player contents in memory.
     *
     * @param subtitles true to include the subtitles in the image, false
     * otherwise.
     * @return The screenshot image, or a null image on failure.
     */
    [[nodiscard]]
    QImage screenshotRaw(bool subtitles);

    /**
     * @brief Create an audio clip given a start and end time in the temporary
     * directory.
     *
     * @param args The arguments to use to make the audio clip.
     * @return Path to the file.
     */
    [[nodiscard]]
    Q_INVOKABLE QString tempAudioClip(const MpvAudioClipArgs &args);

    /**
     * @brief Create an H264 video clip in the temporary directory.
     *
     * @param args The arguments that specify how the video should be cut.
     * @return Path to the file.
     */
    [[nodiscard]]
    Q_INVOKABLE QString tempVideoClip(const MpvVideoClipArgs &args);

    /**
     * @brief Saves a scaled thumbnail to the specified path.
     *
     * Captures the current frame, scales it down to 320x180,
     * and saves as JPEG with quality 85.
     *
     * @param path The full path to save the thumbnail to.
     * @return true if the thumbnail was saved, false otherwise.
     */
    Q_INVOKABLE bool saveThumbnail(const QString &path);

signals:
    /**
     * @brief Emitted when the player being controlled changes.
     */
    void playerChanged();

    /**
     * @brief Emitted when a subtitle file is loaded.
     *
     * @param path The path to the loaded subtitle file.
     */
    void subtitleFileLoaded(const QString &path);

private:
    /**
     * @brief Get the current mpv_handle.
     *
     * @return The mpv_handle to control.
     */
    [[nodiscard]]
    mpv_handle *handle() const noexcept;

    /**
     * @brief Node in the command line argument multi-tree.
     */
    struct LoadFileNode
    {
        /* List of files specific to this level */
        QStringList files;

        /* All the options specific to this level */
        QStringList options;

        /* Children of this node */
        QList<LoadFileNode> children;
    };

    /**
     * @brief Error check that args are well-formatted.
     *
     * @return true if they are valid,
     * @return false otherwise.
     */
    [[nodiscard]]
    static bool argsValid(const QStringList &args);

    /**
     * @brief Parses arguments recursively to build a multi-tree.
     *
     * @param args An array of command line arguments.
     * @param index  index to start processing arguments at.
     * @param[out] parent The parent node to build a tree below.
     * @param depth The maximum number of times to recurse.
     * @return The index of the argument array buildArgsTree stopped at. -1 on
     * error.
     */
    [[nodiscard]]
    static qsizetype buildArgsTree(
        const QStringList &args,
        qsizetype index,
        LoadFileNode &parent,
        qsizetype depth = 64);

    /**
     * @brief Quote escape mpv arguments.
     *
     * @param arg argument to quote.
     * @return The quoted argument if arg contained "=", otherwise arg.
     */
    [[nodiscard]]
    static QString quoteArg(QString arg);

    /**
     * @brief Load files from a tree of LoadFileNodes.
     *
     * @param parent The parent to load files from.
     * @param options Options inherited by files at this level.
     */
    void loadFilesFromTree(const LoadFileNode &parent, QStringList &options);

    /**
     * @brief Checks if a file is a subtitle file.
     *
     * @param path The path to the file.
     * @return true if the file is a subtitle,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool isSubtitleFile(const QString &path) const;

    /**
     * @brief Converts a modifier into an mpv-friendly modifier string.
     *
     * @param modifier The modifier sequence.
     * @return The modifier string ending in +. The empty string if 0.
     */
    [[nodiscard]]
    static QString toModifierString(int modifier);

    /**
     * @brief Create an mpv instance for encoding.
     *
     * @param argString The argument string to pass during loadfile.
     * @param options The options to start the mpv handle with.
     * @param fileExtension The file extension of the output file.
     * @return The path to the file, empty string on failure.
     */
    [[nodiscard]]
    QString encodeFile(
        const QByteArray &argString,
        const QList<QPair<QByteArray, QByteArray>> &options,
        const QString &fileExtension);

    /**
     * @brief Get a node value from an mpv_node map.
     *
     * @param node The node_map to get a value from.
     * @param key The name of the value to get.
     * @return An mpv_node if the value exists and was an mpv_node, nullptr
     * otherwise.
     */
    [[nodiscard]]
    static const mpv_node *mapValue(const mpv_node &node, const char *key);

    /**
     * @brief Get an integer value from an mpv_node map.
     *
     * @param node The node_map to get a value from.
     * @param key The name of the value to get.
     * @return The integer value corresponding to the given key, nullptr
     * otherwise.
     */
    [[nodiscard]]
    static std::optional<int64_t> mapInt(const mpv_node &node, const char *key);

    /**
     * @brief Get a string value from an mpv_node map.
     *
     * @param node The node_map to get a value from.
     * @param key The name of the value to get.
     * @return The string value corresponding to the given key, nullptr
     * otherwise.
     */
    [[nodiscard]]
    static QString mapString(const mpv_node &node, const char *key);

    /**
     * @brief Get an mpv_byte_array value from an mpv_node map.
     *
     * @param node The node_map to get a value from.
     * @param key The name of the value to get.
     * @return The mpv_byte_array value corresponding to the given key, nullptr
     * otherwise.
     */
    [[nodiscard]]
    static const mpv_byte_array *mapByteArray(
        const mpv_node &node, const char *key);

    /* The player that this controller controls */
    MpvPlayer *m_player{nullptr};

    /* The file extension of subtitle files */
    QSet<QString> m_subtitleExtensions;
};
