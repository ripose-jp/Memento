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

#include "kanjiwidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QDebug>

#include "flowlayout.h"
#include "tagwidget.h"
#include "../../util/iconfactory.h"

KanjiWidget::KanjiWidget(const Kanji &kanji, QWidget *parent) : QWidget(parent), kanji(kanji)
{
    QVBoxLayout *layoutParent = new QVBoxLayout(this);

    QHBoxLayout *layoutTop = new QHBoxLayout;
    layoutParent->addLayout(layoutTop);

    IconFactory *factory = IconFactory::create(this);

    QToolButton *buttonBack = new QToolButton;
    buttonBack->setIcon(factory->getIcon(IconFactory::Icon::back));
    buttonBack->setMinimumSize(QSize(30, 30));
    buttonBack->setToolTip("Return to search results");
    connect(buttonBack, &QToolButton::pressed, this, &KanjiWidget::backPressed);
    layoutTop->addWidget(buttonBack);
    layoutTop->setAlignment(buttonBack, Qt::AlignTop | Qt::AlignLeft);

    QLabel *labelKanjiStroke = new QLabel;
    labelKanjiStroke->setText(kanji.character);
    labelKanjiStroke->setFont(QFont("KanjiStrokeOrders", 100));
    labelKanjiStroke->setAlignment(Qt::AlignHCenter);
    labelKanjiStroke->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labelKanjiStroke->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layoutTop->addWidget(labelKanjiStroke);
    layoutTop->setAlignment(labelKanjiStroke, Qt::AlignTop | Qt::AlignHCenter);

    QToolButton *buttonAnkiAdd = new QToolButton;
    buttonAnkiAdd->setIcon(factory->getIcon(IconFactory::Icon::plus));
    buttonAnkiAdd->setMinimumSize(QSize(30, 30));
    buttonAnkiAdd->setToolTip("Add Anki note");
    layoutTop->addWidget(buttonAnkiAdd);
    layoutTop->setAlignment(buttonAnkiAdd, Qt::AlignTop | Qt::AlignRight);

    delete factory;

    FlowLayout *frequencies = new FlowLayout;
    for (const Frequency &freq : kanji.frequencies)
        frequencies->addWidget(new TagWidget(freq));
    layoutParent->addLayout(frequencies);

    QVBoxLayout *layoutDefinitions = new QVBoxLayout;
    layoutParent->addLayout(layoutDefinitions);

    for (const KanjiDefinition &def : kanji.definitions)
    {
        FlowLayout *tagLayout = new FlowLayout;
        layoutDefinitions->addLayout(tagLayout);

        for (const Tag &tag : def.tags)
        {
            tagLayout->addWidget(new TagWidget(tag));
        }
        tagLayout->addWidget(new TagWidget(def.dictionary));
    }

    layoutParent->addStretch();
}