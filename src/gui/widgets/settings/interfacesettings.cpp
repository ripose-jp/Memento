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

#include "../../../util/constants.h"
#include "../../../util/globalmediator.h"

#include <QPushButton>
#include <QSettings>
#include <QDesktopServices>
#include <QColorDialog>

InterfaceSettings::InterfaceSettings(QWidget *parent) : QWidget(parent), m_ui(new Ui::InterfaceSettings)
{
    m_ui->setupUi(this);

    connect(m_ui->buttonSubColor, &QToolButton::clicked, this,
        [=] {
            m_subColor = QColorDialog::getColor(m_subColor, nullptr, QString(), QColorDialog::ShowAlphaChannel);
            setButtonColor(m_ui->buttonSubColor, m_subColor);
        }
    );
    connect(m_ui->buttonSubBackground, &QToolButton::clicked, this,
        [=] {
            m_bgColor = QColorDialog::getColor(m_bgColor, nullptr, QString(), QColorDialog::ShowAlphaChannel);
            setButtonColor(m_ui->buttonSubBackground, m_bgColor);
        }
    );
    connect(m_ui->buttonSubStroke, &QToolButton::clicked, this,
        [=] {
            m_strokeColor = QColorDialog::getColor(m_strokeColor, nullptr, QString(), QColorDialog::ShowAlphaChannel);
            setButtonColor(m_ui->buttonSubStroke, m_strokeColor);
        }
    );

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
    disconnect();
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
    m_ui->fontComboSub->setCurrentFont(QFont(SETTINGS_INTERFACE_SUB_FONT_DEFAULT));
    m_ui->checkSubBold->setChecked(SETTINGS_INTERFACE_SUB_FONT_BOLD_DEFAULT);
    m_ui->checkSubItalic->setChecked(SETTINGS_INTERFACE_SUB_FONT_ITALICS_DEFAULT);

    m_ui->spinSubScale->setValue(SETTINGS_INTERFACE_SUB_SCALE_DEFAULT);
    m_ui->spinSubOffset->setValue(SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT);
    m_ui->spinSubStroke->setValue(SETTINGS_INTERFACE_SUB_STROKE_DEFAULT);

    m_subColor.setNamedColor(SETTINGS_INTERFACE_SUB_TEXT_COLOR_DEFAULT);
    m_bgColor.setNamedColor(SETTINGS_INTERFACE_SUB_BG_COLOR_DEFAULT);
    m_strokeColor.setNamedColor(SETTINGS_INTERFACE_SUB_STROKE_COLOR_DEFAULT);

    setButtonColor(m_ui->buttonSubColor, m_subColor);
    setButtonColor(m_ui->buttonSubBackground, m_bgColor);
    setButtonColor(m_ui->buttonSubStroke, m_strokeColor);

    /* Sub List */
    m_ui->checkSubListTimestamps->setChecked(SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS_DEFAULT);

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
    m_ui->fontComboSub->setCurrentFont(QFont(settings.value(
            SETTINGS_INTERFACE_SUB_FONT,
            SETTINGS_INTERFACE_SUB_FONT_DEFAULT
        ).toString())
    );
    m_ui->checkSubBold->setChecked(
        settings.value(
            SETTINGS_INTERFACE_SUB_FONT_BOLD,
            SETTINGS_INTERFACE_SUB_FONT_BOLD_DEFAULT
        ).toBool()
    );
    m_ui->checkSubItalic->setChecked(
        settings.value(
            SETTINGS_INTERFACE_SUB_FONT_ITALICS,
            SETTINGS_INTERFACE_SUB_FONT_ITALICS_DEFAULT
        ).toBool()
    );

    m_ui->spinSubScale->setValue(settings.value(
            SETTINGS_INTERFACE_SUB_SCALE,
            SETTINGS_INTERFACE_SUB_SCALE_DEFAULT
        ).toDouble()
    );
    m_ui->spinSubOffset->setValue(settings.value(
            SETTINGS_INTERFACE_SUB_OFFSET,
            SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT
        ).toDouble()
    );
    m_ui->spinSubStroke->setValue(settings.value(
            SETTINGS_INTERFACE_SUB_STROKE,
            SETTINGS_INTERFACE_SUB_STROKE_DEFAULT
        ).toDouble()
    );

    m_subColor.setNamedColor(settings.value(
            SETTINGS_INTERFACE_SUB_TEXT_COLOR,
            SETTINGS_INTERFACE_SUB_TEXT_COLOR_DEFAULT
        ).toString()
    );
    setButtonColor(m_ui->buttonSubColor, m_subColor);
    m_bgColor.setNamedColor(settings.value(
            SETTINGS_INTERFACE_SUB_BG_COLOR,
            SETTINGS_INTERFACE_SUB_BG_COLOR_DEFAULT
        ).toString()
    );
    setButtonColor(m_ui->buttonSubBackground, m_bgColor);
    m_strokeColor.setNamedColor(settings.value(
            SETTINGS_INTERFACE_SUB_STROKE_COLOR,
            SETTINGS_INTERFACE_SUB_STROKE_COLOR_DEFAULT
        ).toString()
    );
    setButtonColor(m_ui->buttonSubStroke, m_strokeColor);

    /* Subtitle List */
    m_ui->checkSubListTimestamps->setChecked(
        settings.value(
            SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS,
            SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS_DEFAULT
        ).toBool()
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
    settings.setValue(SETTINGS_INTERFACE_SUB_FONT,         m_ui->fontComboSub->currentFont().family());
    settings.setValue(SETTINGS_INTERFACE_SUB_FONT_BOLD,    m_ui->checkSubBold->isChecked());
    settings.setValue(SETTINGS_INTERFACE_SUB_FONT_ITALICS, m_ui->checkSubItalic->isChecked());

    settings.setValue(SETTINGS_INTERFACE_SUB_SCALE,  m_ui->spinSubScale->value());
    settings.setValue(SETTINGS_INTERFACE_SUB_OFFSET, m_ui->spinSubOffset->value());
    settings.setValue(SETTINGS_INTERFACE_SUB_STROKE, m_ui->spinSubStroke->value());

    settings.setValue(SETTINGS_INTERFACE_SUB_TEXT_COLOR,   m_subColor.name(QColor::HexArgb));
    settings.setValue(SETTINGS_INTERFACE_SUB_BG_COLOR,     m_bgColor.name(QColor::HexArgb));
    settings.setValue(SETTINGS_INTERFACE_SUB_STROKE_COLOR, m_strokeColor.name(QColor::HexArgb));

    /* Subtitle List */
    settings.setValue(SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS, m_ui->checkSubListTimestamps->isChecked());

    /* Style Sheets */
    settings.setValue(SETTINGS_INTERFACE_STYLESHEETS,           m_ui->checkStyleSheets->isChecked());
    settings.setValue(SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE,   m_ui->editSubList->toPlainText());
    settings.setValue(SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE, m_ui->editSplitter->toPlainText());
    settings.setValue(SETTINGS_INTERFACE_DEFINITION_STYLE,      m_ui->editDefinitions->toPlainText());

    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->interfaceSettingsChanged();
}

void InterfaceSettings::setButtonColor(QToolButton *button, const QColor &color)
{
    int height = QFontMetrics(button->font()).height();
    QPixmap pixmap(height, height);
    pixmap.fill(color);
    button->setIcon(pixmap);
}