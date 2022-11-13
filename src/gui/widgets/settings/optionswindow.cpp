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
#include "audiosourcesettings.h"
#include "behaviorsettings.h"
#include "dictionarysettings.h"
#include "interfacesettings.h"
#ifdef OCR_SUPPORT
#include "ocrsettings.h"
#endif // OCR_SUPPORT
#include "searchsettings.h"

#include "util/globalmediator.h"

/* Begin Constructor/Destructor */

#define NAME_ANKI           "Anki Integration"
#define NAME_AUDIO_SOURCE   "Audio Sources"
#define NAME_BEHAVIOR       "Behavior"
#define NAME_DICTIONARIES   "Dictionaries"
#define NAME_INTERFACE      "Interface"
#define NAME_OCR            "OCR"
#define NAME_SEARCH         "Search"

OptionsWindow::OptionsWindow(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::OptionsWindow),
      m_currentWidget(nullptr)
{
    m_ui->setupUi(this);

    QFont labelFont = m_ui->labelOption->font();
#if defined(Q_OS_MACOS)
    labelFont.setPointSize(20);
#else
    labelFont.setPointSize(14);
#endif
    m_ui->labelOption->setFont(labelFont);

    addOption(NAME_ANKI,         new AnkiSettings);
    addOption(NAME_AUDIO_SOURCE, new AudioSourceSettings);
    addOption(NAME_BEHAVIOR,     new BehaviorSettings);
    addOption(NAME_DICTIONARIES, new DictionarySettings);
    addOption(NAME_SEARCH,       new SearchSettings);
    addOption(NAME_INTERFACE,    new InterfaceSettings);
#ifdef OCR_SUPPORT
    addOption(NAME_OCR,          new OCRSettings);
#endif // OCR_SUPPORT

    connect(
        m_ui->listOptions, &QListWidget::itemSelectionChanged,
        this, &OptionsWindow::showSelectedOption
    );
}

#undef NAME_ANKI
#undef NAME_AUDIO_SOURCE
#undef NAME_DICTIONARIES
#undef NAME_INTERFACE
#undef NAME_SEARCH

OptionsWindow::~OptionsWindow()
{
    disconnect();
    delete m_ui;
}

/* End Constructor/Destructor */
/* Begin Event Handlers */

void OptionsWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    m_ui->listOptions->setCurrentRow(0);
    showSelectedOption();
}

/* End Event Handlers */
/* Begin Helpers */

void OptionsWindow::addOption(const QString &name, QWidget *widget)
{
    m_ui->listOptions->addItem(name);
    widget->hide();
    m_ui->layoutWidgets->addWidget(widget);
    m_widgets.insert(name, widget);
}

void OptionsWindow::showSelectedOption()
{
    QString selected = m_ui->listOptions->selectedItems().first()->text();
    if (m_currentWidget)
        m_currentWidget->hide();

    QWidget *widget = m_widgets[selected];
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

/* End Helpers */
