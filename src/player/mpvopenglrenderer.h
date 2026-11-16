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
#include <QQuickRhiItem>

#include <memory>

#include "player/mpvopenglframebackend.h"

class QOpenGLContext;

/**
 * @brief Renders mpv into Qt's OpenGL RHI target on the render thread.
 */
class MpvOpenGlRenderer final : public QObject, public QQuickRhiItemRenderer
{
    Q_OBJECT

public:
    /**
     * @brief Creates a renderer sharing ownership of the player's mpv core.
     *
     * @param mpv The core to render.
     */
    explicit MpvOpenGlRenderer(std::shared_ptr<mpv_handle> mpv);

    /**
     * @brief Frees mpv's render context with the Qt GL context current.
     */
    ~MpvOpenGlRenderer() override;

signals:
    /**
     * @brief Emitted when the render context is ready for playback.
     */
    void contextCreated();

    /**
     * @brief Emitted from mpv's callback to queue a GUI frame request.
     */
    void redraw();

protected:
    /**
     * @brief Validates Qt's target and detects graphics context changes.
     *
     * @param cb The current command buffer, unused until render().
     */
    void initialize(QRhiCommandBuffer *cb) override;

    /**
     * @brief Implements the synchronization hook; no item data is needed.
     *
     * @param item The player item, unused by the renderer.
     */
    void synchronize(QQuickRhiItem *item) override;

    /**
     * @brief Renders into Qt's framebuffer inside external-command boundaries.
     *
     * @param cb The command buffer used to isolate mpv's GL state.
     */
    void render(QRhiCommandBuffer *cb) override;

private:
    /**
     * @brief Releases mpv's render context before Qt destroys its GL context.
     */
    void releaseResources();

    /**
     * @brief Resolves a function in the current Qt OpenGL context.
     *
     * @param ctx Unused callback context.
     * @param name The GL function name.
     * @return The function address, or nullptr if unavailable.
     */
    static void *getProcAddress(void *ctx, const char *name);

    /**
     * @brief Gets the native display needed by Linux hardware decoders.
     *
     * @return A display parameter, or the terminator if none is available.
     */
    static mpv_render_param nativeDisplay();

    /* Shared GL submission and mpv render-context ownership. */
    MpvOpenGlFrameBackend m_backend;

    /* Borrowed RHI used to make the originating GL context current. */
    QRhi *m_rhi{nullptr};

    /* Borrowed Qt context used to detect replacement and assert ownership. */
    QOpenGLContext *m_context{nullptr};

    /* Stops retries and repeated diagnostics after a rendering failure. */
    bool m_failed{false};
};
