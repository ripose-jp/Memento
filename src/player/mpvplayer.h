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

#ifdef Q_OS_MACOS
#include <QQuickItem>
#else
#include <QQuickFramebufferObject>
#endif // Q_OS_MACOS

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include "player/mpvcontroller.h"
#include "player/mpvstate.h"

#ifdef Q_OS_MACOS
class MpvMetalRenderer;
using MpvPlayerBase = QQuickItem;
#else
using MpvPlayerBase = QQuickFramebufferObject;
#endif // Q_OS_MACOS

/**
 * @brief A component that creates an instance of mpv in QML.
 */
class MpvPlayer : public MpvPlayerBase
{
    Q_OBJECT

    Q_PROPERTY(
        MpvState *state
        READ state
        NOTIFY stateChanged
    )

    Q_PROPERTY(
        MpvController *controller
        READ controller
        NOTIFY controllerChanged
    )

public:
    MpvPlayer(QQuickItem *parent = nullptr);
    virtual ~MpvPlayer();

    /**
     * @brief Returns a pointer to the mpv_handle for this player.
     *
     * @return The mpv_handle currently in use.
     */
    [[nodiscard]]
    mpv_handle *handle() const noexcept;

    /**
     * @brief The mpv OpenGL render context in use.
     *
     * @return A pointer to the current render context. Nullptr if
     * createRenderContext() has been called yet.
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
     * @brief Creates a QQuickFramebufferObject::Renderer
     *
     * @return A pointer to the newly created renderer.
     */
#ifndef Q_OS_MACOS
    [[nodiscard]]
    virtual Renderer *createRenderer() const;
#endif // Q_OS_MACOS

    /**
     * @brief Gets the state object tracked by this mpv context.
     *
     * @return The state tracking this MpvPlayer. Nullptr if no state is set.
     */
    [[nodiscard]]
    MpvState *state() const noexcept;

    /**
     * @brief Sets the mpv state object.
     * Takes ownership.
     *
     * @param state The MpvState object.
     */
    void setState(MpvState *state);

    /**
     * @brief The controller for this mpv player.
     *
     * @return The controller for this mpv player.
     */
    [[nodiscard]]
    MpvController *controller() const noexcept;

    /**
     * @brief Sets the controller for this mpv player.
     *
     * @param controller The controller for this mpv player.
     */
    void setController(MpvController *controller);

signals:
    /**
     * @brief Emitted when the render context is created.
     */
    void renderContextCreated();

    /**
     * @brief Emitted when the player finishes initialization.
     */
    void initialized();

    /**
     * @brief Emitted when an mpv is available to handle.
     */
    void mpvEvent();

    /**
     * @brief Emitted when mpv is ready to redraw on the framebuffer.
     */
    void mpvRedraw();

    /**
     * @brief Emitted when a new file is loaded.
     *
     * @param width The width of the video content in pixels.
     * @param height The height of the video content in pixels.
     */
    void fileLoaded(int64_t width, int64_t height);

    /**
     * @brief Emitted when the mpv instance has shutdown.
     */
    void shutdown();

    /**
     * @brief Emitted when the MpvState is changed.
     */
    void stateChanged();

    /**
     * @brief Emitted when the controller is changed.
     */
    void controllerChanged();

private slots:
    /**
     * @brief Initializes mpv.
     */
    void initializeMpv();

    /**
     * @brief A slot wrapper around update().
     */
    void doUpdate();

    /**
     * @brief Handles all available mpv events.
     */
    void handleMpvEvents();

    /**
     * @brief Handles an mpv event.
     *
     * @param event The event to handle.
     */
    void handleMpvEvent(mpv_event *event);

private:
    /**
     * @brief Initializes the property map with all the mpv event handlers.
     */
    void initPropertyMap();

    /**
     * @brief Initializes mpv options and command line arguments.
     */
    void initOptions();

    /* The mpv context */
    mpv_handle *m_mpv{nullptr};

    /* The mpv render context */
    mpv_render_context *m_mpv_gl{nullptr};

    /* Maps mpv properties to the appropriate event handling function */
    QHash<QString, std::function<void(mpv_event_property *)>> m_propertyMap;

    /* The state object tracking this mpv player */
    MpvState *m_state{new MpvState(this)};

    /* The state object tracking this mpv player */
    MpvController *m_controller{new MpvController(this)};

#ifdef Q_OS_MACOS
protected:
    /**
     * @brief Called when the Qt Scene graph wants an update from the player.
     *
     * @param oldNode The current node in use. May be nullptr.
     * @return The new node with the frame rendered.
     */
    [[nodiscard]]
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    /**
     * @brief Called when the player's geometry changes. Forces an update.
     *
     * @param newGeometry The new geometry of the Item.
     * @param oldGeometry The new geometry of the Item.
     */
    void geometryChange(
        const QRectF &newGeometry, const QRectF &oldGeometry) override;

    /**
     * @brief Called when something about this Item changes.
     *
     * Used to check for window changes so a new screen can be connected to.
     *
     * @param change The change in the item.
     * @param value The new value.
     */
    void itemChange(ItemChange change, const ItemChangeData &value) override;

private:
    /* The Metal-OpenGL bridge renderer for macOS */
    std::unique_ptr<MpvMetalRenderer> m_renderer;

    /* The connection managing the screen change */
    QMetaObject::Connection m_screenChangedConnection;
#endif // Q_OS_MACOS
};
