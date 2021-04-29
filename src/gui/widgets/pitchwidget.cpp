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

#include "pitchwidget.h"

#include "tagwidget.h"
#include "flowlayout.h"

#include <QVBoxLayout>

PitchWidget::PitchWidget(const Pitch &pitch, QWidget *parent) : QWidget(parent)
{
    QLayout *layoutParent;  
    if (pitch.position.size() > 1)
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setSpacing(0);
        layoutParent = layout;
    }
    else
    {
        layoutParent = new FlowLayout(this, 0, 6);
    }

    layoutParent->addWidget(new TagWidget(pitch));

    for (const uint8_t pos : pitch.position)
    {
        QLabel *label = new QLabel;
        label->setText(pitch.reading + " [" +QString::number(pos) + "]");
        layoutParent->addWidget(label);
    }
}