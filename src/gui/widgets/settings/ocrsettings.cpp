////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 Ripose
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

#include "ocrsettings.h"
#include "ui_ocrsettings.h"

#include <QFileDialog>
#include <QPushButton>
#include <QSettings>

#include "util/constants.h"
#include "util/globalmediator.h"

/* Begin Constructor/Destructor */

OCRSettings::OCRSettings(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::OCRSettings)
{
    m_ui->setupUi(this);

    /* Button Signals */
    connect(
        m_ui->buttonBrowseModel, &QPushButton::clicked,
        this, &OCRSettings::setLocalModel
    );

    /* Button Box Signals */
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::Reset),
        &QPushButton::clicked,
        this,
        &OCRSettings::restoreSaved
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::RestoreDefaults),
        &QPushButton::clicked,
        this,
        &OCRSettings::restoreDefaults
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::Apply),
        &QPushButton::clicked,
        this,
        &OCRSettings::applySettings
    );
}

OCRSettings::~OCRSettings()
{
    disconnect();
    delete m_ui;
}

/* End Constructor/Destructor */
/* Begin Event Handlers */

void OCRSettings::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    restoreSaved();
}

/* End Event Handlers */
/* Begin Button Box Handlers */

void OCRSettings::restoreSaved()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_OCR);
    m_ui->checkEnable->setChecked(
        settings.value(
            SETTINGS_OCR_ENABLE, SETTINGS_OCR_ENABLE_DEFAULT
        ).toBool()
    );
    m_ui->checkGPU->setChecked(
        settings.value(
            SETTINGS_OCR_ENABLE_GPU, SETTINGS_OCR_ENABLE_GPU_DEFAULT
        ).toBool()
    );
    m_ui->lineEditModel->setText(
        settings.value(
            SETTINGS_OCR_MODEL, SETTINGS_OCR_MODEL_DEFAULT
        ).toString()
    );
    settings.endGroup();
}

void OCRSettings::restoreDefaults()
{
    m_ui->checkEnable->setChecked(SETTINGS_OCR_ENABLE_DEFAULT);
    m_ui->checkGPU->setChecked(SETTINGS_OCR_ENABLE_GPU);
    m_ui->lineEditModel->setText(SETTINGS_OCR_MODEL_DEFAULT);
}

void OCRSettings::applySettings()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_OCR);
    settings.setValue(
        SETTINGS_OCR_ENABLE, m_ui->checkEnable->isChecked()
    );
    settings.setValue(
        SETTINGS_OCR_ENABLE_GPU, m_ui->checkGPU->isChecked()
    );
    settings.setValue(
        SETTINGS_OCR_MODEL, m_ui->lineEditModel->text()
    );
    settings.endGroup();

    Q_EMIT GlobalMediator::getGlobalMediator()->ocrSettingsChanged();
}

/* End Button Box Handlers */
/* Begin Open File Handlers */

void OCRSettings::setLocalModel()
{
    QString file = QFileDialog::getOpenFileName(
        this, "Choose a model", m_ui->lineEditModel->text()
    );
    if (file.isEmpty())
    {
        return;
    }
    m_ui->lineEditModel->setText(file);
}

/* End Open File Handlers */
