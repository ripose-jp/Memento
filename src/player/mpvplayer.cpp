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

#include "player/mpvplayer.h"

#include <cstring>
#include <stdexcept>
#include <utility>

#include <QQuickWindow>

#if defined(Q_OS_WIN)
#include "player/mpvd3d11renderer.h"
#elif defined(Q_OS_MACOS)
#include "player/mpvmetalrenderer.h"
#else
#include "player/mpvopenglrenderer.h"
#endif
#include "util/utils.h"

#if defined(Q_OS_WIN)
using PlatformRenderer = MpvD3D11Renderer;
#elif defined(Q_OS_MACOS)
using PlatformRenderer = MpvMetalRenderer;
#else
using PlatformRenderer = MpvOpenGlRenderer;
#endif

/* Begin Constructor/Deconstructor */

MpvPlayer::MpvPlayer(QQuickItem *parent) : QQuickRhiItem(parent)
{
    mpv_handle *mpv = ::mpv_create();
    if (mpv == nullptr)
    {
        throw std::runtime_error("MpvPlayer: Could not create mpv context");
    }
    m_mpv = {mpv, ::mpv_terminate_destroy};

    initPropertyMap();

    connect(
        this, &MpvPlayer::mpvEvent,
        this, &MpvPlayer::handleMpvEvents,
        Qt::QueuedConnection
    );
    ::mpv_set_wakeup_callback(handle(), &MpvPlayer::onMpvEvents, this);
}

MpvPlayer::~MpvPlayer()
{
    ::mpv_set_wakeup_callback(handle(), nullptr, nullptr);
}

/* End Constructor/Deconstructor */
/* Begin Initializers */

void MpvPlayer::initPropertyMap()
{
    m_propertyMap["chapter-list"] = [this] (mpv_event_property *prop)
    {
        if (prop->format != MPV_FORMAT_NODE)
        {
            return;
        }
        mpv_node *node = reinterpret_cast<mpv_node *>(prop->data);

        if (node->format != MPV_FORMAT_NODE_ARRAY)
        {
            return;
        }
        mpv_node_list *arr = node->u.list;

        QList<double> chapters;
        for (int i = 0; i < arr->num; ++i)
        {
            if (arr->values[i].format != MPV_FORMAT_NODE_MAP)
            {
                continue;
            }
            mpv_node_list *map = arr->values[i].u.list;
            for (int j = 0; j < map->num; ++j)
            {
                if (map->values[j].format == MPV_FORMAT_DOUBLE &&
                    std::strcmp(map->keys[j], "time") == 0)
                {
                    chapters.emplaceBack(map->values[j].u.double_);
                }
            }
        }
        state()->setChapters(std::move(chapters));
    };

    m_propertyMap["duration"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_DOUBLE)
        {
            double time = *reinterpret_cast<double *>(prop->data);
            state()->setDuration(time);
        }
    };

    m_propertyMap["fullscreen"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_FLAG)
        {
            bool full = !!*reinterpret_cast<int *>(prop->data);
            state()->setFullscreen(full);
        }
    };

    m_propertyMap["media-title"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_STRING)
        {
            const char **name = reinterpret_cast<const char **>(prop->data);
            state()->setTitle(*name);
        }
    };

    m_propertyMap["path"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_STRING)
        {
            const char **path = reinterpret_cast<const char **>(prop->data);
            state()->setPath(*path);
        }
    };

    m_propertyMap["pause"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_FLAG)
        {
            bool paused = !!*reinterpret_cast<int *>(prop->data);
            state()->setPause(paused);
        }
    };

    m_propertyMap["time-pos"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_DOUBLE)
        {
            double time = *reinterpret_cast<double *>(prop->data);
            state()->setTimePosition(time);
        }
    };

    m_propertyMap["track-list/count"] = [this] (mpv_event_property *)
    {
        mpv_node node;
        mpv_get_property(handle(), "track-list", MPV_FORMAT_NODE, &node);
        state()->setTracks(&node);
        mpv_free_node_contents(&node);
    };

    m_propertyMap["volume"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_INT64)
        {
            int64_t volume = *reinterpret_cast<int64_t *>(prop->data);
            state()->setVolume(volume);
        }
    };

    m_propertyMap["volume-max"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_INT64)
        {
            int64_t volume = *reinterpret_cast<int64_t *>(prop->data);
            state()->setMaxVolume(volume);
        }
    };

    m_propertyMap["video-params/w"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_INT64)
        {
            int64_t width = *reinterpret_cast<int64_t *>(prop->data);
            state()->setVideoWidth(width);
        }
    };

    m_propertyMap["video-params/h"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_INT64)
        {
            int64_t height = *reinterpret_cast<int64_t *>(prop->data);
            state()->setVideoHeight(height);
        }
    };

    m_propertyMap["aid"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_INT64)
        {
            int64_t id = *reinterpret_cast<int64_t *>(prop->data);
            state()->setAid(id);
        }
        else if (prop->format == MPV_FORMAT_FLAG)
        {
            bool enabled = !!*reinterpret_cast<int *>(prop->data);
            if (!enabled)
            {
                state()->setAid(0);
            }
        }
    };

    m_propertyMap["sid"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_INT64)
        {
            int64_t id = *reinterpret_cast<int64_t *>(prop->data);
            state()->setSid(id);
        }
        else if (prop->format == MPV_FORMAT_FLAG)
        {
            bool enabled = !!*reinterpret_cast<int *>(prop->data);
            if (!enabled)
            {
                state()->setSid(0);
                state()->subtitle()->setStartTime(0);
                state()->subtitle()->setEndTime(0);
                state()->subtitle()->setText("");
            }
        }
    };

    m_propertyMap["secondary-sid"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_INT64)
        {
            int64_t id = *reinterpret_cast<int64_t *>(prop->data);
            state()->setSecondarySid(id);
        }
        else if (prop->format == MPV_FORMAT_FLAG)
        {
            bool enabled = !!*reinterpret_cast<int *>(prop->data);
            if (!enabled)
            {
                state()->setSecondarySid(0);
                state()->secondarySubtitle()->setStartTime(0);
                state()->secondarySubtitle()->setEndTime(0);
                state()->secondarySubtitle()->setText("");
            }
        }
    };


    m_propertyMap["vid"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_INT64)
        {
            int64_t id = *reinterpret_cast<int64_t *>(prop->data);
            state()->setVid(id);
        }
        else if (prop->format == MPV_FORMAT_FLAG)
        {
            bool enabled = !!*reinterpret_cast<int *>(prop->data);
            if (!enabled)
            {
                state()->setVid(0);
            }
        }
    };

    m_propertyMap["sub-delay"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_DOUBLE)
        {
            double delay = *reinterpret_cast<double *>(prop->data);
            state()->subtitle()->setDelay(delay);
        }
    };

    m_propertyMap["secondary-sub-delay"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_DOUBLE)
        {
            double delay = *reinterpret_cast<double *>(prop->data);
            state()->secondarySubtitle()->setDelay(delay);
        }
    };

    m_propertyMap["sub-text"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_STRING)
        {
            QString subtitle = *reinterpret_cast<const char **>(prop->data);

            double start{0};
            double end{0};
            ::mpv_get_property(
                handle(), "sub-start", MPV_FORMAT_DOUBLE, &start
            );
            ::mpv_get_property(
                handle(), "sub-end",   MPV_FORMAT_DOUBLE, &end
            );

            state()->subtitle()->setStartTime(start);
            state()->subtitle()->setEndTime(end);
            state()->subtitle()->setText(std::move(subtitle));
        }
    };

    m_propertyMap["secondary-sub-text"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_STRING)
        {
            QString subtitle = *reinterpret_cast<const char **>(prop->data);

            double start{0};
            double end{0};
            ::mpv_get_property(
                handle(), "secondary-sub-start", MPV_FORMAT_DOUBLE, &start
            );
            ::mpv_get_property(
                handle(), "secondary-sub-end",   MPV_FORMAT_DOUBLE, &end
            );

            state()->secondarySubtitle()->setStartTime(start);
            state()->secondarySubtitle()->setEndTime(end);
            state()->secondarySubtitle()->setText(std::move(subtitle));
        }
    };

    m_propertyMap["sub-visibility"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_FLAG)
        {
            bool visible = !!*reinterpret_cast<int *>(prop->data);
            state()->subtitle()->setVisible(visible);
        }
    };

    m_propertyMap["secondary-sub-visibility"] =
        [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_FLAG)
        {
            bool visible = !!*reinterpret_cast<int *>(prop->data);
            state()->secondarySubtitle()->setVisible(visible);
        }
    };

    m_propertyMap["cursor-autohide"] = [this] (mpv_event_property *prop)
    {
        switch (prop->format)
        {
        case MPV_FORMAT_INT64:
            state()->setCursorAutoHideTime(
                *reinterpret_cast<int64_t *>(prop->data)
            );
            state()->setCursorAutoHideType(MpvState::AutoHideNumber);
            break;

        case MPV_FORMAT_STRING:
        {
            const char *autoHideState = *(const char **)prop->data;
            if (std::strcmp(autoHideState, "always") == 0)
            {
                state()->setCursorAutoHideType(MpvState::AutoHideAlways);
            }
            else if (std::strcmp(autoHideState, "no") == 0)
            {
                state()->setCursorAutoHideType(MpvState::AutoHideNever);
            }
            break;
        }

        default:
            break;
        }
    };

    m_propertyMap["cursor-autohide-fs-only"] = [this] (mpv_event_property *prop)
    {
        if (prop->format == MPV_FORMAT_FLAG)
        {
            bool fullscreenOnly = !!*reinterpret_cast<int *>(prop->data);
            state()->setCursorAutoHideFullscreenOnly(fullscreenOnly);
        }
    };
}

void MpvPlayer::initializeMpv()
{
    if (m_initialized)
    {
        return;
    }

    initOptions();

    if (mpv_initialize(handle()) < 0)
    {
        throw std::runtime_error("MpvPlayer: Failed to initialize mpv context");
    }

    mpv_observe_property(handle(), 0, "chapter-list",        MPV_FORMAT_NODE);
    mpv_observe_property(handle(), 0, "duration",            MPV_FORMAT_DOUBLE);
    mpv_observe_property(handle(), 0, "fullscreen",          MPV_FORMAT_FLAG);
    mpv_observe_property(handle(), 0, "media-title",         MPV_FORMAT_STRING);
    mpv_observe_property(handle(), 0, "path",                MPV_FORMAT_STRING);
    mpv_observe_property(handle(), 0, "pause",               MPV_FORMAT_FLAG);
    mpv_observe_property(handle(), 0, "time-pos",            MPV_FORMAT_DOUBLE);
    mpv_observe_property(handle(), 0, "track-list/count",    MPV_FORMAT_INT64);
    mpv_observe_property(handle(), 0, "volume-max",          MPV_FORMAT_INT64);
    mpv_observe_property(handle(), 0, "volume",              MPV_FORMAT_INT64);
    mpv_observe_property(handle(), 0, "video-params/w",      MPV_FORMAT_INT64);
    mpv_observe_property(handle(), 0, "video-params/h",      MPV_FORMAT_INT64);

    mpv_observe_property(handle(), 0, "aid",                 MPV_FORMAT_INT64);
    mpv_observe_property(handle(), 0, "aid",                 MPV_FORMAT_FLAG);
    mpv_observe_property(handle(), 0, "secondary-sid",       MPV_FORMAT_INT64);
    mpv_observe_property(handle(), 0, "secondary-sid",       MPV_FORMAT_FLAG);
    mpv_observe_property(handle(), 0, "sid",                 MPV_FORMAT_INT64);
    mpv_observe_property(handle(), 0, "sid",                 MPV_FORMAT_FLAG);
    mpv_observe_property(handle(), 0, "vid",                 MPV_FORMAT_INT64);
    mpv_observe_property(handle(), 0, "vid",                 MPV_FORMAT_FLAG);

    mpv_observe_property(handle(), 0, "secondary-sub-text",  MPV_FORMAT_STRING);
    mpv_observe_property(handle(), 0, "secondary-sub-delay", MPV_FORMAT_DOUBLE);
    mpv_observe_property(
        handle(), 0, "secondary-sub-visibility", MPV_FORMAT_FLAG
    );
    mpv_observe_property(handle(), 0, "sub-delay",           MPV_FORMAT_DOUBLE);
    mpv_observe_property(handle(), 0, "sub-text",            MPV_FORMAT_STRING);
    mpv_observe_property(handle(), 0, "sub-visibility",      MPV_FORMAT_FLAG);

    mpv_observe_property(handle(), 0, "cursor-autohide",     MPV_FORMAT_INT64);
    mpv_observe_property(handle(), 0, "cursor-autohide",     MPV_FORMAT_STRING);
    mpv_observe_property(
        handle(), 0, "cursor-autohide-fs-only", MPV_FORMAT_FLAG
    );

    m_initialized = true;
    emit initialized();
}

void MpvPlayer::initOptions()
{
    /* Hardcoded options */
    mpv_set_option_string(handle(), "config",                 "yes");
    mpv_set_option_string(handle(), "input-default-bindings", "yes");
    mpv_set_option_string(handle(), "keep-open",              "yes");
    mpv_set_option_string(handle(), "screenshot-directory",   "~~desktop/");
    mpv_set_option_string(handle(), "terminal",               "yes");
    mpv_set_option_string(handle(), "vo",                     "libmpv");
    mpv_set_option_string(handle(), "ytdl",                   "yes");
    mpv_set_option_string(handle(), "pause",                  "yes");

    QByteArray configDir = DirectoryUtils::getConfigDir().toUtf8();
    mpv_set_option_string(handle(), "config-dir", configDir);

    QByteArray inputFile = DirectoryUtils::getMpvInputConfig().toUtf8();
    mpv_set_option_string(handle(), "input-conf", inputFile);

    /* Commandline options */
    QStringList args = QCoreApplication::arguments();
    for (qsizetype i = 0; i < args.size(); ++i)
    {
        /* Skip non-options */
        if (!args[i].startsWith("--"))
        {
            continue;
        }

        /* Skip per file options */
        if (args[i] == "--{")
        {
            qsizetype depth = 1;
            while (depth > 0)
            {
                ++i;
                if (i >= args.size())
                {
                    break;
                }
                else if (args[i] == "--{")
                {
                    ++depth;
                }
                else if (args[i] == "--}")
                {
                    --depth;
                }
            }
            continue;
        }

        QString arg = args[i].last(args[i].size() - 2);
        if (arg.startsWith("no-"))
        {
            arg = arg.last(arg.size() - 3)
                .append("=no");
        }

        qsizetype sepIndex = arg.indexOf('=');
        if (sepIndex == -1)
        {
            continue;
        }

        QByteArray option = arg.first(sepIndex).toUtf8();
        QByteArray value = arg.mid(sepIndex + 1).toUtf8();
        mpv_set_option_string(handle(), option, value);
    }
}

/* End Initializers */
/* Begin mpv context methods */

mpv_handle *MpvPlayer::handle() const noexcept
{
    return m_mpv.get();
}

QQuickRhiItemRenderer *MpvPlayer::createRenderer()
{
    window()->setPersistentGraphics(true);
    window()->setPersistentSceneGraph(true);

    auto *renderer = new PlatformRenderer(m_mpv);
    connect(
        renderer, &PlatformRenderer::contextCreated,
        this, &MpvPlayer::initializeMpv,
        Qt::QueuedConnection
    );
    connect(
        renderer, &PlatformRenderer::redraw,
        this, &MpvPlayer::update,
        Qt::QueuedConnection
    );
    return renderer;
}

/* End mpv context methods */
/* Begin Mpv Event Slots */

void MpvPlayer::handleMpvEvents()
{
    while (m_mpv)
    {
        mpv_event *event = ::mpv_wait_event(handle(), 0);
        if (event->event_id == MPV_EVENT_NONE)
        {
            break;
        }
        handleMpvEvent(event);
    }
}

void MpvPlayer::handleMpvEvent(mpv_event *event)
{
    switch (event->event_id)
    {
    case MPV_EVENT_PROPERTY_CHANGE:
    {
        if (state() == nullptr)
        {
            return;
        }

        mpv_event_property *prop =
            reinterpret_cast<mpv_event_property *>(event->data);
        auto func = m_propertyMap.find(prop->name);
        if (func != m_propertyMap.end())
        {
            (*func)(prop);
        }
        break;
    }

    case MPV_EVENT_FILE_LOADED:
    {
        int64_t w = 0;
        ::mpv_get_property(handle(), "video-params/w", MPV_FORMAT_INT64, &w);
        int64_t h = 0;
        ::mpv_get_property(handle(), "video-params/h", MPV_FORMAT_INT64, &h);
        emit fileLoaded(w, h);
        break;
    }

    case MPV_EVENT_SHUTDOWN:
    {
        emit shutdown();
        break;
    }

    default:
        break;
    }
}

/* End Mpv Event Slots */
/* Begin Properties */

MpvState *MpvPlayer::state() const noexcept
{
    return m_state;
}

void MpvPlayer::setState(MpvState *state)
{
    if (m_state == state)
    {
        return;
    }

    if (state)
    {
        state->setPlayer(this);
    }
    if (m_state)
    {
        m_state->deleteLater();
    }
    m_state = state;
    emit stateChanged();
}

MpvController *MpvPlayer::controller() const noexcept
{
    return m_controller;
}

void MpvPlayer::setController(MpvController *controller)
{
    if (m_controller == controller)
    {
        return;
    }

    if (controller)
    {
        controller->setPlayer(this);
    }
    if (m_controller)
    {
        m_controller->deleteLater();
    }
    m_controller = controller;
    emit controllerChanged();
}

/* End Properties */
/* Begin Static Callbacks */

void MpvPlayer::onMpvEvents(void *ctx)
{
    MpvPlayer *self = reinterpret_cast<MpvPlayer *>(ctx);
    emit self->mpvEvent();
}

/* End Static Callbacks */
