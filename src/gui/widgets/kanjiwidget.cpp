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
#include "../../util/globalmediator.h"
#include "../../anki/ankiclient.h"

KanjiWidget::KanjiWidget(const Kanji *kanji, QWidget *parent) : QWidget(parent), m_kanji(kanji)
{
    QVBoxLayout *layoutParent = new QVBoxLayout(this);

    QHBoxLayout *layoutTop = new QHBoxLayout;
    layoutParent->addLayout(layoutTop);

    IconFactory *factory = IconFactory::create();

    QToolButton *buttonBack = new QToolButton;
    buttonBack->setIcon(factory->getIcon(IconFactory::Icon::back));
    buttonBack->setMinimumSize(QSize(30, 30));
    buttonBack->setToolTip("Return to search results");
    connect(buttonBack, &QToolButton::clicked, this, &KanjiWidget::backPressed);
    layoutTop->addWidget(buttonBack);
    layoutTop->setAlignment(buttonBack, Qt::AlignTop | Qt::AlignLeft);

    QLabel *labelKanjiStroke = new QLabel;
    labelKanjiStroke->setText(kanji->character);
#if __APPLE__
    labelKanjiStroke->setStyleSheet(
        "QLabel {"\
            "font-family: \"KanjiStrokeOrders\", \"Noto Sans\", \"Noto Sans CJK JP\", sans-serif;"\
            "font-size: 140pt;"\
        "}"
    );
#else
    labelKanjiStroke->setStyleSheet(
        "QLabel {"\
            "font-family: \"KanjiStrokeOrders\", \"Noto Sans\", \"Noto Sans CJK JP\", sans-serif;"\
            "font-size: 100pt;"\
        "}"
    );
#endif
    labelKanjiStroke->setAlignment(Qt::AlignHCenter);
    labelKanjiStroke->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labelKanjiStroke->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    layoutTop->addWidget(labelKanjiStroke);
    layoutTop->setAlignment(labelKanjiStroke, Qt::AlignTop | Qt::AlignCenter);
    layoutTop->addStretch();

    QToolButton *buttonAnkiAdd = new QToolButton;
    buttonAnkiAdd->setIcon(factory->getIcon(IconFactory::Icon::plus));
    buttonAnkiAdd->setMinimumSize(QSize(30, 30));
    buttonAnkiAdd->setToolTip("Add Anki note");
    buttonAnkiAdd->setVisible(false);
    layoutTop->addWidget(buttonAnkiAdd);
    layoutTop->setAlignment(buttonAnkiAdd, Qt::AlignTop | Qt::AlignRight);
    connect(buttonAnkiAdd, &QToolButton::clicked, this, &KanjiWidget::addKanji);
    m_buttonAnkiAdd = buttonAnkiAdd;

    QToolButton *buttonAnkiOpen = new QToolButton;
    buttonAnkiOpen->setIcon(factory->getIcon(IconFactory::Icon::hamburger));
    buttonAnkiOpen->setMinimumSize(QSize(30, 30));
    buttonAnkiOpen->setToolTip("Show in Anki");
    buttonAnkiOpen->setVisible(false);
    layoutTop->addWidget(buttonAnkiOpen);
    layoutTop->setAlignment(buttonAnkiOpen, Qt::AlignTop | Qt::AlignRight);
    connect(buttonAnkiOpen, &QToolButton::clicked, this, &KanjiWidget::openAnki);
    m_buttonAnkiOpen = buttonAnkiOpen;

    AnkiClient *client = GlobalMediator::getGlobalMediator()->getAnkiClient();
    if (client->isEnabled())
    {
        AnkiReply *reply = client->notesAddable(QList<const Kanji *>({kanji}));
        connect(reply, &AnkiReply::finishedBoolList, this,
            [=] (const QList<bool> &value, const QString &error) {
                if (error.isEmpty())
                {
                    m_buttonAnkiAdd->setVisible(value.first());
                    m_buttonAnkiOpen->setVisible(!value.first());
                }
            }
        );
    }
    

    FlowLayout *frequencies = new FlowLayout(-1, 6);
    for (const Frequency &freq : kanji->frequencies)
        frequencies->addWidget(new TagWidget(freq));
    layoutParent->addLayout(frequencies);

    QVBoxLayout *layoutDefinitions = new QVBoxLayout;
    layoutParent->addLayout(layoutDefinitions);

    QFrame *line = nullptr;
    for (const KanjiDefinition &def : kanji->definitions)
    {
        if (line)
            layoutDefinitions->addWidget(line);
        buildDefinitionLabel(def, layoutDefinitions);
        line = createLine();
    }
    delete line;

    layoutParent->addStretch();
}

KanjiWidget::~KanjiWidget()
{
    delete m_kanji;
}

void KanjiWidget::buildDefinitionLabel(const KanjiDefinition &def, QVBoxLayout *layout)
{
    /* Add Tags */
    FlowLayout *tagLayout = new FlowLayout(-1, 6);
    layout->addLayout(tagLayout);
    for (const Tag &tag : def.tags)
    {
        tagLayout->addWidget(new TagWidget(tag));
    }
    tagLayout->addWidget(new TagWidget(def.dictionary));

    /* Add Glossary, Reading, Statistics Header */
    QGridLayout *layoutGLS = new QGridLayout;
    layout->addLayout(layoutGLS);
    layoutGLS->addWidget(createLine(), 0, 0, 1, -1);
    layoutGLS->addWidget(createLabel("Glossary",   true), 1, 0);
    layoutGLS->addWidget(createLabel("Reading",    true), 1, 1);
    layoutGLS->addWidget(createLabel("Statistics", true), 1, 2);
    layoutGLS->addWidget(createLine(), 2, 0, 1, -1);

    /* Add Glossary Entries */
    QString text;
    for (size_t i = 0; i < def.glossary.size(); ++i)
    {
        text += QString::number(i + 1);
        text += ". ";
        text += def.glossary[i];
        text += "<br>";
    }
    layoutGLS->addWidget(createLabel(text));

    /* Add Readings */
    text.clear();
    /* Add Onyomi (Chinese) */
    for (const QString &str : def.onyomi)
    {
        text += str + '\n';
    }
    text += '\n';
    /* Add Kunyomi (Japanese) */
    for (const QString &str : def.kunyomi)
    {
        text += str + '\n';
    }
    text.chop(1);
    layoutGLS->addWidget(createLabel(text));

    /* Add Statistics */
    QVBoxLayout *layoutStats = new QVBoxLayout;
    layoutGLS->addLayout(layoutStats, layoutGLS->rowCount() - 1, layoutGLS->columnCount() - 1);
    for (const QPair<Tag, QString> &kv : def.stats)
    {
        layoutStats->addLayout(createKVLabel(kv.first.notes, kv.second));
    }
    layoutStats->addStretch();

    /* Add Everything Else */
    addKVSection("Classifications", def.clas, layout);
    addKVSection("Codepoints", def.code, layout);
    addKVSection("Dictionary Indices", def.index, layout);
}

QFrame *KanjiWidget::createLine()
{
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);
    return line;
}

QLabel *KanjiWidget::createLabel(const QString &text, const bool bold, const Qt::AlignmentFlag alignment)
{
    QLabel *label = new QLabel;
    label->setText(text);
    label->setAlignment(alignment);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    label->setWordWrap(true);

    if (bold)
    {
        label->setStyleSheet(
            "QLabel {"\
                "font-weight: bold;"\
            "}"
        );
    }

    return label;
}

QLayout *KanjiWidget::createKVLabel(const QString &key, const QString &value)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(createLabel(key, true));
    QLabel *vLabel = createLabel(value, false, Qt::AlignRight);
    vLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    layout->addWidget(vLabel);
    return layout;
}

void KanjiWidget::addKVSection(const QString &title, const QList<QPair<Tag, QString>> &pairs, QVBoxLayout *layout)
{
    if (pairs.isEmpty())
        return;
    
    layout->addWidget(createLine());
    layout->addWidget(createLabel(title, true));
    layout->addWidget(createLine());
    for (const QPair<Tag, QString> &kv : pairs)
    {
        layout->addLayout(createKVLabel(kv.first.notes, kv.second));
    }
}

void KanjiWidget::addKanji()
{
    m_buttonAnkiAdd->setVisible(false);
    Kanji *kanji = new Kanji(*m_kanji);
    AnkiReply *reply = GlobalMediator::getGlobalMediator()->getAnkiClient()->addNote(kanji);
    connect(reply, &AnkiReply::finishedInt, this,
        [=] (const int id, const QString &error) {
            if (!error.isEmpty())
            {
                Q_EMIT GlobalMediator::getGlobalMediator()->showCritical("Error Adding Note", error);
            }
            else
            {
                m_buttonAnkiOpen->setVisible(true);
                m_buttonAnkiAdd->setVisible(false);
            }
        }
    );
}

void KanjiWidget::openAnki()
{
    AnkiClient *client = GlobalMediator::getGlobalMediator()->getAnkiClient();
    QString deck = client->getConfig()->kanjiDeck;
    AnkiReply *reply = client->openBrowse(deck, m_kanji->character);
    connect(reply, &AnkiReply::finishedIntList, this,
        [=] (const QList<int> &value, const QString &error) {
            if (!error.isEmpty())
            {
                Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
                    "Error Opening Anki", error
                );
            }
        }
    );
}