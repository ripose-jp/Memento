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

#include "quick/coloredsvgprovider.h"

#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QPalette>
#include <QSvgRenderer>

ColoredSvgProvider::ColoredSvgProvider() :
    QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage ColoredSvgProvider::requestImage(
    const QString &id, QSize *size, const QSize &requestedSize)
{
    constexpr qsizetype IMAGE_NAME_INDEX = 0;
    constexpr qsizetype IMAGE_COLOR_INDEX = 1;

    QStringList parts = id.split('/');
    if (parts.size() <= IMAGE_NAME_INDEX)
    {
        return QImage();
    }

    QString imageName = parts[IMAGE_NAME_INDEX];
    QString sourcePath = QString(":/images/%1.svg").arg(imageName);

    QColor color = parts.size() > IMAGE_COLOR_INDEX ?
        QColor("#" + parts[IMAGE_COLOR_INDEX]) :
        qApp->palette().color(QPalette::Text);

    QSvgRenderer renderer(sourcePath);
    if (!renderer.isValid())
    {
        return QImage();
    }

    QSize finalSize = renderer.defaultSize();
    if (requestedSize.isValid())
    {
        finalSize =
            renderer.defaultSize().scaled(requestedSize, Qt::KeepAspectRatio);
    }
    if (size)
    {
        *size = finalSize;
    }

    QImage image(finalSize, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::LosslessImageRendering);
    renderer.render(&painter);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(image.rect(), color);
    painter.end();

    return image;
}
