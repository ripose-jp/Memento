////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#include "termwidget.h"
#include "ui_termwidget.h"

#include "tagwidget.h"
#include "glossarywidget.h"
#include "../../util/iconfactory.h"
#include "../../util/globalmediator.h"
#include "../../dict/dictionary.h"

#define KANJI_UNICODE_LOWER_COMMON  "\u4e00"
#define KANJI_UNICODE_UPPER_COMMON  "\u9faf"
#define KANJI_UNICODE_LOWER_RARE    "\u3400"
#define KANJI_UNICODE_UPPER_RARE    "\u4dbf"

#define KANJI_STYLE_STRING      (QString("<style>a { color: black; border: 0; text-decoration: none; }</style>"))
#define KANJI_FORMAT_STRING     (QString("<a href=\"%1\">%1</a>"))

TermWidget::TermWidget(const Term *term, AnkiClient *client, QWidget *parent) 
    : QWidget(parent), m_ui(new Ui::TermWidget), m_term(term), m_client(client)
{
    m_ui->setupUi(this);

    m_layoutTermTags = new FlowLayout;
    m_layoutFreqTags = new FlowLayout;
    m_layoutGlossary = new QVBoxLayout;

    m_ui->verticalLayout->addLayout(m_layoutTermTags);
    m_ui->verticalLayout->addLayout(m_layoutFreqTags);
    m_ui->verticalLayout->addLayout(m_layoutGlossary);
    
    m_ui->verticalLayout->addStretch();

    IconFactory *factory = IconFactory::create(this);

    m_ui->buttonAddCard->setIcon(factory->getIcon(IconFactory::Icon::plus));
    m_ui->buttonAddCard->setVisible(false);

    m_ui->buttonAnkiOpen->setIcon(factory->getIcon(IconFactory::Icon::hamburger));
    m_ui->buttonAnkiOpen->setVisible(false);

    delete factory;

    setTerm(*term);

    connect(m_ui->buttonAddCard,  &QToolButton::clicked,  this, &TermWidget::addNote);
    connect(m_ui->buttonAnkiOpen, &QToolButton::clicked,  this, &TermWidget::openAnki);
    connect(m_ui->labelKanji,     &QLabel::linkActivated, this, &TermWidget::searchKanji);
}

TermWidget::~TermWidget()
{
    delete m_ui;
    delete m_term;
}

void TermWidget::setTerm(const Term &term)
{
    if (term.reading.isEmpty())
    {
        m_ui->labelKana->hide();
    }
    else
    {
        m_ui->labelKana->show();
    }
    m_ui->labelKana->setText(term.reading);
    QString kanjiLabelText = KANJI_STYLE_STRING;
    for (const QString &ch : term.expression)
    {
        kanjiLabelText += isKanji(ch) ? KANJI_FORMAT_STRING.arg(ch) : ch;
    }
    m_ui->labelKanji->setText(kanjiLabelText);
    m_ui->labelJisho->setText(generateJishoLink(term.expression));

    for (const Frequency &freq : term.frequencies)
    {
        TagWidget *tag = new TagWidget(freq, this);
        m_layoutFreqTags->addWidget(tag);
    }

    for (const Tag &termTag : term.tags)
    {
        TagWidget *tag = new TagWidget(termTag, this);
        m_layoutTermTags->addWidget(tag);
    }

    for (size_t i = 0; i < term.definitions.size(); ++i)
    {
        m_layoutGlossary->addWidget(new GlossaryWidget(i + 1, term.definitions[i]));
    }
}

inline QString TermWidget::generateJishoLink(const QString &word)
{
    return QString("<a href=\"https://jisho.org/search/%1\">Jisho</a>").arg(word);
}

inline bool TermWidget::isKanji(const QString &ch)
{
    return ch >= KANJI_UNICODE_LOWER_COMMON && ch <= KANJI_UNICODE_UPPER_COMMON ||
           ch >= KANJI_UNICODE_LOWER_RARE   && ch <= KANJI_UNICODE_UPPER_RARE;
}

void TermWidget::setAddable(bool value)
{
    m_ui->buttonAddCard->setVisible(value);
    m_ui->buttonAnkiOpen->setVisible(!value);
    for (size_t i = 0; i < m_layoutGlossary->count(); ++i)
        ((GlossaryWidget *)m_layoutGlossary->itemAt(i)->widget())->setCheckable(value);
}

void TermWidget::addNote()
{
    m_ui->buttonAddCard->setEnabled(false);

    Term *term = new Term(*m_term);
    term->definitions.clear();
    for (size_t i = 0; i < m_layoutGlossary->count(); ++i)
    {
        GlossaryWidget *widget = (GlossaryWidget *)m_layoutGlossary->itemAt(i)->widget();
        if (widget->isChecked())
        {
            term->definitions.append(TermDefinition(m_term->definitions[i]));
        }
        widget->setCheckable(false);
    }

    AnkiReply *reply = m_client->addNote(term);
    connect(reply, &AnkiReply::finishedInt, this,
        [=] (const int id, const QString &error) {
            if (!error.isEmpty())
            {
                Q_EMIT GlobalMediator::getGlobalMediator()->showCritical("Error Adding Note", error);
            }
            else
            {
                m_ui->buttonAnkiOpen->setVisible(true);
                m_ui->buttonAddCard->setVisible(false);
            }
        }
    );
}

void TermWidget::openAnki()
{
    QString deck = m_client->getConfig()->termDeck;
    AnkiReply *reply = m_client->openBrowse(deck, m_term->expression);
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

void TermWidget::searchKanji(const QString &ch)
{
    Kanji *kanji = GlobalMediator::getGlobalMediator()->getDictionary()->searchKanji(ch);
    if (kanji)
    {
        kanji->sentence    = m_term->sentence;
        kanji->clozePrefix = m_term->clozePrefix;
        kanji->clozeBody   = m_term->clozeBody;
        kanji->clozeSuffix = m_term->clozeSuffix;
        Q_EMIT kanjiSearched(kanji);
    }
}