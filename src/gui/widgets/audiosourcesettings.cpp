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

#include "audiosourcesettings.h"
#include "ui_audiosourcesettings.h"

#include <QPushButton>
#include <QMessageBox>

AudioSourceSettings::AudioSourceSettings(QWidget *parent) : QWidget(parent), m_ui(new Ui::AudioSourceSettings)
{
    m_ui->setupUi(this);

    m_ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    /* Dialog Buttons */
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply),
        &QPushButton::clicked, this, &AudioSourceSettings::applyChanges
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::RestoreDefaults),
        &QPushButton::clicked, this, &AudioSourceSettings::restoreDefaults
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Reset),
        &QPushButton::clicked, this, &AudioSourceSettings::restoreSaved
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Help), 
        &QPushButton::clicked, this, &AudioSourceSettings::showHelp
    );
}

AudioSourceSettings::~AudioSourceSettings()
{
    delete m_ui;
}

void AudioSourceSettings::applyChanges()
{

}

void AudioSourceSettings::restoreDefaults()
{

}

void AudioSourceSettings::restoreSaved()
{

}

void AudioSourceSettings::showHelp()
{
    QMessageBox::information(this, "Audio Source Help", 
        "<b>Source Name</b>: The name of the audio source as it will appear in Memento."
        "<br><br>"
        "<b>URL</b>: The URL of the audio source. "
            "Supports inserting {expression} and {reading} markers into the URL. "
            "See Anki Integration Help for more information."
        "<br><br>"
        "<b>MD5 Skip Hash</b>: Audio that matches this MD5 hash will be ignored."
    );
}
