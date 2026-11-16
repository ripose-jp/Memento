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

#include <mpv/client.h>

/**
 * @brief Presents mpv's OpenGL frames through Qt's Metal RHI on its thread.
 *
 * The renderer owns the CGL context and frees mpv's render context with that
 * context current. Qt owns the renderer and its output texture.
 */
class MpvMetalRenderer final : public QObject, public QQuickRhiItemRenderer
{
    Q_OBJECT

public:
    /**
     * @brief Creates a renderer sharing ownership of the player's mpv core.
     *
     * @param mpv The core to render.
     */
    explicit MpvMetalRenderer(std::shared_ptr<mpv_handle> mpv);

    /**
     * @brief Releases native resources and mpv on Qt's render thread.
     */
    ~MpvMetalRenderer() override;

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
     * @brief Validates Qt's target and initializes the native bridge.
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
     * @brief Renders mpv and presents its shared surface through Metal.
     *
     * @param cb The command buffer for the presentation pass.
     */
    void render(QRhiCommandBuffer *cb) override;

private:
    /* Hides Objective-C types and owns all native rendering resources. */
    class Impl;

    /* The bridge implementation, destroyed on Qt's render thread. */
    std::unique_ptr<Impl> m_impl;
};
