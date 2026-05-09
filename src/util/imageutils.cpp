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

#include "util/imageutils.h"

#include <QDebug>
#include <QImage>
#include <QTemporaryFile>

QString ImageUtils::resizeImage(
    const QString &filePath,
    const QString &ext,
    int maxWidth,
    int maxHeight,
    bool keepAspectRatio)
{
    if ((maxWidth != -1 && maxWidth <= 0) ||
        (maxHeight != -1 && maxHeight <= 0))
    {
        return {};
    }

    QImage originalImage(filePath);
    if (originalImage.isNull())
    {
        qWarning("Failed to load image");
        return {};
    }

    const int originalWidth = originalImage.width();
    const int originalHeight = originalImage.height();
    const int targetWidth = maxWidth == -1 ?
        originalWidth : std::min(maxWidth, originalWidth);
    const int targetHeight = maxHeight == -1 ?
        originalHeight : std::min(maxHeight, originalHeight);

    QImage finalImage = originalImage;
    bool scalingWasDone = true;
    if (keepAspectRatio)
    {
        if (maxWidth != -1 && maxHeight != -1)
        {
            finalImage = originalImage.scaled(
                targetWidth,
                targetHeight,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            );
        }
        else if (maxWidth != -1)
        {
            finalImage = originalImage.scaledToWidth(
                targetWidth, Qt::SmoothTransformation
            );
        }
        else if (maxHeight != -1)
        {
            finalImage = originalImage.scaledToHeight(
                targetHeight, Qt::SmoothTransformation
            );
        }
        else
        {
            scalingWasDone = false;
        }
    }
    else if (maxWidth != -1 ||
        maxHeight != -1 ||
        targetWidth != originalWidth ||
        targetHeight != originalHeight)
    {
        finalImage = originalImage.scaled(
            targetWidth,
            targetHeight,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
        );
    }
    else
    {
        scalingWasDone = false;
    }

    if (scalingWasDone && finalImage.isNull())
    {
        qWarning("New image is null after scaling");
        return {};
    }

    QTemporaryFile newFilePath;
    if (!newFilePath.open())
    {
        return {};
    }
    QString newFileName = newFilePath.fileName() + ext;
    newFilePath.close();
    if (!finalImage.save(newFileName))
    {
        qWarning("Failed to save image");
        return {};
    }

    return newFileName;
}

QString ImageUtils::generatePitchGraph(
    const int moraSize,
    const uint8_t pos,
    const QString &fill,
    const QString &stroke)
{
    /**
     * @brief Header for pitch graphs.
     *
     * @param 1 Width of the viewBox.
     * @param 2 Fill color. Hex string format.
     * @param 3 Stroke color. Hex string format.
     */
    static const QString PITCH_GRAPH_HEADER =
        "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"2em\" viewBox=\"0 0 %1 100\" style=\"display: inline-block;\">"
        "<defs>"
        "<g id=\"black-dot\">"
        "<circle cx=\"0\" cy=\"0\" r=\"15\" style=\"fill: %3; stroke: %3; stroke-width: 5px;\"/>"
        "</g>"
        "<g id=\"white-dot\">"
        "<circle cx=\"0\" cy=\"0\" r=\"15\" style=\"fill: %2; stroke: %3; stroke-width: 5px;\"/>"
        "</g>"
        "<g id=\"change-dot\">"
        "<circle cx=\"0\" cy=\"0\" r=\"15\" style=\"fill: %2; stroke: %3; stroke-width: 5px;\"/>"
        "<circle cx=\"0\" cy=\"0\" r=\"5\" style=\"fill: %2; stroke: %3; stroke-width: 5px;\"/>"
        "</g>"
        "</defs>";

    static const QString PITCH_GRAPH_TRAILER = "</svg>";

    /**
     * @brief Create a dot filled with the stroke color and stroked with the
     * stroke color.
     *
     * @param 1 The x coordinate.
     * @param 2 The y coordinate.
     */
    static const QString BLACK_DOT_FORMAT =
        "<use href=\"#black-dot\" x=\"%1\" y=\"%2\"/>";

    /**
     * @brief Create a dot filled with the fill color and stroked with the
     * stroke color.
     *
     * @param 1 The x coordinate.
     * @param 2 The y coordinate.
     */
    static const QString WHITE_DOT_FORMAT =
        "<use href=\"#white-dot\" x=\"%1\" y=\"%2\"/>";

    /**
     * @brief Create a dot filled with the fill color and stroked with the
     * stroke color.
     *
     * @param 1 The x coordinate.
     * @param 2 The y coordinate.
     */
    static const QString CHANGE_DOT_FORMAT =
        "<use href=\"#change-dot\" x=\"%1\" y=\"%2\"/>";

    /**
     * @brief Draws a line.
     *
     * @param 1 The color of the line.
     * @param 2 The starting x coordinate.
     * @param 3 The starting y coordinate.
     * @param 4 The ending x coordinate.
     * @param 5 The ending y coordinate.
     */
    static const QString LINE_FORMAT =
        "<path d=\"M%2 %3 L%4 %5\" style=\"stroke: %1; stroke-width: 5px;\"/>";

    constexpr int RADIUS = 15;
    /* Literally r * sin(pi / 4) */
    constexpr int DIAGONAL_OFFSET = 10;

    constexpr int GRAPH_TOP_Y = 25;
    constexpr int GRAPH_BOTTOM_Y = 75;
    constexpr int GRAPH_STEP = 50;
    constexpr int GRAPH_OFFSET = 25;

    QString pitchGraph = PITCH_GRAPH_HEADER
        .arg(GRAPH_STEP * (moraSize + 1))
        .arg(fill)
        .arg(stroke);
    QString lineFormat = LINE_FORMAT.arg(stroke);

    QStringList lines;
    QStringList dots;

    switch (pos)
    {
    case 0:
    {
        int x1 = GRAPH_OFFSET;
        int y1 = GRAPH_BOTTOM_Y;
        int x2 = GRAPH_OFFSET + GRAPH_STEP;
        int y2 = GRAPH_TOP_Y;

        lines.append(
            lineFormat
                .arg(x1 + DIAGONAL_OFFSET).arg(y1 - DIAGONAL_OFFSET)
                .arg(x2 - DIAGONAL_OFFSET).arg(y2 + DIAGONAL_OFFSET)
        );
        dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));

        int i;
        for (i = 1; i < moraSize; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_TOP_Y;

            lines.append(
                lineFormat.arg(x1 + RADIUS).arg(y1).arg(x2 - RADIUS).arg(y2)
            );
            dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));
        }

        dots.append(WHITE_DOT_FORMAT.arg(x2).arg(y2));
        break;
    }
    case 1:
    {
        int x1 = GRAPH_OFFSET;
        int y1 = GRAPH_TOP_Y;
        int x2 = GRAPH_OFFSET + GRAPH_STEP;
        int y2 = GRAPH_BOTTOM_Y;

        lines.append(
            lineFormat
                .arg(x1 + DIAGONAL_OFFSET).arg(y1 + DIAGONAL_OFFSET)
                .arg(x2 - DIAGONAL_OFFSET).arg(y2 - DIAGONAL_OFFSET)
        );
        dots.append(CHANGE_DOT_FORMAT.arg(x1).arg(y1));

        int i;
        for (i = 1; i < moraSize; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_BOTTOM_Y;

            lines.append(
                lineFormat.arg(x1 + RADIUS).arg(y1).arg(x2 - RADIUS).arg(y2)
            );
            dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));
        }

        dots.append(WHITE_DOT_FORMAT.arg(x2).arg(y2));
        break;
    }
    default:
    {
        int x1 = GRAPH_OFFSET;
        int y1 = GRAPH_BOTTOM_Y;
        int x2 = GRAPH_OFFSET + GRAPH_STEP;
        int y2 = GRAPH_TOP_Y;

        lines.append(
            lineFormat
                .arg(x1 + DIAGONAL_OFFSET).arg(y1 - DIAGONAL_OFFSET)
                .arg(x2 - DIAGONAL_OFFSET).arg(y2 + DIAGONAL_OFFSET)
        );
        dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));

        int i;
        for (i = 1; i < pos - 1; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_TOP_Y;

            lines.append(
                lineFormat.arg(x1 + RADIUS).arg(y1).arg(x2 - RADIUS).arg(y2)
            );
            dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));
        }

        /* Change dot */
        x1 = x2;
        y1 = y2;
        x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
        y2 = GRAPH_BOTTOM_Y;

        lines.append(
            lineFormat
                .arg(x1 + DIAGONAL_OFFSET).arg(y1 + DIAGONAL_OFFSET)
                .arg(x2 - DIAGONAL_OFFSET).arg(y2 - DIAGONAL_OFFSET)
        );
        dots.append(CHANGE_DOT_FORMAT.arg(x1).arg(y1));

        /* End dots */
        for (i = pos; i < moraSize; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_BOTTOM_Y;

            lines.append(
                lineFormat.arg(x1 + RADIUS).arg(y1).arg(x2 - RADIUS).arg(y2)
            );
            dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));
        }

        dots.append(WHITE_DOT_FORMAT.arg(x2).arg(GRAPH_BOTTOM_Y));
    }
    }

    pitchGraph += lines.join("\n");
    pitchGraph += dots.join("\n");

    pitchGraph += PITCH_GRAPH_TRAILER;

    return pitchGraph;
}
