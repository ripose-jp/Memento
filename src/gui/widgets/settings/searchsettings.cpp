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

#include "util/constants.h"
#include "util/globalmediator.h"

/* Begin Constructor/Destructor */

SearchSettings::SearchSettings(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::SearchSettings)
{
    m_ui->setupUi(this);

    m_ui->frameHover->setVisible(false);

    m_ui->comboBoxMethod->addItem(SEARCH_METHOD_HOVER);
    m_ui->comboBoxMethod->addItem(SEARCH_METHOD_MODIFIER);

    connect(
        m_ui->comboBoxMethod, &QComboBox::currentTextChanged,
        this, &SearchSettings::methodTextChanged
    );

    m_ui->comboBoxModifier->addItem(SEARCH_MODIFIER_ALT);
    m_ui->comboBoxModifier->addItem(SEARCH_MODIFIER_CTRL);
    m_ui->comboBoxModifier->addItem(SEARCH_MODIFIER_SHIFT);
    m_ui->comboBoxModifier->addItem(SEARCH_MODIFIER_SUPER);

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
    QWidget::showEvent(event);
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
        settings.value(
            SETTINGS_SEARCH_LIMIT, SETTINGS_SEARCH_LIMIT_DEFAULT
        ).toInt()
    );
    m_ui->comboBoxMethod->setCurrentText(
        settings.value(
            SETTINGS_SEARCH_METHOD, SETTINGS_SEARCH_METHOD_DEFAULT
        ).toString()
    );
    m_ui->spinBoxDelay->setValue(
        settings.value(
            SETTINGS_SEARCH_DELAY, SETTINGS_SEARCH_DELAY_DEFAULT
        ).toInt()
    );
    m_ui->comboBoxModifier->setCurrentText(
        settings.value(
            SETTINGS_SEARCH_MODIFIER, SETTINGS_SEARCH_MODIFIER_DEFAULT
        ).toString()
    );
    m_ui->checkHideSubs->setChecked(
        settings.value(
            SETTINGS_SEARCH_HIDE_SUBS, SETTINGS_SEARCH_HIDE_SUBS_DEFAULT
        ).toBool()
    );
    m_ui->checkHideSearch->setChecked(
        settings.value(
            SETTINGS_SEARCH_HIDE_BAR, SETTINGS_SEARCH_HIDE_BAR_DEFAULT
        ).toBool()
    );
    m_ui->checkHoverPause->setChecked(
        settings.value(
            SETTINGS_SEARCH_HOVER_PAUSE, SETTINGS_SEARCH_HOVER_PAUSE_DEFAULT
        ).toBool()
    );
    m_ui->checkReplaceNewLines->setChecked(
        settings.value(
            SETTINGS_SEARCH_REPLACE_LINES, SETTINGS_SEARCH_REPLACE_LINES_DEFAULT
        ).toBool()
    );
    m_ui->lineEditReplace->setText(
        settings.value(
            SETTINGS_SEARCH_REPLACE_WITH, SETTINGS_SEARCH_REPLACE_WITH_DEFAULT
        ).toString()
    );
    m_ui->comboGlossaryStyle->setCurrentIndex(
        settings.value(
            SETTINGS_SEARCH_LIST_GLOSSARY,
            static_cast<int>(SETTINGS_SEARCH_LIST_GLOSSARY_DEFAULT)
        ).toInt()
    );
    m_ui->lineRemoveRegex->setText(
        settings.value(
            SETTINGS_SEARCH_REMOVE_REGEX, SETTINGS_SEARCH_REMOVE_REGEX_DEFAULT
        ).toString()
    );
    settings.endGroup();
}

void SearchSettings::restoreDefaults()
{
    m_ui->spinLimitResults->setValue(SETTINGS_SEARCH_LIMIT_DEFAULT);
    m_ui->comboBoxMethod->setCurrentText(SETTINGS_SEARCH_METHOD_DEFAULT);
    m_ui->spinBoxDelay->setValue(SETTINGS_SEARCH_DELAY_DEFAULT);
    m_ui->comboBoxModifier->setCurrentText(SETTINGS_SEARCH_MODIFIER_DEFAULT);
    m_ui->checkHideSubs->setChecked(SETTINGS_SEARCH_HIDE_SUBS_DEFAULT);
    m_ui->checkHideSearch->setChecked(SETTINGS_SEARCH_HIDE_BAR_DEFAULT);
    m_ui->checkHoverPause->setChecked(SETTINGS_SEARCH_HOVER_PAUSE_DEFAULT);
    m_ui->checkReplaceNewLines->setChecked(
        SETTINGS_SEARCH_REPLACE_LINES_DEFAULT
    );
    m_ui->lineEditReplace->setText(SETTINGS_SEARCH_REPLACE_WITH_DEFAULT);
    m_ui->comboGlossaryStyle->setCurrentIndex(
        static_cast<int>(SETTINGS_SEARCH_LIST_GLOSSARY_DEFAULT)
    );
    m_ui->lineRemoveRegex->setText(SETTINGS_SEARCH_REMOVE_REGEX_DEFAULT);
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
        SETTINGS_SEARCH_HOVER_PAUSE, m_ui->checkHoverPause->isChecked()
    );
    settings.setValue(
        SETTINGS_SEARCH_REPLACE_LINES, m_ui->checkReplaceNewLines->isChecked()
    );
    settings.setValue(
        SETTINGS_SEARCH_REPLACE_WITH, m_ui->lineEditReplace->text()
    );
    settings.setValue(
        SETTINGS_SEARCH_LIST_GLOSSARY,
        m_ui->comboGlossaryStyle->currentIndex()
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
    m_ui->frameHover->setVisible(text == SEARCH_METHOD_HOVER);
}

/* End Combo Box Handlers */
