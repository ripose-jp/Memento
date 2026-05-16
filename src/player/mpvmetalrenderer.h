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

#include <QtCore/qglobal.h>

#ifdef Q_OS_MACOS

#include <memory>

#include <QSize>

class MpvPlayer;
class QQuickWindow;
class QSGTexture;

/**
 * @brief macOS renderer that produces mpv frames on a dedicated OpenGL worker
 * thread and exposes the latest completed IOSurface through Metal.
 */
class MpvMetalRenderer
{
public:
    /**
     * @brief Creates the macOS renderer for a player.
     *
     * @param player The player whose mpv render context is rendered.
     */
    explicit MpvMetalRenderer(MpvPlayer *player);

    /**
     * @brief Stops the worker and releases all worker-owned resources.
     */
    ~MpvMetalRenderer();

    /**
     * @brief Requests a new backing size in physical pixels.
     *
     * @param pixelSize The requested frame size in real device pixels.
     */
    void setPixelSize(const QSize &pixelSize);

    /**
     * @brief Requests that the worker render a new mpv frame.
     */
    void requestRender();

    /**
     * @brief Returns a scene graph texture for the latest completed frame.
     *
     * @param window The window that owns the scene graph texture.
     * @return The latest completed frame, or nullptr if no frame is ready yet.
     */
    [[nodiscard]]
    QSGTexture *texture(QQuickWindow *window);

private:
    /* PiPL declaration to the Objective-C++ Metal render bridge. */
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif // Q_OS_MACOS
