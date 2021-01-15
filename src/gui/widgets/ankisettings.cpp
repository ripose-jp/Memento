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
    : m_client(client),
      m_ankiSettingsHelp(new AnkiSettingsHelp),
      m_configs(0),
      QWidget(parent),
      m_ui(new Ui::AnkiSettings)
{
    m_ui->setupUi(this);

    if (m_client->getProfile().isEmpty())
    {
        AnkiConfig defaultConfig;
        defaultConfig.address = DEFAULT_HOST;
        defaultConfig.port = DEFAULT_PORT;
        defaultConfig.tags.append(DEFAULT_TAGS);
        m_client->addProfile(DEFAULT_PROFILE, defaultConfig);
        m_client->setProfile(DEFAULT_PROFILE);
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
    connect(m_ui->m_buttonBox->button(QDialogButtonBox::StandardButton::Help),
        &QPushButton::clicked, m_ankiSettingsHelp, &AnkiSettingsHelp::show);

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
    delete m_ankiSettingsHelp;
    clearConfigs();
}

void AnkiSettings::clearConfigs()
{
    if (m_configs)
    {
        for (auto it = m_configs->begin(); it != m_configs->end(); ++it)
        {
            delete *it;
        }
        delete m_configs;
        m_configs = 0;
    }
}

void AnkiSettings::showEvent(QShowEvent *event)
{
    m_ui->m_checkBoxEnabled->setChecked(m_client->isEnabled());
    m_configs = m_client->getConfigs();
    m_currentProfile = m_client->getProfile();
    populateFields(
        m_client->getProfile(), m_configs->value(m_client->getProfile()));
    connectToClient(false);
}

void AnkiSettings::hideEvent(QHideEvent *event)
{
    const AnkiConfig *config = m_client->getConfig(m_client->getProfile());
    m_client->setServer(config->address, config->port);
    clearConfigs();
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

void AnkiSettings::addProfile()
{
    QString profileName = m_ui->m_lineEditProfileName->text();
    if (m_configs->contains(profileName))
    {
        QMessageBox messageBox;
        messageBox.information(
            0, "Failed",
            "Profile with name " + profileName + " already exists.");
    }
    else
    {
        m_configs->insert(
            profileName,
            new AnkiConfig(
                *m_configs->value(m_ui->m_comboBoxProfile->currentText())));

        m_ui->m_comboBoxProfile->blockSignals(true);
        m_ui->m_comboBoxProfile->addItem(profileName);
        m_ui->m_comboBoxProfile->setCurrentText(profileName);
        m_ui->m_comboBoxProfile->model()->sort(0);
        m_ui->m_comboBoxProfile->blockSignals(false);

        m_currentProfile = profileName;
    }
}

void AnkiSettings::deleteProfile()
{
    QString profile = m_ui->m_comboBoxProfile->currentText();
    if (profile == DEFAULT_PROFILE)
    {
        QMessageBox messageBox;
        messageBox.information(0, "Failed",
                               "The Default profile cannot be deleted");
    }
    else
    {
        delete m_configs->value(profile);
        m_configs->remove(profile);

        m_ui->m_comboBoxProfile->blockSignals(true);
        m_ui->m_comboBoxProfile->removeItem(
            m_ui->m_comboBoxProfile->currentIndex());
        populateFields(
            m_ui->m_comboBoxProfile->currentText(),
            m_configs->value(m_ui->m_comboBoxProfile->currentText()));
        m_ui->m_comboBoxProfile->blockSignals(false);

        m_currentProfile = m_ui->m_comboBoxProfile->currentText();
    }
}

void AnkiSettings::changeProfile(const QString &text)
{
    applyToConfig(m_currentProfile);
    if (m_currentProfile != m_ui->m_lineEditProfileName->text())
        renameProfile(m_currentProfile, m_ui->m_lineEditProfileName->text());
    populateFields(text, m_configs->value(text));
    m_currentProfile = text;
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
                        m_ui->m_comboBoxDeck->clear();
                        m_ui->m_comboBoxDeck->addItems(*decks);
                        m_ui->m_comboBoxDeck->
                            setCurrentText(
                                m_client->getConfig(
                                    m_client->getProfile())->deck);
                        m_ui->m_comboBoxDeck->model()->sort(0);
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
                        m_ui->m_comboBoxModel->blockSignals(true);
                        m_ui->m_comboBoxModel->clear();
                        m_ui->m_comboBoxModel->addItems(*models);
                        m_ui->m_comboBoxModel->
                            setCurrentText(
                                m_client->getConfig(
                                    m_client->getProfile())->model);
                        m_ui->m_comboBoxModel->model()->sort(0);
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
    // Renaming profile if changed
    if (m_ui->m_comboBoxProfile->currentText() !=
        m_ui->m_lineEditProfileName->text())
    {
        renameProfile(
            m_ui->m_comboBoxProfile->currentText(),
            m_ui->m_lineEditProfileName->text()
        );
    }

    applyToConfig(m_ui->m_comboBoxProfile->currentText());

    // Apply changes to the client
    m_client->setEnabled(m_ui->m_checkBoxEnabled->isChecked());
    m_client->clearProfiles();
    for (auto it = m_configs->constKeyValueBegin();
         it != m_configs->constKeyValueEnd();
         ++it)
    {
        m_client->addProfile(it->first, *it->second);
    }
    m_client->setProfile(m_ui->m_comboBoxProfile->currentText());

    // Write the changes to the config file
    m_client->writeChanges();
}

void AnkiSettings::restoreDefaults()
{
    AnkiConfig defaultConfig;
    defaultConfig.address = DEFAULT_HOST;
    defaultConfig.port = DEFAULT_PORT;
    defaultConfig.tags.append(DEFAULT_TAGS);
    defaultConfig.deck = m_ui->m_comboBoxDeck->currentText();
    defaultConfig.model = m_ui->m_comboBoxModel->currentText();
    QStringList fields =
        m_configs->value(m_ui->m_comboBoxProfile->currentText())->fields.keys();
    for (auto it = fields.begin(); it != fields.end(); ++it)
        defaultConfig.fields[*it] = "";
    populateFields(m_ui->m_comboBoxProfile->currentText(), &defaultConfig);
}

void AnkiSettings::restoreSaved()
{
    clearConfigs();
    m_configs = m_client->getConfigs();

    m_ui->m_comboBoxProfile->blockSignals(true);
    m_ui->m_comboBoxProfile->clear();
    for (auto it = m_configs->keyBegin(); it != m_configs->keyEnd(); ++it)
    {
        m_ui->m_comboBoxProfile->addItem(*it);
    }
    m_ui->m_comboBoxProfile->model()->sort(0);
    m_ui->m_comboBoxProfile->blockSignals(false);

    populateFields(
        m_client->getProfile(), m_client->getConfig(m_client->getProfile())
    );

    m_currentProfile = m_client->getProfile();
}

void AnkiSettings::populateFields(const QString &profile,
                                  const AnkiConfig *config)
{
    m_ui->m_comboBoxProfile->blockSignals(true);
    m_ui->m_comboBoxProfile->clear();
    for (auto it = m_configs->keyBegin(); it != m_configs->keyEnd(); ++it)
        m_ui->m_comboBoxProfile->addItem(*it);
    m_ui->m_comboBoxProfile->setCurrentText(profile);
    m_ui->m_comboBoxProfile->model()->sort(0);
    m_ui->m_comboBoxProfile->blockSignals(false);

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

        QString itemText;
        if (!config->fields[key].isUndefined())
        {
            itemText = config->fields[key].toString();
        }
        item = new QTableWidgetItem(itemText);
        m_ui->m_tableFields->setItem(i, 1, item);
    }
}

void AnkiSettings::applyToConfig(const QString &profile)
{
    AnkiConfig *config = m_configs->value(profile);

    config->address = m_ui->m_lineEditHost->text();
    config->port = m_ui->m_lineEditPort->text();

    config->tags = QJsonArray();
    QStringList splitTags =
        m_ui->m_lineEditTags->text().split(QRegExp(REGEX_REMOVE_SPACES_COMMAS));
    for (auto it = splitTags.begin(); it != splitTags.end(); ++it)
        config->tags.append(*it);

    config->deck = m_ui->m_comboBoxDeck->currentText();
    config->model = m_ui->m_comboBoxModel->currentText();

    config->fields = QJsonObject();
    for (size_t i = 0; i < m_ui->m_tableFields->rowCount(); ++i)
    {
        QTableWidgetItem *field = m_ui->m_tableFields->item(i, 0);
        QTableWidgetItem *value = m_ui->m_tableFields->item(i, 1);
        config->fields[field->text()] = value->text();
    }
}

void AnkiSettings::renameProfile(const QString &oldName, const QString &newName)
{

    if (oldName == DEFAULT_PROFILE)
    {
        QMessageBox messageBox;
        messageBox.information(
            0, "Info", "Default profile cannnot be renamed");
        m_ui->m_lineEditProfileName->setText(DEFAULT_PROFILE);
    }
    else if (newName.isEmpty())
    {
        QMessageBox messageBox;
        messageBox.information(
            0, "Info", "Profile must have a name");
        m_ui->m_lineEditProfileName->setText(oldName);
    }
    else
    {
        m_configs->insert(newName, m_configs->value(oldName));
        m_configs->remove(oldName);

        m_ui->m_comboBoxProfile->blockSignals(true);
        m_ui->m_comboBoxProfile->clear();
        for (auto it = m_configs->keyBegin(); it != m_configs->keyEnd(); ++it)
        {
            m_ui->m_comboBoxProfile->addItem(*it);
        }
        m_ui->m_comboBoxProfile->setCurrentText(newName);
        m_ui->m_comboBoxProfile->model()->sort(0);
        m_ui->m_comboBoxProfile->blockSignals(false);
    }
}
