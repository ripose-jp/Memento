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

#include <QSize>

class MpvPlayer;

/**
 * @brief Shared mpv OpenGL rendering core used by platform presentation
 * backends.
 *
 * This class deliberately knows nothing about how the resulting framebuffer is
 * presented. The legacy Qt/OpenGL path and the macOS Metal bridge both provide
 * a framebuffer and let this class submit mpv's frame into it.
 */
class MpvFrameBackend
{
public:
    /**
     * @brief Create an MpvFrameBackend wrapper.
     *
     * @param player The player render frames for.
     */
    explicit MpvFrameBackend(MpvPlayer *player);

    /**
     * @brief Renders the mpv object to an OpenGL framebuffer object.
     *
     * @param framebuffer The framebuffer object to render to.
     * @param size The size of the framebuffer in real device pixels.
     */
    void renderToFramebuffer(unsigned int framebuffer, const QSize &size);

private:
    /* Pointer to the mpv player instance */
    MpvPlayer *m_player{nullptr};
};
