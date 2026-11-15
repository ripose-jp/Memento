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

#include <QOpenGLFramebufferObject>
#include <QQuickWindow>

#include "player/mpvplayer.h"

/* Begin Constructor/Destructor */

MpvOpenGlRenderer::MpvOpenGlRenderer(
    MpvPlayer *player, MpvOpenGlFrameBackend *backend) :
    m_player{player},
    m_frameBackend{backend}
{

}

/* End Constructor/Destructor */
/* Begin Rendering */

QOpenGLFramebufferObject *MpvOpenGlRenderer::createFramebufferObject(
    const QSize &size)
{
    if (m_frameBackend->renderContext() == nullptr)
    {
        m_frameBackend->createRenderContext();
    }
    return QQuickFramebufferObject::Renderer::createFramebufferObject(size);
}

void MpvOpenGlRenderer::render()
{
    m_player->window()->beginExternalCommands();

    QOpenGLFramebufferObject *fbo = framebufferObject();
    m_frameBackend->renderToFramebuffer(fbo->handle(), fbo->size());

    m_player->window()->endExternalCommands();
}

/* End Rendering */
