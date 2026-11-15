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

#include <QSize>

#include <mpv/render_gl.h>

class MpvPlayer;

/**
 * @brief Shared mpv OpenGL rendering core used by platform presentation
 * backends.
 *
 * This class deliberately knows nothing about how the resulting framebuffer is
 * presented. The legacy Qt/OpenGL path and the macOS Metal bridge both provide
 * a framebuffer and let this class submit mpv's frame into it.
 */
class MpvOpenGlFrameBackend : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates an OpenGL frame backend for the player.
     *
     * @param parent The parent of this render backend.
     * @param player The player to render frames for.
     */
    explicit MpvOpenGlFrameBackend(QObject *parent, MpvPlayer *player);

    /**
     * @brief Destroys the render context if it hasn't already been destroyed.
     */
    virtual ~MpvOpenGlFrameBackend();

    /**
     * @brief The mpv OpenGL render context in use.
     *
     * @return A pointer to the current render context. Nullptr if
     * createRenderContext() has not been called yet.
     */
    [[nodiscard]]
    mpv_render_context *renderContext() const noexcept;

    /**
     * @brief Creates the render context.
     */
    void createRenderContext();

    /**
     * @brief Destroys the mpv render context.
     *
     * The OpenGL context that was current when createRenderContext() was called
     * must be current before this function is called.
     */
    void destroyRenderContext();

    /**
     * @brief Renders the mpv object to an OpenGL framebuffer object.
     *
     * @param framebuffer The framebuffer object to render to.
     * @param size The size of the framebuffer in real device pixels.
     */
    void renderToFramebuffer(unsigned int framebuffer, const QSize &size);

signals:
    /**
     * @brief Emitted when the render context is created.
     */
    void contextCreated();

    /**
     * @brief Emitted when mpv is ready to redraw on the framebuffer.
     */
    void redraw();

private:
    /**
     * @brief Notifies the GUI thread that mpv needs another OpenGL frame.
     *
     * @param ctx The player to update.
     */
    static void onMpvRedraw(void *ctx);

    /**
     * @brief Resolves an OpenGL function in the current Qt context.
     *
     * @param ctx Unused callback context.
     * @param name The OpenGL function name.
     * @return The function address, or nullptr if no context is current.
     */
    static void *getProcAddressMpv(void *ctx, const char *name);

    /* Pointer to the mpv player instance */
    MpvPlayer *m_player{nullptr};

    /* The mpv render context */
    mpv_render_context *m_mpv_gl{nullptr};
};
