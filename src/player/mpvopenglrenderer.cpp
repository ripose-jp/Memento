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

#include "player/mpvopenglrenderer.h"

#include <utility>

#include <QDebug>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QQuickOpenGLUtils>
#include <QThread>
#include <rhi/qrhi_platform.h>
#include <rhi/qrhi.h>

/* Begin Constructor/Destructor */

MpvOpenGlRenderer::MpvOpenGlRenderer(std::shared_ptr<mpv_handle> mpv) :
    m_backend{std::move(mpv)}
{
    connect(
        &m_backend, &MpvOpenGlFrameBackend::contextCreated,
        this, &MpvOpenGlRenderer::contextCreated, Qt::DirectConnection
    );
    connect(
        &m_backend, &MpvOpenGlFrameBackend::redraw,
        this, &MpvOpenGlRenderer::redraw, Qt::DirectConnection
    );
}

MpvOpenGlRenderer::~MpvOpenGlRenderer()
{
    releaseResources();
}

/* End Constructor/Destructor */
/* Begin Rendering */

void MpvOpenGlRenderer::initialize([[maybe_unused]] QRhiCommandBuffer *cb)
{
    Q_ASSERT(QThread::currentThread() == thread());
    if (rhi()->backend() != QRhi::OpenGLES2)
    {
        if (!m_failed)
        {
            qCritical("MpvOpenGlRenderer: Qt Quick must use OpenGL.");
        }
        m_failed = true;
        return;
    }

    const auto *handles = static_cast<const QRhiGles2NativeHandles *>(
        rhi()->nativeHandles()
    );
    if (m_rhi != rhi() || m_context != handles->context)
    {
        releaseResources();
        m_rhi = rhi();
        m_context = handles->context;
        m_failed = false;
    }
    if (!m_failed &&
        (!m_context || !colorTexture() ||
        colorTexture()->format() != QRhiTexture::RGBA8))
    {
        qCritical(
            "MpvOpenGlRenderer: An OpenGL context and a single-sample "
            "RGBA8 target are required."
        );
        m_failed = true;
    }
}

void MpvOpenGlRenderer::synchronize([[maybe_unused]] QQuickRhiItem *item)
{

}

void MpvOpenGlRenderer::render(QRhiCommandBuffer *cb)
{
    cb->beginPass(
        renderTarget(),
        Qt::black,
        {1.0f, 0},
        nullptr,
        QRhiCommandBuffer::ExternalContent
    );
    if (!m_failed)
    {
        cb->beginExternal();
        Q_ASSERT(QOpenGLContext::currentContext() == m_context);
        QOpenGLFunctions *gl = m_context->functions();
        GLint framebuffer{0};
        gl->glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
        QQuickOpenGLUtils::resetOpenGLState();

        mpv_opengl_init_params initParams{};
        initParams.get_proc_address = getProcAddress;
        m_failed = !m_backend.createRenderContext(initParams, nativeDisplay());
        if (!m_failed)
        {
            /* QQuickRhiItem flips GL texture coordinates at presentation. */
            m_failed = !m_backend.renderToFramebuffer(
                framebuffer, renderTarget()->pixelSize(), true
            );
        }

        QQuickOpenGLUtils::resetOpenGLState();
        gl->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        cb->endExternal();
    }
    cb->endPass();

    if (m_failed)
    {
        cb->beginPass(renderTarget(), Qt::black, {1.0f, 0});
        cb->endPass();
    }
}

void MpvOpenGlRenderer::releaseResources()
{
    Q_ASSERT(QThread::currentThread() == thread());
    m_backend.stopUpdates();
    if (m_backend.renderContext())
    {
        if (!m_rhi->makeThreadLocalNativeContextCurrent())
        {
            qFatal("MpvOpenGlRenderer: Cannot make cleanup context current.");
        }
        Q_ASSERT(QOpenGLContext::currentContext() == m_context);
        QQuickOpenGLUtils::resetOpenGLState();
        m_backend.destroyRenderContext();
        QQuickOpenGLUtils::resetOpenGLState();
    }
    m_context = nullptr;
    m_rhi = nullptr;
}

/* End Rendering */
/* Begin Native OpenGL */

void *MpvOpenGlRenderer::getProcAddress(
    [[maybe_unused]] void *ctx, const char *name)
{
    auto *context = QOpenGLContext::currentContext();
    return context ? reinterpret_cast<void *>(context->getProcAddress(name)) :
        nullptr;
}

mpv_render_param MpvOpenGlRenderer::nativeDisplay()
{
#if QT_CONFIG(xcb)
    auto *x11 = qGuiApp->nativeInterface<QNativeInterface::QX11Application>();
    if (x11)
    {
        return {MPV_RENDER_PARAM_X11_DISPLAY, x11->display()};
    }
#endif
#if QT_CONFIG(wayland)
    auto *wayland =
        qGuiApp->nativeInterface<QNativeInterface::QWaylandApplication>();
    if (wayland)
    {
        return {MPV_RENDER_PARAM_WL_DISPLAY, wayland->display()};
    }
#endif
    return {MPV_RENDER_PARAM_INVALID, nullptr};
}

/* End Native OpenGL */
