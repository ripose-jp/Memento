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

#include "../../dict/expression.h"

#include <QLabel>

class PitchWidget : public QWidget
{
    Q_OBJECT

public:
    PitchWidget(const Pitch &pitch, QWidget *parent = nullptr);

private:
    QLabel *createLabel(const QString &text, const QString &style);
};

#endif // PITCHWIDGET_H