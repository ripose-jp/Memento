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

#ifndef KANJIWIDGET_H
#define KANJIWIDGET_H

#include <QWidget>

#include "../../dict/expression.h"

class KanjiWidget : public QWidget
{
    Q_OBJECT

public:
    KanjiWidget(const Kanji &kanji, QWidget *parent = nullptr);
    ~KanjiWidget();

private:
    const Kanji kanji;
};

#endif // KANJIWIDGET_H