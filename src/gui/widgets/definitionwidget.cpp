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

#include "definition.h"

#include <QFrame>
#include <QScrollBar>

DefinitionWidget::DefinitionWidget(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::DefinitionWidget)
{
    m_ui->setupUi(this);
    hide();
}

DefinitionWidget::~DefinitionWidget()
{
    clearEntries();
    delete m_ui;
}

void DefinitionWidget::setEntries(const QList<const Entry *> *entries)
{
    clearEntries();
    if (entries->empty())
    {
        delete entries;
        return;
    }

    Definition *definition = new Definition(*entries->begin(), this);
    m_ui->m_scrollAreaWidgetContents->layout()->addWidget(definition);

    for (auto it = entries->begin() + 1; it != entries->end(); ++it)
    {
        QFrame *line = new QFrame(this);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
        definition = new Definition(*it, this);
        m_ui->m_scrollAreaWidgetContents->layout()->addWidget(line);
        m_ui->m_scrollAreaWidgetContents->layout()->addWidget(definition);
    }

    delete entries;

    m_ui->m_scrollArea->verticalScrollBar()->setValue(0);
    show();
}

void DefinitionWidget::clearEntries()
{
    QLayoutItem* child;
    while (child = m_ui->m_scrollAreaWidgetContents->layout()->takeAt(0))
    {
        delete child->widget();
        delete child;
    }
}

void DefinitionWidget::leaveEvent(QEvent *event)
{
    hide();
    Q_EMIT definitionHidden();
}