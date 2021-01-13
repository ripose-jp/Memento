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

#define DEFAULT_PROFILE "Default"
#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "8765"
#define DEFAULT_TAGS "memento"

#define REGEX_REMOVE_SPACES_COMMAS "[, ]+"

AnkiSettings::AnkiSettings(AnkiClient *client, QWidget *parent)
    : m_client(client), QWidget(parent), m_ui(new Ui::AnkiSettings)
{
    m_ui->setupUi(this);

    QStringList profiles = m_client->getProfiles();
    if (profiles.isEmpty())
    {
        m_ui->m_comboBoxProfile->addItem(DEFAULT_PROFILE);
        applyChanges();
        m_client->setProfile(DEFAULT_PROFILE);
        profiles.append(DEFAULT_PROFILE);
    }
    else
    {
        m_ui->m_comboBoxProfile->clear();
        m_ui->m_comboBoxProfile->addItems(m_client->getProfiles());
    }

    connect(m_ui->m_checkBoxEnabled, &QCheckBox::stateChanged,
            this, &AnkiSettings::enabledStateChanged);
    connect(m_ui->m_comboBoxProfile, &QComboBox::currentTextChanged,
            this, &AnkiSettings::changeProfile);
    connect(m_ui->m_buttonConnect, &QPushButton::clicked,
            [=] { connectToClient(true); });
    connect(m_ui->m_comboBoxModel, &QComboBox::currentTextChanged,
            this, &AnkiSettings::updateModelFields);

    // Dialog Buttons
    connect(m_ui->m_buttonBox->button(QDialogButtonBox::StandardButton::Reset),
            &QPushButton::clicked, this, &AnkiSettings::restoreSaved);
    connect(m_ui->m_buttonBox->button(
                QDialogButtonBox::StandardButton::RestoreDefaults),
            &QPushButton::clicked, this, &AnkiSettings::restoreDefaults);
    connect(m_ui->m_buttonBox->button(QDialogButtonBox::StandardButton::Apply),
            &QPushButton::clicked, this, &AnkiSettings::applyChanges);
    connect(m_ui->m_buttonBox->button(QDialogButtonBox::StandardButton::Close),
            &QPushButton::clicked, this, &AnkiSettings::hide);
    //connect(m_ui->m_buttonBox->button(QDialogButtonBox::StandardButton::Help),
    //    &QPushButton::clicked, this, &AnkiSettings::hide);

    // Profile actions
    connect(m_ui->m_buttonAdd, &QToolButton::clicked,
            this, &AnkiSettings::addProfile);
    connect(m_ui->m_buttonDelete, &QToolButton::clicked,
            this, &AnkiSettings::deleteProfile);
    connect(m_ui->m_comboBoxProfile, &QComboBox::currentTextChanged,
            this, &AnkiSettings::changeProfile);
}

AnkiSettings::~AnkiSettings()
{
    delete m_ui;
}

void AnkiSettings::showEvent(QShowEvent *event)
{
    restoreSaved();
    connectToClient(false);
}

void AnkiSettings::hideEvent(QHideEvent *event)
{
    const AnkiConfig *config = m_client->getConfig(m_client->getProfile());
    m_client->setServer(config->address, config->port);
}

void AnkiSettings::enabledStateChanged(int state)
{
    bool enabled = state == Qt::CheckState::Checked;

    // Buttons
    m_ui->m_buttonConnect->setEnabled(enabled);
    m_ui->m_buttonAdd->setEnabled(enabled);
    m_ui->m_buttonDelete->setEnabled(enabled);

    // Combo Boxes
    m_ui->m_comboBoxDeck->setEnabled(enabled);
    m_ui->m_comboBoxModel->setEnabled(enabled);
    m_ui->m_comboBoxProfile->setEnabled(enabled);

    // Labels
    m_ui->m_labelDeck->setEnabled(enabled);
    m_ui->m_labelHostName->setEnabled(enabled);
    m_ui->m_labelModel->setEnabled(enabled);
    m_ui->m_labelPort->setEnabled(enabled);
    m_ui->m_labelTags->setEnabled(enabled);
    m_ui->m_labelProfile->setEnabled(enabled);
    m_ui->m_labelProfileName->setEnabled(enabled);

    // Line Edits
    m_ui->m_lineEditHost->setEnabled(enabled);
    m_ui->m_lineEditPort->setEnabled(enabled);
    m_ui->m_lineEditTags->setEnabled(enabled);
    m_ui->m_lineEditProfileName->setEnabled(enabled);

    // Table
    m_ui->m_tableFields->setEnabled(enabled);
}

void AnkiSettings::connectToClient(const bool showErrors)
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
                    {
                        const QString &savedDeck =
                            m_client->getConfig(m_client->getProfile())->deck;
                        m_ui->m_comboBoxDeck->clear();
                        m_ui->m_comboBoxDeck->addItems(*decks);
                        if (!savedDeck.isEmpty())
                        {
                            m_ui->m_comboBoxDeck->setCurrentText(savedDeck);
                        }
                    }
                    else if (showErrors)
                    {
                        QMessageBox messageBox;
                        messageBox.critical(0, "Error", error);
                    }
                    delete decks;
                });
            m_client->getModelNames(
                [=](const QStringList *models, const QString &error) {
                    if (error.isEmpty())
                    {
                        const QString &savedModel =
                            m_client->getConfig(m_client->getProfile())->model;

                        m_ui->m_comboBoxModel->blockSignals(true);
                        m_ui->m_comboBoxModel->clear();
                        m_ui->m_comboBoxModel->addItems(*models);
                        if (!savedModel.isEmpty())
                        {
                            m_ui->m_comboBoxModel->setCurrentText(savedModel);
                        }
                        m_ui->m_comboBoxModel->blockSignals(false);
                    }
                    else if (showErrors)
                    {
                        QMessageBox messageBox;
                        messageBox.critical(0, "Error", error);
                    }
                    delete models;
                });
        }
        else if (showErrors)
        {
            QMessageBox messageBox;
            messageBox.critical(0, "Error", error);
        }

        m_ui->m_buttonConnect->setEnabled(m_ui->m_checkBoxEnabled->isChecked());
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
                for (size_t i = 0; i < fields->size(); ++i)
                {
                    QTableWidgetItem *item =
                        new QTableWidgetItem(fields->at(i));
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                    m_ui->m_tableFields->setItem(i, 0, item);
                }
                for (size_t i = 0; i < fields->size(); ++i)
                {
                    QTableWidgetItem *item = new QTableWidgetItem("");
                    m_ui->m_tableFields->setItem(i, 1, item);
                }
            }
            else
            {
                QMessageBox messageBox;
                messageBox.critical(0, "Error", error);
            }
            m_mutexUpdateModelFields.unlock();
            delete fields;
        },
        model);
}

void AnkiSettings::applyChanges()
{
    AnkiConfig config;

    m_client->setEnabled(m_ui->m_checkBoxEnabled->isChecked());
    m_client->setProfile(m_ui->m_comboBoxProfile->currentText());
    config.address = m_ui->m_lineEditHost->text();
    config.port = m_ui->m_lineEditPort->text();
    QStringList splitTags =
        m_ui->m_lineEditTags->text().split(QRegExp(REGEX_REMOVE_SPACES_COMMAS));
    for (auto it = splitTags.begin(); it != splitTags.end(); ++it)
        config.tags.append(*it);
    config.deck = m_ui->m_comboBoxDeck->currentText();
    config.model = m_ui->m_comboBoxModel->currentText();
    for (size_t i = 0; i < m_ui->m_tableFields->rowCount(); ++i)
    {
        QTableWidgetItem *field = m_ui->m_tableFields->item(i, 0);
        QTableWidgetItem *value = m_ui->m_tableFields->item(i, 1);
        config.fields[field->text()] = value->text();
    }

    m_client->addProfile(m_ui->m_comboBoxProfile->currentText(), config);
}

void AnkiSettings::restoreDefaults()
{
    m_ui->m_lineEditHost->setText(DEFAULT_HOST);
    m_ui->m_lineEditPort->setText(DEFAULT_PORT);
    m_ui->m_lineEditTags->setText(DEFAULT_TAGS);
    for (size_t i = 0; i < m_ui->m_tableFields->rowCount(); ++i)
    {
        m_ui->m_tableFields->item(i, 1)->setText("");
    }
}

void AnkiSettings::restoreSaved()
{
    populateFields(
        m_client->getProfile(), m_client->getConfig(m_client->getProfile()));

    m_ui->m_comboBoxProfile->blockSignals(true);
    m_ui->m_comboBoxProfile->setCurrentText(m_client->getProfile());
    m_ui->m_comboBoxProfile->blockSignals(false);
}

void AnkiSettings::addProfile()
{
    QString profileName = m_ui->m_lineEditProfileName->text();
    if (m_client->getProfiles().contains(profileName))
    {
        QMessageBox messageBox;
        messageBox.information(
            0, "Failed", 
            "Profile with name " + profileName + " already exists.");
    }
    else
    {
        m_ui->m_comboBoxProfile->blockSignals(true);
        m_ui->m_comboBoxProfile->addItem(profileName);
        m_ui->m_comboBoxProfile->setCurrentText(profileName);
        m_ui->m_comboBoxProfile->blockSignals(false);
        applyChanges();
    }
    
}

void AnkiSettings::deleteProfile()
{
    if (m_ui->m_comboBoxProfile->currentText() == DEFAULT_PROFILE)
    {
        QMessageBox messageBox;
        messageBox.information(0, "Failed",
                               "The Default profile cannot be deleted");
        return;
    }

    m_client->deleteProfile(m_ui->m_comboBoxProfile->currentText());
    m_ui->m_comboBoxProfile->removeItem(
        m_ui->m_comboBoxProfile->currentIndex());
}

void AnkiSettings::changeProfile(const QString &text)
{
    populateFields(text, m_client->getConfig(text));
}

void AnkiSettings::populateFields(const QString &profile,
                                  const AnkiConfig *config)
{
    m_ui->m_checkBoxEnabled->setChecked(m_client->isEnabled());

    m_ui->m_lineEditProfileName->setText(profile);

    m_ui->m_lineEditHost->setText(config->address);
    m_ui->m_lineEditPort->setText(config->port);

    m_client->setServer(config->address, config->port);

    QString tags;
    for (auto it = config->tags.begin(); it != config->tags.end(); ++it)
        tags += it->toString() + ",";
    tags.chop(1);
    m_ui->m_lineEditTags->setText(tags);

    m_ui->m_comboBoxDeck->blockSignals(true);
    m_ui->m_comboBoxDeck->setCurrentText(config->deck);
    m_ui->m_comboBoxDeck->blockSignals(false);

    m_ui->m_comboBoxModel->blockSignals(true);
    m_ui->m_comboBoxModel->setCurrentText(config->model);
    m_ui->m_comboBoxModel->blockSignals(false);

    m_ui->m_tableFields->setRowCount(config->fields.size());
    for (size_t i = 0; i < config->fields.size(); ++i)
    {
        QString key = config->fields.keys()[i];
        QTableWidgetItem *item = new QTableWidgetItem(key);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        m_ui->m_tableFields->setItem(i, 0, item);

        item = new QTableWidgetItem(config->fields[key].toString());
        m_ui->m_tableFields->setItem(i, 1, item);
    }
}