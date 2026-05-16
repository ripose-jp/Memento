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

#include "player/mpvframebackend.h"

#include <mpv/render_gl.h>

#include "player/mpvplayer.h"

MpvFrameBackend::MpvFrameBackend(MpvPlayer *player) : m_player{player}
{

}

void MpvFrameBackend::renderToFramebuffer(
    unsigned int framebuffer, const QSize &size)
{
    mpv_opengl_fbo mpfbo{
        .fbo = static_cast<int>(framebuffer),
        .w = size.width(),
        .h = size.height(),
        .internal_format = 0
    };
    int flipY{0};

    mpv_render_param params[]{
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpfbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flipY},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };
    ::mpv_render_context_render(m_player->renderContext(), params);
}
