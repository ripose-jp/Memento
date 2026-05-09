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

#pragma once

#include <QQuickImageProvider>

/**
 * @brief Fills SVG with a requested color.
 */
class ColoredSvgProvider : public QQuickImageProvider
{
public:
    ColoredSvgProvider();
    virtual ~ColoredSvgProvider() = default;

    /**
     * @brief Request an image with a fill.
     *
     * @param id The ID of the requested image. Follows the format of
     * filename/color where images/filename.svg is filled with the requested
     * color. If color is omitted, the color is the system palette text color.
     * @param size The size of the requested image.
     * @param requestedSize The requested size of the image.
     * @return The requested icon with a fill. Empty image if failure.
     */
    [[nodiscard]]
    QImage requestImage(
        const QString &id, QSize *size, const QSize &requestedSize) override;
};
