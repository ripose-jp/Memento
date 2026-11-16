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

#include <memory>

#include <QSize>

#include <mpv/render_gl.h>

/**
 * @brief Owns an mpv OpenGL render context independently of its presentation.
 *
 * The renderer must make its original GL context current before creating,
 * rendering, or destroying this backend's render context.
 */
class MpvOpenGlFrameBackend final : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Shares ownership of the mpv core until rendering is cleaned up.
     *
     * @param mpv The core to render.
     */
    explicit MpvOpenGlFrameBackend(std::shared_ptr<mpv_handle> mpv);

    /**
     * @brief Verifies that the renderer released its context before the core.
     */
    ~MpvOpenGlFrameBackend() override;

    /**
     * @brief Returns the current mpv render context.
     *
     * @return The context, or nullptr before creation and after cleanup.
     */
    [[nodiscard]]
    mpv_render_context *renderContext() const noexcept;

    /**
     * @brief Creates the render context using the current OpenGL context.
     *
     * @param initParams The GL function resolver and its callback context.
     * @param display Optional native display for hardware decoding.
     * @return True if the context is ready, or false after logging a failure.
     */
    [[nodiscard]]
    bool createRenderContext(
        mpv_opengl_init_params initParams,
        mpv_render_param display = {MPV_RENDER_PARAM_INVALID, nullptr});

    /**
     * @brief Stops frame notifications before renderer cleanup.
     */
    void stopUpdates();

    /**
     * @brief Frees the render context with its original GL context current.
     * Safe to call repeatedly; must run on the renderer's owning thread.
     */
    void destroyRenderContext();

    /**
     * @brief Renders without waiting for mpv's target presentation time.
     *
     * @param framebuffer The framebuffer to render into.
     * @param size The framebuffer size in physical pixels.
     * @param flipY Whether the target uses bottom-up image coordinates.
     * @return True on success, or false after logging a failure.
     */
    [[nodiscard]]
    bool renderToFramebuffer(
        unsigned int framebuffer, const QSize &size, bool flipY);

signals:
    /**
     * @brief Emitted when the render context is ready for playback.
     */
    void contextCreated();

    /**
     * @brief Emitted from mpv's callback to request another frame.
     */
    void redraw();

private:
    /**
     * @brief Forwards a frame notification without calling rendering APIs.
     *
     * @param ctx The backend whose frame needs updating.
     */
    static void onMpvRedraw(void *ctx);

    /* Keeps the core alive until the renderer has released its GL context. */
    std::shared_ptr<mpv_handle> m_mpv;

    /* The render context, used and freed only on the renderer's thread. */
    mpv_render_context *m_renderContext{nullptr};
};
