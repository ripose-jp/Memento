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

InterfaceSettings::InterfaceSettings(QWidget *parent) : QWidget(parent), m_ui(new Ui::InterfaceSettings)
{
    m_ui->setupUi(this);

    connect(m_ui->buttonBox->button(QDialogButtonBox::StandardButton::RestoreDefaults), &QPushButton::clicked,
        this, &InterfaceSettings::restoreDefaults);
    connect(m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Reset), &QPushButton::clicked,
        this, &InterfaceSettings::restoreSaved);
    connect(m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply), &QPushButton::clicked,
        this, &InterfaceSettings::applyChanges);
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
}

void InterfaceSettings::restoreSaved()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    m_ui->comboTheme->setCurrentIndex(settings.value(SETTINGS_INTERFACE_THEME, (int)SETTINGS_INTERFACE_THEME_DEFAULT).toInt());
    settings.endGroup();
}

void InterfaceSettings::applyChanges()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    settings.setValue(SETTINGS_INTERFACE_THEME, m_ui->comboTheme->currentIndex());
    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->interfaceSettingsChanged();
}