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

#include <QFrame>
#include <QScrollBar>

DefinitionWidget::DefinitionWidget(const QList<Term *> *terms, AnkiClient *client, QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::DefinitionWidget),
      m_client(client)
{
    m_ui->setupUi(this);

    QFrame *line = nullptr;
    for (const Term *term : *terms)
    {
        if (line)
            m_ui->scrollAreaContents->layout()->addWidget(line);

        TermWidget *termWidget = new TermWidget(term, m_client, this);
        m_termWidgets.append(termWidget);
        m_ui->scrollAreaContents->layout()->addWidget(termWidget);

        line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
    }
    delete line;

    m_ui->scrollArea->verticalScrollBar()->setValue(0);

    // Check if entries are addable to anki
    if (m_client->isEnabled())
    {
        AnkiReply *reply = m_client->termsAddable(terms);
        connect(reply, &AnkiReply::finishedBoolList,
            [=] (const QList<bool> &addable, const QString &error)
            {
                if (error.isEmpty())
                {
                    for (size_t i = 0; i < addable.size(); ++i)
                        m_termWidgets[i]->setAddable(addable[i]);
                }
            }
        );
    }

    delete terms;
}

DefinitionWidget::~DefinitionWidget()
{
    QLayoutItem* child;
    while (child = m_ui->scrollAreaContents->layout()->takeAt(0))
    {
        delete child->widget();
        delete child;
    }
    delete m_ui;
}