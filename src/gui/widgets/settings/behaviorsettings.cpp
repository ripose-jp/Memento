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

#include "behaviorsettings.h"
#include "ui_behaviorsettings.h"

#include <QFileDialog>
#include <QPushButton>
#include <QSettings>

#include "util/constants.h"
#include "util/globalmediator.h"

/* Begin Constructor/Destructor */

BehaviorSettings::BehaviorSettings(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::BehaviorSettings)
{
    m_ui->setupUi(this);

    /* Add the items to the File Open combo box */
    m_fileOpenMap = QMap<QString, FileOpenDirectory>({
        {"Current", FileOpenDirectory::Current},
        {"Home", FileOpenDirectory::Home},
        {"Movies", FileOpenDirectory::Movies},
        {"Documents", FileOpenDirectory::Documents},
        {"Custom", FileOpenDirectory::Custom}
    });
    m_ui->comboFileOpenDir->addItems(QStringList(m_fileOpenMap.keys()));

    /* File Open Signals */
    connect(
        m_ui->comboFileOpenDir, &QComboBox::currentTextChanged,
        this, &BehaviorSettings::handleFileOpenChange
    );
    connect(
        m_ui->buttonFileOpenCustom, &QPushButton::clicked,
        this, &BehaviorSettings::setCustomDirectory
    );

    /* Button Box Signals */
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::Reset),
        &QPushButton::clicked,
        this,
        &BehaviorSettings::restoreSaved
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults),
        &QPushButton::clicked,
        this,
        &BehaviorSettings::restoreDefaults
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::Apply),
        &QPushButton::clicked,
        this,
        &BehaviorSettings::applySettings
    );
}

BehaviorSettings::~BehaviorSettings()
{
    disconnect();
    delete m_ui;
}

/* End Constructor/Destructor */
/* Begin Event Handlers */

void BehaviorSettings::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    restoreSaved();
    handleFileOpenChange(m_ui->comboFileOpenDir->currentText());
}

/* End Event Handlers */
/* Begin Button Box Handlers */

void BehaviorSettings::restoreSaved()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_BEHAVIOR);
    m_ui->checkAutofit->setChecked(
        settings.value(
            SETTINGS_BEHAVIOR_AUTOFIT, SETTINGS_BEHAVIOR_AUTOFIT_DEFAULT
        ).toBool()
    );
    m_ui->spinAutofit->setValue(
        settings.value(
            SETTINGS_BEHAVIOR_AUTOFIT_PERCENT,
            SETTINGS_BEHAVIOR_AUTOFIT_PERCENT_DEFAULT
        ).toInt()
    );
    m_ui->checkCursorOSC->setChecked(
        settings.value(
            SETTINGS_BEHAVIOR_CURSOR_HIDE_OSC,
            SETTINGS_BEHAVIOR_CURSOR_HIDE_OSC_DEFAULT
        ).toBool()
    );
    m_ui->checkSubtitlePause->setChecked(
        settings.value(
            SETTINGS_BEHAVIOR_SUBTITLE_PAUSE,
            SETTINGS_BEHAVIOR_SUBTITLE_PAUSE_DEFAULT
        ).toBool()
    );
    m_ui->spinOSCDuration->setValue(
        settings.value(
            SETTINGS_BEHAVIOR_OSC_DURATION,
            SETTINGS_BEHAVIOR_OSC_DURATION_DEFAULT
        ).toInt()
    );
    m_ui->spinOSCFade->setValue(
        settings.value(
            SETTINGS_BEHAVIOR_OSC_FADE,
            SETTINGS_BEHAVIOR_OSC_FADE_DEFAULT
        ).toInt()
    );
    m_ui->spinOSCMinMove->setValue(
        settings.value(
            SETTINGS_BEHAVIOR_OSC_MIN_MOVE,
            SETTINGS_BEHAVIOR_OSC_MIN_MOVE_DEFAULT
        ).toInt()
    );
    m_ui->comboFileOpenDir->setCurrentText(
        m_fileOpenMap.key(
            (FileOpenDirectory)settings.value(
                SETTINGS_BEHAVIOR_FILE_OPEN_DIR,
                (int)SETTINGS_BEHAVIOR_FILE_OPEN_DIR_DEFAULT
            ).toInt()
        )
    );
    m_ui->lineEditFileOpenCustom->setText(
        settings.value(
            SETTINGS_BEHAVIOR_FILE_OPEN_CUSTOM,
            SETTINGS_BEHAVIOR_FILE_OPEN_CUSTOM_DEFAULT
        ).toString()
    );
    settings.endGroup();
}

void BehaviorSettings::restoreDefaults()
{
    m_ui->checkAutofit->setChecked(SETTINGS_BEHAVIOR_AUTOFIT_DEFAULT);
    m_ui->spinAutofit->setValue(SETTINGS_BEHAVIOR_AUTOFIT_PERCENT_DEFAULT);
    m_ui->checkCursorOSC->setChecked(SETTINGS_BEHAVIOR_CURSOR_HIDE_OSC_DEFAULT);
    m_ui->checkSubtitlePause->setChecked(
        SETTINGS_BEHAVIOR_SUBTITLE_PAUSE_DEFAULT
    );
    m_ui->spinOSCDuration->setValue(SETTINGS_BEHAVIOR_OSC_DURATION_DEFAULT);
    m_ui->spinOSCFade->setValue(SETTINGS_BEHAVIOR_OSC_FADE_DEFAULT);
    m_ui->spinOSCMinMove->setValue(SETTINGS_BEHAVIOR_OSC_MIN_MOVE_DEFAULT);
    m_ui->comboFileOpenDir->setCurrentText(
        m_fileOpenMap.key(SETTINGS_BEHAVIOR_FILE_OPEN_DIR_DEFAULT)
    );
    m_ui->lineEditFileOpenCustom->setText(
        SETTINGS_BEHAVIOR_FILE_OPEN_CUSTOM_DEFAULT
    );
}

void BehaviorSettings::applySettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_BEHAVIOR);
    settings.setValue(
        SETTINGS_BEHAVIOR_AUTOFIT, m_ui->checkAutofit->isChecked()
    );
    settings.setValue(
        SETTINGS_BEHAVIOR_AUTOFIT_PERCENT, m_ui->spinAutofit->value()
    );
    settings.setValue(
        SETTINGS_BEHAVIOR_CURSOR_HIDE_OSC, m_ui->checkCursorOSC->isChecked()
    );
    settings.setValue(
        SETTINGS_BEHAVIOR_SUBTITLE_PAUSE, m_ui->checkSubtitlePause->isChecked()
    );
    settings.setValue(
        SETTINGS_BEHAVIOR_OSC_DURATION, m_ui->spinOSCDuration->value()
    );
    settings.setValue(
        SETTINGS_BEHAVIOR_OSC_FADE, m_ui->spinOSCFade->value()
    );
    settings.setValue(
        SETTINGS_BEHAVIOR_OSC_MIN_MOVE, m_ui->spinOSCMinMove->value()
    );
    settings.setValue(
        SETTINGS_BEHAVIOR_FILE_OPEN_DIR,
        (int)m_fileOpenMap[m_ui->comboFileOpenDir->currentText()]
    );
    if (m_ui->comboFileOpenDir->isVisibleTo(this))
    {
        settings.setValue(
            SETTINGS_BEHAVIOR_FILE_OPEN_CUSTOM,
            m_ui->lineEditFileOpenCustom->text()
        );
    }
    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->behaviorSettingsChanged();
}

/* End Button Box Handlers */
/* Begin Open File Handlers */

void BehaviorSettings::handleFileOpenChange(const QString &text)
{
    bool vis = m_fileOpenMap[text] == FileOpenDirectory::Custom;
    m_ui->lineEditFileOpenCustom->setVisible(vis);
    m_ui->buttonFileOpenCustom->setVisible(vis);
}

void BehaviorSettings::setCustomDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, "Choose a directory", m_ui->lineEditFileOpenCustom->text()
    );
    if (dir.isEmpty())
    {
        return;
    }
    m_ui->lineEditFileOpenCustom->setText(dir);
}

/* End Open File Handlers */
