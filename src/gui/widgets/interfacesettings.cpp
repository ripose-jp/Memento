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

#include "interfacesettings.h"
#include "ui_interfacesettings.h"

#include "../../util/constants.h"
#include "../../util/globalmediator.h"

#include <QPushButton>
#include <QSettings>
#include <QDesktopServices>

InterfaceSettings::InterfaceSettings(QWidget *parent) : QWidget(parent), m_ui(new Ui::InterfaceSettings)
{
    m_ui->setupUi(this);

    connect(m_ui->checkStyleSheets, &QCheckBox::stateChanged, this, 
        [=] (const int state) {
            m_ui->frameStyleSheets->setEnabled(state == Qt::CheckState::Checked);
        }
    );

    connect(m_ui->buttonBox->button(QDialogButtonBox::StandardButton::RestoreDefaults), &QPushButton::clicked,
        this, &InterfaceSettings::restoreDefaults);
    connect(m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Reset), &QPushButton::clicked,
        this, &InterfaceSettings::restoreSaved);
    connect(m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply), &QPushButton::clicked,
        this, &InterfaceSettings::applyChanges);
    connect(m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Help), &QPushButton::clicked,
        this, [=] {
            QDesktopServices::openUrl(QUrl("https://doc.qt.io/qt-5/stylesheet-reference.html"));
        }
    );
}

InterfaceSettings::~InterfaceSettings()
{
    delete m_ui;
}

void InterfaceSettings::showEvent(QShowEvent *event)
{
    restoreSaved();
}

void InterfaceSettings::restoreDefaults()
{
    m_ui->comboTheme->setCurrentIndex((int)SETTINGS_INTERFACE_THEME_DEFAULT);

    /* Subtitle */
    m_ui->spinSubOffset->setValue(SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT);

    /* Style Sheets */
    m_ui->checkStyleSheets->setChecked(SETTINGS_INTERFACE_STYLESHEETS_DEFAULT);
    m_ui->editSubList->setPlainText(SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT);
    m_ui->editSplitter->setPlainText(SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE_DEFAULT);
    m_ui->editDefinitions->setPlainText(SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT);
}

void InterfaceSettings::restoreSaved()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    m_ui->comboTheme->setCurrentIndex(
        settings.value(
            SETTINGS_INTERFACE_THEME, 
            (int)SETTINGS_INTERFACE_THEME_DEFAULT
        ).toInt()
    );

    /* Subtitle */
    m_ui->spinSubOffset->setValue(settings.value(
            SETTINGS_INTERFACE_SUB_OFFSET,
            SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT
        ).toDouble()
    );

    /* Style Sheets */
    m_ui->checkStyleSheets->setChecked(
        settings.value(
            SETTINGS_INTERFACE_STYLESHEETS,
            SETTINGS_INTERFACE_STYLESHEETS_DEFAULT
        ).toBool()
    );
    m_ui->editSubList->setPlainText(
        settings.value(
            SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE,
            SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT
        ).toString()
    );
    m_ui->editSplitter->setPlainText(
        settings.value(
            SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE,
            SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE_DEFAULT
        ).toString()
    );
    m_ui->editDefinitions->setPlainText(
        settings.value(
            SETTINGS_INTERFACE_DEFINITION_STYLE,
            SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT
        ).toString()
    );
    settings.endGroup();
}

void InterfaceSettings::applyChanges()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);

    settings.setValue(SETTINGS_INTERFACE_THEME, m_ui->comboTheme->currentIndex());

    /* Subtitle */
    settings.setValue(SETTINGS_INTERFACE_SUB_OFFSET, m_ui->spinSubOffset->value());

    /* Style Sheets */
    settings.setValue(SETTINGS_INTERFACE_STYLESHEETS,           m_ui->checkStyleSheets->isChecked());
    settings.setValue(SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE,   m_ui->editSubList->toPlainText());
    settings.setValue(SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE, m_ui->editSplitter->toPlainText());
    settings.setValue(SETTINGS_INTERFACE_DEFINITION_STYLE,      m_ui->editDefinitions->toPlainText());

    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->interfaceSettingsChanged();
}