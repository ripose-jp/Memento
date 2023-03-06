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

#ifndef PITCHWIDGET_H
#define PITCHWIDGET_H

#include <QWidget>

#include "dict/expression.h"

class QLabel;

/**
 * Widget for graphically displaying pitch accents.
 */
class PitchWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor for PitchWidget.
     * @param pitch  The pitch to display.
     * @param parent The parent of this widget.
     */
    PitchWidget(const Pitch &pitch, QWidget *parent = nullptr);

private:
    /**
     * Helper method for creating a properly configured label.
     * @param text  The text of the label.
     * @param style The stylesheet of the label.
     */
    QLabel *createLabel(const QString &text, const QString &style) const;
};

#endif // PITCHWIDGET_H
