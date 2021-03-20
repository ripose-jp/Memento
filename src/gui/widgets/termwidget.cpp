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

#include <QMessageBox>

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

    IconFactory *factory = IconFactory::create(this);
    m_ui->buttonAddCard->setIcon(factory->getIcon(IconFactory::Icon::plus));
    delete factory;
    m_ui->buttonAddCard->setVisible(m_client->isEnabled());

    setTerm(*term);

    connect(m_ui->buttonAddCard, &QToolButton::clicked, this, &TermWidget::addNote);
}

TermWidget::~TermWidget()
{
    QLayoutItem *item;
    while (item = m_layoutTermTags->takeAt(0))
    {
        delete item->widget();
        delete item;
    }
    delete m_layoutTermTags;

    while (item = m_layoutFreqTags->takeAt(0))
        {
        delete item->widget();
        delete item;
    }
    delete m_layoutFreqTags;

    while (item = m_layoutGlossary->takeAt(0))
    {
        delete item->widget();
        delete item;
    }
    delete m_layoutGlossary;
    
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
    m_ui->labelKanji->setText(term.expression);
    m_ui->labelJisho->setText(generateJishoLink(term.expression));

    for (const TermFrequency &freq : term.frequencies)
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

QString TermWidget::generateJishoLink(const QString &word)
{
    return QString("<a href=\"https://jisho.org/search/%1\">Jisho</a>").arg(word);
}

void TermWidget::setAddable(bool value)
{
    m_ui->buttonAddCard->setEnabled(value);
    for (size_t i = 0; i < m_layoutGlossary->count(); ++i)
        ((GlossaryWidget *)m_layoutGlossary->itemAt(i)->widget())->setCheckable(value);
}

void TermWidget::addNote()
{
    m_ui->buttonAddCard->setEnabled(false);

    Term term(*m_term);
    term.definitions.clear();
    for (size_t i = 0; i < m_layoutGlossary->count(); ++i)
    {
        GlossaryWidget *widget = (GlossaryWidget *)m_layoutGlossary->itemAt(i)->widget();
        if (widget->isChecked())
        {
            term.definitions.append(Definition(m_term->definitions[i]));
        }
        widget->setCheckable(false);
    }

    AnkiReply *reply = m_client->addTerm(&term);
    connect(reply, &AnkiReply::finishedInt, this,
        [=] (const int id, const QString &error) {
            if (!error.isEmpty())
            {
                QMessageBox messageBox;
                messageBox.critical(0,"Error", error);
            }
        }
    );
}
