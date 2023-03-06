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

#include "tagwidget.h"

#include <QDebug>

/**
 * Stylesheet format string.
 * @param 1 The background color of the tag.
 */
#define STYLE_FORMAT    (QString(\
                            "font-weight: bold;"\
                            "padding-top: 1px;"\
                            "padding-bottom: 1px;"\
                            "padding-left: 5px;"\
                            "padding-right: 5px;"\
                            "color: white;"\
                            "border-radius: 3px;"\
                            "background: %1;"\
                        ))

/**
 * Frequency format string.
 * @param 1 The name of the frequency dictionary.
 * @param 2 The number of times the term appears.
 */
#define FREQ_FORMAT     (QString("%1 <span style=\"color: black;\">%2</span>"))

TagWidget::TagWidget(const Tag &tag, QWidget *parent) : TagWidget(parent)
{
    TagColor color = def;
    if (tag.category == "name")
    {
        color = name;
    }
    else if (tag.category == "expression")
    {
        color = expression;
    }
    else if (tag.category == "popular")
    {
        color = popular;
    }
    else if (tag.category == "frequent")
    {
        color = frequent;
    }
    else if (tag.category == "archaism")
    {
        color = archaism;
    }
    else if (tag.category == "dictionary")
    {
        color = dictionary;
    }
    else if (tag.category == "frequency")
    {
        color = frequency;
    }
    else if (tag.category == "partOfSpeech")
    {
        color = pos;
    }
    else if (tag.category == "search")
    {
        color = search;
    }
    else if (tag.category == "pitch-accent-dictionary")
    {
        color = pitch_accent;
    }

    setStyleSheet(STYLE_FORMAT.arg(colors[color]));
    setToolTip(tag.notes);
    setText(tag.name);
}

TagWidget::TagWidget(const Frequency &freq, QWidget *parent) : TagWidget(parent)
{
    setStyleSheet(STYLE_FORMAT.arg(colors[frequency]));
    setText(FREQ_FORMAT.arg(freq.dictionary).arg(freq.freq));
    setToolTip(freq.dictionary);
}

TagWidget::TagWidget(const Pitch &pitch, QWidget *parent) : TagWidget(parent)
{
    setStyleSheet(STYLE_FORMAT.arg(colors[pitch_accent]));
    setText(pitch.dictionary);
    setToolTip(pitch.dictionary);
}

TagWidget::TagWidget(const QString &dicName, QWidget *parent) : TagWidget(parent)
{
    setStyleSheet(STYLE_FORMAT.arg(colors[dictionary]));
    setToolTip(dicName);
    setText(dicName);
}

TagWidget::TagWidget(QWidget *parent) : QLabel(parent)
{
    setTextInteractionFlags(Qt::TextSelectableByMouse);
    setAlignment(Qt::AlignCenter);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
