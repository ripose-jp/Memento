////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2023 Ripose
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

#include "hittestwidget.h"

#include <QMouseEvent>

HitTestWidget::HitTestWidget(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

void HitTestWidget::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    handleRegions(event->pos());
    event->ignore();
}

inline void HitTestWidget::handleRegions(const QPoint &pos)
{
    handleRegions(mapToRegion(pos));
}

void HitTestWidget::handleRegions(const QPointF &pos)
{
    for (Region &region : m_regions)
    {
        if (region.enterFlag() && region.entered(pos))
        {
            region.handle();
        }
        else if (region.exitFlag() && region.exited(pos))
        {
            region.handle();
        }
        else if (region.moveFlag() && region.contains(pos))
        {
            region.handle();
        }

        region.update(pos);
    }
}

inline QPointF HitTestWidget::mapToRegion(const QPoint &pos) const
{

    return QPointF{
        static_cast<qreal>(pos.x()) / width(),
        static_cast<qreal>(pos.y()) / height()
    };
}
