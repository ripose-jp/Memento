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

#include "definitionwidget.h"
#include "ui_definitionwidget.h"

#include "../../util/globalmediator.h"

#include <QFrame>
#include <QScrollBar>

DefinitionWidget::DefinitionWidget(const QList<Term *> *terms, AnkiClient *client, QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::DefinitionWidget),
      m_client(client)
{
    m_ui->setupUi(this);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    QFrame *line = nullptr;
    for (const Term *term : *terms)
    {
        if (line)
            m_ui->scrollAreaContents->layout()->addWidget(line);

        TermWidget *termWidget = new TermWidget(term, m_client, this);
        connect(termWidget, &TermWidget::kanjiSearched, this, &DefinitionWidget::showKanji);
        m_termWidgets.append(termWidget);
        m_ui->scrollAreaContents->layout()->addWidget(termWidget);

        line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
    }
    delete line;

    m_ui->scrollArea->verticalScrollBar()->setValue(0);

    // Check if entries are addable to anki
    if (m_client->isEnabled())
    {
        AnkiReply *reply = m_client->notesAddable(*terms);
        connect(reply, &AnkiReply::finishedBoolList, this, &DefinitionWidget::setAddable);
    }

    delete terms;
}

DefinitionWidget::~DefinitionWidget()
{
    delete m_ui;
}

void DefinitionWidget::setAddable(const QList<bool> &addable, const QString &error)
{
    if (error.isEmpty())
    {
        for (size_t i = 0; i < addable.size(); ++i)
        m_termWidgets[i]->setAddable(addable[i]);
    }
}

void DefinitionWidget::showKanji(const Kanji *kanji)
{
    m_savedScroll = m_ui->scrollArea->verticalScrollBar()->value();
    for (size_t i = 0; i < m_ui->scrollAreaContents->layout()->count(); ++i)
    {
        m_ui->scrollAreaContents->layout()->itemAt(i)->widget()->hide();
    }
    KanjiWidget *kanjiWidget = new KanjiWidget(kanji);
    connect(kanjiWidget, &KanjiWidget::backPressed, this, &DefinitionWidget::hideKanji);
    m_ui->scrollAreaContents->layout()->addWidget(kanjiWidget);
    m_ui->scrollArea->verticalScrollBar()->setValue(0);
}

void DefinitionWidget::hideKanji()
{
    QLayout *scrollLayout = m_ui->scrollAreaContents->layout();
    QLayoutItem *kanjiItem = scrollLayout->takeAt(scrollLayout->count() - 1);
    delete kanjiItem->widget();
    delete kanjiItem;

    for (size_t i = 0; i < scrollLayout->count(); ++i)
    {
        scrollLayout->itemAt(i)->widget()->show();
    }
    QApplication::processEvents();
    m_ui->scrollArea->verticalScrollBar()->setValue(m_savedScroll);
}


void DefinitionWidget::hideEvent(QHideEvent *event)
{
    Q_EMIT GlobalMediator::getGlobalMediator()->definitionsHidden();
}

void DefinitionWidget::showEvent(QShowEvent *event)
{
    Q_EMIT GlobalMediator::getGlobalMediator()->definitionsShown();
}