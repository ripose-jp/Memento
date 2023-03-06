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

#include <QDoubleSpinBox>

#include <QWheelEvent>

/**
 * A QDoubleSpinBox that ignores the mouse wheel.
 * Used to prevent a the widget from stealing focus when scrolling.
 */
class ScrollDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT

public:
    ScrollDoubleSpinBox(QWidget *parent = nullptr) : QDoubleSpinBox(parent)
    {
        setFocusPolicy(Qt::StrongFocus);
    }

protected:
    void wheelEvent(QWheelEvent *event) override
        { event->ignore(); }
};
