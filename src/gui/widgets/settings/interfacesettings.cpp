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

#include "util/constants.h"
#include "util/globalmediator.h"

/* Begin Constructor/Destructor */

InterfaceSettings::InterfaceSettings(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::InterfaceSettings)
{
    m_ui->setupUi(this);

#if !defined(Q_OS_UNIX) || defined(Q_OS_DARWIN)
    m_ui->checkSystemIcons->hide();
    m_ui->widgetDpiScaling->hide();
#endif

#if !defined(Q_OS_WIN)
    m_ui->labelMenuBar->hide();
    m_ui->checkMenuFullscreen->hide();
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

    connect(
        m_ui->checkStyleSheets,
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
        &QCheckBox::stateChanged,
#else
        &QCheckBox::checkStateChanged,
#endif
        this,
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
        [=] (int state)
#else
        [=] (Qt::CheckState state)
#endif
        {
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
    m_ui->comboTheme->setCurrentIndex(
        static_cast<int>(Constants::Settings::Interface::THEME_DEFAULT)
    );
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    m_ui->checkSystemIcons->setChecked(
        Constants::Settings::Interface::SYSTEM_ICONS_DEFAULT
    );
#endif

    /* Subtitle */
    m_ui->fontComboSub->setCurrentFont(
        QFont(Constants::Settings::Interface::Subtitle::FONT_DEFAULT)
    );
    m_ui->checkSubBold->setChecked(
        Constants::Settings::Interface::Subtitle::FONT_BOLD_DEFAULT
    );
    m_ui->checkSubItalic->setChecked(
        Constants::Settings::Interface::Subtitle::FONT_ITALICS_DEFAULT
    );

    m_ui->spinSubScale->setValue(
        Constants::Settings::Interface::Subtitle::SCALE_DEFAULT
    );
    m_ui->spinSubOffset->setValue(
        Constants::Settings::Interface::Subtitle::OFFSET_DEFAULT
    );
    m_ui->spinSubStroke->setValue(
        Constants::Settings::Interface::Subtitle::STROKE_DEFAULT
    );

    m_subColor = QColor(
        Constants::Settings::Interface::Subtitle::TEXT_COLOR_DEFAULT
    );
    m_bgColor = QColor(
        Constants::Settings::Interface::Subtitle::BACKGROUND_COLOR_DEFAULT
    );
    m_strokeColor = QColor(
        Constants::Settings::Interface::Subtitle::STROKE_COLOR_DEFAULT
    );

    setButtonColor(m_ui->buttonSubColor, m_subColor);
    setButtonColor(m_ui->buttonSubBackground, m_bgColor);
    setButtonColor(m_ui->buttonSubStroke, m_strokeColor);

    /* Sub List */
    m_ui->checkSubListWindow->setChecked(
        Constants::Settings::Interface::Subtitle::LIST_WINDOW_DEFAULT
    );
    m_ui->checkSubListTimestamps->setChecked(
        Constants::Settings::Interface::Subtitle::LIST_TIMESTAMPS_DEFAULT
    );

    /* Aux Search */
    m_ui->checkAuxSearchWindow->setChecked(
        Constants::Settings::Interface::Subtitle::SEARCH_WINDOW_DEFAULT
    );

    /* Menu Bar */
#if defined(Q_OS_WIN)
    m_ui->checkMenuFullscreen->setChecked(
        Constants::Settings::Interface::Subtitle::MENUBAR_FULLSCREEN_DEFAULT
    );
#endif

    /* DPI Scaling */
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    m_ui->checkDpiScaleOverride->setChecked(
        Constants::Settings::Interface::DPI_SCALE_OVERRIDE_DEFAULT
    );
    m_ui->spinDpiScaleFactor->setValue(
        Constants::Settings::Interface::DPI_SCALE_FACTOR_DEFAULT
    );
#endif

    /* Style Sheets */
    m_ui->checkStyleSheets->setChecked(
        Constants::Settings::Interface::STYLESHEETS_DEFAULT
    );
    m_ui->editSubList->setPlainText(
        Constants::Settings::Interface::Style::SUBTITLE_LIST_DEFAULT
    );
    m_ui->editSplitter->setPlainText(
        Constants::Settings::Interface::Style::SPLITTER_DEFAULT
    );
    m_ui->editDefinitions->setPlainText(
        Constants::Settings::Interface::Style::DEFINITION_DEFAULT
    );
}

void InterfaceSettings::restoreSaved()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);
    m_ui->comboTheme->setCurrentIndex(
        settings.value(
            Constants::Settings::Interface::THEME,
            (int)Constants::Settings::Interface::THEME_DEFAULT
        ).toInt()
    );
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    m_ui->checkSystemIcons->setChecked(
        settings.value(
            Constants::Settings::Interface::SYSTEM_ICONS,
            Constants::Settings::Interface::SYSTEM_ICONS_DEFAULT
        ).toBool()
    );
#endif

    /* Subtitle */
    m_ui->fontComboSub->setCurrentFont(QFont(settings.value(
            Constants::Settings::Interface::Subtitle::FONT,
            Constants::Settings::Interface::Subtitle::FONT_DEFAULT
        ).toString())
    );
    m_ui->checkSubBold->setChecked(
        settings.value(
            Constants::Settings::Interface::Subtitle::FONT_BOLD,
            Constants::Settings::Interface::Subtitle::FONT_BOLD_DEFAULT
        ).toBool()
    );
    m_ui->checkSubItalic->setChecked(
        settings.value(
            Constants::Settings::Interface::Subtitle::FONT_ITALICS,
            Constants::Settings::Interface::Subtitle::FONT_ITALICS_DEFAULT
        ).toBool()
    );

    m_ui->spinSubScale->setValue(settings.value(
            Constants::Settings::Interface::Subtitle::SCALE,
            Constants::Settings::Interface::Subtitle::SCALE_DEFAULT
        ).toDouble()
    );
    m_ui->spinSubOffset->setValue(settings.value(
            Constants::Settings::Interface::Subtitle::OFFSET,
            Constants::Settings::Interface::Subtitle::OFFSET_DEFAULT
        ).toDouble()
    );
    m_ui->spinSubStroke->setValue(settings.value(
            Constants::Settings::Interface::Subtitle::STROKE,
            Constants::Settings::Interface::Subtitle::STROKE_DEFAULT
        ).toDouble()
    );

    m_subColor = QColor(settings.value(
            Constants::Settings::Interface::Subtitle::TEXT_COLOR,
            Constants::Settings::Interface::Subtitle::TEXT_COLOR_DEFAULT
        ).toString()
    );
    setButtonColor(m_ui->buttonSubColor, m_subColor);
    m_bgColor = QColor(settings.value(
            Constants::Settings::Interface::Subtitle::BACKGROUND_COLOR,
            Constants::Settings::Interface::Subtitle::BACKGROUND_COLOR_DEFAULT
        ).toString()
    );
    setButtonColor(m_ui->buttonSubBackground, m_bgColor);
    m_strokeColor = QColor(settings.value(
            Constants::Settings::Interface::Subtitle::STROKE_COLOR,
            Constants::Settings::Interface::Subtitle::STROKE_COLOR_DEFAULT
        ).toString()
    );
    setButtonColor(m_ui->buttonSubStroke, m_strokeColor);

    /* Subtitle List */
    m_ui->checkSubListWindow->setChecked(
        settings.value(
            Constants::Settings::Interface::Subtitle::LIST_WINDOW,
            Constants::Settings::Interface::Subtitle::LIST_WINDOW_DEFAULT
        ).toBool()
    );
    m_ui->checkSubListTimestamps->setChecked(
        settings.value(
            Constants::Settings::Interface::Subtitle::LIST_TIMESTAMPS,
            Constants::Settings::Interface::Subtitle::LIST_TIMESTAMPS_DEFAULT
        ).toBool()
    );

    /* Aux Search */
    m_ui->checkAuxSearchWindow->setChecked(
        settings.value(
            Constants::Settings::Interface::Subtitle::SEARCH_WINDOW,
            Constants::Settings::Interface::Subtitle::SEARCH_WINDOW_DEFAULT
        ).toBool()
    );

    /* Menu Bar */
#if defined(Q_OS_WIN)
    m_ui->checkMenuFullscreen->setChecked(
        settings.value(
            Constants::Settings::Interface::Subtitle::MENUBAR_FULLSCREEN,
            Constants::Settings::Interface::Subtitle::MENUBAR_FULLSCREEN_DEFAULT
        ).toBool()
    );
#endif

    /* DPI Scaling */
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    m_ui->checkDpiScaleOverride->setChecked(
        settings.value(
            Constants::Settings::Interface::DPI_SCALE_OVERRIDE,
            Constants::Settings::Interface::DPI_SCALE_OVERRIDE_DEFAULT
        ).toBool()
    );
    m_ui->spinDpiScaleFactor->setValue(
        settings.value(
            Constants::Settings::Interface::DPI_SCALE_FACTOR,
            Constants::Settings::Interface::DPI_SCALE_FACTOR_DEFAULT
        ).toDouble()
    );
#endif

    /* Style Sheets */
    m_ui->checkStyleSheets->setChecked(
        settings.value(
            Constants::Settings::Interface::STYLESHEETS,
            Constants::Settings::Interface::STYLESHEETS_DEFAULT
        ).toBool()
    );
    m_ui->editSubList->setPlainText(
        settings.value(
            Constants::Settings::Interface::Style::SUBTITLE_LIST,
            Constants::Settings::Interface::Style::SUBTITLE_LIST_DEFAULT
        ).toString()
    );
    m_ui->editSplitter->setPlainText(
        settings.value(
            Constants::Settings::Interface::Style::SPLITTER,
            Constants::Settings::Interface::Style::SPLITTER_DEFAULT
        ).toString()
    );
    m_ui->editDefinitions->setPlainText(
        settings.value(
            Constants::Settings::Interface::Style::DEFINITION,
            Constants::Settings::Interface::Style::DEFINITION_DEFAULT
        ).toString()
    );
    settings.endGroup();
}

void InterfaceSettings::applyChanges()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);

    settings.setValue(
        Constants::Settings::Interface::THEME,
        m_ui->comboTheme->currentIndex()
    );
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    settings.setValue(
        Constants::Settings::Interface::SYSTEM_ICONS,
        m_ui->checkSystemIcons->isChecked()
    );
#endif

    /* Subtitle */
    settings.setValue(
        Constants::Settings::Interface::Subtitle::FONT,
        m_ui->fontComboSub->currentFont().family()
    );
    settings.setValue(
        Constants::Settings::Interface::Subtitle::FONT_BOLD,
        m_ui->checkSubBold->isChecked()
    );
    settings.setValue(
        Constants::Settings::Interface::Subtitle::FONT_ITALICS,
        m_ui->checkSubItalic->isChecked()
    );

    settings.setValue(
        Constants::Settings::Interface::Subtitle::SCALE,
        m_ui->spinSubScale->value()
    );
    settings.setValue(
        Constants::Settings::Interface::Subtitle::OFFSET,
        m_ui->spinSubOffset->value()
    );
    settings.setValue(
        Constants::Settings::Interface::Subtitle::STROKE,
        m_ui->spinSubStroke->value()
    );

    settings.setValue(
        Constants::Settings::Interface::Subtitle::TEXT_COLOR,
        m_subColor.name(QColor::HexArgb)
    );
    settings.setValue(
        Constants::Settings::Interface::Subtitle::BACKGROUND_COLOR,
        m_bgColor.name(QColor::HexArgb)
    );
    settings.setValue(
        Constants::Settings::Interface::Subtitle::STROKE_COLOR,
        m_strokeColor.name(QColor::HexArgb)
    );

    /* Subtitle List */
    settings.setValue(
        Constants::Settings::Interface::Subtitle::LIST_WINDOW,
        m_ui->checkSubListWindow->isChecked()
    );
    settings.setValue(
        Constants::Settings::Interface::Subtitle::LIST_TIMESTAMPS,
        m_ui->checkSubListTimestamps->isChecked()
    );

    /* Aux Search */
    settings.setValue(
        Constants::Settings::Interface::Subtitle::SEARCH_WINDOW,
        m_ui->checkAuxSearchWindow->isChecked()
    );

    /* Menu Bar */
#if defined(Q_OS_WIN)
    settings.setValue(
        Constants::Settings::Interface::Subtitle::MENUBAR_FULLSCREEN,
        m_ui->checkMenuFullscreen->isChecked()
    );
#endif

    /* DPI Scaling */
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    settings.setValue(
        Constants::Settings::Interface::DPI_SCALE_OVERRIDE,
        m_ui->checkDpiScaleOverride->isChecked()
    );
    settings.setValue(
        Constants::Settings::Interface::DPI_SCALE_FACTOR,
        m_ui->spinDpiScaleFactor->value()
    );
#endif

    /* Style Sheets */
    settings.setValue(
        Constants::Settings::Interface::STYLESHEETS,
        m_ui->checkStyleSheets->isChecked()
    );
    settings.setValue(
        Constants::Settings::Interface::Style::SUBTITLE_LIST,
        m_ui->editSubList->toPlainText()
    );
    settings.setValue(
        Constants::Settings::Interface::Style::SPLITTER,
        m_ui->editSplitter->toPlainText()
    );
    settings.setValue(
        Constants::Settings::Interface::Style::DEFINITION,
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
