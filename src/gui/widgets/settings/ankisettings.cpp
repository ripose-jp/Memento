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

#include <QSettings>
#include <QTableWidgetItem>

#include "anki/ankiclient.h"
#include "anki/marker.h"
#include "dict/dictionary.h"
#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/iconfactory.h"

static constexpr int ROLE_DICTIONARY_ID{Qt::UserRole + 0};
static constexpr int ROLE_DICTIONARY_NAME{Qt::UserRole + 1};

/* Begin Constructor/Destructors */

AnkiSettings::AnkiSettings(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::AnkiSettings),
      m_ankiSettingsHelp(new AnkiSettingsHelp)
{
    m_ui->setupUi(this);
    m_ui->frameAdvanced->hide();

    const auto makeSuggestion = [] (const char *str) -> QString
    {
        return QString("{%1}").arg(str);
    };
    m_ui->termCardBuilder->setSuggestions({
        /* Shared */
        makeSuggestion(Anki::Marker::AUDIO_CONTEXT),
        makeSuggestion(Anki::Marker::AUDIO_MEDIA),
        makeSuggestion(Anki::Marker::CLIPBOARD),
        makeSuggestion(Anki::Marker::CLOZE_BODY),
        makeSuggestion(Anki::Marker::CLOZE_PREFIX),
        makeSuggestion(Anki::Marker::CLOZE_SUFFIX),
        makeSuggestion(Anki::Marker::CONTEXT_SEC),
        makeSuggestion(Anki::Marker::CONTEXT),
        makeSuggestion(Anki::Marker::FREQ_AVERAGE_OCCU),
        makeSuggestion(Anki::Marker::FREQ_AVERAGE_RANK),
        makeSuggestion(Anki::Marker::FREQ_HARMONIC_OCCU),
        makeSuggestion(Anki::Marker::FREQ_HARMONIC_RANK),
        makeSuggestion(Anki::Marker::FREQUENCIES),
        makeSuggestion(Anki::Marker::GLOSSARY),
        makeSuggestion(Anki::Marker::SCREENSHOT_VIDEO),
        makeSuggestion(Anki::Marker::SCREENSHOT),
        makeSuggestion(Anki::Marker::SENTENCE_SEC),
        makeSuggestion(Anki::Marker::SENTENCE),
        makeSuggestion(Anki::Marker::TAGS_BRIEF),
        makeSuggestion(Anki::Marker::TAGS),
        makeSuggestion(Anki::Marker::TITLE),

        /* Term */
        makeSuggestion(Anki::Marker::AUDIO),
        makeSuggestion(Anki::Marker::EXPRESSION),
        makeSuggestion(Anki::Marker::FURIGANA),
        makeSuggestion(Anki::Marker::FURIGANA_PLAIN),
        makeSuggestion(Anki::Marker::GLOSSARY_BRIEF),
        makeSuggestion(Anki::Marker::GLOSSARY_COMPACT),
        makeSuggestion(Anki::Marker::PITCH),
        makeSuggestion(Anki::Marker::PITCH_CATEGORIES),
        makeSuggestion(Anki::Marker::PITCH_GRAPHS),
        makeSuggestion(Anki::Marker::PITCH_POSITION),
        makeSuggestion(Anki::Marker::READING),
        makeSuggestion(Anki::Marker::SELECTION),
    });
    m_ui->kanjiCardBuilder->setSuggestions({
        /* Shared */
        makeSuggestion(Anki::Marker::AUDIO_CONTEXT),
        makeSuggestion(Anki::Marker::AUDIO_MEDIA),
        makeSuggestion(Anki::Marker::CLIPBOARD),
        makeSuggestion(Anki::Marker::CLOZE_BODY),
        makeSuggestion(Anki::Marker::CLOZE_PREFIX),
        makeSuggestion(Anki::Marker::CLOZE_SUFFIX),
        makeSuggestion(Anki::Marker::CONTEXT_SEC),
        makeSuggestion(Anki::Marker::CONTEXT),
        makeSuggestion(Anki::Marker::FREQ_AVERAGE_OCCU),
        makeSuggestion(Anki::Marker::FREQ_AVERAGE_RANK),
        makeSuggestion(Anki::Marker::FREQ_HARMONIC_OCCU),
        makeSuggestion(Anki::Marker::FREQ_HARMONIC_RANK),
        makeSuggestion(Anki::Marker::FREQUENCIES),
        makeSuggestion(Anki::Marker::GLOSSARY),
        makeSuggestion(Anki::Marker::SCREENSHOT_VIDEO),
        makeSuggestion(Anki::Marker::SCREENSHOT),
        makeSuggestion(Anki::Marker::SENTENCE_SEC),
        makeSuggestion(Anki::Marker::SENTENCE),
        makeSuggestion(Anki::Marker::TAGS_BRIEF),
        makeSuggestion(Anki::Marker::TAGS),
        makeSuggestion(Anki::Marker::TITLE),

        /* Kanji */
        makeSuggestion(Anki::Marker::CHARACTER),
        makeSuggestion(Anki::Marker::KUNYOMI),
        makeSuggestion(Anki::Marker::ONYOMI),
        makeSuggestion(Anki::Marker::STROKE_COUNT),
    });

    initIcons();

    connect(
        m_ui->checkBoxEnabled,
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
        &QCheckBox::stateChanged,
#else
        &QCheckBox::checkStateChanged,
#endif
        this,
        &AnkiSettings::enabledStateChanged
    );
    connect(
        m_ui->comboBoxProfile, &QComboBox::currentTextChanged,
        this,                  &AnkiSettings::changeProfile
    );
    connect(
        m_ui->buttonConnect, &QPushButton::clicked,
        this, [this] { return connectToClient(true); }
    );
    connect(
        m_ui->checkboxAdvanced,
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
        &QCheckBox::stateChanged,
#else
        &QCheckBox::checkStateChanged,
#endif
        this,
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
        [this] (int state)
#else
        [this] (Qt::CheckState state)
#endif
        {
            m_ui->frameAdvanced->setVisible(state == Qt::Checked);
        }
    );
    connect(m_ui->termCardBuilder,  &CardBuilder::modelTextChanged, this,
        [this] (const QString &model)
        {
            updateModelFields(m_ui->termCardBuilder, model);
        }
    );
    connect(m_ui->kanjiCardBuilder, &CardBuilder::modelTextChanged, this,
        [this] (const QString &model)
        {
            updateModelFields(m_ui->kanjiCardBuilder, model);
        }
    );

    /* Dialog Buttons */
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Reset),
            &QPushButton::clicked,
        this, &AnkiSettings::restoreSaved
    );
    connect(
        m_ui->buttonBox->button(
                QDialogButtonBox::StandardButton::RestoreDefaults
            ),
        &QPushButton::clicked,
        this,
        &AnkiSettings::restoreDefaults
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply),
        &QPushButton::clicked,
        this,
        &AnkiSettings::applyChanges
    );
    connect(
        m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Help),
        &QPushButton::clicked,
        m_ankiSettingsHelp,
        &AnkiSettingsHelp::show
    );

    /* Profile actions */
    connect(
        m_ui->buttonAdd, &QToolButton::clicked,
        this,            &AnkiSettings::addProfile
    );
    connect(
        m_ui->buttonDelete, &QToolButton::clicked,
        this,               &AnkiSettings::deleteProfile
    );
    connect(
        m_ui->comboBoxProfile, &QComboBox::currentTextChanged,
        this,                  &AnkiSettings::changeProfile
    );

    /* Theme Changes */
    connect(
        GlobalMediator::getGlobalMediator(),
        &GlobalMediator::requestThemeRefresh,
        this,
        &AnkiSettings::initIcons
    );
}

AnkiSettings::~AnkiSettings()
{
    disconnect();
    delete m_ui;
    delete m_ankiSettingsHelp;
}

/* End Constructor/Destructors */
/* Begin Initializers */

void AnkiSettings::initIcons()
{
    IconFactory *factory = IconFactory::create();
    m_ui->buttonAdd->setIcon(factory->getIcon(IconFactory::Icon::plus));
    m_ui->buttonDelete->setIcon(factory->getIcon(IconFactory::Icon::minus));
}

/* End Initializers */
/* Begin Event Handlers */

void AnkiSettings::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    AnkiClient *client = GlobalMediator::getGlobalMediator()->getAnkiClient();
    m_ui->checkBoxEnabled->setChecked(client->isEnabled());
    m_configs = client->getConfigs();
    m_currentProfile = client->getProfile();
    m_ui->labelHelpMessage->setVisible(!client->configExists());
    populateFields(
        client->getProfile(), *m_configs[client->getProfile()]
    );
    connectToClient(false);
}

void AnkiSettings::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);

    AnkiClient *client =
        GlobalMediator::getGlobalMediator()->getAnkiClient();
    QSharedPointer<const AnkiConfig> config =
        client->getConfig(client->getProfile());
    client->setServer(config->address, config->port);
    m_configs.clear();
}

/* End Event Handlers */
/* Begin Dialog Button Actions */

void AnkiSettings::applyChanges()
{
    /* Renaming profile if changed */
    if (m_ui->comboBoxProfile->currentText() !=
        m_ui->lineEditProfileName->text())
    {
        renameProfile(
            m_ui->comboBoxProfile->currentText(),
            m_ui->lineEditProfileName->text()
        );
    }

    applyToConfig(m_ui->comboBoxProfile->currentText());

    /* Apply changes to the client */
    AnkiClient *client = GlobalMediator::getGlobalMediator()->getAnkiClient();
    client->setEnabled(m_ui->checkBoxEnabled->isChecked());
    client->clearProfiles();
    for (auto it = m_configs.constKeyValueBegin();
         it != m_configs.constKeyValueEnd();
         ++it)
    {
        client->addProfile(it->first, *it->second);
    }
    client->setProfile(m_ui->comboBoxProfile->currentText());

    /* Write the changes to the config file */
    client->writeChanges();
}

void AnkiSettings::restoreDefaults()
{
    AnkiConfig defaultConfig;
    defaultConfig.address         = DEFAULT_HOST;
    defaultConfig.port            = DEFAULT_PORT;
    defaultConfig.duplicatePolicy = DEFAULT_DUPLICATE_POLICY;
    defaultConfig.newlineReplacer = DEFAULT_NEWLINE_REPLACER;
    defaultConfig.screenshotType  = DEFAULT_SCREENSHOT;
    defaultConfig.audioPadStart   = DEFAULT_AUDIO_PAD_START;
    defaultConfig.audioPadStart   = DEFAULT_AUDIO_PAD_END;
    defaultConfig.audioNormalize  = DEFAULT_AUDIO_NORMALIZE;
    defaultConfig.audioDb         = DEFAULT_AUDIO_DB;
    defaultConfig.tags.append(DEFAULT_TAGS);
    defaultConfig.excludeGloss.clear();

    defaultConfig.termDeck        = m_ui->termCardBuilder->getDeckText();
    defaultConfig.termModel       = m_ui->termCardBuilder->getModelText();
    QStringList fields            = m_configs.value(
            m_ui->comboBoxProfile->currentText()
        )->termFields.keys();
    for (const QString &field : fields)
        defaultConfig.termFields[field] = "";

    defaultConfig.kanjiDeck       = m_ui->kanjiCardBuilder->getDeckText();
    defaultConfig.kanjiModel      = m_ui->kanjiCardBuilder->getModelText();
    fields                        = m_configs.value(
            m_ui->comboBoxProfile->currentText()
        )->kanjiFields.keys();
    for (const QString &field : fields)
        defaultConfig.kanjiFields[field] = "";

    populateFields(m_ui->comboBoxProfile->currentText(), defaultConfig);
}

void AnkiSettings::restoreSaved()
{
    m_configs.clear();

    AnkiClient *client = GlobalMediator::getGlobalMediator()->getAnkiClient();
    m_configs = client->getConfigs();

    m_ui->comboBoxProfile->blockSignals(true);
    m_ui->comboBoxProfile->clear();
    for (auto it = m_configs.keyBegin(); it != m_configs.keyEnd(); ++it)
    {
        m_ui->comboBoxProfile->addItem(*it);
    }
    m_ui->comboBoxProfile->model()->sort(0);
    m_ui->comboBoxProfile->blockSignals(false);

    populateFields(
        client->getProfile(), *client->getConfig(client->getProfile())
    );

    m_currentProfile = client->getProfile();
}

/* End Dialog Button Actions */
/* Begin AnkiConnect Actions */

QCoro::Task<void> AnkiSettings::connectToClient(const bool showErrors)
{
    m_ui->buttonConnect->setEnabled(false);

    AnkiClient *client = GlobalMediator::getGlobalMediator()->getAnkiClient();
    client->setServer(m_ui->lineEditHost->text(), m_ui->lineEditPort->text());

    AnkiReply<bool> testResult = co_await client->testConnection();
    if (!testResult.value)
    {
        if (showErrors)
        {
            emit GlobalMediator::getGlobalMediator()
                ->showCritical("Error", testResult.error);
        }
        m_ui->buttonConnect->setEnabled(m_ui->checkBoxEnabled->isChecked());
        co_return;
    }

    AnkiReply<QStringList> deckNames = co_await client->getDeckNames();
    if (!deckNames.error.isEmpty())
    {
        if (showErrors)
        {
            emit GlobalMediator::getGlobalMediator()
                ->showCritical("Error", deckNames.error);
        }
        m_ui->buttonConnect->setEnabled(m_ui->checkBoxEnabled->isChecked());
        co_return;
    }
    m_ui->termCardBuilder->setDecks(
        deckNames.value,
        client->getConfig(client->getProfile())->termDeck
    );
    m_ui->kanjiCardBuilder->setDecks(
        deckNames.value,
        client->getConfig(client->getProfile())->kanjiDeck
    );

    AnkiReply<QStringList> modelNames = co_await client->getModelNames();
    if (!modelNames.error.isEmpty())
    {
        if (showErrors)
        {
            emit GlobalMediator::getGlobalMediator()
                ->showCritical("Error", modelNames.error);
        }
        m_ui->buttonConnect->setEnabled(m_ui->checkBoxEnabled->isChecked());
        co_return;
    }
    m_ui->termCardBuilder->blockSignals(true);
    m_ui->termCardBuilder->setModels(
        modelNames.value,
        client->getConfig(client->getProfile())->termModel
    );
    m_ui->termCardBuilder->blockSignals(false);

    m_ui->kanjiCardBuilder->blockSignals(true);
    m_ui->kanjiCardBuilder->setModels(
        modelNames.value,
        client->getConfig(client->getProfile())->kanjiModel
    );
    m_ui->kanjiCardBuilder->blockSignals(false);

    m_ui->buttonConnect->setEnabled(m_ui->checkBoxEnabled->isChecked());
}

QCoro::Task<void> AnkiSettings::updateModelFields(
    CardBuilder *cb, const QString &model)
{
    m_mutexUpdateModelFields.lock();
    AnkiClient *client = GlobalMediator::getGlobalMediator()->getAnkiClient();
    AnkiReply<QStringList> fieldNames = co_await client->getFieldNames(model);
    if (fieldNames.error.isEmpty())
    {
        cb->setFields(fieldNames.value);
    }
    else
    {
        emit GlobalMediator::getGlobalMediator()
            ->showCritical("Error", fieldNames.error);
    }
    m_mutexUpdateModelFields.unlock();
}

/* End AnkiConnect Actions */
/* Begin UI Management */

#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
void AnkiSettings::enabledStateChanged(int state)
#else
void AnkiSettings::enabledStateChanged(Qt::CheckState state)
#endif
{
    bool enabled = state == Qt::CheckState::Checked;
    m_ui->frameContent->setEnabled(enabled);
    m_ui->buttonAdd->setEnabled(enabled);
    m_ui->buttonDelete->setEnabled(enabled);
    m_ui->buttonConnect->setEnabled(enabled);
}

void AnkiSettings::populateFields(
    const QString &profile,
    const AnkiConfig &config)
{
    AnkiClient *client = GlobalMediator::getGlobalMediator()->getAnkiClient();

    m_ui->comboBoxProfile->blockSignals(true);
    m_ui->comboBoxProfile->clear();
    for (auto it = m_configs.keyBegin(); it != m_configs.keyEnd(); ++it)
    {
        m_ui->comboBoxProfile->addItem(*it);
    }
    m_ui->comboBoxProfile->setCurrentText(profile);
    m_ui->comboBoxProfile->model()->sort(0);
    m_ui->comboBoxProfile->blockSignals(false);

    m_ui->lineEditProfileName->setText(profile);

    m_ui->lineEditHost->setText(config.address);
    m_ui->lineEditPort->setText(config.port);

    client->setServer(config.address, config.port);

    m_ui->comboBoxDuplicates->setCurrentText(
        duplicatePolicyToString(config.duplicatePolicy)
    );

    m_ui->lineEditNewlineReplacer->setText(config.newlineReplacer);

    m_ui->comboBoxScreenshot->setCurrentText(
        fileTypeToString(config.screenshotType)
    );

    m_ui->spinAudioPadStart->setValue(config.audioPadStart);
    m_ui->spinAudioPadEnd->setValue(config.audioPadEnd);

    m_ui->checkAudioNormalize->setChecked(config.audioNormalize);
    m_ui->doubleAudioDb->setValue(config.audioDb);

    m_ui->listIncludeGlossary->clear();
    Dictionary *dict = GlobalMediator::getGlobalMediator()->getDictionary();
    QList<DictionaryInfo> dictionaries = dict->getDictionaries();
    for (const DictionaryInfo &info : dictionaries)
    {
        QListWidgetItem *item = new QListWidgetItem(info.name);
        item->setData(ROLE_DICTIONARY_ID, QVariant::fromValue(info.id));
        item->setData(ROLE_DICTIONARY_NAME, QVariant::fromValue(info.name));
        m_ui->listIncludeGlossary->addItem(item);
    }
    for (int i = 0; i < m_ui->listIncludeGlossary->count(); ++i)
    {
        QListWidgetItem *item = m_ui->listIncludeGlossary->item(i);
        item->setFlags(item->flags() | Qt::ItemFlag::ItemIsUserCheckable);
        item->setCheckState(
            config.excludeGloss.contains(
                item->data(ROLE_DICTIONARY_NAME).toString()
            ) ? Qt::Unchecked : Qt::Checked
        );
    }

    QString tags;
    for (const QJsonValue &tag : config.tags)
    {
        tags += tag.toString() + ",";
    }
    tags.chop(1);
    m_ui->lineEditTags->setText(tags);

    m_ui->termCardBuilder->blockSignals(true);
    m_ui->termCardBuilder->setDeckCurrentText(config.termDeck);
    m_ui->termCardBuilder->setModelCurrentText(config.termModel);
    m_ui->termCardBuilder->blockSignals(false);

    m_ui->termCardBuilder->setFields(config.termFields);

    m_ui->kanjiCardBuilder->blockSignals(true);
    m_ui->kanjiCardBuilder->setDeckCurrentText(config.kanjiDeck);
    m_ui->kanjiCardBuilder->setModelCurrentText(config.kanjiModel);
    m_ui->kanjiCardBuilder->blockSignals(false);

    m_ui->kanjiCardBuilder->setFields(config.kanjiFields);
}

void AnkiSettings::changeProfile(const QString &text)
{
    applyToConfig(m_currentProfile);
    if (m_currentProfile != m_ui->lineEditProfileName->text())
    {
       renameProfile(m_currentProfile, m_ui->lineEditProfileName->text());
    }
    populateFields(text, *m_configs[text]);
    m_currentProfile = text;
}

/* End UI Management */
/* Begin Cache Management */

void AnkiSettings::addProfile()
{
    QString profileName = m_ui->lineEditProfileName->text();
    if (m_configs.contains(profileName))
    {
        emit GlobalMediator::getGlobalMediator()->showInformation(
            "Failed",
            "Profile with name " + profileName + " already exists."
        );
    }
    else
    {
        m_configs[profileName] = QSharedPointer<AnkiConfig>(
            new AnkiConfig(*m_configs[m_ui->comboBoxProfile->currentText()])
        );

        m_ui->comboBoxProfile->blockSignals(true);
        m_ui->comboBoxProfile->addItem(profileName);
        m_ui->comboBoxProfile->setCurrentText(profileName);
        m_ui->comboBoxProfile->model()->sort(0);
        m_ui->comboBoxProfile->blockSignals(false);

        m_currentProfile = profileName;
    }
}

void AnkiSettings::deleteProfile()
{
    QString profile = m_ui->comboBoxProfile->currentText();
    if (profile == DEFAULT_PROFILE)
    {
        emit GlobalMediator::getGlobalMediator()->showInformation(
            "Failed",
            "The Default profile cannot be deleted"
        );
    }
    else
    {
        m_configs.remove(profile);

        m_ui->comboBoxProfile->blockSignals(true);
        m_ui->comboBoxProfile->removeItem(
            m_ui->comboBoxProfile->currentIndex());
        populateFields(
            m_ui->comboBoxProfile->currentText(),
            *m_configs[m_ui->comboBoxProfile->currentText()]
        );
        m_ui->comboBoxProfile->blockSignals(false);

        m_currentProfile = m_ui->comboBoxProfile->currentText();
    }
}

void AnkiSettings::renameProfile(const QString &oldName, const QString &newName)
{

    if (oldName == DEFAULT_PROFILE)
    {
        emit GlobalMediator::getGlobalMediator()->showInformation(
            "Info",
            "Default profile cannot be renamed"
        );
        m_ui->lineEditProfileName->setText(DEFAULT_PROFILE);
    }
    else if (newName.isEmpty())
    {
        emit GlobalMediator::getGlobalMediator()->showInformation(
            "Info",
            "Profile must have a name"
        );
        m_ui->lineEditProfileName->setText(oldName);
    }
    else
    {
        m_configs[newName] = m_configs[oldName];
        m_configs.remove(oldName);

        m_ui->comboBoxProfile->blockSignals(true);
        m_ui->comboBoxProfile->clear();
        for (auto it = m_configs.keyBegin(); it != m_configs.keyEnd(); ++it)
        {
            m_ui->comboBoxProfile->addItem(*it);
        }
        m_ui->comboBoxProfile->setCurrentText(newName);
        m_ui->comboBoxProfile->model()->sort(0);
        m_ui->comboBoxProfile->blockSignals(false);
    }
}

#define REGEX_REMOVE_SPACES_COMMAS "[, ]+"

void AnkiSettings::applyToConfig(const QString &profile)
{
    QSharedPointer<AnkiConfig> config = m_configs[profile];

    config->address = m_ui->lineEditHost->text();
    config->port = m_ui->lineEditPort->text();

    config->duplicatePolicy =
        stringToDuplicatePolicy(m_ui->comboBoxDuplicates->currentText()
    );
    config->newlineReplacer = m_ui->lineEditNewlineReplacer->text();
    config->screenshotType =
        stringToFileType(m_ui->comboBoxScreenshot->currentText()
    );

    config->audioPadStart = m_ui->spinAudioPadStart->value();
    config->audioPadEnd   = m_ui->spinAudioPadEnd->value();

    config->audioNormalize = m_ui->checkAudioNormalize->isChecked();
    config->audioDb        = m_ui->doubleAudioDb->value();

    config->tags = QJsonArray();
    QStringList splitTags =
        m_ui->lineEditTags->text()
            .split(QRegularExpression(REGEX_REMOVE_SPACES_COMMAS));
    for (const QString &tag : splitTags)
    {
        config->tags.append(tag);
    }

    config->excludeGloss.clear();
    for (int i = 0; i < m_ui->listIncludeGlossary->count(); ++i)
    {
        QListWidgetItem *item = m_ui->listIncludeGlossary->item(i);
        if (item->checkState() == Qt::Unchecked)
        {
            config->excludeGloss << item->text();
        }
    }

    if (!m_ui->termCardBuilder->getDeckText().isEmpty())
    {
        config->termDeck = m_ui->termCardBuilder->getDeckText();
    }
    if (!m_ui->termCardBuilder->getModelText().isEmpty())
    {
        config->termModel = m_ui->termCardBuilder->getModelText();
    }
    config->termFields = m_ui->termCardBuilder->getFields();

    if (!m_ui->kanjiCardBuilder->getDeckText().isEmpty())
    {
        config->kanjiDeck = m_ui->kanjiCardBuilder->getDeckText();
    }
    if (!m_ui->kanjiCardBuilder->getModelText().isEmpty())
    {
        config->kanjiModel = m_ui->kanjiCardBuilder->getModelText();
    }
    config->kanjiFields = m_ui->kanjiCardBuilder->getFields();
}

#undef REGEX_REMOVE_SPACES_COMMAS

/* End Cache Management */
/* Begin Helpers */

#define DUPLICATE_POLICY_NONE       "None"
#define DUPLICATE_POLICY_DIFFERENT  "Allowed in Different Decks"
#define DUPLICATE_POLICY_SAME       "Allowed in Same Deck"

QString AnkiSettings::duplicatePolicyToString(
    AnkiConfig::DuplicatePolicy policy)
{
    switch(policy)
    {
    case AnkiConfig::DuplicatePolicy::None:
        return DUPLICATE_POLICY_NONE;
    case AnkiConfig::DuplicatePolicy::DifferentDeck:
        return DUPLICATE_POLICY_DIFFERENT;
    case AnkiConfig::DuplicatePolicy::SameDeck:
        return DUPLICATE_POLICY_SAME;
    default:
        return DUPLICATE_POLICY_DIFFERENT;
    }
}

AnkiConfig::DuplicatePolicy AnkiSettings::stringToDuplicatePolicy(
    const QString &str)
{
    if (str == DUPLICATE_POLICY_NONE)
    {
        return AnkiConfig::DuplicatePolicy::None;
    }
    else if (str == DUPLICATE_POLICY_DIFFERENT)
    {
        return AnkiConfig::DuplicatePolicy::DifferentDeck;
    }
    else if (str == DUPLICATE_POLICY_SAME)
    {
        return AnkiConfig::DuplicatePolicy::SameDeck;
    }

    qDebug() << "Invalid duplicate policy string:" << str;
    return DEFAULT_DUPLICATE_POLICY;
}

#undef DUPLICATE_POLICY_NONE
#undef DUPLICATE_POLICY_DIFFERENT
#undef DUPLICATE_POLICY_SAME

#define SCREENSHOT_PNG  "PNG"
#define SCREENSHOT_JPG  "JPG"
#define SCREENSHOT_WEBP "WebP"

QString AnkiSettings::fileTypeToString(AnkiConfig::FileType type)
{
    switch(type)
    {
    case AnkiConfig::FileType::png:
        return SCREENSHOT_PNG;
    case AnkiConfig::FileType::jpg:
        return SCREENSHOT_JPG;
    case AnkiConfig::FileType::webp:
        return SCREENSHOT_WEBP;
    default:
        return SCREENSHOT_JPG;
    }
}

AnkiConfig::FileType AnkiSettings::stringToFileType(const QString &str)
{
    if (str == SCREENSHOT_JPG)
    {
        return AnkiConfig::FileType::jpg;
    }
    else if (str == SCREENSHOT_PNG)
    {
        return AnkiConfig::FileType::png;
    }
    else if (str == SCREENSHOT_WEBP)
    {
        return AnkiConfig::FileType::webp;
    }

    qDebug() << "Invalid file type string:" << str;
    return DEFAULT_SCREENSHOT;
}

#undef SCREENSHOT_PNG
#undef SCREENSHOT_JPG
#undef SCREENSHOT_WEBP

/* End Helpers */
