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

#include "player/mpvrenderer.h"

#include <QQuickWindow>
#include <QOpenGLFramebufferObject>

MpvRenderer::MpvRenderer(MpvPlayer *player) : m_player{player}
{

}

QOpenGLFramebufferObject *MpvRenderer::createFramebufferObject(
    const QSize &size)
{
    if (m_player->renderContext() == nullptr)
    {
        m_player->createRenderContext();
    }
    return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
}

void MpvRenderer::render()
{
    m_player->window()->beginExternalCommands();

    QOpenGLFramebufferObject *fbo = framebufferObject();
    mpv_opengl_fbo mpfbo{
        .fbo = static_cast<int>(fbo->handle()),
        .w = fbo->width(),
        .h = fbo->height(),
        .internal_format = 0
    };
    int flip_y{0};

    mpv_render_param params[] = {
        // Specify the default framebuffer (0) as target. This will
        // render onto the entire screen. If you want to show the video
        // in a smaller rectangle or apply fancy transformations, you'll
        // need to render into a separate FBO and draw it manually.
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        // Flip rendering (needed due to flipped GL coordinate system).
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    // See render_gl.h on what OpenGL environment mpv expects, and
    // other API details.
    ::mpv_render_context_render(m_player->renderContext(), params);

    m_player->window()->endExternalCommands();
}
