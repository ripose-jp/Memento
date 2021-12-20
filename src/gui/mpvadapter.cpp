////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#include "mpvadapter.h"

#include <QApplication>
#include <QDebug>
#include <QTemporaryFile>

#include "../util/globalmediator.h"

MpvAdapter::MpvAdapter(MpvWidget *mpv, QObject *parent)
    : m_mpv(mpv),
      m_handle(mpv->getHandle()),
      PlayerAdapter(parent)
{
    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    GlobalMediator::getGlobalMediator()->setPlayerAdapter(this);

    m_subExts << "srt"
              << "ass"
              << "ssa"
              << "ttml"
              << "sbv"
              << "dfxp"
              << "vtt"
              << "txt";

    /* Signals */
    connect(
        m_mpv, &MpvWidget::tracklistChanged, this,
        [=] (const mpv_node *node) {
            QList<const Track *> tracks = processTracks(node);
            Q_EMIT mediator->playerTracksChanged(tracks);
            for (const Track *track : tracks)
            {
                delete track;
            }
        }
    );
    connect(
        m_mpv, &MpvWidget::chaptersChanged,
        mediator, &GlobalMediator::playerChaptersChanged
    );

    connect(
        m_mpv,    &MpvWidget::audioTrackChanged,
        mediator, &GlobalMediator::playerAudioTrackChanged
    );
    connect(
        m_mpv,    &MpvWidget::videoTrackChanged,
        mediator, &GlobalMediator::playerVideoTrackChanged
    );
    connect(
        m_mpv,    &MpvWidget::subtitleTrackChanged,
        mediator, &GlobalMediator::playerSubtitleTrackChanged
    );
    connect(
        m_mpv,    &MpvWidget::subtitleTwoTrackChanged,
        mediator, &GlobalMediator::playerSecondSubtitleTrackChanged
    );

    connect(
        m_mpv,    &MpvWidget::audioDisabled,
        mediator, &GlobalMediator::playerAudioDisabled
    );
    connect(
        m_mpv,    &MpvWidget::videoDisabled,
        mediator, &GlobalMediator::playerVideoDisabled
    );
    connect(
        m_mpv,    &MpvWidget::subtitleDisabled,
        mediator, &GlobalMediator::playerSubtitlesDisabled
    );
    connect(
        m_mpv,    &MpvWidget::subtitleTwoDisabled,
        mediator, &GlobalMediator::playerSecondSubtitlesDisabled
    );

    connect(
        m_mpv,    &MpvWidget::subtitleChanged,
        mediator, &GlobalMediator::playerSubtitleChanged
    );
    connect(
        m_mpv,    &MpvWidget::subtitleChangedSecondary,
        mediator, &GlobalMediator::playerSecSubtitleChanged
    );
    connect(
        m_mpv,    &MpvWidget::subDelayChanged,
        mediator, &GlobalMediator::playerSubDelayChanged
    );
    connect(
        m_mpv,    &MpvWidget::durationChanged,
        mediator, &GlobalMediator::playerDurationChanged
    );
    connect(
        m_mpv,    &MpvWidget::positionChanged,
        mediator, &GlobalMediator::playerPositionChanged
    );
    connect(
        m_mpv,    &MpvWidget::pauseChanged,
        mediator, &GlobalMediator::playerPauseStateChanged
    );
    connect(
        m_mpv,    &MpvWidget::fullscreenChanged,
        mediator, &GlobalMediator::playerFullscreenChanged
    );
    connect(
        m_mpv,    &MpvWidget::volumeChanged,
        mediator, &GlobalMediator::playerVolumeChanged
    );
    connect(
        m_mpv,    &MpvWidget::volumeMaxChanged,
        mediator, &GlobalMediator::playerMaxVolumeChanged
    );
    connect(
        m_mpv,    &MpvWidget::titleChanged,
        mediator, &GlobalMediator::playerTitleChanged
    );
    connect(
        m_mpv,    &MpvWidget::fileChanged,
        mediator, &GlobalMediator::playerFileChanged
    );

    connect(
        m_mpv,    &MpvWidget::cursorHidden,
        mediator, &GlobalMediator::playerCursorHidden
    );
    connect(
        m_mpv,    &MpvWidget::mouseMoved,
        mediator, &GlobalMediator::playerMouseMoved
    );
    connect(
        m_mpv,    &MpvWidget::shutdown,
        mediator, &GlobalMediator::playerClosed
    );
    connect(
        m_mpv,    &MpvWidget::newFileLoaded,
        mediator, &GlobalMediator::playerFileLoaded
    );

    /* Slots */
    connect(
        mediator, &GlobalMediator::controlsPlay,
        this,     &PlayerAdapter::play
    );
    connect(
        mediator, &GlobalMediator::controlsPause,
        this,     &PlayerAdapter::pause
    );
    connect(
        mediator, &GlobalMediator::controlsPositionChanged,
        this,     &PlayerAdapter::seek
    );
    connect(
        mediator, &GlobalMediator::controlsSkipForward,
        this,     &PlayerAdapter::skipForward
    );
    connect(
        mediator, &GlobalMediator::controlsSkipBackward,
        this,     &PlayerAdapter::skipBackward
    );
    connect(
        mediator, &GlobalMediator::controlsSeekForward,
        this,     &PlayerAdapter::seekForward
    );
    connect(
        mediator, &GlobalMediator::controlsSeekBackward,
        this,     &PlayerAdapter::seekBackward
    );
    connect(
        mediator, &GlobalMediator::controlsFullscreenChanged,
        this,     &PlayerAdapter::setFullscreen
    );
    connect(
        mediator, &GlobalMediator::controlsVolumeChanged,
        this,     &PlayerAdapter::setVolume
    );

    connect(
        mediator, &GlobalMediator::keyPressed,
        this,     &PlayerAdapter::keyPressed
    );
    connect(
        mediator, &GlobalMediator::wheelMoved,
        this,     &PlayerAdapter::mouseWheelMoved
    );

    connect(
        mediator, &GlobalMediator::requestSetSubtitleVisibility,
        this,     &PlayerAdapter::setSubVisiblity
    );
}

QString MpvAdapter::quoteArg(QString arg) const
{
    int eqidx = arg.indexOf("=");
    if (eqidx == -1)
    {
        return arg;
    }
    return arg.insert(eqidx + 1, "\"").append("\"");
}

bool MpvAdapter::commandLineArgsValid(const QStringList &args) const
{
    int count = 0;
    for (size_t i = 1; i < args.size(); ++i)
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

int MpvAdapter::buildArgsTree(const QStringList &args,
                              int i,
                              struct LoadFileNode &parent,
                              int depth) const
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
            parent.files << "--"; // Used as a marker to indicate a level deeper
            parent.children << LoadFileNode();
            struct LoadFileNode &child = parent.children.last();
            i = buildArgsTree(args, i + 1, child, depth - 1);
            if (i < 0)
            {
                return -1;
            }
        }
        else if (args[i].startsWith("--"))
        {
            parent.options << quoteArg(args[i].mid(2));
        }
        else
        {
            parent.files << args[i];
        }

        ++i;
    }

    return i;
}

void MpvAdapter::loadFilesFromTree(const struct LoadFileNode &parent,
                                                QStringList  &options)
{
    int lastSize = options.size();
    options.append(parent.options);

    size_t currentChild = 0;
    for (const QString &file : parent.files)
    {
        if (file == "--")
        {
            loadFilesFromTree(parent.children[currentChild], options);
            currentChild++;
        }
        else
        {
            open(file, true, options);
        }
    }

    while (options.size() > lastSize)
    {
        options.removeLast();
    }
}

void MpvAdapter::loadCommandLineArgs()
{
    QStringList args = QApplication::arguments();
    if (!commandLineArgsValid(args))
    {
        qDebug() << "Invalid command line arguments.";
        return;
    }
    struct LoadFileNode parent;
    if (buildArgsTree(args, 1, parent) == -1)
    {
        qDebug() << "Maximum number of nested per-file arguments exceeded.";
        return;
    }
    QStringList emptyOpts;
    loadFilesFromTree(parent, emptyOpts);
}

int64_t MpvAdapter::getMaxVolume() const
{
    int64_t max;
    if (mpv_get_property(m_handle, "volume-max", MPV_FORMAT_INT64, &max) < 0)
    {
        qDebug() << "Could not get volume-max property";
        return 0;
    }
    return max;
}

double MpvAdapter::getTimePos() const
{
    double timePos;
    if (mpv_get_property(m_handle, "time-pos", MPV_FORMAT_DOUBLE, &timePos) < 0)
    {
        qDebug() << "Could not get time-pos property";
        return 0;
    }
    return timePos;
}

double MpvAdapter::getSubStart() const
{
    double start;
    if (mpv_get_property(m_handle, "sub-start", MPV_FORMAT_DOUBLE, &start) < 0)
    {
        qDebug() << "Could not get sub-start property";
        return 0;
    }
    return start;
}

double MpvAdapter::getSubEnd() const
{
    double end;
    if (mpv_get_property(m_handle, "sub-end", MPV_FORMAT_DOUBLE, &end) < 0)
    {
        qDebug() << "Could not get sub-end property";
        return 0;
    }
    return end;
}

double MpvAdapter::getSubDelay() const
{
    double delay;
    if (mpv_get_property(m_handle, "sub-delay", MPV_FORMAT_DOUBLE, &delay) < 0)
    {
        qDebug() << "Could not get sub-delay property";
        return 0;
    }
    return delay;
}

QString MpvAdapter::getSecondarySubtitle() const
{
    QString subtitle;
    char *sub = mpv_get_property_string(m_handle, "secondary-sub-text");
    if (sub)
    {
        subtitle = QString::fromUtf8(sub);
    }
    mpv_free(sub);
    return subtitle;
}

bool MpvAdapter::getSubVisibility() const
{
    int flag;
    if (mpv_get_property(m_handle, "sub-visibility", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qDebug() << "Could not get mpv sub-visibility property";
        return 0;
    }
    return flag;
}

double MpvAdapter::getAudioDelay() const
{
    double delay;
    if (mpv_get_property(m_handle, "audio-delay", MPV_FORMAT_DOUBLE, &delay) < 0)
    {
        qDebug() << "Could not get mpv delay property";
        return 0;
    }
    return delay;
}

QList<const Track *> MpvAdapter::getTracks()
{
    mpv_node node;
    if (mpv_get_property(m_handle, "track-list", MPV_FORMAT_NODE, &node) < 0)
    {
        qDebug() << "Could not get track-list property";
        return QList<const Track *>();
    }
    QList<const Track *> tracks = processTracks(&node);
    mpv_free_node_contents(&node);
    return tracks;
}

int64_t MpvAdapter::getAudioTrack() const
{
    int64_t track = 0;
    if (mpv_get_property(m_handle, "aid", MPV_FORMAT_INT64, &track) < 0)
    {
        return 0;
    }
    return track;
}

int64_t MpvAdapter::getSubtitleTrack() const
{
    int64_t track = 0;
    if (mpv_get_property(m_handle, "sid", MPV_FORMAT_INT64, &track) < 0)
    {
        return 0;
    }
    return track;
}

int64_t MpvAdapter::getSecondarySubtitleTrack() const
{
    int64_t track = 0;
    if (mpv_get_property(m_handle, "secondary-sid", MPV_FORMAT_INT64, &track) < 0)
    {
        return 0;
    }
    return track;
}

QString MpvAdapter::getPath() const
{
    char *path = NULL;
    if (mpv_get_property(m_handle, "path", MPV_FORMAT_STRING, &path) < 0)
    {
        qDebug() << "Could not get mpv path property";
        return "";
    }
    QString path_str(path);
    mpv_free(path);
    return path_str;
}

QString MpvAdapter::getTitle() const
{
    char *title = NULL;
    if (mpv_get_property(m_handle, "media-title", MPV_FORMAT_STRING, &title) < 0)
    {
        qDebug() << "Could not get mpv media-title property";
        return "";
    }
    QString title_str(title);
    mpv_free(title);
    return title_str;
}

bool MpvAdapter::isFullscreen() const
{
    int flag;
    if (mpv_get_property(m_handle, "fullscreen", MPV_FORMAT_FLAG, &flag))
    {
        qDebug() << "Could not get mpv property fullscreen";
        return false;
    }
    return flag;
}

bool MpvAdapter::isPaused() const
{
    int flag;
    if (mpv_get_property(m_handle, "pause", MPV_FORMAT_FLAG, &flag))
    {
        qDebug() << "Could not get mpv property pause";
        return false;
    }
    return flag;
}

bool MpvAdapter::canGetSecondarySubText() const
{
    char *str = NULL;
    int res = mpv_get_property(
        m_handle, "secondary-sub-text", MPV_FORMAT_STRING, &str
    );
    mpv_free(str);
    return res != MPV_ERROR_PROPERTY_NOT_FOUND;
}

void MpvAdapter::open(const QString     &file,
                      const bool         append,
                      const QStringList &options)
{
    if (file.isEmpty())
    {
        return;
    }

    QByteArray filename = file.toUtf8();
    QByteArray opts = options.join(',').toUtf8();

    const char *args[5] = {
        "loadfile",
        filename,
        append ? "append-play" : "replace",
        opts,
        NULL
    };

    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not open file" << file;
    }
}

bool MpvAdapter::open(const QList<QUrl> &files)
{
    if (files.isEmpty())
    {
        return false;
    }

    bool firstFile = true;

    for (const auto &file : files)
    {
        const QString &path = file.toLocalFile();
        QString ext =
            path.right(path.size() - path.lastIndexOf('.') - 1).toLower();
        if (m_subExts.contains(ext))
        {
            addSubtitle(path);
        }
        else
        {
            if (firstFile)
            {
                stop();
            }
            open(path, !firstFile);
            firstFile = false;
        }
    }

    return !firstFile;
}

void MpvAdapter::addSubtitle(const QString &file)
{
    if (file.isEmpty())
        return;

    QByteArray fileName = file.toUtf8();

    const char *args[3] = {
        "sub-add",
        fileName,
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not add subtitle file" << file;
    }
}

void MpvAdapter::seek(const double time)
{
    QByteArray timestr = QString::number(time).toUtf8();
    const char *args[4] = {
        "seek",
        timestr,
        "absolute",
        NULL
    };
    if (mpv_command_async(m_handle, -1, args) < 0)
    {
        qDebug() << "Seeking failed";
    }
}

void MpvAdapter::play()
{
    int flag = 0;
    if (mpv_set_property(m_handle, "pause", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qDebug() << "Could not set mpv property pause to false";
    }
}

void MpvAdapter::pause()
{
    int flag = 1;
    if (mpv_set_property(m_handle, "pause", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qDebug() << "Could not set mpv property pause to true";
    }
}

void MpvAdapter::stop()
{
    const char *args[2] = {
        "stop",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not stop mpv";
    }
}

void MpvAdapter::seekForward()
{
    const char *args[3] = {
        "sub-seek",
        "1",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not seek the next subtitle";
    }
}

void MpvAdapter::seekBackward()
{
    const char *args[3] = {
        "sub-seek",
        "-1",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not seek the last subtitle";
    }
}

void MpvAdapter::skipForward()
{
    const char *args[2] = {
        "playlist-next",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not skip to the next file in the playlist";
    }
}

void MpvAdapter::skipBackward()
{
    const char *args[2] = {
        "playlist-prev",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not skip to the next file in the playlist";
    }
}

void MpvAdapter::disableAudio()
{
    const char *value = "no";
    if (mpv_set_property(m_handle, "aid", MPV_FORMAT_STRING, &value) < 0)
    {
        qDebug() << "Could not set mpv property aid to no";
    }
}

void MpvAdapter::disableVideo()
{
    const char *value = "no";
    if (mpv_set_property(m_handle, "vid", MPV_FORMAT_STRING, &value) < 0)
    {
        qDebug() << "Could not set mpv property vid to no";
    }
}

void MpvAdapter::disableSubtitles()
{
    const char *value = "no";
    if (mpv_set_property(m_handle, "sid", MPV_FORMAT_STRING, &value) < 0)
    {
        qDebug() << "Could not set mpv property sid to no";
    }
}

void MpvAdapter::disableSubtitleTwo()
{
    const char *value = "no";
    if (mpv_set_property(m_handle, "secondary-sid", MPV_FORMAT_STRING, &value) < 0)
    {
        qDebug() << "Could not set mpv property secondary-sid to no";
    }
}

void MpvAdapter::setSubVisiblity(const bool visible)
{
    int flag = visible ? 1 : 0;
    if (mpv_set_property(m_handle, "sub-visibility", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qDebug() << "Could not set mpv property sub-visibility to" << flag;
    }
}

void MpvAdapter::setAudioTrack(int64_t id)
{
    if (mpv_set_property(m_handle, "aid", MPV_FORMAT_INT64, &id) < 0)
    {
        qDebug() << "Could not set mpv property aid";
    }
}

void MpvAdapter::setVideoTrack(int64_t id)
{
    if (mpv_set_property(m_handle, "vid", MPV_FORMAT_INT64, &id) < 0)
    {
        qDebug() << "Could not set mpv property vid";
    }
}

void MpvAdapter::setSubtitleTrack(int64_t id)
{
    if (mpv_set_property(m_handle, "sid", MPV_FORMAT_INT64, &id) < 0)
    {
        qDebug() << "Could not set mpv property sid";
    }
}

void MpvAdapter::setSubtitleTwoTrack(int64_t id)
{
    if (mpv_set_property(m_handle, "secondary-sid", MPV_FORMAT_INT64, &id) < 0)
    {
        qDebug() << "Could not set mpv property secondary-sid";
    }
}

void MpvAdapter::setFullscreen(bool value)
{
    int flag = value ? 1 : 0;
    if (mpv_set_property(m_handle, "fullscreen", MPV_FORMAT_FLAG, &flag) < 0)
    {
        qDebug() << "Could not set mpv property fullscreen";
    }
}

void MpvAdapter::setVolume(int64_t value)
{
    if (mpv_set_property(m_handle, "volume", MPV_FORMAT_INT64, &value) < 0)
    {
        qDebug() << "Could not set mpv property volume";
    }
}

QString MpvAdapter::tempScreenshot(const bool subtitles, const QString &ext)
{
    // Get a temporary file name
    QTemporaryFile file;
    if (!file.open())
        return "";
    QByteArray filename = (file.fileName() + ext).toUtf8();
    file.close();

    const char *args[] = {
        "screenshot-to-file",
        filename,
        subtitles ? NULL : "video",
        NULL
    };
    if (mpv_command(m_handle, args) < 0)
    {
        qDebug() << "Could not take temporary screenshot";
        return "";
    }

    return filename;
}

QString MpvAdapter::tempAudioClip(double start, double end, const QString &ext)
{
    int64_t aid = getAudioTrack();
    if (aid == -1)
    {
        return "";
    }

    // Get a temporary file name
    QTemporaryFile file;
    if (!file.open())
        return "";
    QByteArray filename = (file.fileName() + ext).toUtf8();
    file.close();

    QByteArray input = getPath().toUtf8();
    QByteArray optionCmd;
    optionCmd += QString("start=%1").arg(start, 0, 'f', 3).toUtf8();
    optionCmd += QString(",end=%1").arg(end, 0, 'f', 3).toUtf8();
    optionCmd += QString(",aid=%1").arg(aid).toUtf8();
    const char *args[] = {
        "loadfile",
        input,
        "replace",
        optionCmd,
        NULL
    };

    char *script_opts = NULL;
    mpv_event *event = NULL;
    mpv_handle *enc_h = mpv_create();
    if (enc_h == NULL)
    {
        qDebug() << "Error creating encoder handle";
        filename = "";
        goto cleanup;
    }

    mpv_set_option_string(enc_h, "cover-art-auto", "no");
    mpv_set_option_string(enc_h, "keep-open", "no");
    mpv_set_option_string(enc_h, "vid", "no");
    mpv_set_option_string(enc_h, "sid", "no");
    mpv_set_option_string(enc_h, "secondary-sid", "no");
    mpv_set_option_string(enc_h, "ytdl", "yes");
    mpv_set_option_string(enc_h, "config", "no");
    mpv_set_option_string(enc_h, "o", filename);

    /* This guarantees the correct version of youtube-dl is used. */
    script_opts = mpv_get_property_string(m_handle, "script-opts");
    if (script_opts && QByteArray(script_opts).contains("ytdl_hook-ytdl_path="))
    {
        mpv_set_option_string(enc_h, "script-opts", script_opts);
    }
#if APPBUNDLE
    else
    {
        QByteArray ytdlPath = "ytdl_hook-ytdl_path=";
        char *config_dir = mpv_get_property_string(m_handle, "config-dir");
        if (config_dir)
        {
            ytdlPath += config_dir;
            ytdlPath += SLASH;
        }
        else
        {
            ytdlPath += DirectoryUtils::getConfigDir().toUtf8();
        }
        mpv_free(config_dir);
        ytdlPath += "youtube-dl";
        mpv_set_option_string(enc_h, "script-opts", ytdlPath);
    }
#endif
    mpv_free(script_opts);

    if (mpv_initialize(enc_h) < 0)
    {
        qDebug() << "Could not initialize encoder";
        filename = "";
        goto cleanup;
    }

    if (mpv_command(enc_h, args) < 0)
    {
        qDebug() << "Could not encode file";
        filename = "";
        goto cleanup;
    }

    do
    {
        event = mpv_wait_event(enc_h, 100);
        if (event->event_id == MPV_EVENT_NONE ||
            event->event_id == MPV_EVENT_QUEUE_OVERFLOW)
        {
            qDebug() << "mpv returned a bad event" << event->event_id;
            filename = "";
            goto cleanup;
        }
    }
    while (event->event_id != MPV_EVENT_END_FILE);

cleanup:
    mpv_destroy(enc_h);

    return filename;
}

void MpvAdapter::keyPressed(const QKeyEvent *event)
{
    QString key = "";
    if (event->modifiers() & Qt::ShiftModifier)
    {
        key += "Shift+";
    }
    if (event->modifiers() & Qt::ControlModifier)
    {
        key += "Ctrl+";
    }
    if (event->modifiers() & Qt::AltModifier)
    {
        key += "Alt+";
    }
    if (event->modifiers() & Qt::MetaModifier)
    {
        key += "Meta+";
    }
#if !__APPLE__
    if (event->modifiers() & Qt::KeypadModifier)
    {
        key += "KP";
    }
#endif

    if (event->key() >= Qt::Key::Key_F1 && event->key() <= Qt::Key::Key_F30)
    {
        key += "F" + QString::number(event->key() - Qt::Key::Key_F1 + 1);
    }
    else
    {
        switch (event->key())
        {
        case Qt::Key::Key_Shift:
        case Qt::Key::Key_Control:
        case Qt::Key::Key_Alt:
        case Qt::Key::Key_Meta:
            return;
        case Qt::Key::Key_Left:
            key += "LEFT";
            break;
        case Qt::Key::Key_Right:
            key += "RIGHT";
            break;
        case Qt::Key::Key_Up:
            key += "UP";
            break;
        case Qt::Key::Key_Down:
            key += "DOWN";
            break;
        case Qt::Key::Key_Enter:
            key += "ENTER";
            break;
        case Qt::Key::Key_Escape:
            key += "ESC";
            break;
        case Qt::Key::Key_PageUp:
            key += "PGUP";
            break;
        case Qt::Key::Key_PageDown:
            key += "PGDWN";
            break;
        case Qt::Key::Key_Backspace:
            key += "BS";
            break;
        default:
        {
            if (event->modifiers() & Qt::ControlModifier)
            {
                if (event->modifiers() & Qt::ShiftModifier)
                    key += QKeySequence(event->key()).toString();
                else
                    key += QKeySequence(event->key()).toString().toLower();
            }
            else
                key = event->text();
        }
        }
    }

    QByteArray keypress = key.toUtf8();
    const char *args[3] = {
        "keypress",
        keypress,
        NULL
    };
    if (mpv_command_async(m_handle, -1, args) < 0)
    {
        qDebug() << "Could not send keypress command for key" << key;
    }
}

void MpvAdapter::mouseWheelMoved(const QWheelEvent *event)
{
    QByteArray direction = "WHEEL_";
    if (event->angleDelta().y() > 0)
    {
        direction += "UP";
    }
    else if (event->angleDelta().y() < 0)
    {
        direction += "DOWN";
    }
    else if (event->angleDelta().x() > 0)
    {
        direction += "RIGHT";
    }
    else if (event->angleDelta().x() < 0)
    {
        direction += "LEFT";
    }

    /* Prevent bad input from being sent to mpv. */
    if (direction == "WHEEL_")
    {
        return;
    }

    const char *args[3] = {
        "keypress",
        direction,
        NULL
    };
    if (mpv_command_async(m_handle, -1, args) < 0)
    {
        qDebug() << "Could not send keypress command for direction"
                 << direction;
    }
}

/* Code modified from loadTracks() */
/* https://github.com/u8sand/Baka-MPlayer/blob/master/src/mpvhandler.cpp */
QList<const Track *> MpvAdapter::processTracks(const mpv_node *node)
{
    QList<const Track *> tracks;
    if (node->format == MPV_FORMAT_NODE_ARRAY)
    {
        for (size_t i = 0; i < node->u.list->num; i++)
        {
            Track *track = new Track;
            if (node->u.list->values[i].format == MPV_FORMAT_NODE_MAP)
            {
                for (size_t n = 0; n < node->u.list->values[i].u.list->num; n++)
                {
                    if (QString(node->u.list->values[i].u.list->keys[n]) == "id")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                            track->id = node->u.list->values[i].u.list->values[n].u.int64;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "type")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                        {
                            QString type = node->u.list->values[i].u.list->values[n].u.string;
                            if (type == "audio")
                                track->type = Track::Type::audio;
                            else if (type == "video")
                                track->type = Track::Type::video;
                            else if (type == "sub")
                                track->type = Track::Type::subtitle;
                        }
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "src-id")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                            track->srcId = node->u.list->values[i].u.list->values[n].u.int64;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "title")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track->title = node->u.list->values[i].u.list->values[n].u.string;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "lang")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track->lang = node->u.list->values[i].u.list->values[n].u.string;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "albumart")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track->albumart = node->u.list->values[i].u.list->values[n].u.flag != 0;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "default")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track->def = node->u.list->values[i].u.list->values[n].u.flag != 0;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "selected")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track->selected = node->u.list->values[i].u.list->values[n].u.flag != 0;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "main-selection")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_INT64)
                            track->mainSelection = node->u.list->values[i].u.list->values[n].u.int64;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "external")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_FLAG)
                            track->external = node->u.list->values[i].u.list->values[n].u.flag != 0;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "external-filename")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track->externalFilename = node->u.list->values[i].u.list->values[n].u.string;
                    }
                    else if (QString(node->u.list->values[i].u.list->keys[n]) == "codec")
                    {
                        if (node->u.list->values[i].u.list->values[n].format == MPV_FORMAT_STRING)
                            track->codec = node->u.list->values[i].u.list->values[n].u.string;
                    }
                }
                tracks.push_back(track);
            }
        }
    }
    return tracks;
}