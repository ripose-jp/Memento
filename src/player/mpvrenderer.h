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

#include <QQuickFramebufferObject>

#include <atomic>

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include "player/mpvplayer.h"
#include "player/mpvframebackend.h"

/**
 * @brief The mpv render that allows mpv to render to the QML framebuffer.
 */
class MpvRenderer : public QQuickFramebufferObject::Renderer
{
public:
    /**
     * @brief Constructs a new mpv renderer object.
     * Should only be called from MpvPlayer.
     *
     * @param player The MpvPlayer that created this MpvRenderer.
     */
    MpvRenderer(MpvPlayer *player);
    virtual ~MpvRenderer() = default;

    /**
     * @brief Creates a new framebuffer object.
     * This happens on the initialframe.
     *
     * @param size The size of the requested framebuffer.
     * @return A QOpenGLFramebufferObject of size.
     */
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);

    /**
     * @brief Renders the data from mpv to the framebuffer.
     */
    void render();

private:
    /* The player that created this framebuffer */
    MpvPlayer *m_player{nullptr};

    /* Shared mpv render core */
    MpvFrameBackend m_frameBackend;
};
