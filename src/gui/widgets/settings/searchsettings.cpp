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

    m_ui->comboBoxMethod->addItem(Constants::Settings::Search::Method::HOVER);
    m_ui->comboBoxMethod->addItem(
        Constants::Settings::Search::Method::MODIFIER
    );

    connect(
        m_ui->comboBoxMethod, &QComboBox::currentTextChanged,
        this, &SearchSettings::methodTextChanged
    );

    m_ui->comboBoxModifier->addItem(Constants::Settings::Search::Modifier::ALT);
    m_ui->comboBoxModifier->addItem(
        Constants::Settings::Search::Modifier::CTRL
    );
    m_ui->comboBoxModifier->addItem(
        Constants::Settings::Search::Modifier::SHIFT
    );
    m_ui->comboBoxModifier->addItem(
        Constants::Settings::Search::Modifier::SUPER
    );

#ifndef MECAB_SUPPORT
    m_ui->checkMecabIpadic->setVisible(false);
#endif // MECAB_SUPPORT

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
    settings.beginGroup(Constants::Settings::Search::GROUP);

    m_ui->checkExact->setChecked(
        settings.value(
            Constants::Settings::Search::Matcher::EXACT,
            Constants::Settings::Search::Matcher::EXACT_DEFAULT
        ).toBool()
    );
    m_ui->checkDeconj->setChecked(
        settings.value(
            Constants::Settings::Search::Matcher::DECONJ,
            Constants::Settings::Search::Matcher::DECONJ_DEFAULT
        ).toBool()
    );
#ifdef MECAB_SUPPORT
    m_ui->checkMecabIpadic->setChecked(
        settings.value(
            Constants::Settings::Search::Matcher::MECAB_IPADIC,
            Constants::Settings::Search::Matcher::MECAB_IPADIC_DEFAULT
        ).toBool()
    );
#endif // MECAB_SUPPORT

    m_ui->spinLimitResults->setValue(
        settings.value(
            Constants::Settings::Search::LIMIT,
            Constants::Settings::Search::LIMIT_DEFAULT
        ).toInt()
    );
    m_ui->comboBoxMethod->setCurrentText(
        settings.value(
            Constants::Settings::Search::METHOD,
            Constants::Settings::Search::METHOD_DEFAULT
        ).toString()
    );
    m_ui->spinBoxDelay->setValue(
        settings.value(
            Constants::Settings::Search::DELAY,
            Constants::Settings::Search::DELAY_DEFAULT
        ).toInt()
    );
    m_ui->comboBoxModifier->setCurrentText(
        settings.value(
            Constants::Settings::Search::MODIFIER,
            Constants::Settings::Search::MODIFIER_DEFAULT
        ).toString()
    );
    m_ui->checkHideSubs->setChecked(
        settings.value(
            Constants::Settings::Search::HIDE_SUBS,
            Constants::Settings::Search::HIDE_SUBS_DEFAULT
        ).toBool()
    );
    m_ui->checkHideSearch->setChecked(
        settings.value(
            Constants::Settings::Search::HIDE_BAR,
            Constants::Settings::Search::HIDE_BAR_DEFAULT
        ).toBool()
    );
    m_ui->checkHoverPause->setChecked(
        settings.value(
            Constants::Settings::Search::HOVER_PAUSE,
            Constants::Settings::Search::HOVER_PAUSE_DEFAULT
        ).toBool()
    );
    m_ui->checkAutoPlayAudio->setChecked(
        settings.value(
            Constants::Settings::Search::AUTO_PLAY_AUDIO,
            Constants::Settings::Search::AUTO_PLAY_AUDIO_DEFAULT
        ).toBool()
    );
    m_ui->checkReplaceNewLines->setChecked(
        settings.value(
            Constants::Settings::Search::REPLACE_LINES,
            Constants::Settings::Search::REPLACE_LINES_DEFAULT
        ).toBool()
    );
    m_ui->lineEditReplace->setText(
        settings.value(
            Constants::Settings::Search::REPLACE_WITH,
            Constants::Settings::Search::REPLACE_WITH_DEFAULT
        ).toString()
    );
    m_ui->comboGlossaryStyle->setCurrentIndex(
        settings.value(
            Constants::Settings::Search::LIST_GLOSSARY,
            static_cast<int>(Constants::Settings::Search::LIST_GLOSSARY_DEFAULT)
        ).toInt()
    );
    m_ui->lineRemoveRegex->setText(
        settings.value(
            Constants::Settings::Search::REMOVE_REGEX,
            Constants::Settings::Search::REMOVE_REGEX_DEFAULT
        ).toString()
    );

    settings.endGroup();
}

void SearchSettings::restoreDefaults()
{
    m_ui->checkExact->setChecked(
        Constants::Settings::Search::Matcher::EXACT_DEFAULT
    );
    m_ui->checkDeconj->setChecked(
        Constants::Settings::Search::Matcher::DECONJ_DEFAULT
    );
#ifdef MECAB_SUPPORT
    m_ui->checkMecabIpadic->setChecked(
        Constants::Settings::Search::Matcher::MECAB_IPADIC_DEFAULT
    );
#endif // MECAB_SUPPORT

    m_ui->spinLimitResults->setValue(
        Constants::Settings::Search::LIMIT_DEFAULT
    );
    m_ui->comboBoxMethod->setCurrentText(
        Constants::Settings::Search::METHOD_DEFAULT
    );
    m_ui->spinBoxDelay->setValue(Constants::Settings::Search::DELAY_DEFAULT);
    m_ui->comboBoxModifier->setCurrentText(
        Constants::Settings::Search::MODIFIER_DEFAULT
    );
    m_ui->checkHideSubs->setChecked(
        Constants::Settings::Search::HIDE_SUBS_DEFAULT
    );
    m_ui->checkHideSearch->setChecked(
        Constants::Settings::Search::HIDE_BAR_DEFAULT
    );
    m_ui->checkHoverPause->setChecked(
        Constants::Settings::Search::HOVER_PAUSE_DEFAULT
    );
    m_ui->checkAutoPlayAudio->setChecked(
        Constants::Settings::Search::AUTO_PLAY_AUDIO_DEFAULT
    );
    m_ui->checkReplaceNewLines->setChecked(
        Constants::Settings::Search::REPLACE_LINES_DEFAULT
    );
    m_ui->lineEditReplace->setText(
        Constants::Settings::Search::REPLACE_WITH_DEFAULT
    );
    m_ui->comboGlossaryStyle->setCurrentIndex(
        static_cast<int>(Constants::Settings::Search::LIST_GLOSSARY_DEFAULT)
    );
    m_ui->lineRemoveRegex->setText(
        Constants::Settings::Search::REMOVE_REGEX_DEFAULT
    );
}

void SearchSettings::applySettings()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Search::GROUP);

    settings.setValue(
        Constants::Settings::Search::Matcher::EXACT,
        m_ui->checkExact->isChecked()
    );
    settings.setValue(
        Constants::Settings::Search::Matcher::DECONJ,
        m_ui->checkDeconj->isChecked()
    );
#ifdef MECAB_SUPPORT
    settings.setValue(
        Constants::Settings::Search::Matcher::MECAB_IPADIC,
        m_ui->checkMecabIpadic->isChecked()
    );
#endif // MECAB_SUPPORT

    settings.setValue(
        Constants::Settings::Search::LIMIT,
        m_ui->spinLimitResults->value()
    );
    settings.setValue(
        Constants::Settings::Search::METHOD,
        m_ui->comboBoxMethod->currentText()
    );
    settings.setValue(
        Constants::Settings::Search::DELAY,
        m_ui->spinBoxDelay->value()
    );
    settings.setValue(
        Constants::Settings::Search::MODIFIER,
        m_ui->comboBoxModifier->currentText()
    );
    settings.setValue(
        Constants::Settings::Search::HIDE_SUBS,
        m_ui->checkHideSubs->isChecked()
    );
    settings.setValue(
        Constants::Settings::Search::HIDE_BAR,
        m_ui->checkHideSearch->isChecked()
    );
    settings.setValue(
        Constants::Settings::Search::HOVER_PAUSE,
        m_ui->checkHoverPause->isChecked()
    );
    settings.setValue(
        Constants::Settings::Search::AUTO_PLAY_AUDIO,
        m_ui->checkAutoPlayAudio->isChecked()
    );
    settings.setValue(
        Constants::Settings::Search::REPLACE_LINES,
        m_ui->checkReplaceNewLines->isChecked()
    );
    settings.setValue(
        Constants::Settings::Search::REPLACE_WITH,
        m_ui->lineEditReplace->text()
    );
    settings.setValue(
        Constants::Settings::Search::LIST_GLOSSARY,
        m_ui->comboGlossaryStyle->currentIndex()
    );
    settings.setValue(
        Constants::Settings::Search::REMOVE_REGEX,
        m_ui->lineRemoveRegex->text()
    );

    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->searchSettingsChanged();
}

/* Begin Button Box Handlers */
/* Begin Combo Box Handlers */

void SearchSettings::methodTextChanged(const QString &text)
{
    m_ui->frameHover->setVisible(
        text == Constants::Settings::Search::Method::HOVER
    );
}

/* End Combo Box Handlers */
