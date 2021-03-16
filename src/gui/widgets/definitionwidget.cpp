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

#include "definitionbox.h"

#include <QFrame>
#include <QScrollBar>

DefinitionWidget::DefinitionWidget(AnkiClient *client, QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::DefinitionWidget),
      m_client(client),
      m_searchId(0)
{
    m_ui->setupUi(this);
    hide();
}

DefinitionWidget::~DefinitionWidget()
{
    clearEntries();
    delete m_ui;
}

void DefinitionWidget::setTerms(const QList<Term *> *terms)
{
    m_searchIdMutex.lock();
    unsigned int searchId = ++m_searchId;
    m_searchIdMutex.unlock();

    clearEntries();
    if (terms->empty())
    {
        delete terms;
        return;
    }

    DefinitionBox *definition = new DefinitionBox(terms->first(), m_client, this);
    m_definitions.append(definition);
    m_ui->m_scrollAreaWidgetContents->layout()->addWidget(definition);

    for (auto it = terms->begin() + 1; it != terms->end(); ++it)
    {
        QFrame *line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
        definition = new DefinitionBox(*it, m_client, this);
        m_definitions.append(definition);
        m_ui->m_scrollAreaWidgetContents->layout()->addWidget(line);
        m_ui->m_scrollAreaWidgetContents->layout()->addWidget(definition);
    }

    m_ui->m_scrollArea->verticalScrollBar()->setValue(0);
    show();

    // Check if entries are addable to anki
    if (m_client->isEnabled())
    {
        m_client->termsAddable(
            [=](const QList<bool> *addable, const QString &error)
            {
                if (error.isEmpty())
                {
                    m_searchIdMutex.lock();
                    m_entryMutex.lock();
                    if (searchId == m_searchId)
                    {
                        for (size_t i = 0; i < addable->size(); ++i)
                            m_definitions[i]->setAddable(addable->at(i));
                    }
                    m_entryMutex.unlock();
                    m_searchIdMutex.unlock();
                }
                delete addable;
                delete terms;
            }, terms);
    }
}

void DefinitionWidget::clearEntries()
{
    m_entryMutex.lock();
    QLayoutItem* child;
    while (child = m_ui->m_scrollAreaWidgetContents->layout()->takeAt(0))
    {
        delete child->widget();
        delete child;
    }
    m_definitions.clear();
    m_entryMutex.unlock();
}

void DefinitionWidget::leaveEvent(QEvent *event)
{
    hide();
    Q_EMIT definitionHidden();
}