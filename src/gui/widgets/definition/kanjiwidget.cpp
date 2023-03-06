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

#include <QClipboard>
#include <QDebug>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "tagwidget.h"

#include "anki/ankiclient.h"
#include "gui/widgets/common/flowlayout.h"
#include "gui/widgets/subtitlelistwidget.h"
#include "util/globalmediator.h"
#include "util/iconfactory.h"

/* Begin Constructor/Destructor */

KanjiWidget::KanjiWidget(
    QSharedPointer<const Kanji> kanji,
    bool showBack,
    QWidget *parent)
    : QWidget(parent),
      m_kanji(kanji)
{
    QVBoxLayout *layoutParent = new QVBoxLayout(this);

    QHBoxLayout *layoutTop = new QHBoxLayout;
    layoutParent->addLayout(layoutTop);

    IconFactory *factory = IconFactory::create();

    if (showBack)
    {
        QToolButton *buttonBack = new QToolButton;
        buttonBack->setIcon(factory->getIcon(IconFactory::Icon::back));
        buttonBack->setMinimumSize(QSize(30, 30));
        buttonBack->setToolTip("Return to search results");
        connect(
            buttonBack, &QToolButton::clicked, this, &KanjiWidget::backPressed
        );
        layoutTop->addWidget(buttonBack);
        layoutTop->setAlignment(buttonBack, Qt::AlignTop | Qt::AlignLeft);
    }

    QLabel *labelKanjiStroke = new QLabel;
    labelKanjiStroke->setText(kanji->character);
    labelKanjiStroke->setStyleSheet(
        "QLabel {"
            "font-family: \"KanjiStrokeOrders\", \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", sans-serif;"
#if defined(Q_OS_MACOS)
            "font-size: 140pt;"
#else
            "font-size: 100pt;"
#endif
        "}"
    );
    labelKanjiStroke->setAlignment(Qt::AlignHCenter);
    labelKanjiStroke->setTextInteractionFlags(Qt::TextSelectableByMouse);
    labelKanjiStroke->setSizePolicy(
        QSizePolicy::Preferred, QSizePolicy::Preferred
    );
    layoutTop->addWidget(labelKanjiStroke);
    layoutTop->setAlignment(labelKanjiStroke, Qt::AlignTop | Qt::AlignCenter);
    layoutTop->addStretch();

    m_buttonAnkiAddOpen = new QToolButton;
    m_buttonAnkiAddOpen->setMinimumSize(QSize(30, 30));
    m_buttonAnkiAddOpen->hide();
    layoutTop->addWidget(m_buttonAnkiAddOpen);
    layoutTop->setAlignment(m_buttonAnkiAddOpen, Qt::AlignTop | Qt::AlignRight);

    AnkiClient *client = GlobalMediator::getGlobalMediator()->getAnkiClient();
    if (client->isEnabled())
    {
        AnkiReply *reply = client->notesAddable(
            QList<QSharedPointer<const Kanji>>({m_kanji})
        );
        connect(reply, &AnkiReply::finishedBoolList, this,
            [=] (const QList<bool> &value, const QString &error) {
                if (!error.isEmpty())
                {
                    return;
                }

                /* Kanji Addable */
                if (value.first())
                {
                    m_buttonAnkiAddOpen->setIcon(
                        factory->getIcon(IconFactory::Icon::plus)
                    );
                    m_buttonAnkiAddOpen->setToolTip("Add Anki note");
                    connect(
                        m_buttonAnkiAddOpen, &QToolButton::clicked,
                        this, &KanjiWidget::addKanji
                    );
                    m_buttonAnkiAddOpen->show();
                }
                /* Kanji Already Added */
                else
                {
                    m_buttonAnkiAddOpen->setIcon(
                        factory->getIcon(IconFactory::Icon::hamburger)
                    );
                    m_buttonAnkiAddOpen->setToolTip("Show in Anki");
                    connect(
                        m_buttonAnkiAddOpen, &QToolButton::clicked,
                        this, &KanjiWidget::openAnki
                    );
                    m_buttonAnkiAddOpen->show();
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

/* Begin Constructor/Destructor */
/* Begin Builders */

void KanjiWidget::buildDefinitionLabel(const KanjiDefinition &def,
                                             QVBoxLayout     *layout)
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
    for (int i = 0; i < def.glossary.size(); ++i)
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
    layoutGLS->addLayout(
        layoutStats, layoutGLS->rowCount() - 1, layoutGLS->columnCount() - 1
    );
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

void KanjiWidget::addKVSection(const QString &title,
                               const QList<QPair<Tag, QString>> &pairs,
                               QVBoxLayout *layout)
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

/* End Builders */
/* Begin Button Handlers */

void KanjiWidget::addKanji()
{
    m_buttonAnkiAddOpen->setEnabled(false);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    PlayerAdapter *player = mediator->getPlayerAdapter();
    SubtitleListWidget *subList = mediator->getSubtitleListWidget();
    Kanji *kanji = new Kanji(*m_kanji);
    double delay =
        mediator->getPlayerAdapter()->getSubDelay() -
        mediator->getPlayerAdapter()->getAudioDelay();
    kanji->clipboard = QGuiApplication::clipboard()->text();
    kanji->title = player->getTitle();
    kanji->sentence = player->getSubtitle(true);
    kanji->sentence2 = player->getSecondarySubtitle();
    kanji->startTime = player->getSubStart() + delay;
    kanji->endTime = player->getSubEnd() + delay;
    kanji->context = subList->getPrimaryContext("\n");
    kanji->context2 = subList->getSecondaryContext("\n");
    QPair<double, double> contextTimes = subList->getPrimaryContextTime();
    kanji->startTimeContext = contextTimes.first + delay;
    kanji->endTimeContext = contextTimes.second + delay;

    AnkiReply *reply = mediator->getAnkiClient()->addNote(kanji);
    connect(reply, &AnkiReply::finishedInt, this,
        [=] (const int, const QString &error) {
            if (!error.isEmpty())
            {
                Q_EMIT mediator->showCritical("Error Adding Note", error);
            }
            else
            {
                m_buttonAnkiAddOpen->disconnect();
                m_buttonAnkiAddOpen->setIcon(
                    IconFactory::create()->getIcon(IconFactory::Icon::hamburger)
                );
                m_buttonAnkiAddOpen->setToolTip("Show in Anki");
                connect(
                    m_buttonAnkiAddOpen, &QToolButton::clicked,
                    this, &KanjiWidget::openAnki
                );
                m_buttonAnkiAddOpen->setEnabled(true);
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
        [=] (const QList<int> &, const QString &error) {
            if (!error.isEmpty())
            {
                Q_EMIT GlobalMediator::getGlobalMediator()->showCritical(
                    "Error Opening Anki", error
                );
            }
        }
    );
}

/* End Button Handler */
/* Begin Helpers */

QFrame *KanjiWidget::createLine() const
{
    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setLineWidth(1);
    return line;
}

QLabel *KanjiWidget::createLabel(const QString &text,
                                 const bool bold,
                                 const Qt::AlignmentFlag alignment) const
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

QLayout *KanjiWidget::createKVLabel(const QString &key,
                                    const QString &value) const
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(createLabel(key, true));
    QLabel *vLabel = createLabel(value, false, Qt::AlignRight);
    vLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    layout->addWidget(vLabel);
    return layout;
}

/* End Helpers */
