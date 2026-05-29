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

#include "player/mpvcontroller.h"

#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <limits>

#include <QScopeGuard>
#include <QTemporaryFile>

#include "player/mpvplayer.h"
#include "util/directoryutils.h"

MpvController::MpvController(MpvPlayer *parent) : QObject(parent)
{
    setPlayer(parent);
    m_subtitleExtensions = {
        "ass",
        "idx",
        "lrc",
        "mks",
        "pgs",
        "rt",
        "scc",
        "smi",
        "srt",
        "ssa",
        "sub",
        "sup",
        "utf-8",
        "utf",
        "utf8",
        "vtt",
    };
}

/* Begin Public Functions */

MpvPlayer *MpvController::player() const noexcept
{
    return m_player;
}

void MpvController::setPlayer(MpvPlayer *value)
{
    if (m_player == value)
    {
        return;
    }
    m_player = value;
    setParent(m_player);
    emit playerChanged();
}

bool MpvController::loadFile(
    const QString &file,
    bool append,
    const QStringList &options)
{
    /* Since mpv client API 2.3 (mpv 0.38.0) "loadfile" places an extra argument
     * before the options, so we need to conditionally re-arrange the arguments
     * array */
    constexpr bool IS_API_23{MPV_CLIENT_API_VERSION >= MPV_MAKE_VERSION(2, 3)};

    if (file.isEmpty())
    {
        return false;
    }

    if (isSubtitleFile(file))
    {
        return loadSubtitle(file);
    }

    QByteArray path = file.toUtf8();
    QByteArray opts = options.join(',').toUtf8();
    char *argOpts = opts.isEmpty() ? nullptr : opts.data();

    const char *args[]{
        "loadfile",
        path,
        append ? "append-play" : "replace",
        IS_API_23 ? "0" : argOpts,
        IS_API_23 ? argOpts : nullptr,
        nullptr
    };

    int ret = ::mpv_command(handle(), args);
    if (ret < 0)
    {
        qWarning(
            "Could not open file '%s': %s",
            qUtf8Printable(file),
            ::mpv_error_string(ret)
        );
        return false;
    }
    return true;
}

bool MpvController::loadArgs(const QStringList &args)
{
    if (args.size() <= 1)
    {
        return false;
    }

    if (!argsValid(args))
    {
        qInfo() << tr("Invalid command line arguments.");
        return false;
    }
    LoadFileNode parent{};
    if (buildArgsTree(args, 1, parent) == -1)
    {
        qInfo() << tr("Maximum number of nested per-file arguments exceeded.");
        return false;
    }
    QStringList emptyOpts;
    loadFilesFromTree(parent, emptyOpts);
    return true;
}

void MpvController::loadFile(const QStringList &files, bool append)
{
    if (files.isEmpty())
    {
        return;
    }

    for (const QString &file : files)
    {
        if (isSubtitleFile(file))
        {
            loadSubtitle(file);
        }
        else
        {
            if (!append)
            {
                stop();
            }
            loadFile(file, append);
            append = true;
        }
    }
}

bool MpvController::loadSubtitle(const QString &file)
{
    if (file.isEmpty())
    {
        return false;
    }

    QByteArray path = file.toUtf8();
    const char *args[]{
        "sub-add",
        path,
        nullptr
    };
    int ret = ::mpv_command_async(handle(), 0, args);
    if (ret < 0)
    {
        qWarning(
            "Could not add subtitle file '%s': %s",
            qUtf8Printable(file),
            ::mpv_error_string(ret)
        );
        return false;
    }
    return true;
}

void MpvController::seek(double time)
{
    QByteArray timestr = QString::number(time).toUtf8();
    const char *args[]{
        "seek",
        timestr,
        "absolute",
        NULL
    };
    if (::mpv_command_async(handle(), 0, args) < 0)
    {
        qWarning("Seeking failed");
    }
}

void MpvController::play()
{
    int flag = 0;
    if (::mpv_set_property(handle(), "pause", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qWarning("Could not set mpv pause property to %d", flag);
    }
}

void MpvController::pause()
{
    int flag = 1;
    if (::mpv_set_property(handle(), "pause", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qWarning("Could not set mpv pause property to %d", flag);
    }
}

void MpvController::stop()
{
    const char *args[]{
        "stop",
        nullptr
    };
    if (::mpv_command(handle(), args) < 0)
    {
        qWarning("Could not stop mpv");
    }
}

void MpvController::subtitleSeek(int count, bool primary)
{
    QByteArray countStr = QString::number(count).toUtf8();
    const char *args[]{
        "sub-seek",
        countStr,
        primary ? "primary" : "secondary",
        nullptr
    };
    if (::mpv_command_async(handle(), 0, args) < 0)
    {
        qWarning(
            "Could not seek %d subtitles in the %s track",
            count,
            primary ? "primary" : "secondary"
        );
    }
}

void MpvController::playlistNext()
{
    const char *args[]{
        "playlist-next",
        nullptr
    };
    if (::mpv_command_async(handle(), 0, args) < 0)
    {
        qWarning("Could not skip to the next file in the playlist");
    }
}

void MpvController::playlistPrev()
{
    const char *args[]{
        "playlist-prev",
        nullptr
    };
    if (::mpv_command_async(handle(), 0, args) < 0)
    {
        qWarning("Could not skip to the next file in the playlist");
    }
}

void MpvController::setAid(int64_t id)
{
    if (::mpv_set_property_async(handle(), 0, "aid", MPV_FORMAT_INT64, &id) < 0)
    {
        qWarning("Could not set audio track to %" PRId64, id);
    }
}

void MpvController::setSid(int64_t id)
{
    if (::mpv_set_property_async(handle(), 0, "sid", MPV_FORMAT_INT64, &id) < 0)
    {
        qWarning("Could not set subtitle track to %" PRId64, id);
    }
}

void MpvController::setSecondarySid(int64_t id)
{
    if (::mpv_set_property_async(handle(), 0, "secondary-sid", MPV_FORMAT_INT64, &id) < 0)
    {
        qWarning("Could not set secondary subtitle track to %" PRId64, id);
    }
}

void MpvController::setVid(int64_t id)
{
    if (::mpv_set_property_async(handle(), 0, "vid", MPV_FORMAT_INT64, &id) < 0)
    {
        qWarning("Could not set video track to %" PRId64, id);
    }
}

void MpvController::setSubtitleVisibility(bool visible)
{
    int flag = visible ? 1 : 0;
    if (::mpv_set_property_async(handle(), 0, "sub-visibility", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qWarning("Could not set subtitle visibility");
    }
}

void MpvController::setSecondarySubtitleVisibility(bool visible)
{
    int flag = visible ? 1 : 0;
    if (::mpv_set_property_async(handle(), 0, "secondary-sub-visibility", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qWarning("Could not set secondary subtitle visibility");
    }
}

void MpvController::setSubtitleDelay(double delay)
{
    if (::mpv_set_property_async(handle(), 0, "sub-delay", MPV_FORMAT_DOUBLE, &delay) < 0)
    {
        qWarning("Could not set subtitle delay to %lf", delay);
    }
}

void MpvController::setSecondarySubtitleDelay(double delay)
{
    if (::mpv_set_property_async(handle(), 0, "secondary-sub-delay", MPV_FORMAT_DOUBLE, &delay) < 0)
    {
        qWarning("Could not set secondary subtitle delay to %lf", delay);
    }
}

void MpvController::setFullscreen(bool value)
{
    int flag = value ? 1 : 0;
    if (mpv_set_property_async(handle(), 0, "fullscreen", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qWarning("Could not set fullscreen");
    }
}

void MpvController::setVolume(int64_t value)
{
    if (::mpv_set_property_async(handle(), 0, "volume", MPV_FORMAT_INT64, &value) < 0)
    {
        qWarning("Could not set volume to %" PRId64, value);
    }
}

void MpvController::showText(const QString &text)
{
    QByteArray utf8Text = text.toUtf8();
    const char *command[] = {
        "show-text",
        utf8Text.data(),
        NULL
    };
    if (::mpv_command_async(handle(), 0, command))
    {
        qWarning("Could not show text '%s'", qUtf8Printable(text));
    }
}

void MpvController::sendKeyPress(int key, int modifiers)
{
    QString keypress = toModifierString(modifiers);
#if !defined(Q_OS_MACOS)
    if (modifiers & Qt::KeypadModifier)
    {
        switch (key)
        {
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
            keypress += "KP";
            break;
        case Qt::Key_Delete:
        case Qt::Key_Insert:
        case Qt::Key_Home:
        case Qt::Key_End:
        case Qt::Key_Clear:
        case Qt::Key_PageUp:
        case Qt::Key_PageDown:
        case Qt::Key_Right:
        case Qt::Key_Left:
        case Qt::Key_Down:
        case Qt::Key_Up:
        case Qt::Key_Enter:
            keypress += "KP_";
            break;
        }
    }
#endif // !defined(Q_OS_MACOS)

    if (key >= Qt::Key::Key_F1 && key <= Qt::Key::Key_F30)
    {
        keypress += "F" + QString::number(key - Qt::Key::Key_F1 + 1);
    }
    else
    {
        switch (key)
        {
        case Qt::Key::Key_Shift:
        case Qt::Key::Key_Control:
        case Qt::Key::Key_Alt:
        case Qt::Key::Key_Meta:
            return;
        case Qt::Key::Key_Left:
            keypress += "LEFT";
            break;
        case Qt::Key::Key_Right:
            keypress += "RIGHT";
            break;
        case Qt::Key::Key_Up:
            keypress += "UP";
            break;
        case Qt::Key::Key_Down:
            keypress += "DOWN";
            break;
        case Qt::Key::Key_Enter:
        case Qt::Key::Key_Return:
            keypress += "ENTER";
            break;
        case Qt::Key::Key_Escape:
            keypress += "ESC";
            break;
        case Qt::Key::Key_Backspace:
            keypress += "BS";
            break;
        case Qt::Key::Key_Pause:
            keypress += "PAUSE";
            break;
        case Qt::Key::Key_Delete:
            keypress += "DEL";
            break;
        case Qt::Key::Key_Insert:
            keypress += "INS";
            break;
        case Qt::Key::Key_Home:
            keypress += "HOME";
            break;
        case Qt::Key::Key_End:
            keypress += "END";
            break;
        case Qt::Key::Key_Clear:
            keypress += "DEC";
            break;
        case Qt::Key::Key_PageUp:
            keypress += "PGUP";
            break;
        case Qt::Key::Key_PageDown:
            keypress += "PGDWN";
            break;
        default:
        {
            QString text{QKeySequence(key).toString()};
            keypress += modifiers & Qt::ShiftModifier ?
                text : text.toLower();
        }
        }
    }

    QByteArray keypressUtf8 = keypress.toUtf8();
    const char *args[]{
        "keypress",
        keypressUtf8,
        nullptr
    };
    if (::mpv_command_async(handle(), 0, args) < 0)
    {
        qWarning("Could not send keypress for '%s'", qUtf8Printable(keypress));
    }
}

void MpvController::sendMouse(double x, double y)
{
    QByteArray xArg = QString::number(static_cast<int>(x)).toUtf8();
    QByteArray yArg = QString::number(static_cast<int>(y)).toUtf8();
    const char *args[]{
        "mouse",
        xArg,
        yArg,
        nullptr
    };
    if (::mpv_command_async(handle(), 0, args) < 0)
    {
        qWarning("Could not send mouse movement");
    }
}

void MpvController::sendMouseButton(
    double x, double y, Qt::MouseButton button, bool single)
{
    QByteArray buttonArg;
    switch (button)
    {
    case Qt::MouseButton::LeftButton:
        buttonArg = "0";
        break;
    case Qt::MouseButton::MiddleButton:
        buttonArg = "1";
        break;
    case Qt::MouseButton::RightButton:
        buttonArg = "2";
        break;
    case Qt::MouseButton::BackButton:
        buttonArg = "7";
        break;
    case Qt::MouseButton::ForwardButton:
        buttonArg = "8";
        break;
    default:
        return;
    }
    QByteArray xArg = QString::number(static_cast<int>(x)).toUtf8();
    QByteArray yArg = QString::number(static_cast<int>(y)).toUtf8();
    const char *mouseArgs[]{
        "mouse",
        xArg,
        yArg,
        buttonArg,
        single ? "single" : "double",
        nullptr
    };
    if (::mpv_command_async(handle(), 0, mouseArgs) < 0)
    {
        qWarning("Could not send mouse button");
    }
}

void MpvController::sendWheel(double x, double y, QPoint angleDelta)
{
    QByteArray buttonArg;
    if (angleDelta.y() > 0)
    {
        buttonArg = "3";
    }
    else if (angleDelta.y() < 0)
    {
        buttonArg = "4";
    }
    else if (angleDelta.x() < 0)
    {
        buttonArg = "5";
    }
    else if (angleDelta.x() > 0)
    {
        buttonArg = "6";
    }
    else
    {
        return;
    }
    QByteArray xArg = QString::number(static_cast<int>(x)).toUtf8();
    QByteArray yArg = QString::number(static_cast<int>(y)).toUtf8();
    const char *mouseArgs[]{
        "mouse",
        xArg,
        yArg,
        buttonArg,
        nullptr
    };
    if (::mpv_command_async(handle(), 0, mouseArgs) < 0)
    {
        qWarning("Could not send mouse button");
    }
}


QString MpvController::tempScreenshot(bool subtitles, const QString &ext)
{
    /* Create a valid temporary file name */
    QTemporaryFile file;
    if (!file.open())
    {
        return {};
    }
    QByteArray filename = (file.fileName() + ext).toUtf8();
    file.close();

    const char *args[] = {
        "screenshot-to-file",
        filename,
        subtitles ? NULL : "video",
        NULL
    };
    if (mpv_command(handle(), args) < 0)
    {
        qWarning("Could not take temporary screenshot");
        return {};
    }

    return filename;
}

QImage MpvController::screenshotRaw(bool subtitles)
{
    const char *args[] = {
        "screenshot-raw",
        subtitles ? "subtitles" : "video",
        "rgba",
        NULL,
    };

    mpv_node result{};
    int ret = ::mpv_command_ret(handle(), args, &result);
    if (ret < 0)
    {
        qWarning("Could not take raw screenshot: %s", ::mpv_error_string(ret));
        return {};
    }

    QImage image;
    auto cleanup = qScopeGuard(
        [&result] { ::mpv_free_node_contents(&result); }
    );

    std::optional<int64_t> width = mapInt(result, "w");
    std::optional<int64_t> height = mapInt(result, "h");
    std::optional<int64_t> stride = mapInt(result, "stride");
    QString format = mapString(result, "format");
    const mpv_byte_array *data = mapByteArray(result, "data");
    if (!width ||
        !height ||
        !stride ||
        format != "rgba" ||
        data == nullptr ||
        data->data == nullptr)
    {
        qWarning("Raw screenshot result had an unexpected format");
        return {};
    }
    if (*width <= 0 ||
        *height <= 0 ||
        *width > std::numeric_limits<int>::max() ||
        *height > std::numeric_limits<int>::max())
    {
        qWarning("Raw screenshot result had invalid dimensions");
        return {};
    }

    constexpr int BYTES_PER_PIXEL = 4;
    const int64_t minimumLineSize = *width * BYTES_PER_PIXEL;
    if (std::llabs(*stride) < minimumLineSize)
    {
        qWarning("Raw screenshot result had an invalid stride");
        return {};
    }

    image = QImage(
        static_cast<int>(*width),
        static_cast<int>(*height),
        QImage::Format_RGBA8888
    );
    if (image.isNull())
    {
        qWarning("Could not allocate raw screenshot image");
        return {};
    }

    const char *source = static_cast<const char *>(data->data);
    for (int y = 0; y < image.height(); ++y)
    {
        std::memcpy(
            image.scanLine(y),
            source + y * *stride,
            static_cast<size_t>(minimumLineSize)
        );
    }

    return image;
}

QString MpvController::tempAudioClip(const MpvAudioClipArgs &args)
{
    int64_t aid = player()->state()->aid();
    if (aid == -1)
    {
        return {};
    }

    QByteArray argString = QString("start=%1,end=%2,aid=%3")
        .arg(args.start, 0, 'f', 3)
        .arg(args.end, 0, 'f', 3)
        .arg(aid)
        .toUtf8();

    QList<QPair<QByteArray, QByteArray>> options = {
        {"vid", "no"},
        {"aid", "yes"},
        {"sid", "no"},
        {"secondary-sid", "no"},
    };
    if (args.normalize)
    {
        QByteArray audioFilter;
        audioFilter = "loudnorm=I=";
        audioFilter += QByteArray::number(args.db, 'f', 1);
        options.emplaceBack("af", std::move(audioFilter));
    }

    return encodeFile(argString, options, args.extension);
}

QString MpvController::tempVideoClip(const MpvVideoClipArgs &args)
{
    constexpr const char *FILE_EXTENSION = ".mp4";

    QByteArray input = player()->state()->path().toUtf8();
    QByteArray argString = QString("ovc=libx264,oac=aac,start=%1,end=%2")
        .arg(args.start, 0, 'f', 3)
        .arg(args.end, 0, 'f', 3)
        .toUtf8();
    if (args.audio)
    {
        int64_t aid = player()->state()->aid();
        if (aid == -1)
        {
            return {};
        }
        argString += QString(",aid=%1").arg(aid).toUtf8();
    }
    if (args.subtitles)
    {
        int64_t sid = player()->state()->sid();
        if (sid == -1)
        {
            return {};
        }
        argString += QString(",sid=%1").arg(sid).toUtf8();
    }

    QList<QPair<QByteArray, QByteArray>> options = {
        {"vid", "yes"},
        {"aid", args.audio ? "yes" : "no"},
        {"sid", args.subtitles ? "yes" : "no"},
        {"secondary-sid", "no"},
    };
    if (args.normalize)
    {
        options.emplaceBack(
            "af",
            QString("loudnorm=I=%1").arg(args.db, 'f', 1).toUtf8()
        );
    }

    return encodeFile(argString, options, FILE_EXTENSION);
}

/* End Public Functions */
/* Begin Private Functions */

mpv_handle *MpvController::handle() const noexcept
{
    return player()->handle();
}

QString MpvController::quoteArg(QString arg)
{
    qsizetype eqidx = arg.indexOf('=');
    if (eqidx == -1)
    {
        return arg;
    }
    return arg.insert(eqidx + 1, "\"").append("\"");
}

bool MpvController::argsValid(const QStringList &args)
{
    qsizetype count = 0;
    for (qsizetype i = 1; i < args.size(); ++i)
    {
        if (args[i] == "--{")
        {
            count++;
        }
        else if (args[i] == "--}")
        {
            count--;
            if (count < 0)
            {
                return false;
            }
        }
    }

    return count == 0;
}

qsizetype MpvController::buildArgsTree(
    const QStringList &args, qsizetype i, LoadFileNode &parent, qsizetype depth)
{
    if (depth < 1)
    {
        return -1;
    }

    while (i < args.size())
    {
        if (args[i] == "--}")
        {
            return i;
        }
        else if (args[i] == "--{")
        {
            parent.files.emplaceBack("--"); // Used to indicate a level deeper
            parent.children.emplaceBack();
            LoadFileNode &child = parent.children.last();
            i = buildArgsTree(args, i + 1, child, depth - 1);
            if (i < 0)
            {
                return -1;
            }
        }
        else if (args[i].startsWith("--"))
        {
            parent.options.emplaceBack(quoteArg(args[i].mid(2)));
        }
        else
        {
            parent.files.emplaceBack(args[i]);
        }
        ++i;
    }

    return i;
}

void MpvController::loadFilesFromTree(
    const LoadFileNode &parent, QStringList &options)
{
    qsizetype lastSize = options.size();
    options.append(parent.options);

    qsizetype currentChild = 0;
    for (const QString &file : parent.files)
    {
        if (file == "--")
        {
            loadFilesFromTree(parent.children[currentChild], options);
            currentChild++;
        }
        else
        {
            loadFile(file, true, options);
        }
    }

    while (options.size() > lastSize)
    {
        options.removeLast();
    }
}

bool MpvController::isSubtitleFile(const QString &file) const
{
    QString ext =
        file.right(file.size() - file.lastIndexOf('.') - 1).toLower();
    return m_subtitleExtensions.contains(ext);
}

QString MpvController::toModifierString(int modifiers)
{
    QString keypress;
    if (modifiers & Qt::ShiftModifier)
    {
        keypress += "Shift+";
    }
    if (modifiers & Qt::ControlModifier)
    {
        keypress += "Ctrl+";
    }
    if (modifiers & Qt::AltModifier)
    {
        keypress += "Alt+";
    }
    if (modifiers & Qt::MetaModifier)
    {
        keypress += "Meta+";
    }
    return keypress;
}

QString MpvController::encodeFile(
    const QByteArray &argString,
    const QList<QPair<QByteArray, QByteArray>> &options,
    const QString &fileExtension)
{
    /* Create a valid temporary file name */
    QTemporaryFile file;
    if (!file.open())
    {
        return {};
    }
    QByteArray filename = (file.fileName() + fileExtension).toUtf8();
    file.close();

    QByteArray input = player()->state()->path().toUtf8();
    const char *argOpts = argString.isEmpty() ? NULL : argString.data();

    bool isApi23 = mpv_client_api_version() >= MPV_MAKE_VERSION(2, 3);
    const char *args[] = {
        "loadfile",
        input,
        "replace",
        isApi23 ? "0"     : argOpts,
        isApi23 ? argOpts : NULL,
        NULL
    };

    char *script_opts = NULL;
    mpv_event *event = NULL;
    mpv_handle *enc_h = mpv_create();
    if (enc_h == NULL)
    {
        qWarning("Error creating encoder handle");
        filename = "";
        goto cleanup;
    }

    for (const auto &[k, v] : options)
    {
        ::mpv_set_option_string(enc_h, k, v);
    }
    ::mpv_set_option_string(enc_h, "cover-art-auto", "no");
    ::mpv_set_option_string(enc_h, "keep-open", "no");
    ::mpv_set_option_string(enc_h, "ytdl", "yes");
    ::mpv_set_option_string(enc_h, "config", "no");
    ::mpv_set_option_string(enc_h, "o", filename);

    /* This guarantees the correct version of youtube-dl is used. */
    script_opts = ::mpv_get_property_string(handle(), "script-opts");
    if (script_opts && QByteArray(script_opts).contains("ytdl_hook-ytdl_path="))
    {
        ::mpv_set_option_string(enc_h, "script-opts", script_opts);
    }
#if MEMENTO_BUNDLE
    else
    {
        QByteArray ytdlPath = "ytdl_hook-ytdl_path=";
        char *config_dir = ::mpv_get_property_string(handle(), "config-dir");
        if (config_dir)
        {
            ytdlPath += config_dir;
            ytdlPath += '/';
        }
        else
        {
            ytdlPath += DirectoryUtils::getConfigDir().toUtf8();
        }
        ::mpv_free(config_dir);
        ytdlPath += "youtube-dl";
        ::mpv_set_option_string(enc_h, "script-opts", ytdlPath);
    }
#endif // MEMENTO_BUNDLE
    ::mpv_free(script_opts);

    if (::mpv_initialize(enc_h) < 0)
    {
        qWarning("Could not initialize encoder");
        filename = "";
        goto cleanup;
    }

    if (::mpv_command(enc_h, args) < 0)
    {
        qWarning("Could not encode file");
        filename = "";
        goto cleanup;
    }

    do
    {
        event = ::mpv_wait_event(enc_h, 100);
        if (event->event_id == MPV_EVENT_NONE ||
            event->event_id == MPV_EVENT_QUEUE_OVERFLOW)
        {
            qWarning()
                << QString("mpv returned a bad event: %1").arg(event->event_id);
            filename = "";
            goto cleanup;
        }
    }
    while (event->event_id != MPV_EVENT_END_FILE);

cleanup:
    ::mpv_destroy(enc_h);

    return filename;
}

const mpv_node *MpvController::mapValue(const mpv_node &node, const char *key)
{
    if (node.format != MPV_FORMAT_NODE_MAP || node.u.list == nullptr)
    {
        return nullptr;
    }

    mpv_node_list *list = node.u.list;
    for (int i = 0; i < list->num; ++i)
    {
        if (std::strcmp(list->keys[i], key) == 0)
        {
            return &list->values[i];
        }
    }
    return nullptr;
}

std::optional<int64_t> MpvController::mapInt(
    const mpv_node &node, const char *key)
{
    const mpv_node *value = mapValue(node, key);
    if (value == nullptr || value->format != MPV_FORMAT_INT64)
    {
        return std::nullopt;
    }
    return value->u.int64;
}

QString MpvController::mapString(const mpv_node &node, const char *key)
{
    const mpv_node *value = mapValue(node, key);
    if (value == nullptr || value->format != MPV_FORMAT_STRING)
    {
        return {};
    }
    return QString::fromUtf8(value->u.string);
}

const mpv_byte_array *MpvController::mapByteArray(
    const mpv_node &node, const char *key)
{
    const mpv_node *value = mapValue(node, key);
    if (value == nullptr || value->format != MPV_FORMAT_BYTE_ARRAY)
    {
        return nullptr;
    }
    return value->u.ba;
}

/* End Private Functions */
