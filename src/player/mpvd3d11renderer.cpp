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

#include "player/mpvd3d11renderer.h"

#include <utility>

#include <QDebug>

#include <rhi/qrhi_platform.h>
#include <rhi/qrhi.h>

#include <d3d11.h>

#include <mpv/render_d3d11.h>

/* Begin Constructor/Destructor */

MpvD3D11Renderer::MpvD3D11Renderer(std::shared_ptr<mpv_handle> mpv) :
    m_mpv{std::move(mpv)}
{

}

MpvD3D11Renderer::~MpvD3D11Renderer()
{
    destroyRenderContext();
}

/* End Constructor/Destructor */
/* Begin Render Context */

void MpvD3D11Renderer::createRenderContext()
{
    mpv_d3d11_init_params initParams{m_device};
    mpv_render_param params[]{
        {
            MPV_RENDER_PARAM_API_TYPE,
            const_cast<char *>(MPV_RENDER_API_TYPE_D3D11)
        },
        {MPV_RENDER_PARAM_D3D11_INIT_PARAMS, &initParams},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    const int error = ::mpv_render_context_create(
        &m_renderContext, m_mpv.get(), params
    );
    if (error < 0)
    {
        qCritical(
            "MpvD3D11Renderer: Cannot initialize D3D11: %s. "
            "Use mpv 0.42 or newer with D3D11 support.",
            ::mpv_error_string(error)
        );
        m_failed = true;
        return;
    }

    ::mpv_render_context_set_update_callback(
        m_renderContext, onMpvRedraw, this
    );
    emit contextCreated();
}

void MpvD3D11Renderer::destroyRenderContext()
{
    if (m_renderContext == nullptr)
    {
        return;
    }

    ::mpv_render_context_set_update_callback(m_renderContext, nullptr, nullptr);
    ::mpv_render_context_free(m_renderContext);
    m_renderContext = nullptr;
}

/* End Render Context */
/* Begin Rendering */

void MpvD3D11Renderer::initialize([[maybe_unused]] QRhiCommandBuffer *cb)
{
    if (rhi()->backend() != QRhi::D3D11)
    {
        if (!m_failed)
        {
            qCritical("MpvD3D11Renderer: Qt Quick must use Direct3D 11.");
        }
        m_failed = true;
        return;
    }

    const auto *handles = static_cast<const QRhiD3D11NativeHandles *>(
        rhi()->nativeHandles()
    );
    if (m_device != handles->dev)
    {
        destroyRenderContext();
        m_device = handles->dev;
        m_failed = false;
    }
}

void MpvD3D11Renderer::synchronize([[maybe_unused]] QQuickRhiItem *item)
{

}

void MpvD3D11Renderer::render(QRhiCommandBuffer *cb)
{
    cb->beginPass(
        renderTarget(), Qt::black, {1.0f, 0}, nullptr,
        QRhiCommandBuffer::ExternalContent
    );

    if (!m_failed)
    {
        const auto *handles = static_cast<const QRhiD3D11NativeHandles *>(
            rhi()->nativeHandles()
        );
        auto *context = static_cast<ID3D11DeviceContext *>(handles->context);
        if (m_device == nullptr || context == nullptr || !colorTexture())
        {
            qCritical(
                "MpvD3D11Renderer: A D3D11 device and a single-sample "
                "color texture are required."
            );
            m_failed = true;
        }
        else
        {
            cb->beginExternal();
            context->ClearState();

            if (m_renderContext == nullptr)
            {
                createRenderContext();
            }
            if (m_renderContext)
            {
                const auto texture = colorTexture()->nativeTexture();
                mpv_d3d11_fbo fbo{
                    reinterpret_cast<void *>(texture.object)
                };
                int blockForTargetTime{0};
                mpv_render_param params[]{
                    {MPV_RENDER_PARAM_D3D11_FBO, &fbo},
                    {
                        MPV_RENDER_PARAM_BLOCK_FOR_TARGET_TIME,
                        &blockForTargetTime
                    },
                    {MPV_RENDER_PARAM_INVALID, nullptr}
                };
                const int error = ::mpv_render_context_render(
                    m_renderContext, params
                );
                if (error < 0)
                {
                    qCritical(
                        "MpvD3D11Renderer: Cannot render frame: %s",
                        ::mpv_error_string(error)
                    );
                    m_failed = true;
                }
            }

            /* Unbind mpv's target before Qt samples it in the main pass. */
            context->ClearState();
            cb->endExternal();
        }
    }
    cb->endPass();

    if (m_failed)
    {
        /* A failed mpv call may have partially rendered into the texture. */
        cb->beginPass(renderTarget(), Qt::black, {1.0f, 0});
        cb->endPass();
    }
}

/* End Rendering */
/* Begin Callbacks */

void MpvD3D11Renderer::onMpvRedraw(void *ctx)
{
    auto *renderer = static_cast<MpvD3D11Renderer *>(ctx);
    emit renderer->redraw();
}

/* End Callbacks */
