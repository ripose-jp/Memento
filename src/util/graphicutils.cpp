////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#include "graphicutils.h"

#include <QStringList>

#define PITCH_GRAPH_HEADER (QString(                                                                                    \
    "<svg xmlns=\"http://www.w3.org/2000/svg\" height=\"2em\" viewBox=\"0 0 %1 100\" style=\"display: inline-block;\">" \
    "<defs>"                                                                                                            \
    "<g id=\"black-dot\">"                                                                                              \
    "<circle cx=\"0\" cy=\"0\" r=\"15\" style=\"fill: %3; stroke: %3; stroke-width: 5px;\"/>"                           \
    "</g>"                                                                                                              \
    "<g id=\"white-dot\">"                                                                                              \
    "<circle cx=\"0\" cy=\"0\" r=\"15\" style=\"fill: %2; stroke: %3; stroke-width: 5px;\"/>"                           \
    "</g>"                                                                                                              \
    "<g id=\"change-dot\">"                                                                                             \
    "<circle cx=\"0\" cy=\"0\" r=\"15\" style=\"fill: %2; stroke: %3; stroke-width: 5px;\"/>"                           \
    "<circle cx=\"0\" cy=\"0\" r=\"5\" style=\"fill: %2; stroke: %3; stroke-width: 5px;\"/>"                            \
    "</g>"                                                                                                              \
    "</defs>"))
#define PITCH_GRAPH_TRAILER (QString("</svg>"))

#define BLACK_DOT_FORMAT    (QString("<use href=\"#black-dot\" x=\"%1\" y=\"%2\"/>"))
#define WHITE_DOT_FORMAT    (QString("<use href=\"#white-dot\" x=\"%1\" y=\"%2\"/>"))
#define CHANGE_DOT_FORMAT   (QString("<use href=\"#change-dot\" x=\"%1\" y=\"%2\"/>"))
#define LINE_FORMAT         (QString("<path d=\"M%2 %3 L%4 %5\" style=\"stroke: %1; stroke-width: 5px;\"/>"))

#define GRAPH_TOP_Y     25
#define GRAPH_BOTTOM_Y  75
#define GRAPH_STEP      50
#define GRAPH_OFFSET    25

QString GraphicUtils::generatePitchGraph(const int     moraSize,
                                         const uint8_t pos,
                                         const QString &fill,
                                         const QString &stroke)
{
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

        lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
        dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));

        size_t i;
        for (i = 1; i < moraSize; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_TOP_Y;

            lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
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

        lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
        dots.append(CHANGE_DOT_FORMAT.arg(x1).arg(y1));

        size_t i;
        for (i = 1; i < moraSize; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_BOTTOM_Y;

            lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
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

        lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
        dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));

        size_t i;
        for (i = 1; i < pos - 1; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_TOP_Y;

            lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
            dots.append(BLACK_DOT_FORMAT.arg(x1).arg(y1));
        }

        /* Change dot */
        x1 = x2;
        y1 = y2;
        x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
        y2 = GRAPH_BOTTOM_Y;

        lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
        dots.append(CHANGE_DOT_FORMAT.arg(x1).arg(y1));

        /* End dots */
        for (i = pos; i < moraSize; ++i)
        {
            x1 = x2;
            y1 = y2;
            x2 = GRAPH_OFFSET + GRAPH_STEP * (i + 1);
            y2 = GRAPH_BOTTOM_Y;

            lines.append(lineFormat.arg(x1).arg(y1).arg(x2).arg(y2));
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

#undef PITCH_GRAPH_HEADER
#undef PITCH_GRAPH_TRAILER

#undef BLACK_DOT_FORMAT
#undef WHITE_DOT_FORMAT
#undef CHANGE_DOT_FORMAT
#undef LINE_FORMAT

#undef GRAPH_TOP_Y
#undef GRAPH_BOTTOM_Y
#undef GRAPH_STEP
#undef GRAPH_OFFSET