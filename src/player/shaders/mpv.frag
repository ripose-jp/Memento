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

#version 440

layout(binding = 0) uniform sampler2D frameTexture;
layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 fragColor;

/* Presents the BGRA texture through its typed view as opaque video. */
void main()
{
    fragColor = vec4(texture(frameTexture, texCoord).rgb, 1.0);
}
