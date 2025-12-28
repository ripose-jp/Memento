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

/* Begin Constructor/Destructor */

OptionsWindow::OptionsWindow(Context *context, QWidget *parent) :
    QWidget(parent),
    m_ui(std::make_unique<Ui::OptionsWindow>()),
    m_context(std::move(context))
{
    static const QString NAME_ANKI = tr("Anki Integration");
    static const QString NAME_AUDIO_SOURCE = tr("Audio Sources");
    static const QString NAME_BEHAVIOR = tr("Behavior");
    static const QString NAME_DICTIONARIES = tr("Dictionaries");
    static const QString NAME_INTERFACE = tr("Interface");
    static const QString NAME_SEARCH = tr("Search");
#ifdef OCR_SUPPORT
    static const QString NAME_OCR = tr("OCR");
#endif // OCR_SUPPORT

    m_ui->setupUi(this);

    QFont labelFont = m_ui->labelOption->font();
#if defined(Q_OS_MACOS)
    labelFont.setPointSize(20);
#else
    labelFont.setPointSize(14);
#endif
    m_ui->labelOption->setFont(labelFont);

    addOption(NAME_ANKI, new AnkiSettings(m_context));
    addOption(NAME_AUDIO_SOURCE, new AudioSourceSettings(m_context));
    addOption(NAME_BEHAVIOR, new BehaviorSettings(m_context));
    addOption(NAME_DICTIONARIES, new DictionarySettings(m_context));
    addOption(NAME_SEARCH, new SearchSettings(m_context));
    addOption(NAME_INTERFACE, new InterfaceSettings(m_context));
#ifdef OCR_SUPPORT
    addOption(NAME_OCR, new OCRSettings(m_context));
#endif // OCR_SUPPORT

    connect(
        m_ui->listOptions, &QListWidget::itemSelectionChanged,
        this, &OptionsWindow::showSelectedOption
    );
}

OptionsWindow::~OptionsWindow()
{

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

void OptionsWindow::addOption(const QString &name, QPointer<QWidget> widget)
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
