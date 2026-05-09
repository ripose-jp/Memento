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

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include "player/mpvcontroller.h"
#include "player/mpvstate.h"

/**
 * @brief A component that creates an instance of mpv in QML.
 */
class MpvPlayer : public QQuickFramebufferObject
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
     * @brief Creates a QQuickFramebufferObject::Renderer
     *
     * @return A pointer to the newly created renderer.
     */
    [[nodiscard]]
    virtual Renderer *createRenderer() const;

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
};
