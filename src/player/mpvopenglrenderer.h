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

#include <QQuickFramebufferObject>

#include "player/mpvopenglframebackend.h"

/**
 * @brief Renders mpv into a Qt Quick OpenGL framebuffer.
 */
class MpvOpenGlRenderer : public QQuickFramebufferObject::Renderer
{
public:
    /**
     * @brief Constructs a new mpv renderer object.
     * Should only be called from MpvPlayer.
     *
     * @param player The MpvPlayer that created this MpvOpenGlRenderer.
     * @param backend The OpenGL framebackend for this player.
     */
    MpvOpenGlRenderer(MpvPlayer *player, MpvOpenGlFrameBackend *backend);

    /**
     * @brief Releases the OpenGL renderer's framebuffer wrapper.
     */
    ~MpvOpenGlRenderer() override = default;

    /**
     * @brief Creates a new framebuffer object.
     * Called for the first frame and whenever the framebuffer size changes.
     *
     * @param size The size of the requested framebuffer.
     * @return A QOpenGLFramebufferObject of size.
     */
    QOpenGLFramebufferObject *createFramebufferObject(
        const QSize &size) override;

    /**
     * @brief Renders the data from mpv to the framebuffer.
     */
    void render() override;

private:
    /* The player that created this framebuffer */
    MpvPlayer *m_player{nullptr};

    /* Shared mpv render core */
    MpvOpenGlFrameBackend *m_frameBackend{nullptr};
};
