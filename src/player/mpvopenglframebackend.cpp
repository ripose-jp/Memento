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

#include <QOpenGLContext>

#include "player/mpvplayer.h"

/* Begin Constructor/Destructor */

MpvOpenGlFrameBackend::MpvOpenGlFrameBackend(
    QObject *parent, MpvPlayer *player) :
    QObject(parent),
    m_player{player}
{

}

MpvOpenGlFrameBackend::~MpvOpenGlFrameBackend()
{
    destroyRenderContext();
}

/* End Constructor/Destructor */
/* Begin Lifetime Functions */

mpv_render_context *MpvOpenGlFrameBackend::renderContext() const noexcept
{
    return m_mpv_gl;
}

void MpvOpenGlFrameBackend::createRenderContext()
{
    if (m_mpv_gl)
    {
        return;
    }

    mpv_opengl_init_params gl_init_params{};
    gl_init_params.get_proc_address = &MpvOpenGlFrameBackend::getProcAddressMpv;
    gl_init_params.get_proc_address_ctx = nullptr;
#if MPV_CLIENT_API_VERSION < MPV_MAKE_VERSION(2, 0)
    gl_init_params.extra_exts = nullptr;
#endif
    mpv_render_param params[]{
        {
            MPV_RENDER_PARAM_API_TYPE,
            const_cast<char *>(MPV_RENDER_API_TYPE_OPENGL)
        },
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr},
        {MPV_RENDER_PARAM_INVALID, nullptr},
    };

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    if (auto x11App = qApp->nativeInterface<QNativeInterface::QX11Application>())
    {
        params[2].type = MPV_RENDER_PARAM_X11_DISPLAY;
        params[2].data = x11App->display();
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    else if (auto wlApp = qApp->nativeInterface<QNativeInterface::QWaylandApplication>())
    {
        params[2].type = MPV_RENDER_PARAM_WL_DISPLAY;
        params[2].data = wlApp->display();
    }
#endif // QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
#endif // defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)

    if (::mpv_render_context_create(&m_mpv_gl, m_player->handle(), params) < 0)
    {
        throw std::runtime_error(
            "MpvPlayer: Failed to initialize mpv GL context"
        );
    }
    ::mpv_render_context_set_update_callback(
        m_mpv_gl, &MpvOpenGlFrameBackend::onMpvRedraw, this
    );

    emit contextCreated();
}

void MpvOpenGlFrameBackend::destroyRenderContext()
{
    if (m_mpv_gl == nullptr)
    {
        return;
    }

    ::mpv_render_context_set_update_callback(m_mpv_gl, nullptr, nullptr);
    ::mpv_render_context_free(m_mpv_gl);
    m_mpv_gl = nullptr;
}

/* End Lifetime Functions */
/* Begin Rendering */

void MpvOpenGlFrameBackend::renderToFramebuffer(
    unsigned int framebuffer, const QSize &size)
{
    mpv_opengl_fbo mpfbo{
        .fbo = static_cast<int>(framebuffer),
        .w = size.width(),
        .h = size.height(),
        .internal_format = 0
    };

    mpv_render_param params[]{
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    ::mpv_render_context_render(renderContext(), params);
}

/* End Rendering */
/* Begin Static Callbacks */

void MpvOpenGlFrameBackend::onMpvRedraw(void *ctx)
{
    MpvOpenGlFrameBackend *self =
        reinterpret_cast<MpvOpenGlFrameBackend *>(ctx);
    emit self->redraw();
}

void *MpvOpenGlFrameBackend::getProcAddressMpv(
    [[maybe_unused]] void *ctx, const char *name)
{
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (glctx == nullptr)
    {
        return nullptr;
    }
    return reinterpret_cast<void *>(glctx->getProcAddress(QByteArray(name)));
}

/* End Static Callbacks */
