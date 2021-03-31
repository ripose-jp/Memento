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

#include "optionswindow.h"
#include "ui_optionswindow.h"

#include "ankisettings.h"
#include "dictionarysettings.h"

#define NAME_ANKI           "Anki Integration"
#define NAME_DICTIONARIES   "Dictionaries"

OptionsWindow::OptionsWindow(QWidget *parent)
    : QWidget(parent), m_ui(new Ui::OptionsWindow), m_currentWidget(nullptr)
{
    m_ui->setupUi(this);

    m_ui->listOptions->addItem(NAME_ANKI);
    m_ui->listOptions->addItem(NAME_DICTIONARIES);

    AnkiSettings *ankiSettings = new AnkiSettings;
    ankiSettings->hide();
    m_ui->layoutWidgets->addWidget(ankiSettings);
    widgets.insert(NAME_ANKI, ankiSettings);

    DictionarySettings *dictSettings = new DictionarySettings;
    dictSettings->hide();
    m_ui->layoutWidgets->addWidget(dictSettings);
    widgets.insert(NAME_DICTIONARIES, dictSettings);

    connect(m_ui->listOptions, &QListWidget::itemSelectionChanged, this, &OptionsWindow::showSelectedOption);
}

OptionsWindow::~OptionsWindow()
{
    delete m_ui;
}

void OptionsWindow::showEvent(QShowEvent *event)
{
    m_ui->listOptions->setCurrentRow(0);
    showSelectedOption();
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