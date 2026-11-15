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

#include <QtGlobal>

#if defined(Q_OS_WIN)
#include <QQuickRhiItem>
#elif defined(Q_OS_MACOS)
#include <QQuickItem>
#else
#include <QQuickFramebufferObject>
#endif

#include <mpv/client.h>
#include <mpv/render.h>

#include "player/mpvcontroller.h"
#include "player/mpvstate.h"
#ifndef Q_OS_WIN
#include "player/mpvopenglframebackend.h"
#endif // Q_OS_WIN

#if defined(Q_OS_WIN)
using MpvPlayerBase = QQuickRhiItem;
#elif defined(Q_OS_MACOS)
class MpvMetalRenderer;
using MpvPlayerBase = QQuickItem;
#else
using MpvPlayerBase = QQuickFramebufferObject;
#endif

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
    /**
     * @brief Creates the mpv core and connects its event notifications.
     *
     * @param parent The parent Quick item.
     */
    MpvPlayer(QQuickItem *parent = nullptr);

    /**
     * @brief Disconnects callbacks and releases the player's mpv resources.
     */
    virtual ~MpvPlayer();

    /**
     * @brief Returns a pointer to the mpv_handle for this player.
     *
     * @return The mpv_handle currently in use.
     */
    [[nodiscard]]
    mpv_handle *handle() const noexcept;

#if defined(Q_OS_WIN)
    /**
     * @brief Creates the Windows D3D11 renderer on the Qt render thread.
     *
     * @return A pointer to the newly created renderer.
     */
    [[nodiscard]]
    QQuickRhiItemRenderer *createRenderer() override;
#elif !defined(Q_OS_MACOS)
    /**
     * @brief Creates the OpenGL framebuffer renderer.
     *
     * @return A pointer to the newly created renderer.
     */
    [[nodiscard]]
    Renderer *createRenderer() const override;
#endif

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
     * @brief Emitted when the player finishes initialization.
     */
    void initialized();

    /**
     * @brief Emitted when an mpv is available to handle.
     */
    void mpvEvent();

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

protected:
    /**
     * @brief Initializes mpv options and command line arguments.
     *
     * Can be overwritten to be initialized with different options.
     */
    virtual void initOptions();

private slots:
    /**
     * @brief Initializes mpv once its render context is ready.
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
     * @brief Notifies the GUI thread that mpv has queued events.
     *
     * @param ctx The player receiving the events.
     */
    static void onMpvEvents(void *ctx);

    /* The mpv core */
    mpv_handle *m_mpv{nullptr};

    /* Keeps the core alive until both the player and renderer release it. */
    std::shared_ptr<mpv_handle> m_mpvOwner;

    /* Prevents renderer recreation from initializing the mpv core twice. */
    bool m_initialized{false};

    /* Maps mpv properties to the appropriate event handling function */
    QHash<QString, std::function<void(mpv_event_property *)>> m_propertyMap;

    /* The state object tracking this mpv player */
    MpvState *m_state{new MpvState(this)};

    /* The state object tracking this mpv player */
    MpvController *m_controller{new MpvController(this)};

#ifndef Q_OS_WIN
    /* The OpenGL framebackend which manges the OpenGL context on Linux and
     * macOS */
    MpvOpenGlFrameBackend *m_frameBackend{
        new MpvOpenGlFrameBackend{this, this}
    };
#endif // Q_OS_WIN

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
