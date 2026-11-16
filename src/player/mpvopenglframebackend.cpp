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

#include "player/mpvopenglframebackend.h"

#include <utility>

#include <QDebug>
#include <QThread>

/* Begin Constructor/Destructor */

MpvOpenGlFrameBackend::MpvOpenGlFrameBackend(
    std::shared_ptr<mpv_handle> mpv) :
    m_mpv{std::move(mpv)}
{

}

MpvOpenGlFrameBackend::~MpvOpenGlFrameBackend()
{
    Q_ASSERT(m_renderContext == nullptr);
}

/* End Constructor/Destructor */
/* Begin Render Context */

mpv_render_context *MpvOpenGlFrameBackend::renderContext() const noexcept
{
    return m_renderContext;
}

bool MpvOpenGlFrameBackend::createRenderContext(
    mpv_opengl_init_params initParams, mpv_render_param display)
{
    Q_ASSERT(QThread::currentThread() == thread());
    if (m_renderContext)
    {
        return true;
    }

    mpv_render_param params[]{
        {
            MPV_RENDER_PARAM_API_TYPE,
            const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)
        },
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &initParams},
        display,
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    const int error = ::mpv_render_context_create(
        &m_renderContext, m_mpv.get(), params
    );
    if (error < 0)
    {
        qCritical(
            "MpvOpenGlFrameBackend: Cannot initialize OpenGL: %s",
            ::mpv_error_string(error)
        );
        return false;
    }

    ::mpv_render_context_set_update_callback(
        m_renderContext, onMpvRedraw, this
    );
    emit contextCreated();
    return true;
}

void MpvOpenGlFrameBackend::stopUpdates()
{
    if (m_renderContext)
    {
        ::mpv_render_context_set_update_callback(
            m_renderContext, nullptr, nullptr
        );
    }
}

void MpvOpenGlFrameBackend::destroyRenderContext()
{
    Q_ASSERT(QThread::currentThread() == thread());
    if (m_renderContext == nullptr)
    {
        return;
    }

    stopUpdates();
    ::mpv_render_context_free(m_renderContext);
    m_renderContext = nullptr;
}

/* End Render Context */
/* Begin Rendering */

bool MpvOpenGlFrameBackend::renderToFramebuffer(
    unsigned int framebuffer, const QSize &size, bool flipY)
{
    Q_ASSERT(QThread::currentThread() == thread());
    Q_ASSERT(m_renderContext);
    mpv_opengl_fbo fbo{
        static_cast<int>(framebuffer), size.width(), size.height(), 0
    };
    int flip{flipY};
    int blockForTargetTime{0};
    mpv_render_param params[]{
        {MPV_RENDER_PARAM_OPENGL_FBO, &fbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip},
        {MPV_RENDER_PARAM_BLOCK_FOR_TARGET_TIME, &blockForTargetTime},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    const int error = ::mpv_render_context_render(m_renderContext, params);
    if (error < 0)
    {
        qCritical(
            "MpvOpenGlFrameBackend: Cannot render frame: %s",
            ::mpv_error_string(error)
        );
        return false;
    }
    return true;
}

/* End Rendering */
/* Begin Callbacks */

void MpvOpenGlFrameBackend::onMpvRedraw(void *ctx)
{
    auto *backend = static_cast<MpvOpenGlFrameBackend *>(ctx);
    emit backend->redraw();
}

/* End Callbacks */
