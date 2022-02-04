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

#include "searchsettings.h"
#include "ui_searchsettings.h"

#include <QPushButton>
#include <QSettings>

#include "../../../util/constants.h"
#include "../../../util/globalmediator.h"

/* Begin Constructor/Destructor */

SearchSettings::SearchSettings(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::SearchSettings)
{
    m_ui->setupUi(this);

    m_ui->frameHover->setVisible(false);
    m_ui->frameModifier->setVisible(false);

    m_ui->comboBoxMethod->addItem(SEARCH_METHOD_HOVER);
    m_ui->comboBoxMethod->addItem(SEARCH_METHOD_MODIFIER);

    connect(
        m_ui->comboBoxMethod, &QComboBox::currentTextChanged,
        this,                 &SearchSettings::methodTextChanged
    );

    m_ui->comboBoxModifier->addItem(MODIFIER_ALT);
    m_ui->comboBoxModifier->addItem(MODIFIER_CTRL);
    m_ui->comboBoxModifier->addItem(MODIFIER_SHIFT);
    m_ui->comboBoxModifier->addItem(MODIFIER_SUPER);

    connect(
        m_ui->buttonBox->button(QDialogButtonBox::Reset),
        &QPushButton::clicked,
        this,
        &SearchSettings::restoreSaved
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults),
        &QPushButton::clicked,
        this,
        &SearchSettings::restoreDefaults
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::Apply),
        &QPushButton::clicked,
        this,
        &SearchSettings::applySettings
    );
}

SearchSettings::~SearchSettings()
{
    disconnect();
    delete m_ui;
}

/* End Constructor/Destructor */
/* Begin Event Handlers */

void SearchSettings::showEvent(QShowEvent *event)
{
    restoreSaved();
    methodTextChanged(m_ui->comboBoxMethod->currentText());
}

/* End Event Handlers */
/* Begin Button Box Handlers */

void SearchSettings::restoreSaved()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);
    m_ui->spinLimitResults->setValue(
        settings.value(SETTINGS_SEARCH_LIMIT, DEFAULT_LIMIT).toInt()
    );
    m_ui->comboBoxMethod->setCurrentText(
        settings.value(SETTINGS_SEARCH_METHOD, DEFAULT_METHOD).toString()
    );
    m_ui->spinBoxDelay->setValue(
        settings.value(SETTINGS_SEARCH_DELAY, DEFAULT_DELAY).toInt()
    );
    m_ui->comboBoxModifier->setCurrentText(
        settings.value(SETTINGS_SEARCH_MODIFIER, DEFAULT_MODIFIER).toString()
    );
    m_ui->checkHideSubs->setChecked(
        settings.value(SETTINGS_SEARCH_HIDE_SUBS, DEFAULT_HIDE_SUBS).toBool()
    );
    m_ui->checkHideSearch->setChecked(
        settings.value(SETTINGS_SEARCH_HIDE_BAR, DEFAULT_HIDE_BAR).toBool()
    );
    m_ui->checkGlossaryList->setChecked(
        settings.value(
            SETTINGS_SEARCH_LIST_GLOSSARY, DEFAULT_LIST_GLOSSARY
        ).toBool()
    );
    m_ui->checkReplaceNewLines->setChecked(
        settings.value(
            SETTINGS_SEARCH_REPLACE_LINES, DEFAULT_REPLACE_LINES
        ).toBool()
    );
    m_ui->lineEditReplace->setText(
        settings.value(
            SETTINGS_SEARCH_REPLACE_WITH, DEFAULT_REPLACE_WITH
        ).toString()
    );
    m_ui->lineRemoveRegex->setText(
        settings.value(
            SETTINGS_SEARCH_REMOVE_REGEX, DEFAULT_REMOVE_REGEX
        ).toString()
    );
    settings.endGroup();
}

void SearchSettings::restoreDefaults()
{
    m_ui->spinLimitResults    ->setValue      (DEFAULT_LIMIT);
    m_ui->comboBoxMethod      ->setCurrentText(DEFAULT_METHOD);
    m_ui->spinBoxDelay        ->setValue      (DEFAULT_DELAY);
    m_ui->comboBoxModifier    ->setCurrentText(DEFAULT_MODIFIER);
    m_ui->checkHideSubs       ->setChecked    (DEFAULT_HIDE_SUBS);
    m_ui->checkHideSearch     ->setChecked    (DEFAULT_HIDE_BAR);
    m_ui->checkGlossaryList   ->setChecked    (DEFAULT_LIST_GLOSSARY);
    m_ui->checkReplaceNewLines->setChecked    (DEFAULT_REPLACE_LINES);
    m_ui->lineEditReplace     ->setText       (DEFAULT_REPLACE_WITH);
    m_ui->lineRemoveRegex     ->setText       (DEFAULT_REMOVE_REGEX);
}

void SearchSettings::applySettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);
    settings.setValue(
        SETTINGS_SEARCH_LIMIT, m_ui->spinLimitResults->value()
    );
    settings.setValue(
        SETTINGS_SEARCH_METHOD, m_ui->comboBoxMethod->currentText()
    );
    settings.setValue(
        SETTINGS_SEARCH_DELAY, m_ui->spinBoxDelay->value()
    );
    settings.setValue(
        SETTINGS_SEARCH_MODIFIER, m_ui->comboBoxModifier->currentText()
    );
    settings.setValue(
        SETTINGS_SEARCH_HIDE_SUBS, m_ui->checkHideSubs->isChecked()
    );
    settings.setValue(
        SETTINGS_SEARCH_HIDE_BAR, m_ui->checkHideSearch->isChecked()
    );
    settings.setValue(
        SETTINGS_SEARCH_LIST_GLOSSARY, m_ui->checkGlossaryList->isChecked()
    );
    settings.setValue(
        SETTINGS_SEARCH_REPLACE_LINES, m_ui->checkReplaceNewLines->isChecked()
    );
    settings.setValue(
        SETTINGS_SEARCH_REPLACE_WITH, m_ui->lineEditReplace->text()
    );
    settings.setValue(
        SETTINGS_SEARCH_REMOVE_REGEX, m_ui->lineRemoveRegex->text()
    );
    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->searchSettingsChanged();
}

/* Begin Button Box Handlers */
/* Begin Combo Box Handlers */

void SearchSettings::methodTextChanged(const QString &text)
{
    if (text == SEARCH_METHOD_HOVER)
    {
        m_ui->frameHover->setVisible(true);
        m_ui->frameModifier->setVisible(false);
    }
    else if (text == SEARCH_METHOD_MODIFIER)
    {
        m_ui->frameHover->setVisible(false);
        m_ui->frameModifier->setVisible(true);
    }
}

/* End Combo Box Handlers */
