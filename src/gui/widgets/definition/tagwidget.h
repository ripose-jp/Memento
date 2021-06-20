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

#ifndef TAGWIDGET_H
#define TAGWIDGET_H

#include <QLabel>

#include "../../../dict/expression.h"

class TagWidget : public QLabel
{
    Q_OBJECT

public:
    TagWidget(const Tag       &tag,     QWidget *parent = 0);
    TagWidget(const Frequency &freq,    QWidget *parent = 0);
    TagWidget(const Pitch     &pitch,   QWidget *parent = 0);
    TagWidget(const QString   &dicName, QWidget *parent = 0);

private:
    TagWidget(QWidget *parent = 0);

    enum TagColor
    {
        def,
        name,
        expression,
        popular,
        frequent,
        archaism,
        dictionary,
        frequency,
        pos,
        search,
        pitch_accent
    };

    const char *colors[pitch_accent + 1] = {
        "#8a8a91",
        "#b6327a",
        "#f0ad4e",
        "#0275d8",
        "#5bc0de",
        "#d9534f",
        "#aa66cc",
        "#5cb85c",
        "#565656",
        "#8a8a91",
        "#6640be"
    };
};

#endif // TAGWIDGET_H