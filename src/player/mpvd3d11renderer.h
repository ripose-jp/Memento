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

#include <memory>

#include <QObject>
#include <QQuickRhiItem>

#include <mpv/render.h>

/**
 * @brief Renders mpv directly into Qt's D3D11 texture on the render thread.
 *
 * Qt owns the device and backing texture. The renderer owns the mpv render
 * context and keeps the core alive until that context has been released.
 */
class MpvD3D11Renderer final : public QObject, public QQuickRhiItemRenderer
{
    Q_OBJECT

public:
    /**
     * @brief Creates a renderer sharing ownership of the player's mpv core.
     *
     * @param mpv The core to render.
     */
    explicit MpvD3D11Renderer(std::shared_ptr<mpv_handle> mpv);

    /**
     * @brief Stops callbacks and frees the render context before the core.
     */
    ~MpvD3D11Renderer() override;

signals:
    /**
     * @brief Emitted when the D3D11 render context is ready for playback.
     */
    void contextCreated();

    /**
     * @brief Emitted from mpv's update callback to queue a GUI frame request.
     */
    void redraw();

protected:
    /**
     * @brief Checks the Qt backend and releases a context for an old device.
     *
     * @param cb The current command buffer, unused until render().
     */
    void initialize(QRhiCommandBuffer *cb) override;

    /**
     * @brief Implements Qt's synchronization hook; no item data is needed.
     *
     * @param item The player item, unused by the renderer.
     */
    void synchronize(QQuickRhiItem *item) override;

    /**
     * @brief Renders a frame into Qt's backing texture with native D3D11 calls.
     *
     * @param cb The command buffer used to isolate mpv's graphics state.
     */
    void render(QRhiCommandBuffer *cb) override;

private:
    /**
     * @brief Creates the mpv context during Qt's external command recording.
     */
    void createRenderContext();

    /**
     * @brief Disables callbacks and releases the mpv render context.
     */
    void destroyRenderContext();

    /**
     * @brief Forwards an mpv frame notification without calling rendering APIs.
     *
     * @param ctx The renderer whose frame needs updating.
     */
    static void onMpvRedraw(void *ctx);

    /* Keeps the core alive during deferred renderer cleanup. */
    std::shared_ptr<mpv_handle> m_mpv;

    /* The mpv render context, used and destroyed only on the render thread. */
    mpv_render_context *m_renderContext{nullptr};

    /* Borrowed Qt D3D11 device used to detect device changes. */
    void *m_device{nullptr};

    /* Stops retries and repeated diagnostics after a rendering failure. */
    bool m_failed{false};
};
