////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#include "ankisettings.h"
#include "ui_ankisettings.h"

#include <QMessageBox>
#include <QTableWidgetItem>

AnkiSettings::AnkiSettings(AnkiClient *client, QWidget *parent)
    : m_client(client), QWidget(parent), m_ui(new Ui::AnkiSettings)
{
    m_ui->setupUi(this);

    connect(m_ui->m_checkBoxEnabled, &QCheckBox::stateChanged,
        this, &AnkiSettings::enabledStateChanged);
    connect(m_ui->m_buttonConnect, &QPushButton::clicked,
        this, &AnkiSettings::connectToClient);
    connect(m_ui->m_comboBoxModel, &QComboBox::currentTextChanged,
        this, &AnkiSettings::updateModelFields);
}

AnkiSettings::~AnkiSettings()
{
    delete m_ui;
}

void AnkiSettings::enabledStateChanged(int state)
{
    if (state == Qt::CheckState::Checked)
    {
        m_ui->m_buttonConnect->setEnabled(true);
        m_ui->m_comboBoxDeck->setEnabled(true);
        m_ui->m_comboBoxModel->setEnabled(true);
        m_ui->m_lineEditHost->setEnabled(true);
        m_ui->m_lineEditPort->setEnabled(true);
        m_ui->m_lineEditTags->setEnabled(true);
    }
    else if (state == Qt::CheckState::Unchecked)
    {
        m_ui->m_buttonConnect->setEnabled(false);
        m_ui->m_comboBoxDeck->setEnabled(false);
        m_ui->m_comboBoxModel->setEnabled(false);
        m_ui->m_lineEditHost->setEnabled(false);
        m_ui->m_lineEditPort->setEnabled(false);
        m_ui->m_lineEditTags->setEnabled(false);
    }
}

void AnkiSettings::connectToClient()
{
    m_ui->m_buttonConnect->setEnabled(false);

    m_client->setServer(m_ui->m_lineEditHost->text(),
                        m_ui->m_lineEditPort->text());
    m_client->testConnection([=](const bool val, const QString &error) {
        if (val)
        {
            m_client->getDeckNames(
                [=](const QStringList *decks, const QString &error) {
                    if (error.isEmpty())
                        m_ui->m_comboBoxDeck->addItems(*decks);
                    else
                    {
                        QMessageBox messageBox;
                        messageBox.critical(0,"Error", error);
                    } 
                    delete decks;
                });
            m_client->getModelNames(
                [=](const QStringList *models, const QString &error) {
                    if (error.isEmpty())
                        m_ui->m_comboBoxModel->addItems(*models);
                    else
                    {
                        QMessageBox messageBox;
                        messageBox.critical(0,"Error", error);
                    } 
                    delete models;
                });
        }
        else
        {
            QMessageBox messageBox;
            messageBox.critical(0,"Error", error);
        }

        m_ui->m_buttonConnect->setEnabled(true);
    });
}

void AnkiSettings::updateModelFields(const QString &model)
{
    m_mutexUpdateModelFields.lock();
    m_client->getFieldNames(
        [=](const QStringList *fields, const QString error) {
            if (error.isEmpty())
            {
                m_ui->m_tableFields->setRowCount(fields->size());
                for (uint i = 0; i < fields->size(); ++i)
                {
                    QTableWidgetItem *item = 
                        new QTableWidgetItem(fields->at(i));
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                    m_ui->m_tableFields->setItem(i, 0, item);
                }
            }
            else
            {
                QMessageBox messageBox;
                messageBox.critical(0,"Error", error);
            }
            m_mutexUpdateModelFields.unlock();
            delete fields;
        }, model);
}
