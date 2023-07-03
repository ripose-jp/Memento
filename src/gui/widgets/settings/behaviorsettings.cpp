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
    m_fileOpenMap = QMap<QString, Constants::FileOpenDirectory>({
        {"Current", Constants::FileOpenDirectory::Current},
        {"Home", Constants::FileOpenDirectory::Home},
        {"Movies", Constants::FileOpenDirectory::Movies},
        {"Documents", Constants::FileOpenDirectory::Documents},
        {"Custom", Constants::FileOpenDirectory::Custom}
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
    settings.beginGroup(Constants::Settings::Behavior::GROUP);

    m_ui->checkAutofit->setChecked(
        settings.value(
            Constants::Settings::Behavior::AUTOFIT,
            Constants::Settings::Behavior::AUTOFIT_DEFAULT
        ).toBool()
    );
    m_ui->spinAutofit->setValue(
        settings.value(
            Constants::Settings::Behavior::AUTOFIT_PERCENT,
            Constants::Settings::Behavior::AUTOFIT_PERCENT_DEFAULT
        ).toInt()
    );

    m_ui->checkCursorOSC->setChecked(
        settings.value(
            Constants::Settings::Behavior::OSC_CURSOR_HIDE,
            Constants::Settings::Behavior::OSC_CURSOR_HIDE_DEFAULT
        ).toBool()
    );
    m_ui->spinOSCDuration->setValue(
        settings.value(
            Constants::Settings::Behavior::OSC_DURATION,
            Constants::Settings::Behavior::OSC_DURATION_DEFAULT
        ).toInt()
    );
    m_ui->spinOSCFade->setValue(
        settings.value(
            Constants::Settings::Behavior::OSC_FADE,
            Constants::Settings::Behavior::OSC_FADE_DEFAULT
        ).toInt()
    );
    m_ui->spinOSCMinMove->setValue(
        settings.value(
            Constants::Settings::Behavior::OSC_MIN_MOVE,
            Constants::Settings::Behavior::OSC_MIN_MOVE_DEFAULT
        ).toInt()
    );

    m_ui->checkSubtitlePause->setChecked(
        settings.value(
            Constants::Settings::Behavior::SUBTITLE_PAUSE,
            Constants::Settings::Behavior::SUBTITLE_PAUSE_DEFAULT
        ).toBool()
    );
    m_ui->checkSubtitleShow->setChecked(
        settings.value(
            Constants::Settings::Behavior::SUBTITLE_CURSOR_SHOW,
            Constants::Settings::Behavior::SUBTITLE_CURSOR_SHOW_DEFAULT
        ).toBool()
    );
    m_ui->checkSubtitleSecShow->setChecked(
        settings.value(
            Constants::Settings::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW,
            Constants::Settings::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW_DEFAULT
        ).toBool()
    );

    m_ui->comboFileOpenDir->setCurrentText(
        m_fileOpenMap.key(
            static_cast<Constants::FileOpenDirectory>(settings.value(
                Constants::Settings::Behavior::FILE_OPEN_DIR,
                static_cast<int>(
                    Constants::Settings::Behavior::FILE_OPEN_DIR_DEFAULT
                )
            ).toInt())
        )
    );
    m_ui->lineEditFileOpenCustom->setText(
        settings.value(
            Constants::Settings::Behavior::FILE_OPEN_CUSTOM,
            Constants::Settings::Behavior::FILE_OPEN_CUSTOM_DEFAULT
        ).toString()
    );

    settings.endGroup();
}

void BehaviorSettings::restoreDefaults()
{
    m_ui->checkAutofit->setChecked(
        Constants::Settings::Behavior::AUTOFIT_DEFAULT
    );
    m_ui->spinAutofit->setValue(
        Constants::Settings::Behavior::AUTOFIT_PERCENT_DEFAULT
    );

    m_ui->checkCursorOSC->setChecked(
        Constants::Settings::Behavior::OSC_CURSOR_HIDE_DEFAULT
    );
    m_ui->spinOSCDuration->setValue(
        Constants::Settings::Behavior::OSC_DURATION_DEFAULT
    );
    m_ui->spinOSCFade->setValue(
        Constants::Settings::Behavior::OSC_FADE_DEFAULT
    );
    m_ui->spinOSCMinMove->setValue(
        Constants::Settings::Behavior::OSC_MIN_MOVE_DEFAULT
    );

    m_ui->checkSubtitlePause->setChecked(
        Constants::Settings::Behavior::SUBTITLE_PAUSE_DEFAULT
    );
    m_ui->checkSubtitleShow->setChecked(
        Constants::Settings::Behavior::SUBTITLE_CURSOR_SHOW_DEFAULT
    );
    m_ui->checkSubtitleSecShow->setChecked(
        Constants::Settings::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW_DEFAULT
    );

    m_ui->comboFileOpenDir->setCurrentText(
        m_fileOpenMap.key(Constants::Settings::Behavior::FILE_OPEN_DIR_DEFAULT)
    );
    m_ui->lineEditFileOpenCustom->setText(
        Constants::Settings::Behavior::FILE_OPEN_CUSTOM_DEFAULT
    );
}

void BehaviorSettings::applySettings()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Behavior::GROUP);

    settings.setValue(
        Constants::Settings::Behavior::AUTOFIT,
        m_ui->checkAutofit->isChecked()
    );
    settings.setValue(
        Constants::Settings::Behavior::AUTOFIT_PERCENT,
        m_ui->spinAutofit->value()
    );

    settings.setValue(
        Constants::Settings::Behavior::OSC_CURSOR_HIDE,
        m_ui->checkCursorOSC->isChecked()
    );
    settings.setValue(
        Constants::Settings::Behavior::OSC_DURATION,
        m_ui->spinOSCDuration->value()
    );
    settings.setValue(
        Constants::Settings::Behavior::OSC_FADE,
        m_ui->spinOSCFade->value()
    );
    settings.setValue(
        Constants::Settings::Behavior::OSC_MIN_MOVE,
        m_ui->spinOSCMinMove->value()
    );

    settings.setValue(
        Constants::Settings::Behavior::SUBTITLE_PAUSE,
        m_ui->checkSubtitlePause->isChecked()
    );
    settings.setValue(
        Constants::Settings::Behavior::SUBTITLE_CURSOR_SHOW,
        m_ui->checkSubtitleShow->isChecked()
    );
    settings.setValue(
        Constants::Settings::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW,
        m_ui->checkSubtitleSecShow->isChecked()
    );

    settings.setValue(
        Constants::Settings::Behavior::FILE_OPEN_DIR,
        (int)m_fileOpenMap[m_ui->comboFileOpenDir->currentText()]
    );
    if (m_ui->comboFileOpenDir->isVisibleTo(this))
    {
        settings.setValue(
            Constants::Settings::Behavior::FILE_OPEN_CUSTOM,
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
    bool vis = m_fileOpenMap[text] == Constants::FileOpenDirectory::Custom;
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
