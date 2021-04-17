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

#include "../../util/globalmediator.h"
#include "../../util/constants.h"

#include <QSettings>
#include <QPushButton>

SearchSettings::SearchSettings(QWidget *parent) : QWidget(parent), m_ui(new Ui::SearchSettings)
{
    m_ui->setupUi(this);

    m_ui->frameHover->setVisible(false);
    m_ui->frameModifier->setVisible(false);

    m_ui->comboBoxMethod->addItem(SEARCH_METHOD_HOVER);
    m_ui->comboBoxMethod->addItem(SEARCH_METHOD_MODIFIER);

    connect(m_ui->comboBoxMethod, &QComboBox::currentTextChanged, this, &SearchSettings::methodTextChanged);

    m_ui->comboBoxModifier->addItem(MODIFIER_ALT);
    m_ui->comboBoxModifier->addItem(MODIFIER_CTRL);
    m_ui->comboBoxModifier->addItem(MODIFIER_SHIFT);
    m_ui->comboBoxModifier->addItem(MODIFIER_SUPER);

    connect(m_ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked,
        this, &SearchSettings::resetSaved);
    connect(m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked,
        this, &SearchSettings::resetDefault);
    connect(m_ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
        this, &SearchSettings::applySettings);
}

SearchSettings::~SearchSettings()
{
    delete m_ui;
}

void SearchSettings::resetSaved()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);
    m_ui->spinLimitResults    ->setValue      (settings.value(SETTINGS_SEARCH_LIMIT,         DEFAULT_LIMIT   ).toInt());
    m_ui->comboBoxMethod      ->setCurrentText(settings.value(SETTINGS_SEARCH_METHOD,        DEFAULT_METHOD  ).toString());
    m_ui->spinBoxDelay        ->setValue      (settings.value(SETTINGS_SEARCH_DELAY,         DEFAULT_DELAY   ).toInt());
    m_ui->comboBoxModifier    ->setCurrentText(settings.value(SETTINGS_SEARCH_MODIFIER,      DEFAULT_MODIFIER).toString());
    m_ui->checkHideSubs       ->setChecked    (settings.value(SETTINGS_SEARCH_HIDE_SUBS,     DEFAULT_HIDE_SUBS).toBool());
    m_ui->checkHideSearch     ->setChecked    (settings.value(SETTINGS_SEARCH_HIDE_BAR,      DEFAULT_HIDE_BAR).toBool());
    m_ui->checkReplaceNewLines->setChecked    (settings.value(SETTINGS_SEARCH_REPLACE_LINES, DEFAULT_REPLACE_LINES).toBool());
    m_ui->lineEditReplace     ->setText       (settings.value(SETTINGS_SERACH_REPLACE_WITH,  DEFAULT_REPLACE_WITH).toString());
    settings.endGroup();
}

void SearchSettings::resetDefault()
{
    m_ui->spinLimitResults    ->setValue      (DEFAULT_LIMIT);
    m_ui->comboBoxMethod      ->setCurrentText(DEFAULT_METHOD);
    m_ui->spinBoxDelay        ->setValue      (DEFAULT_DELAY);
    m_ui->comboBoxModifier    ->setCurrentText(DEFAULT_MODIFIER);
    m_ui->checkHideSubs       ->setChecked    (DEFAULT_HIDE_SUBS);
    m_ui->checkHideSearch     ->setChecked    (DEFAULT_HIDE_BAR);
    m_ui->checkReplaceNewLines->setChecked    (DEFAULT_REPLACE_LINES);
    m_ui->lineEditReplace     ->setText       (DEFAULT_REPLACE_WITH);
}

void SearchSettings::applySettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);
    settings.setValue(SETTINGS_SEARCH_LIMIT,         m_ui->spinLimitResults->value());
    settings.setValue(SETTINGS_SEARCH_METHOD,        m_ui->comboBoxMethod->currentText());
    settings.setValue(SETTINGS_SEARCH_DELAY,         m_ui->spinBoxDelay->value());
    settings.setValue(SETTINGS_SEARCH_MODIFIER,      m_ui->comboBoxModifier->currentText());
    settings.setValue(SETTINGS_SEARCH_HIDE_SUBS,     m_ui->checkHideSubs->isChecked());
    settings.setValue(SETTINGS_SEARCH_HIDE_BAR,      m_ui->checkHideSearch->isChecked());
    settings.setValue(SETTINGS_SEARCH_REPLACE_LINES, m_ui->checkReplaceNewLines->isChecked());
    settings.setValue(SETTINGS_SERACH_REPLACE_WITH,  m_ui->lineEditReplace->text());
    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->searchSettingsChanged();
}

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

void SearchSettings::showEvent(QShowEvent *event)
{
    resetSaved();
    methodTextChanged(m_ui->comboBoxMethod->currentText());
}