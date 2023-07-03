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
    settings.beginGroup(Constants::Settings::OCR::GROUP);
    m_ui->checkEnable->setChecked(
        settings.value(
            Constants::Settings::OCR::ENABLED,
            Constants::Settings::OCR::ENABLED_DEFAULT
        ).toBool()
    );
    m_ui->checkGPU->setChecked(
        settings.value(
            Constants::Settings::OCR::ENABLE_GPU,
            Constants::Settings::OCR::ENABLE_GPU_DEFAULT
        ).toBool()
    );
    m_ui->lineEditModel->setText(
        settings.value(
            Constants::Settings::OCR::MODEL,
            Constants::Settings::OCR::MODEL_DEFAULT
        ).toString()
    );
    settings.endGroup();
}

void OCRSettings::restoreDefaults()
{
    m_ui->checkEnable->setChecked(Constants::Settings::OCR::ENABLED_DEFAULT);
    m_ui->checkGPU->setChecked(Constants::Settings::OCR::ENABLE_GPU_DEFAULT);
    m_ui->lineEditModel->setText(Constants::Settings::OCR::MODEL_DEFAULT);
}

void OCRSettings::applySettings()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::OCR::GROUP);
    settings.setValue(
        Constants::Settings::OCR::ENABLED, m_ui->checkEnable->isChecked()
    );
    settings.setValue(
        Constants::Settings::OCR::ENABLE_GPU, m_ui->checkGPU->isChecked()
    );
    settings.setValue(
        Constants::Settings::OCR::MODEL, m_ui->lineEditModel->text()
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
