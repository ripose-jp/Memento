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

#include "optionswindow.h"
#include "ui_optionswindow.h"

#include "ankisettings.h"
#include "dictionarysettings.h"
#include "searchsettings.h"
#include "interfacesettings.h"

#define NAME_ANKI           "Anki Integration"
#define NAME_DICTIONARIES   "Dictionaries"
#define NAME_SEARCH         "Search"
#define NAME_INTERFACE      "Interface"

OptionsWindow::OptionsWindow(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::OptionsWindow), m_currentWidget(nullptr)
{
    m_ui->setupUi(this);

    addOption(NAME_ANKI,         new AnkiSettings);
    addOption(NAME_DICTIONARIES, new DictionarySettings);
    addOption(NAME_SEARCH,       new SearchSettings);
    addOption(NAME_INTERFACE,    new InterfaceSettings);

    connect(m_ui->listOptions, &QListWidget::itemSelectionChanged, this, &OptionsWindow::showSelectedOption);
}

OptionsWindow::~OptionsWindow()
{
    disconnect();
    delete m_ui;
}

void OptionsWindow::showEvent(QShowEvent *event)
{
    m_ui->listOptions->setCurrentRow(0);
    showSelectedOption();
}

void OptionsWindow::addOption(const QString &name, QWidget *widget)
{
    m_ui->listOptions->addItem(name);
    widget->hide();
    m_ui->layoutWidgets->addWidget(widget);
    widgets.insert(name, widget);
}

void OptionsWindow::showSelectedOption()
{
    QString selected = m_ui->listOptions->selectedItems().first()->text();
    if (m_currentWidget)
        m_currentWidget->hide();

    QWidget *widget = widgets[selected];
    if (widget)
    {
        widget->show();
        m_currentWidget = widget;
        m_ui->labelOption->setText(selected);
    }
    else if (m_currentWidget)
    {
        m_currentWidget->show();
    }
}