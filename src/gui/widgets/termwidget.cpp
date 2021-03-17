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
#include "../../util/iconfactory.h"

#include <QMessageBox>

TermWidget::TermWidget(const Term *term, AnkiClient *client, QWidget *parent) 
    : QWidget(parent), m_ui(new Ui::TermWidget), m_term(term),
      m_client(client)
{
    m_ui->setupUi(this);

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
    while (item = m_ui->layoutFrequency->takeAt(0))
    {
        delete item->widget();
        delete item;
    }
    while (item = m_ui->layoutGlossary->takeAt(0))
        {
        delete item->widget();
        delete item;
    }
    while (item = m_ui->layoutTermTags->takeAt(0))
    {
        delete item->widget();
        delete item;
    }
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
        m_ui->layoutFrequency->addWidget(tag);
    }
    m_ui->layoutFrequency->addStretch();

    for (const Tag &termTag : term.tags)
    {
        TagWidget *tag = new TagWidget(termTag, this);
        m_ui->layoutTermTags->addWidget(tag);
    }
    m_ui->layoutTermTags->addStretch();

    QLabel *label = new QLabel(this);
    label->setWordWrap(true);
    label->setText(buildDefinition(term.definitions));
    m_ui->layoutGlossary->addWidget(label);
}

QString TermWidget::generateJishoLink(const QString &word)
{
    return QString("<a href=\"https://jisho.org/search/%1\">Jisho</a>").arg(word);
}

QString TermWidget::buildDefinition(const QList<Definition> &definitions)
{
    QString glossary;
    glossary.append("<div style=\"text-align: left;\"><ol>");

    for (const Definition &def : definitions)
    {
        glossary += "<li>";

        glossary += "<i>(";
        for (const Tag &tag : def.tags)
        {
            glossary += tag.name + ", ";
        }
        glossary += def.dictionary;
        glossary += ")</i>";

        glossary += "<ul>";
        for (const QString &glos : def.glossary)
        {
            glossary += "<li>";
            glossary += glos;
            glossary += "</li>";
        }
        glossary += "</ul>";

        glossary += "</li>";
    }

    glossary.append("</ol></div>");

    return glossary;
}

void TermWidget::setAddable(bool value)
{
    m_ui->buttonAddCard->setEnabled(value);
}

void TermWidget::addNote()
{
    m_ui->buttonAddCard->setEnabled(false);
    AnkiReply *reply = m_client->addTerm(m_term);
    connect(reply, &AnkiReply::finishedInt,
        [=](const int id, const QString &error) {
            if (!error.isEmpty())
            {
                QMessageBox messageBox;
                messageBox.critical(0,"Error", error);
            }
        }
    );
}
