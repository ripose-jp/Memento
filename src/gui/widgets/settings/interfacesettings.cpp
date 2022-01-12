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

#include <QColorDialog>
#include <QDesktopServices>
#include <QPushButton>
#include <QSettings>

#include "../../../util/constants.h"
#include "../../../util/globalmediator.h"

/* Begin Constructor/Destructor */

InterfaceSettings::InterfaceSettings(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::InterfaceSettings)
{
    m_ui->setupUi(this);

#if !defined(Q_OS_UNIX) || defined(Q_OS_DARWIN)
    m_ui->checkSystemIcons->hide();
#endif

    /* Signals */
    connect(m_ui->buttonSubColor, &QToolButton::clicked, this,
        [=] { askButtonColor(m_ui->buttonSubColor, m_subColor); }
    );
    connect(m_ui->buttonSubBackground, &QToolButton::clicked, this,
        [=] { askButtonColor(m_ui->buttonSubBackground, m_bgColor); }
    );
    connect(m_ui->buttonSubStroke, &QToolButton::clicked, this,
        [=] { askButtonColor(m_ui->buttonSubStroke, m_strokeColor); }
    );

    connect(m_ui->checkStyleSheets, &QCheckBox::stateChanged, this,
        [=] (const int state) {
            m_ui->frameStyleSheets->setEnabled(
                state == Qt::CheckState::Checked
            );
        }
    );

    connect(
        m_ui->buttonBox->button(
            QDialogButtonBox::StandardButton::RestoreDefaults
        ),
        &QPushButton::clicked,
        this,
        &InterfaceSettings::restoreDefaults);
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Reset),
        &QPushButton::clicked,
        this,
        &InterfaceSettings::restoreSaved
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply),
        &QPushButton::clicked,
        this,
        &InterfaceSettings::applyChanges
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Help),
        &QPushButton::clicked,
        this,
        &InterfaceSettings::showHelp
    );
}

InterfaceSettings::~InterfaceSettings()
{
    disconnect();
    delete m_ui;
}

/* End Constructor/Destructor */
/* Begin Event Handlers */

void InterfaceSettings::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    restoreSaved();
}

/* End Event Handlers */
/* Begin Button Event Handlers */

void InterfaceSettings::restoreDefaults()
{
    m_ui->comboTheme->setCurrentIndex((int)SETTINGS_INTERFACE_THEME_DEFAULT);
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    m_ui->checkSystemIcons->setChecked(SETTINGS_INTERFACE_SYSTEM_ICONS_DEFAULT);
#endif

    /* Subtitle */
    m_ui->fontComboSub->setCurrentFont(
        QFont(SETTINGS_INTERFACE_SUB_FONT_DEFAULT)
    );
    m_ui->checkSubBold->setChecked(SETTINGS_INTERFACE_SUB_FONT_BOLD_DEFAULT);
    m_ui->checkSubItalic->setChecked(
        SETTINGS_INTERFACE_SUB_FONT_ITALICS_DEFAULT
    );

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
    m_ui->checkSubListWindow->setChecked(
        SETTINGS_INTERFACE_SUB_LIST_WINDOW_DEFAULT
    );
    m_ui->checkSubListTimestamps->setChecked(
        SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS_DEFAULT
    );

    /* Aux Search */
    m_ui->checkAuxSearchWindow->setChecked(
        SETTINGS_INTERFACE_AUX_SEARCH_WINDOW_DEFAULT
    );

    /* Style Sheets */
    m_ui->checkStyleSheets->setChecked(SETTINGS_INTERFACE_STYLESHEETS_DEFAULT);
    m_ui->editSubList->setPlainText(
        SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT
    );
    m_ui->editSplitter->setPlainText(
        SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE_DEFAULT
    );
    m_ui->editDefinitions->setPlainText(
        SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT
    );
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
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    m_ui->checkSystemIcons->setChecked(
        settings.value(
            SETTINGS_INTERFACE_SYSTEM_ICONS,
            SETTINGS_INTERFACE_SYSTEM_ICONS_DEFAULT
        ).toBool()
    );
#endif

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
    m_ui->checkSubListWindow->setChecked(
        settings.value(
            SETTINGS_INTERFACE_SUB_LIST_WINDOW,
            SETTINGS_INTERFACE_SUB_LIST_WINDOW_DEFAULT
        ).toBool()
    );
    m_ui->checkSubListTimestamps->setChecked(
        settings.value(
            SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS,
            SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS_DEFAULT
        ).toBool()
    );

    /* Aux Search */
    m_ui->checkAuxSearchWindow->setChecked(
        settings.value(
            SETTINGS_INTERFACE_AUX_SEARCH_WINDOW,
            SETTINGS_INTERFACE_AUX_SEARCH_WINDOW_DEFAULT
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

    settings.setValue(
        SETTINGS_INTERFACE_THEME, m_ui->comboTheme->currentIndex()
    );
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    settings.setValue(
        SETTINGS_INTERFACE_SYSTEM_ICONS, m_ui->checkSystemIcons->isChecked()
    );
#endif

    /* Subtitle */
    settings.setValue(
        SETTINGS_INTERFACE_SUB_FONT, m_ui->fontComboSub->currentFont().family()
    );
    settings.setValue(
        SETTINGS_INTERFACE_SUB_FONT_BOLD, m_ui->checkSubBold->isChecked()
    );
    settings.setValue(
        SETTINGS_INTERFACE_SUB_FONT_ITALICS, m_ui->checkSubItalic->isChecked()
    );

    settings.setValue(
        SETTINGS_INTERFACE_SUB_SCALE, m_ui->spinSubScale->value()
    );
    settings.setValue(
        SETTINGS_INTERFACE_SUB_OFFSET, m_ui->spinSubOffset->value()
    );
    settings.setValue(
        SETTINGS_INTERFACE_SUB_STROKE, m_ui->spinSubStroke->value()
    );

    settings.setValue(
        SETTINGS_INTERFACE_SUB_TEXT_COLOR, m_subColor.name(QColor::HexArgb)
    );
    settings.setValue(
        SETTINGS_INTERFACE_SUB_BG_COLOR, m_bgColor.name(QColor::HexArgb)
    );
    settings.setValue(
        SETTINGS_INTERFACE_SUB_STROKE_COLOR, m_strokeColor.name(QColor::HexArgb)
    );

    /* Subtitle List */
    settings.setValue(
        SETTINGS_INTERFACE_SUB_LIST_WINDOW,
        m_ui->checkSubListWindow->isChecked()
    );
    settings.setValue(
        SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS,
        m_ui->checkSubListTimestamps->isChecked()
    );

    /* Aux Search */
    settings.setValue(
        SETTINGS_INTERFACE_AUX_SEARCH_WINDOW,
        m_ui->checkAuxSearchWindow->isChecked()
    );

    /* Style Sheets */
    settings.setValue(
        SETTINGS_INTERFACE_STYLESHEETS, m_ui->checkStyleSheets->isChecked()
    );
    settings.setValue(
        SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE, m_ui->editSubList->toPlainText()
    );
    settings.setValue(
        SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE,
        m_ui->editSplitter->toPlainText()
    );
    settings.setValue(
        SETTINGS_INTERFACE_DEFINITION_STYLE,
        m_ui->editDefinitions->toPlainText()
    );

    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->interfaceSettingsChanged();
}

void InterfaceSettings::showHelp() const
{
    QDesktopServices::openUrl(
        QUrl("https://doc.qt.io/qt-5/stylesheet-reference.html")
    );
}

/* End Button Event Handlers */
/* Begin Helpers */

void InterfaceSettings::askButtonColor(QToolButton *button, QColor &color)
{
    QColor tempColor = QColorDialog::getColor(
        color, nullptr, QString(), QColorDialog::ShowAlphaChannel
    );
    if (tempColor.isValid())
    {
        color = tempColor;
        setButtonColor(button, color);
    }
}

void InterfaceSettings::setButtonColor(QToolButton *button, const QColor &color)
{
    int height = QFontMetrics(button->font()).height();
    QPixmap pixmap(height, height);
    pixmap.fill(color);
    button->setIcon(pixmap);
}

/* End Helpers */
