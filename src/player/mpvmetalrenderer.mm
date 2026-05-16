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

#include "player/mpvmetalrenderer.h"

#ifdef Q_OS_MACOS

#define GL_SILENCE_DEPRECATION

#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include <QMetaObject>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QSGRendererInterface>
#include <QSGTexture>
#include <QSurfaceFormat>
#include <QtQuick/qsgtexture_platform.h>

#import <CoreVideo/CVPixelBuffer.h>
#import <IOSurface/IOSurface.h>
#import <Metal/Metal.h>
#import <OpenGL/CGLIOSurface.h>
#import <OpenGL/OpenGL.h>

#include "player/mpvframebackend.h"
#include "player/mpvplayer.h"

/* The maximum number of frames to buffer */
static constexpr int FRAME_SLOT_COUNT{3};

/**
 * @brief The PiPL implementation of the mpv-Metal renderer.
 */
class MpvMetalRenderer::Impl final
{
public:
    /**
     * @brief Creates a worker-owned OpenGL producer for a player.
     *
     * @param player The player rendered by this backend.
     */
    explicit Impl(MpvPlayer *player);

    /**
     * @brief Stops the worker and releases all renderer resources.
     */
    ~Impl();

    /**
     * @brief Requests a new worker-side backing size.
     *
     * @param pixelSize The requested frame size in physical pixels.
     */
    void setPixelSize(const QSize &pixelSize);

    /**
     * @brief Requests a new frame from the worker.
     */
    void requestRender();

    /**
     * @brief Gets a Metal-backed Qt texture for the latest completed frame.
     *
     * @param window The window that owns the Qt texture wrapper.
     * @return The latest completed frame texture, or nullptr if none exists.
     */
    [[nodiscard]]
    QSGTexture *texture(QQuickWindow *window);

private:
    /**
     * @brief One IOSurface-backed render target owned by the GL worker.
     */
    struct FrameSlot
    {
        /* Shared storage visible from both OpenGL and Metal. */
        IOSurfaceRef surface{nullptr};

        /* OpenGL texture view of surface. */
        GLuint texture{0};

        /* OpenGL framebuffer rendered by mpv. */
        GLuint framebuffer{0};

        /* Monotonic frame number written into this slot. */
        quint64 serial{0};
    };

    /**
     * @brief Metal-side cache for a published IOSurface.
     */
    struct MetalTexture
    {
        /* IOSurface used to decide whether the cache entry is still current. */
        IOSurfaceRef surface{nullptr};

        /* Metal view of surface used by Qt Quick. */
        id<MTLTexture> metalTexture{nil};

        /* Qt scene graph wrapper around metalTexture. */
        QSGTexture *qsgTexture{nullptr};
    };

    /**
     * @brief Runs the OpenGL producer loop.
     */
    void run();

    /**
     * @brief Creates the worker-owned OpenGL context and mpv render context.
     */
    void createContext();

    /**
     * @brief Recreates the active frame generation for a size change.
     *
     * Old generations are retained until shutdown so Metal never observes an
     * IOSurface being destroyed while GPU reads may still be in flight.
     *
     * @param size The new frame size in physical pixels.
     */
    void recreateFrames(const QSize &size);

    /**
     * @brief Creates one IOSurface-backed OpenGL target.
     *
     * @param size The target size in physical pixels.
     * @return A newly created frame slot.
     */
    [[nodiscard]]
    FrameSlot createFrame(const QSize &size);

    /**
     * @brief Renders one frame into the next available active frame slot.
     */
    void renderFrame();

    /**
     * @brief Releases a worker-owned OpenGL frame slot.
     *
     * @param frame The frame to release.
     */
    void destroyFrame(FrameSlot &frame);

    /**
     * @brief Releases all worker-owned OpenGL resources.
     */
    void destroyWorkerResources();

    /* The player rendered by this backend. */
    MpvPlayer *m_player{nullptr};

    /* Shared mpv framebuffer submission logic. */
    MpvFrameBackend m_frameBackend;

    /* Synchronizes requests and frame publication between threads. */
    std::mutex m_mutex;

    /* Wakes the worker for resize, render, or shutdown requests. */
    std::condition_variable m_condition;

    /* Dedicated worker that owns all OpenGL objects. */
    std::thread m_thread;

    /* True once the worker should exit. */
    bool m_stopping{false};

    /* True when mpv has requested a new rendered frame. */
    bool m_renderRequested{true};

    /* Most recently requested frame size in physical pixels. */
    QSize m_requestedSize;

    /* Size of the currently active worker frame generation. */
    QSize m_activeSize;

    /* Active render targets used as a small producer ring. */
    std::vector<FrameSlot> m_frames;

    /* Old resize generations retained until worker shutdown. */
    std::vector<std::vector<FrameSlot>> m_retiredFrames;

    /* Index of the most recently completed active frame slot. */
    int m_latestFrame{-1};

    /* Index of the frame slot rendered most recently by the worker. */
    int m_writeFrame{-1};

    /* Monotonic serial assigned to completed frames. */
    quint64 m_nextSerial{1};

    /* Worker-owned OpenGL context. */
    std::unique_ptr<QOpenGLContext> m_context;

    /* Surface used to make m_context current on the worker. */
    std::unique_ptr<QOffscreenSurface> m_surface;

    /* Render-thread-owned wrappers for published IOSurfaces. */
    std::vector<MetalTexture> m_metalTextures;
};

MpvMetalRenderer::Impl::Impl(MpvPlayer *player) :
    m_player{player},
    m_frameBackend{player}
{
    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);

    /*
     * QOffscreenSurface native creation and destruction belong on the GUI
     * thread, although the surface may later be used by makeCurrent() on the
     * worker thread.
     */
    m_surface = std::make_unique<QOffscreenSurface>();
    m_surface->setFormat(format);
    m_surface->create();

    m_thread = std::thread{&Impl::run, this};
}

MpvMetalRenderer::Impl::~Impl()
{
    {
        std::scoped_lock lock{m_mutex};
        m_stopping = true;
    }
    m_condition.notify_one();
    m_thread.join();

    for (MetalTexture &texture : m_metalTextures)
    {
        delete texture.qsgTexture;
        texture.qsgTexture = nullptr;
        texture.metalTexture = nil;
        texture.surface = nullptr;
    }
    m_surface.reset();
}

void MpvMetalRenderer::Impl::setPixelSize(const QSize &pixelSize)
{
    {
        std::scoped_lock lock{m_mutex};
        if (m_requestedSize == pixelSize)
        {
            return;
        }
        m_requestedSize = pixelSize;
        m_renderRequested = true;
    }
    m_condition.notify_one();
}

void MpvMetalRenderer::Impl::requestRender()
{
    {
        std::scoped_lock lock{m_mutex};
        m_renderRequested = true;
    }
    m_condition.notify_one();
}

QSGTexture *MpvMetalRenderer::Impl::texture(QQuickWindow *window)
{
    IOSurfaceRef latestSurface{nullptr};
    QSize latestSize;
    {
        std::scoped_lock lock{m_mutex};
        if (m_latestFrame < 0)
        {
            return nullptr;
        }
        latestSurface = m_frames[m_latestFrame].surface;
        latestSize = m_activeSize;
    }

    for (const MetalTexture &texture : m_metalTextures)
    {
        if (texture.surface == latestSurface)
        {
            return texture.qsgTexture;
        }
    }

    MetalTexture texture;
    texture.surface = latestSurface;

    auto *rendererInterface = window->rendererInterface();
    id<MTLDevice> device = (__bridge id<MTLDevice>)rendererInterface->getResource(
        window, QSGRendererInterface::DeviceResource
    );
    MTLTextureDescriptor *descriptor = [
        MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat : MTLPixelFormatBGRA8Unorm
            width : latestSize.width()
            height : latestSize.height()
            mipmapped : NO
    ];
    descriptor.usage = MTLTextureUsageShaderRead;
    texture.metalTexture = [
        device
            newTextureWithDescriptor : descriptor
            iosurface : latestSurface
            plane : 0
    ];
    texture.qsgTexture = QNativeInterface::QSGMetalTexture::fromNative(
        texture.metalTexture, window, latestSize
    );
    m_metalTextures.emplace_back(texture);
    return m_metalTextures.back().qsgTexture;
}

void MpvMetalRenderer::Impl::run()
{
    createContext();

    while (true)
    {
        QSize requestedSize;
        bool shouldRender{false};
        {
            std::unique_lock lock{m_mutex};
            m_condition.wait(lock, [this] ()
            {
                return m_stopping || m_renderRequested ||
                    m_requestedSize != m_activeSize;
            });
            if (m_stopping)
            {
                break;
            }
            requestedSize = m_requestedSize;
            shouldRender = m_renderRequested;
            m_renderRequested = false;
        }

        if (!requestedSize.isEmpty() && requestedSize != m_activeSize)
        {
            recreateFrames(requestedSize);
            shouldRender = true;
        }
        if (shouldRender && !m_frames.empty())
        {
            renderFrame();
        }
    }

    destroyWorkerResources();
}

void MpvMetalRenderer::Impl::createContext()
{
    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);

    m_context = std::make_unique<QOpenGLContext>();
    m_context->setFormat(format);
    m_context->create();

    m_context->makeCurrent(m_surface.get());
    m_player->createRenderContext();
}

void MpvMetalRenderer::Impl::recreateFrames(const QSize &size)
{
    std::vector<FrameSlot> newFrames;
    newFrames.reserve(FRAME_SLOT_COUNT);
    for (int i = 0; i < FRAME_SLOT_COUNT; ++i)
    {
        newFrames.emplace_back(createFrame(size));
    }

    std::scoped_lock lock{m_mutex};
    if (!m_frames.empty())
    {
        m_retiredFrames.emplace_back(std::move(m_frames));
    }
    m_frames = std::move(newFrames);
    m_activeSize = size;
    m_latestFrame = -1;
    m_writeFrame = -1;
}

MpvMetalRenderer::Impl::FrameSlot
MpvMetalRenderer::Impl::createFrame(const QSize &size)
{
    FrameSlot frame;
    NSDictionary *properties = @{
        (id)kIOSurfaceWidth : @(size.width()),
        (id)kIOSurfaceHeight : @(size.height()),
        (id)kIOSurfaceBytesPerElement : @4,
        (id)kIOSurfacePixelFormat : @(kCVPixelFormatType_32BGRA),
    };
    frame.surface = IOSurfaceCreate((CFDictionaryRef)properties);

    glGenTextures(1, &frame.texture);
    glBindTexture(GL_TEXTURE_RECTANGLE, frame.texture);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    CGLTexImageIOSurface2D(
        CGLGetCurrentContext(),
        GL_TEXTURE_RECTANGLE,
        GL_RGBA8,
        size.width(),
        size.height(),
        GL_BGRA,
        GL_UNSIGNED_INT_8_8_8_8_REV,
        frame.surface,
        0
    );

    glGenFramebuffers(1, &frame.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame.framebuffer);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_RECTANGLE,
        frame.texture,
        0
    );
    return frame;
}

void MpvMetalRenderer::Impl::renderFrame()
{
    int frameIndex;
    QSize activeSize;
    {
        std::scoped_lock lock{m_mutex};
        frameIndex = (m_writeFrame + 1) % static_cast<int>(m_frames.size());
        m_writeFrame = frameIndex;
        activeSize = m_activeSize;
    }

    FrameSlot &frame = m_frames[frameIndex];
    glBindFramebuffer(GL_FRAMEBUFFER, frame.framebuffer);
    m_frameBackend.renderToFramebuffer(frame.framebuffer, activeSize);
    glFlush();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    {
        std::scoped_lock lock{m_mutex};
        frame.serial = m_nextSerial++;
        m_latestFrame = frameIndex;
    }

    /*
     * The scene graph update that requested this frame may already have run
     * before the worker finished rendering. Schedule a follow-up pass so
     * paused video and item-only resizes still present the newly published
     * frame.
     */
    QMetaObject::invokeMethod(
        m_player,
        [player = m_player] () { player->update(); },
        Qt::QueuedConnection
    );
}

void MpvMetalRenderer::Impl::destroyFrame(FrameSlot &frame)
{
    if (frame.framebuffer)
    {
        glDeleteFramebuffers(1, &frame.framebuffer);
        frame.framebuffer = 0;
    }
    if (frame.texture)
    {
        glDeleteTextures(1, &frame.texture);
        frame.texture = 0;
    }
    if (frame.surface)
    {
        CFRelease(frame.surface);
        frame.surface = nullptr;
    }
}

void MpvMetalRenderer::Impl::destroyWorkerResources()
{
    m_context->makeCurrent(m_surface.get());
    m_player->destroyRenderContext();

    for (FrameSlot &frame : m_frames)
    {
        destroyFrame(frame);
    }
    for (auto &generation : m_retiredFrames)
    {
        for (FrameSlot &frame : generation)
        {
            destroyFrame(frame);
        }
    }
    m_frames.clear();
    m_retiredFrames.clear();

    m_context->doneCurrent();
    m_context.reset();
}

MpvMetalRenderer::MpvMetalRenderer(MpvPlayer *player) :
    m_impl{std::make_unique<Impl>(player)}
{

}

MpvMetalRenderer::~MpvMetalRenderer() = default;

void MpvMetalRenderer::setPixelSize(const QSize &pixelSize)
{
    m_impl->setPixelSize(pixelSize);
}

void MpvMetalRenderer::requestRender()
{
    m_impl->requestRender();
}

QSGTexture *MpvMetalRenderer::texture(QQuickWindow *window)
{
    return m_impl->texture(window);
}

#endif // Q_OS_MACOS
