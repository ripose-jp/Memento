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

#define GL_SILENCE_DEPRECATION

#include <dlfcn.h>
#include <utility>
#include <vector>

#include <QDebug>
#include <QFile>
#include <QThread>
#include <rhi/qrhi_platform.h>
#include <rhi/qrhi.h>
#include <rhi/qshader.h>

#import <CoreVideo/CVPixelBuffer.h>
#import <IOSurface/IOSurface.h>
#import <Metal/Metal.h>
#import <OpenGL/CGLIOSurface.h>
#import <OpenGL/gl3.h>
#import <OpenGL/OpenGL.h>

#include "player/mpvopenglframebackend.h"

/**
 * @brief Makes a CGL context current and restores the caller's context.
 */
class CglContextGuard final
{
public:
    /**
     * @brief Selects the supplied context on this thread.
     *
     * @param context The context to make current.
     */
    explicit CglContextGuard(CGLContextObj context) :
        m_previous{CGLGetCurrentContext()},
        m_valid{CGLSetCurrentContext(context) == kCGLNoError}
    {

    }

    /**
     * @brief Restores the context that was current before construction.
     */
    ~CglContextGuard()
    {
        CGLSetCurrentContext(m_previous);
    }

    /**
     * @brief Reports whether selecting the requested context succeeded.
     *
     * @return True if the requested context is current.
     */
    [[nodiscard]]
    bool isValid() const noexcept
    {
        return m_valid;
    }

private:
    /* The caller's context, which may be null with Qt's Metal backend. */
    CGLContextObj m_previous;

    /* Whether the requested context was successfully made current. */
    bool m_valid;
};

/**
 * @brief Owns the CGL-to-Metal bridge and its RHI presentation resources.
 */
class MpvMetalRenderer::Impl final
{
public:
    /**
     * @brief Connects backend notifications without creating GL resources.
     *
     * @param renderer The renderer forwarding notifications to the GUI.
     * @param mpv The core to render.
     */
    Impl(MpvMetalRenderer *renderer, std::shared_ptr<mpv_handle> mpv);

    /**
     * @brief Completes GPU work and releases resources on the render thread.
     */
    ~Impl();

    /**
     * @brief Initializes resources for Qt's current Metal device and target.
     *
     * @param rhi The RHI owning the current frame.
     * @param colorTexture Qt's single-sample output texture.
     */
    void initialize(QRhi *rhi, QRhiTexture *colorTexture);

    /**
     * @brief Renders mpv into the current slot and samples it in a RHI pass.
     *
     * @param cb The command buffer receiving the presentation pass.
     * @param target Qt's output render target.
     */
    void render(QRhiCommandBuffer *cb, QRhiRenderTarget *target);

private:
    /**
     * @brief Shared storage and API wrappers for one in-flight frame slot.
     */
    struct FrameSlot
    {
        /* Shared BGRA storage for GL rendering and Metal sampling. */
        IOSurfaceRef surface{nullptr};

        /* OpenGL rectangle texture attached to the shared storage. */
        GLuint texture{0};

        /* OpenGL framebuffer used by mpv. */
        GLuint framebuffer{0};

        /* Owned Metal texture view; QRhi does not own imported textures. */
        id<MTLTexture> metalTexture{nil};

        /* RHI view of metalTexture, used only by this frame slot. */
        std::unique_ptr<QRhiTexture> rhiTexture;

        /* Texture and sampler bindings for this slot. */
        std::unique_ptr<QRhiShaderResourceBindings> bindings;

        /* Allocated size in physical pixels. */
        QSize size;
    };

    /**
     * @brief Creates the private OpenGL 4.1 core context.
     *
     * @return True on success, or false if CGL initialization failed.
     */
    [[nodiscard]]
    bool createContext();

    /**
     * @brief Allocates a bridge slot after its previous GPU use has finished.
     *
     * @param frame The slot to allocate or resize.
     * @param size The required physical pixel size.
     * @return True if every GL, Metal, and RHI resource was created.
     */
    [[nodiscard]]
    bool createFrame(FrameSlot &frame, const QSize &size);

    /**
     * @brief Creates the presentation pipeline for the current target.
     *
     * @param target The render target defining the compatible render pass.
     * @return True if the pipeline was created successfully.
     */
    [[nodiscard]]
    bool createPipeline(QRhiRenderTarget *target);

    /**
     * @brief Releases a completed slot with the GL context current.
     *
     * @param frame The slot whose resources are released.
     */
    void destroyFrame(FrameSlot &frame);

    /**
     * @brief Stops callbacks and releases all resources before the core.
     * Safe to call repeatedly and only outside a render pass.
     */
    void releaseResources();

    /**
     * @brief Resolves an OpenGL symbol exported by the linked framework.
     *
     * @param ctx Unused callback context.
     * @param name The GL function name.
     * @return The function address, or nullptr if unavailable.
     */
    static void *getProcAddress(void *ctx, const char *name);

    /**
     * @brief Loads a compiled shader from the application's resources.
     *
     * @param path The embedded shader path.
     * @return The shader, or an invalid shader if loading failed.
     */
    static QShader loadShader(const char *path);

    /* Retains the mpv core until all native rendering resources are gone. */
    MpvOpenGlFrameBackend m_backend;

    /* Private CGL context used exclusively on Qt's render thread. */
    CGLContextObj m_context{nullptr};

    /* Borrowed RHI, kept for GPU completion during native-resource cleanup. */
    QRhi *m_rhi{nullptr};

    /* Borrowed Metal device used to construct views of IOSurfaces. */
    id<MTLDevice> m_device{nil};

    /* Bounded bridge storage indexed by QRhi's current frame slot. */
    std::vector<FrameSlot> m_frames;

    /* Shared linear sampler for the presentation pass. */
    std::unique_ptr<QRhiSampler> m_sampler;

    /* Pipeline drawing a full-screen triangle without a vertex buffer. */
    std::unique_ptr<QRhiGraphicsPipeline> m_pipeline;

    /* Stable binding layout independent of resized frame-slot textures. */
    std::unique_ptr<QRhiShaderResourceBindings> m_layout;

    /* Owned compatible pass descriptor independent of Qt's resized target. */
    std::unique_ptr<QRhiRenderPassDescriptor> m_renderPass;

    /* Stops retries and repeated diagnostics after a rendering failure. */
    bool m_failed{false};
};

/* Begin Constructor/Destructor */

MpvMetalRenderer::MpvMetalRenderer(std::shared_ptr<mpv_handle> mpv) :
    m_impl{std::make_unique<Impl>(this, std::move(mpv))}
{
    Q_INIT_RESOURCE(mpv_shaders);
}

MpvMetalRenderer::~MpvMetalRenderer() = default;

MpvMetalRenderer::Impl::Impl(
    MpvMetalRenderer *renderer, std::shared_ptr<mpv_handle> mpv) :
    m_backend{std::move(mpv)}
{
    QObject::connect(
        &m_backend, &MpvOpenGlFrameBackend::contextCreated,
        renderer, &MpvMetalRenderer::contextCreated, Qt::DirectConnection
    );
    QObject::connect(
        &m_backend, &MpvOpenGlFrameBackend::redraw,
        renderer, &MpvMetalRenderer::redraw, Qt::DirectConnection
    );
}

MpvMetalRenderer::Impl::~Impl()
{
    releaseResources();
}

/* End Constructor/Destructor */
/* Begin Initializers */

void MpvMetalRenderer::initialize([[maybe_unused]] QRhiCommandBuffer *cb)
{
    m_impl->initialize(rhi(), colorTexture());
}

void MpvMetalRenderer::Impl::initialize(QRhi *rhi, QRhiTexture *colorTexture)
{
    Q_ASSERT(QThread::currentThread() == m_backend.thread());
    if (m_rhi != rhi)
    {
        releaseResources();
        m_rhi = rhi;
        m_failed = false;
    }
    if (m_failed)
    {
        return;
    }
    if (rhi->backend() != QRhi::Metal ||
        !colorTexture ||
        colorTexture->format() != QRhiTexture::RGBA8)
    {
        qCritical(
            "MpvMetalRenderer: Metal and a single-sample RGBA8 target "
            "are required."
        );
        m_failed = true;
        return;
    }
    if (m_context)
    {
        return;
    }

    const auto *handles = static_cast<const QRhiMetalNativeHandles *>(
        rhi->nativeHandles()
    );
    m_device = (id<MTLDevice>)handles->dev;
    if (!m_device || !createContext())
    {
        qCritical("MpvMetalRenderer: Cannot create the native GL bridge.");
        m_failed = true;
        return;
    }

    CglContextGuard guard{m_context};
    if (!guard.isValid())
    {
        qCritical("MpvMetalRenderer: Cannot make the GL context current.");
        m_failed = true;
        return;
    }
    Q_ASSERT(CGLGetCurrentContext() == m_context);
    mpv_opengl_init_params initParams{};
    initParams.get_proc_address = getProcAddress;
    if (!m_backend.createRenderContext(initParams))
    {
        m_failed = true;
        return;
    }

    m_frames.resize(rhi->resourceLimit(QRhi::FramesInFlight));
    m_sampler.reset(rhi->newSampler(
        QRhiSampler::Linear,
        QRhiSampler::Linear,
        QRhiSampler::None,
        QRhiSampler::ClampToEdge,
        QRhiSampler::ClampToEdge
    ));
    if (!m_sampler->create())
    {
        qCritical("MpvMetalRenderer: Cannot create the presentation sampler.");
        m_failed = true;
    }
}

bool MpvMetalRenderer::Impl::createContext()
{
    const CGLPixelFormatAttribute attributes[]{
        kCGLPFAOpenGLProfile,
        static_cast<CGLPixelFormatAttribute>(kCGLOGLPVersion_GL4_Core),
        kCGLPFAAccelerated,
        kCGLPFAAllowOfflineRenderers,
        static_cast<CGLPixelFormatAttribute>(0)
    };
    CGLPixelFormatObj format{nullptr};
    GLint count{0};
    const CGLError error = CGLChoosePixelFormat(attributes, &format, &count);
    if (error != kCGLNoError || !format)
    {
        return false;
    }
    const CGLError contextError = CGLCreateContext(format, nullptr, &m_context);
    CGLDestroyPixelFormat(format);
    return contextError == kCGLNoError && m_context;
}

bool MpvMetalRenderer::Impl::createFrame(FrameSlot &frame, const QSize &size)
{
    destroyFrame(frame);
    NSDictionary *properties = @{
        (id)kIOSurfaceWidth : @(size.width()),
        (id)kIOSurfaceHeight : @(size.height()),
        (id)kIOSurfaceBytesPerElement : @4,
        (id)kIOSurfacePixelFormat : @(kCVPixelFormatType_32BGRA)
    };
    frame.surface = IOSurfaceCreate((CFDictionaryRef)properties);
    if (!frame.surface)
    {
        return false;
    }

    glGenTextures(1, &frame.texture);
    glBindTexture(GL_TEXTURE_RECTANGLE, frame.texture);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    const CGLError error = CGLTexImageIOSurface2D(
        m_context,
        GL_TEXTURE_RECTANGLE,
        GL_RGBA8,
        size.width(),
        size.height(),
        GL_BGRA,
        GL_UNSIGNED_INT_8_8_8_8_REV,
        frame.surface,
        0
    );
    if (error != kCGLNoError)
    {
        return false;
    }
    glGenFramebuffers(1, &frame.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frame.framebuffer);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_RECTANGLE,
        frame.texture,
        0
    );
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);

    MTLTextureDescriptor *descriptor = [
        MTLTextureDescriptor
            texture2DDescriptorWithPixelFormat : MTLPixelFormatBGRA8Unorm
            width : size.width()
            height : size.height()
            mipmapped : NO
    ];
    descriptor.usage = MTLTextureUsageShaderRead;
    frame.metalTexture = [
        m_device newTextureWithDescriptor : descriptor
            iosurface : frame.surface
            plane : 0
    ];
    if (!frame.metalTexture)
    {
        return false;
    }

    frame.rhiTexture.reset(m_rhi->newTexture(QRhiTexture::BGRA8, size));
    if (!frame.rhiTexture->createFrom({quint64(frame.metalTexture), 0}))
    {
        return false;
    }
    frame.bindings.reset(m_rhi->newShaderResourceBindings());
    frame.bindings->setBindings({
        QRhiShaderResourceBinding::sampledTexture(
            0,
            QRhiShaderResourceBinding::FragmentStage,
            frame.rhiTexture.get(),
            m_sampler.get()
        )
    });
    if (!frame.bindings->create())
    {
        return false;
    }
    frame.size = size;
    return true;
}

bool MpvMetalRenderer::Impl::createPipeline(QRhiRenderTarget *target)
{
    const QShader vertex = loadShader(":/player/mpv.vert.qsb");
    const QShader fragment = loadShader(":/player/mpv.frag.qsb");
    if (!vertex.isValid() || !fragment.isValid())
    {
        return false;
    }
    m_layout.reset(m_rhi->newShaderResourceBindings());
    m_layout->setBindings({
        QRhiShaderResourceBinding::sampledTexture(
            0, QRhiShaderResourceBinding::FragmentStage, nullptr, nullptr
        )
    });
    if (!m_layout->create())
    {
        return false;
    }
    m_renderPass.reset(
        target->renderPassDescriptor()->newCompatibleRenderPassDescriptor()
    );
    m_pipeline.reset(m_rhi->newGraphicsPipeline());
    m_pipeline->setShaderStages({
        {QRhiShaderStage::Vertex, vertex},
        {QRhiShaderStage::Fragment, fragment}
    });
    m_pipeline->setShaderResourceBindings(m_layout.get());
    m_pipeline->setRenderPassDescriptor(m_renderPass.get());
    return m_pipeline->create();
}

/* End Initializers */
/* Begin Rendering */

void MpvMetalRenderer::synchronize([[maybe_unused]] QQuickRhiItem *item)
{

}

void MpvMetalRenderer::render(QRhiCommandBuffer *cb)
{
    m_impl->render(cb, renderTarget());
}

void MpvMetalRenderer::Impl::render(
    QRhiCommandBuffer *cb, QRhiRenderTarget *target)
{
    Q_ASSERT(QThread::currentThread() == m_backend.thread());
    FrameSlot *frame{nullptr};
    if (!m_failed)
    {
        CglContextGuard guard{m_context};
        if (!guard.isValid())
        {
            qCritical("MpvMetalRenderer: Cannot make the GL context current.");
            m_failed = true;
        }
        else
        {
            Q_ASSERT(CGLGetCurrentContext() == m_context);
            /* Qt has completed the previous Metal use of this frame slot. */
            frame = &m_frames[m_rhi->currentFrameSlot()];
            if (frame->size != target->pixelSize() &&
                !createFrame(*frame, target->pixelSize()))
            {
                qCritical("MpvMetalRenderer: Cannot create a bridge texture.");
                m_failed = true;
            }
            if (!m_failed)
            {
                m_failed = !m_backend.renderToFramebuffer(
                    frame->framebuffer, frame->size, false
                );
            }
            /* IOSurface sharing alone does not synchronize the two APIs. */
            glFinish();
        }
        if (!m_failed && !m_pipeline && !createPipeline(target))
        {
            qCritical("MpvMetalRenderer: Cannot create the presentation pass.");
            m_failed = true;
        }
    }

    cb->beginPass(target, Qt::black, {1.0f, 0});
    if (!m_failed)
    {
        cb->setGraphicsPipeline(m_pipeline.get());
        cb->setShaderResources(frame->bindings.get());
        cb->setViewport(
            {0, 0, float(frame->size.width()), float(frame->size.height())}
        );
        cb->draw(3);
    }
    cb->endPass();
}

/* End Rendering */
/* Begin Cleanup */

void MpvMetalRenderer::Impl::destroyFrame(FrameSlot &frame)
{
    if (frame.framebuffer)
    {
        glDeleteFramebuffers(1, &frame.framebuffer);
    }
    if (frame.texture)
    {
        glDeleteTextures(1, &frame.texture);
    }
    frame.bindings.reset();
    frame.rhiTexture.reset();
    [frame.metalTexture release];
    if (frame.surface)
    {
        CFRelease(frame.surface);
    }
    frame = FrameSlot{};
}

void MpvMetalRenderer::Impl::releaseResources()
{
    Q_ASSERT(QThread::currentThread() == m_backend.thread());
    m_backend.stopUpdates();
    if (m_rhi)
    {
        /* Imported native textures must outlive all in-flight Metal reads. */
        m_rhi->finish();
    }
    if (m_context)
    {
        {
            CglContextGuard guard{m_context};
            if (!guard.isValid())
            {
                qFatal(
                    "MpvMetalRenderer: Cannot make cleanup context current."
                );
            }
            Q_ASSERT(CGLGetCurrentContext() == m_context);
            m_backend.destroyRenderContext();
            glFinish();
            m_pipeline.reset();
            m_layout.reset();
            m_renderPass.reset();
            for (FrameSlot &frame : m_frames)
            {
                destroyFrame(frame);
            }
            m_frames.clear();
            m_sampler.reset();
        }
        if (CGLGetCurrentContext() == m_context)
        {
            CGLSetCurrentContext(nullptr);
        }
        CGLDestroyContext(m_context);
        m_context = nullptr;
    }
    m_device = nil;
    m_rhi = nullptr;
}

/* End Cleanup */
/* Begin Native Resources */

void *MpvMetalRenderer::Impl::getProcAddress(
    [[maybe_unused]] void *ctx, const char *name)
{
    return dlsym(RTLD_DEFAULT, name);
}

QShader MpvMetalRenderer::Impl::loadShader(const char *path)
{
    QFile file{QString::fromUtf8(path)};
    if (!file.open(QIODevice::ReadOnly))
    {
        return {};
    }
    return QShader::fromSerialized(file.readAll());
}

/* End Native Resources */
