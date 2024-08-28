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

#include "ankiclient.h"

#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QMetaType>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTemporaryFile>
#include <QThreadPool>

#include "glossarybuilder.h"

#include "gui/widgets/subtitlelistwidget.h"
#include "player/playeradapter.h"
#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/utils.h"

/* Anki request fields */
#define ANKI_ACTION                     "action"
#define ANKI_RESULT                     "result"
#define ANKI_PARAMS                     "params"

/* Anki reply fields */
#define ANKI_ERROR                      "error"
#define ANKI_VERSION                    "version"

/* Anki actions */
#define ANKI_ACTION_DECK_NAMES          "deckNames"
#define ANKI_ACTION_MODEL_NAMES         "modelNames"
#define ANKI_ACTION_FIELD_NAMES         "modelFieldNames"
#define ANKI_ACTION_VERSION             "version"
#define ANKI_ACTION_CAN_ADD_NOTES       "canAddNotes"
#define ANKI_ACTION_ADD_NOTE            "addNote"

#define ANKI_ACTION_GUI_BROWSE          "guiBrowse"
#define ANKI_ACTION_STORE_MEDIA_FILE    "storeMediaFile"
#define ANKI_ACTION_MULTI               "multi"

/* Anki param fields */
#define ANKI_PARAM_MODEL_NAME           "modelName"
#define ANKI_PARAM_QUERY                "query"
#define ANKI_PARAM_ADD_NOTE             "note"
#define ANKI_PARAM_ACTIONS              "actions"

/* Anki note fields */
#define ANKI_CAN_ADD_NOTES_PARAM        "notes"
#define ANKI_NOTE_DECK                  "deckName"
#define ANKI_NOTE_MODEL                 "modelName"
#define ANKI_NOTE_FIELDS                "fields"
#define ANKI_NOTE_OPTIONS               "options"
#define ANKI_NOTE_TAGS                  "tags"
#define ANKI_NOTE_AUDIO                 "audio"
#define ANKI_NOTE_PICTURE               "picture"
#define ANKI_NOTE_DATA                  "data"
#define ANKI_NOTE_URL                   "url"
#define ANKI_NOTE_PATH                  "path"
#define ANKI_NOTE_FILENAME              "filename"
#define ANKI_NOTE_SKIPHASH              "skipHash"

/* Anki note option fields */
#define ANKI_NOTE_OPTIONS_ALLOW_DUP         "allowDuplicate"
#define ANKI_NOTE_OPTIONS_SCOPE             "duplicateScope"
#define ANKI_NOTE_OPTIONS_SCOPE_CHECK_DECK  "deck"

#define MIN_ANKICONNECT_VERSION         6
#define TIMEOUT                         5000
#define CONFIG_FILE                     "anki_connect.json"
#define FURIGANA_FORMAT_STRING          (QString("<ruby>%1<rt>%2</rt></ruby>"))
#define AUDIO_FILENAME_FORMAT_STRING    (QString("memento_%1_%2_%3.mp3"))

/* Config file fields */
#define CONFIG_ENABLED          "enabled"
#define CONFIG_PROFILES         "profiles"
#define CONFIG_SET_PROFILE      "setProfile"
#define CONFIG_NAME             "name"
#define CONFIG_HOST             "host"
#define CONFIG_PORT             "port"
#define CONFIG_DUPLICATE        "duplicate"
#define CONFIG_NEWLINE_REPLACER "newline-replace"
#define CONFIG_SCREENSHOT       "screenshot"
#define CONFIG_AUDIO_PAD_START  "audio-pad-start"
#define CONFIG_AUDIO_PAD_END    "audio-pad-end"
#define CONFIG_AUDIO_NORMALIZE  "audio-normalize"
#define CONFIG_AUDIO_DB         "audio-db"
#define CONFIG_TERM             "term"
#define CONFIG_KANJI            "kanji"
#define CONFIG_TAGS             "tags"
#define CONFIG_DECK             "deck"
#define CONFIG_MODEL            "model"
#define CONFIG_FIELDS           "fields"
#define CONFIG_EXCLUDE_GLOSSARY "ex-glos"

/* Begin Constructor/Destructors */

AnkiClient::AnkiClient(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    m_manager->setTransferTimeout(TIMEOUT);

    if (!readConfigFromFile(CONFIG_FILE) || m_currentConfig == nullptr)
    {
        setDefaultConfig();
    }

    qRegisterMetaType<QList<QPair<QString, QString>>>("FileMap");

    connect(
        this, &AnkiClient::requestAddMedia,
        this, &AnkiClient::addMedia
    );
    connect(
        this, &AnkiClient::sendIntRequest,
        this, &AnkiClient::receiveIntRequest
    );
    connect(
        this, &AnkiClient::sendBoolListRequest,
        this, &AnkiClient::receiveBoolListRequest
    );

    GlobalMediator::getGlobalMediator()->setAnkiClient(this);
}

AnkiClient::~AnkiClient()
{
    delete m_manager;
}

void AnkiClient::clearProfiles()
{
    m_configs.clear();
}

/* End Constructor/Destructors */
/* Begin Config File Methods */

bool AnkiClient::readConfigFromFile(const QString &filename)
{
    clearProfiles();

    QFile configFile(DirectoryUtils::getConfigDir() + filename);
    if (!configFile.exists())
    {
        return false;
    }

    if (!configFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "AnkiConnect config file exists, can't open it";
        return false;
    }

    /* Read the file into memory */
    QJsonDocument jsonDoc = QJsonDocument::fromJson(configFile.readAll());
    configFile.close();

    /* Error check the JSON */
    if (jsonDoc.isNull())
    {
        qDebug() << filename << "is not JSON";
        return false;
    }
    else if (!jsonDoc.isObject())
    {
        qDebug() << filename << "is not an object";
        return false;
    }

    QJsonObject jsonObj = jsonDoc.object();
    /* Error check the config */
    if (!jsonObj[CONFIG_ENABLED].isBool())
    {
        qDebug() << CONFIG_ENABLED << "is not a boolean";
        return false;
    }
    else if (!jsonObj[CONFIG_SET_PROFILE].isString())
    {
        qDebug() << CONFIG_SET_PROFILE << "is not a string";
        return false;
    }
    else if (!jsonObj[CONFIG_PROFILES].isArray())
    {
        qDebug() << CONFIG_PROFILES << "is not an array";
        return false;
    }
    QJsonArray profiles = jsonObj[CONFIG_PROFILES].toArray();
    for (int i = 0; i < profiles.size(); ++i)
    {
        if (!profiles[i].isObject())
        {
            qDebug() << CONFIG_PROFILES << "element is not an object";
            return false;
        }
        QJsonObject profile = profiles[i].toObject();

        /* Initialize default values for old configs */
        bool modified = false;
        if (profile[CONFIG_DUPLICATE].isNull())
        {
            modified = true;
            profile[CONFIG_DUPLICATE] = DEFAULT_DUPLICATE_POLICY;
        }
        if (profile[CONFIG_NEWLINE_REPLACER].isNull())
        {
            modified = true;
            profile[CONFIG_NEWLINE_REPLACER] = DEFAULT_NEWLINE_REPLACER;
        }
        if (profile[CONFIG_SCREENSHOT].isNull())
        {
            modified = true;
            profile[CONFIG_SCREENSHOT] = DEFAULT_SCREENSHOT;
        }
        if (profile[CONFIG_TERM].isNull())
        {
            modified = true;
            QJsonObject obj;
            obj[CONFIG_DECK]      = profile[CONFIG_DECK].toString("");
            obj[CONFIG_MODEL]     = profile[CONFIG_MODEL].toString("");
            obj[CONFIG_FIELDS]    = profile[CONFIG_FIELDS].toObject();
            profile[CONFIG_TERM]  = obj;
            profile[CONFIG_KANJI] = QJsonObject();
        }
        if (profile[CONFIG_AUDIO_PAD_START].isNull())
        {
            modified = true;
            profile[CONFIG_AUDIO_PAD_START] = DEFAULT_AUDIO_PAD_START;
        }
        if (profile[CONFIG_AUDIO_PAD_END].isNull())
        {
            modified = true;
            profile[CONFIG_AUDIO_PAD_END] = DEFAULT_AUDIO_PAD_END;
        }
        if (profile[CONFIG_AUDIO_NORMALIZE].isNull())
        {
            modified = true;
            profile[CONFIG_AUDIO_NORMALIZE] = DEFAULT_AUDIO_NORMALIZE;
        }
        if (profile[CONFIG_AUDIO_DB].isNull())
        {
            modified = true;
            profile[CONFIG_AUDIO_DB] = DEFAULT_AUDIO_DB;
        }
        if (profile[CONFIG_EXCLUDE_GLOSSARY].isNull())
        {
            modified = true;
            profile[CONFIG_EXCLUDE_GLOSSARY] = QJsonArray();
        }

        if (modified)
        {
            profiles[i] = profile;
        }

        /* Error check values */
        if (!profile[CONFIG_NAME].isString())
        {
            qDebug() << CONFIG_NAME << "is not a string";
            return false;
        }
        else if (!profile[CONFIG_HOST].isString())
        {
            qDebug() << CONFIG_HOST << "is not a string";
            return false;
        }
        else if (!profile[CONFIG_PORT].isString())
        {
            qDebug() << CONFIG_PORT << "is not a string";
            return false;
        }
        else if (!profile[CONFIG_DUPLICATE].isDouble())
        {
            qDebug() << CONFIG_DUPLICATE << "is not a double";
            return false;
        }
        else if (!profile[CONFIG_NEWLINE_REPLACER].isString())
        {
            qDebug() << CONFIG_NEWLINE_REPLACER << "is not a string";
            return false;
        }
        else if (!profile[CONFIG_SCREENSHOT].isDouble())
        {
            qDebug() << CONFIG_SCREENSHOT << "is not a double";
            return false;
        }
        else if (!profile[CONFIG_AUDIO_PAD_START].isDouble())
        {
            qDebug() << CONFIG_AUDIO_PAD_START << "is not a double";
            return false;
        }
        else if (!profile[CONFIG_AUDIO_PAD_END].isDouble())
        {
            qDebug() << CONFIG_AUDIO_PAD_END << "is not a double";
            return false;
        }
        else if (!profile[CONFIG_AUDIO_NORMALIZE].isBool())
        {
            qDebug() << CONFIG_AUDIO_NORMALIZE << "is not a bool";
        }
        else if (!profile[CONFIG_AUDIO_DB].isDouble())
        {
            qDebug() << CONFIG_AUDIO_DB << "is not a double";
        }
        else if (!profile[CONFIG_TAGS].isArray())
        {
            qDebug() << CONFIG_TAGS << "is not an array";
            return false;
        }
        else if (!profile[CONFIG_TERM].isObject())
        {
            qDebug() << CONFIG_TERM << "is not an object";
            return false;
        }
        else if (!profile[CONFIG_KANJI].isObject())
        {
            qDebug() << CONFIG_KANJI << "is not an object";
            return false;
        }
        else if (!profile[CONFIG_EXCLUDE_GLOSSARY].isArray())
        {
            qDebug() << CONFIG_EXCLUDE_GLOSSARY << "is not an array";
            return false;
        }
    }

    m_enabled = jsonObj[CONFIG_ENABLED].toBool();
    m_currentProfile = jsonObj[CONFIG_SET_PROFILE].toString();
    for (const QJsonValue &val : profiles)
    {
        QSharedPointer<AnkiConfig> config(new AnkiConfig);
        QJsonObject profile     = val.toObject();
        config->address         = profile[CONFIG_HOST].toString();
        config->port            = profile[CONFIG_PORT].toString();
        config->duplicatePolicy = (AnkiConfig::DuplicatePolicy)
            profile[CONFIG_DUPLICATE].toInt(DEFAULT_DUPLICATE_POLICY);
        config->newlineReplacer =
            profile[CONFIG_NEWLINE_REPLACER].toString(DEFAULT_NEWLINE_REPLACER);

        config->screenshotType  = (AnkiConfig::FileType)
            profile[CONFIG_SCREENSHOT].toInt(DEFAULT_SCREENSHOT);
        config->audioPadStart   = profile[CONFIG_AUDIO_PAD_START].toDouble();
        config->audioPadEnd     = profile[CONFIG_AUDIO_PAD_END].toDouble();
        config->audioNormalize  = profile[CONFIG_AUDIO_NORMALIZE].toBool();
        config->audioDb         = profile[CONFIG_AUDIO_DB].toDouble();
        config->tags            = profile[CONFIG_TAGS].toArray();
        for (const QJsonValue &val : profile[CONFIG_EXCLUDE_GLOSSARY].toArray())
        {
            if (val.isString())
            {
                config->excludeGloss.insert(val.toString());
            }
        }

        QJsonObject obj         = profile[CONFIG_TERM].toObject();
        config->termDeck        = obj[CONFIG_DECK].toString();
        config->termModel       = obj[CONFIG_MODEL].toString();
        config->termFields      = obj[CONFIG_FIELDS].toObject();

        obj                     = profile[CONFIG_KANJI].toObject();
        config->kanjiDeck       = obj[CONFIG_DECK].toString();
        config->kanjiModel      = obj[CONFIG_MODEL].toString();
        config->kanjiFields     = obj[CONFIG_FIELDS].toObject();

        QString profileName = profile[CONFIG_NAME].toString();
        m_configs.insert(profileName, config);
        if (profileName == m_currentProfile)
        {
            m_currentConfig = config;
        }
    }

    if (m_currentConfig)
    {
        setServer(m_currentConfig->address, m_currentConfig->port);
    }

    m_configExists = true;
    return true;
}

bool AnkiClient::writeConfigToFile(const QString &filename)
{
    QFile configFile(DirectoryUtils::getConfigDir() + filename);
    if (!configFile.open(QIODevice::ReadWrite |
                         QIODevice::Truncate |
                         QIODevice::Text))
    {
        qDebug() << "Could not open file" << filename;
        return false;
    }

    QJsonObject jsonObj;
    jsonObj[CONFIG_ENABLED] = m_enabled;
    jsonObj[CONFIG_SET_PROFILE] = m_currentProfile;
    QJsonArray configArr;
    QList<QString> profiles = m_configs.keys();
    for (const QString &profile : profiles)
    {
        QSharedPointer<const AnkiConfig> config = m_configs.value(profile);

        QJsonObject configObj;
        configObj[CONFIG_NAME]             = profile;
        configObj[CONFIG_HOST]             = config->address;
        configObj[CONFIG_PORT]             = config->port;
        configObj[CONFIG_DUPLICATE]        = config->duplicatePolicy;
        configObj[CONFIG_NEWLINE_REPLACER] = config->newlineReplacer;
        configObj[CONFIG_SCREENSHOT]       = config->screenshotType;
        configObj[CONFIG_AUDIO_PAD_START]  = config->audioPadStart;
        configObj[CONFIG_AUDIO_PAD_END]    = config->audioPadEnd;
        configObj[CONFIG_AUDIO_NORMALIZE]  = config->audioNormalize;
        configObj[CONFIG_AUDIO_DB]         = config->audioDb;
        configObj[CONFIG_TAGS]             = config->tags;

        QJsonArray excludeGloss;
        for (const QString &dict : config->excludeGloss)
        {
            excludeGloss << dict;
        }
        configObj[CONFIG_EXCLUDE_GLOSSARY] = excludeGloss;

        QJsonObject obj;
        obj[CONFIG_DECK]       = config->termDeck;
        obj[CONFIG_MODEL]      = config->termModel;
        obj[CONFIG_FIELDS]     = config->termFields;
        configObj[CONFIG_TERM] = obj;

        obj[CONFIG_DECK]        = config->kanjiDeck;
        obj[CONFIG_MODEL]       = config->kanjiModel;
        obj[CONFIG_FIELDS]      = config->kanjiFields;
        configObj[CONFIG_KANJI] = obj;

        configArr.append(configObj);
    }
    jsonObj[CONFIG_PROFILES] = configArr;
    QJsonDocument jsonDoc(jsonObj);

    configFile.write(jsonDoc.toJson());
    configFile.close();

    m_configExists = true;
    return true;
}

void AnkiClient::writeChanges()
{
    writeConfigToFile(CONFIG_FILE);
    Q_EMIT GlobalMediator::getGlobalMediator()->ankiSettingsChanged();
}

/* End Config File Methods */
/* Begin Config/Profile Methods */

QString AnkiClient::getProfile() const
{
    return m_currentProfile;
}

QStringList AnkiClient::getProfiles() const
{
    QStringList result;
    for (const QString &key : m_configs.keys())
    {
        result.append(key);
    }
    return result;
}

bool AnkiClient::setProfile(const QString &profile)
{
    QSharedPointer<const AnkiConfig> config = m_configs.value(profile);
    if (config)
    {
        m_currentConfig = config;
        m_currentProfile = profile;
        setServer(m_currentConfig->address, m_currentConfig->port);
        return true;
    }
    return false;
}

void AnkiClient::addProfile(const QString &profile, const AnkiConfig &config)
{
    m_configs.insert(
        profile, QSharedPointer<const AnkiConfig>(new AnkiConfig(config))
    );
}

QSharedPointer<const AnkiConfig> AnkiClient::getConfig(
    const QString &profile) const
{
    return m_configs.value(profile);
}

QSharedPointer<const AnkiConfig> AnkiClient::getConfig() const
{
    return m_currentConfig;
}

QHash<QString, QSharedPointer<AnkiConfig>> AnkiClient::getConfigs() const
{
    QHash<QString, QSharedPointer<AnkiConfig>> configs;
    for (auto it = m_configs.constKeyValueBegin();
         it != m_configs.constKeyValueEnd();
         ++it)
    {
        configs[it->first] =
            QSharedPointer<AnkiConfig>(new AnkiConfig(*it->second));
    }
    return configs;
}

void AnkiClient::setDefaultConfig()
{
    QSharedPointer<AnkiConfig> config =
        QSharedPointer<AnkiConfig>(new AnkiConfig);
    config->address         = DEFAULT_HOST;
    config->port            = DEFAULT_PORT;
    config->duplicatePolicy = DEFAULT_DUPLICATE_POLICY;
    config->screenshotType  = DEFAULT_SCREENSHOT;
    config->audioPadStart   = DEFAULT_AUDIO_PAD_START;
    config->audioPadEnd     = DEFAULT_AUDIO_PAD_END;
    config->audioNormalize  = DEFAULT_AUDIO_NORMALIZE;
    config->audioDb         = DEFAULT_AUDIO_DB;
    config->tags.append(DEFAULT_TAGS);
    config->excludeGloss.clear();

    m_configs[DEFAULT_PROFILE] = config;
    m_currentConfig = config;
    m_currentProfile = DEFAULT_PROFILE;

    setServer(config->address, config->port);
}

/* End Config/Profile Methods */
/* Begin Getter/Setters */

bool AnkiClient::configExists() const
{
    return m_configExists;
}

bool AnkiClient::isEnabled() const
{
    return m_enabled;
}

void AnkiClient::setEnabled(const bool value)
{
    m_enabled = value;
}

void AnkiClient::setServer(const QString &address, const QString &port)
{
    m_address = address;
    m_port = port;
}

/* End Getter/Setters */
/* Begin Commands */

AnkiReply *AnkiClient::testConnection()
{
    QNetworkReply *reply = makeRequest(ANKI_ACTION_VERSION);
    AnkiReply *ankiReply = new AnkiReply;
    connect(reply, &QNetworkReply::finished, this,
        [=] {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedBool(false, error);
            }
            else if (!replyObj[ANKI_RESULT].isDouble())
            {
                Q_EMIT ankiReply->finishedBool(
                    false, "AnkiConnect result is not a number"
                );
            }
            else if (replyObj[ANKI_RESULT].toInt() < MIN_ANKICONNECT_VERSION)
            {
                error = "AnkiConnect version %1 < %2";
                error = error.arg(
                        QString::number(replyObj[ANKI_RESULT].toInt()),
                        QString::number(MIN_ANKICONNECT_VERSION)
                    );
                Q_EMIT ankiReply->finishedBool(false, error);
            }
            else
            {
                Q_EMIT ankiReply->finishedBool(true, error);
            }
            ankiReply->deleteLater();
            reply->deleteLater();
        }
    );
    return ankiReply;
}

AnkiReply *AnkiClient::getDeckNames()
{
    return requestStringList(ANKI_ACTION_DECK_NAMES);
}

AnkiReply *AnkiClient::getModelNames()
{
    return requestStringList(ANKI_ACTION_MODEL_NAMES);
}

AnkiReply *AnkiClient::getFieldNames(const QString &model)
{
    QJsonObject params;
    params[ANKI_PARAM_MODEL_NAME] = model;
    return requestStringList(ANKI_ACTION_FIELD_NAMES, params);
}

AnkiReply *AnkiClient::notesAddable(QList<QSharedPointer<const Term>> terms)
{
    AnkiReply *ankiReply = new AnkiReply;

    QThreadPool::globalInstance()->start(
    [this, terms, ankiReply]
    {
        QJsonArray notes;
        for (QSharedPointer<const Term> term : terms)
        {
            /* Make sure to check for both reading as expression and not */
            Term termCopy(*term);
            termCopy.readingAsExpression = false;
            notes.append(createAnkiNoteObject(termCopy, false));
            if (!term->reading.isEmpty())
            {
                termCopy.readingAsExpression = true;
                notes.append(createAnkiNoteObject(termCopy, false));
            }
        }
        QJsonObject params;
        params[ANKI_CAN_ADD_NOTES_PARAM] = notes;

        AnkiReply *proxyReply = new AnkiReply;
        connect(proxyReply, &AnkiReply::finishedBoolList, this,
        [ankiReply, terms] (const QList<bool> &value, const QString &error)
        {
            QList<bool> result;
            for (int termsIdx = 0, valueIdx = 0;
                 termsIdx < terms.size() && valueIdx < value.size();
                 ++termsIdx)
            {
                if (terms[termsIdx]->reading.isEmpty())
                {
                    result << value[valueIdx] << false;
                    ++valueIdx;
                }
                else
                {
                    result << value[valueIdx] << value[valueIdx + 1];
                    valueIdx += 2;
                }
            }
            Q_EMIT ankiReply->finishedBoolList(result, error);
            ankiReply->deleteLater();
        }
        );

        Q_EMIT sendBoolListRequest(
            ANKI_ACTION_CAN_ADD_NOTES, params, proxyReply
        );
    }
    );

    return ankiReply;
}

AnkiReply *AnkiClient::notesAddable(QList<QSharedPointer<const Kanji>> kanji)
{
    AnkiReply *ankiReply = new AnkiReply;

    QThreadPool::globalInstance()->start(
        [this, kanji, ankiReply] {
            QJsonArray notes;
            for (QSharedPointer<const Kanji> kanji : kanji)
            {
                notes.append(createAnkiNoteObject(*kanji, false));
            }
            QJsonObject params;
            params[ANKI_CAN_ADD_NOTES_PARAM] = notes;
            Q_EMIT sendBoolListRequest(
                ANKI_ACTION_CAN_ADD_NOTES, params, ankiReply
            );
        }
    );

    return ankiReply;
}

AnkiReply *AnkiClient::addNote(const Term *term)
{
    AnkiReply *ankiReply = new AnkiReply;

    QThreadPool::globalInstance()->start(
        [=] {
            QJsonObject params;
            QList<QPair<QString, QString>> filemap;
            QJsonObject note = createAnkiNoteObject(*term, true, filemap);
            if (!filemap.isEmpty())
            {
                Q_EMIT requestAddMedia(filemap);
            }
            delete term;
            params[ANKI_PARAM_ADD_NOTE] = note;
            Q_EMIT sendIntRequest(ANKI_ACTION_ADD_NOTE, params, ankiReply);
        }
    );

    return ankiReply;
}

AnkiReply *AnkiClient::addNote(const Kanji *kanji)
{
    AnkiReply *ankiReply = new AnkiReply;

    QThreadPool::globalInstance()->start(
        [=] {
            QJsonObject params;
            QJsonObject note = createAnkiNoteObject(*kanji, true);
            delete kanji;
            params[ANKI_PARAM_ADD_NOTE] = note;
            Q_EMIT sendIntRequest(ANKI_ACTION_ADD_NOTE, params, ankiReply);
        }
    );

    return ankiReply;
}

AnkiReply *AnkiClient::openBrowse(const QString &deck, const QString &query)
{
    QJsonObject params;
    QString queryStr;
    switch (m_currentConfig->duplicatePolicy)
    {
    case AnkiConfig::DifferentDeck:
    case AnkiConfig::SameDeck:
        queryStr += "\"deck:" + deck + "\" ";
        __attribute__((fallthrough));
    case AnkiConfig::None:
    default:
        queryStr += '(';
        queryStr += query;
        queryStr += ')';
    }
    params[ANKI_PARAM_QUERY] = queryStr;
    QNetworkReply *reply = makeRequest(ANKI_ACTION_GUI_BROWSE, params);
    AnkiReply *ankiReply = new AnkiReply;
    connect(reply, &QNetworkReply::finished, this,
        [=] {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedIntList(QList<int>(), error);
            }
            else if (!replyObj[ANKI_RESULT].isArray())
            {
                Q_EMIT ankiReply->finishedIntList(
                    QList<int>(), "Result is not an array"
                );
            }
            else
            {
                QList<int> response;
                QJsonArray resultArray = replyObj[ANKI_RESULT].toArray();
                for (const QJsonValueRef &addable : resultArray)
                {
                    if (addable.isDouble())
                    {
                        response.append(addable.toInt());
                    }
                    else
                    {
                        Q_EMIT ankiReply->finishedIntList(
                            QList<int>(), "Response was not an array of bool"
                        );
                    }
                }
            Q_EMIT ankiReply->finishedIntList(response, error);
        }
        ankiReply->deleteLater();
        reply->deleteLater();
        }
    );
    return ankiReply;
}

AnkiReply *AnkiClient::addMedia(const QList<QPair<QString, QString>> &fileMap)
{
    QJsonObject params;
    QJsonArray actions;
    for (const QPair<QString, QString> &p : fileMap)
    {
        QJsonObject command;
        command[ANKI_ACTION] = ANKI_ACTION_STORE_MEDIA_FILE;
        QJsonObject fileParams;
        fileParams[ANKI_NOTE_DATA] = fileToBase64(p.first);
        fileParams[ANKI_NOTE_FILENAME] = p.second;
        command[ANKI_PARAMS] = fileParams;

        actions << command;
    }
    params[ANKI_PARAM_ACTIONS] = actions;

    QNetworkReply *reply = makeRequest(ANKI_ACTION_MULTI, params);
    AnkiReply *ankiReply = new AnkiReply;
    connect(reply, &QNetworkReply::finished, this,
        [=] {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedStringList(QStringList(), error);
            }
            else if (!replyObj[ANKI_RESULT].isArray())
            {
                Q_EMIT ankiReply->finishedStringList(
                    QStringList(), "Result is not an array"
                );
            }
            else
            {
                QStringList filenames;
                QJsonArray resultObjects = replyObj[ANKI_RESULT].toArray();
                for (const QJsonValueRef &result : resultObjects)
                {
                    QJsonObject obj;
                    if (!result.isObject())
                    {
                        Q_EMIT ankiReply->finishedStringList(
                            QStringList(), "Result is not an array of objects"
                        );
                        goto exit;
                    }

                    obj = result.toObject();
                    if (!obj[ANKI_ERROR].isNull())
                    {
                        Q_EMIT ankiReply->finishedStringList(
                            QStringList(), obj[ANKI_ERROR].toString()
                        );
                        goto exit;
                    }

                    if (!obj[ANKI_RESULT].isString())
                    {
                        Q_EMIT ankiReply->finishedStringList(
                            QStringList(), "A result is not a string"
                        );
                        goto exit;
                    }

                    filenames << obj[ANKI_RESULT].toString();
                }
                Q_EMIT ankiReply->finishedStringList(filenames, error);
            }
        exit:
            ankiReply->deleteLater();
            reply->deleteLater();
        }
    );
    return ankiReply;
}

/* End Commands */
/* Begin Network Helpers */

QNetworkReply *AnkiClient::makeRequest(const QString     &action,
                                       const QJsonObject &params)
{
    QNetworkRequest request;
    request.setUrl(QUrl("http://" + m_address + ":" + m_port));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject jsonMsg;
    jsonMsg[ANKI_ACTION] = action;
    jsonMsg[ANKI_VERSION] = MIN_ANKICONNECT_VERSION;
    if (!params.isEmpty())
    {
        jsonMsg[ANKI_PARAMS] = params;
    }
    QJsonDocument jsonDoc(jsonMsg);

    return m_manager->post(request, jsonDoc.toJson());
}

QJsonObject AnkiClient::processReply(QNetworkReply *reply, QString &error)
{
    switch (reply->error())
    {
    case QNetworkReply::NoError:
    {
        QJsonDocument replyDoc = QJsonDocument::fromJson(reply->readAll());
        if (replyDoc.isNull())
        {
            error = "Reply was not JSON";
            qDebug() << reply->readAll();
            return QJsonObject();
        }
        else if (!replyDoc.isObject())
        {
            error = "Reply was not an object";
            return QJsonObject();
        }

        QJsonObject replyObj = replyDoc.object();
        if (replyObj.length() != 2)
        {
            error = "Anki response has unexpected number of fields";
        }
        else if (!replyObj.contains(ANKI_ERROR))
        {
            error = "Anki response is missing error field";
        }
        else if (!replyObj.contains(ANKI_RESULT))
        {
            error = "Anki response is missing result field";
        }
        else if (!replyObj[ANKI_ERROR].isNull())
        {
            error = replyObj[ANKI_ERROR].toString();
        }
        else
        {
            return replyObj;
        }
        break;
    }
    default:
        error = reply->errorString();
    }

    return QJsonObject();
}

void AnkiClient::receiveIntRequest(const QString     &action,
                                   const QJsonObject &params,
                                   AnkiReply         *ankiReply)
{
    QNetworkReply *reply = makeRequest(action, params);
    connect(reply, &QNetworkReply::finished, this,
        [=] {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedInt(0, error);
            }
            else if (!replyObj[ANKI_RESULT].isDouble())
            {
                Q_EMIT ankiReply->finishedInt(
                    0, "AnkiConnect result is not a double"
                );
            }
            else
            {
                Q_EMIT ankiReply->finishedInt(
                    replyObj[ANKI_RESULT].toInt(), error
                );
            }
            ankiReply->deleteLater();
            reply->deleteLater();
        }
    );
}

void AnkiClient::receiveBoolListRequest(const QString     &,
                                        const QJsonObject &params,
                                        AnkiReply         *ankiReply)
{
    QNetworkReply *reply = makeRequest(ANKI_ACTION_CAN_ADD_NOTES, params);
    connect(reply, &QNetworkReply::finished, this,
        [=] {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedBoolList(QList<bool>(), error);
            }
            else if (!replyObj[ANKI_RESULT].isArray())
            {
                Q_EMIT ankiReply->finishedBoolList(
                    QList<bool>(), "Result is not an array"
                );
            }
            else
            {
                QList<bool> response;
                QJsonArray resultArray = replyObj[ANKI_RESULT].toArray();
                for (const QJsonValueRef &addable : resultArray)
                {
                    if (addable.isBool())
                    {
                        response.append(addable.toBool());
                    }
                    else
                    {
                        Q_EMIT ankiReply->finishedBoolList(
                            QList<bool>(), "Response was not an array of bool"
                        );
                    }
                }
                Q_EMIT ankiReply->finishedBoolList(response, error);
            }
            ankiReply->deleteLater();
            reply->deleteLater();
        }
    );
}

AnkiReply *AnkiClient::requestStringList(const QString     &action,
                                         const QJsonObject &params)
{
    QNetworkReply *reply = makeRequest(action, params);
    AnkiReply *ankiReply = new AnkiReply;
    connect(reply, &QNetworkReply::finished, this,
        [=] {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedStringList(QStringList(), error);
            }
            else if (!replyObj[ANKI_RESULT].isArray())
            {
                Q_EMIT ankiReply->finishedStringList(
                    QStringList(), "Result is not an array"
                );
            }
            else
            {
                QStringList decks;
                QJsonArray deckNames = replyObj[ANKI_RESULT].toArray();
                for (const QJsonValueRef &name : deckNames)
                    decks.append(name.toString());
                Q_EMIT ankiReply->finishedStringList(decks, error);
            }
            ankiReply->deleteLater();
            reply->deleteLater();
        }
    );
    return ankiReply;
}

/* End Network Helpers */
/* Begin Note Helpers */

QJsonObject AnkiClient::createAnkiNoteObject(const Term &term, bool media)
{
    QList<QPair<QString, QString>> filemap;
    return createAnkiNoteObject(term, media, filemap);
}

QJsonObject AnkiClient::createAnkiNoteObject(
    const Term &term,
    bool media,
    QList<QPair<QString, QString>> &filemap)
{
    /* Build common parts of a note */
    QJsonObject note;
    QJsonObject fieldsObj;
    buildCommonNote(
        m_currentConfig->termFields, term, media, note, fieldsObj
    );

    /* Set Term and Model */
    note[ANKI_NOTE_DECK] = m_currentConfig->termDeck;
    note[ANKI_NOTE_MODEL] = m_currentConfig->termModel;

    /* Process Fields */
    QString audioFile = AUDIO_FILENAME_FORMAT_STRING
        .arg(term.audioSrcName)
        .arg(term.reading)
        .arg(term.expression)
        .replace(' ', '_');

    QString expression = term.readingAsExpression
        ? term.reading : term.expression;
    QString reading;
    QString furigana;
    QString furiganaPlain;
    if (term.reading.isEmpty() || term.readingAsExpression)
    {
        reading       = expression;
        furigana      = expression;
        furiganaPlain = expression;
    }
    else
    {
        reading = term.reading;
        furigana = FURIGANA_FORMAT_STRING.arg(expression).arg(reading);
        furiganaPlain = expression + "[" + reading + "]";
    }

    QString glossary, glossaryBrief, glossaryCompact;
    filemap = buildGlossary(
        term.definitions, glossary, glossaryBrief, glossaryCompact
    );

    QString pitch;
    QString pitchGraph;
    QString pitchPosition;
    buildPitchInfo(term.pitches, pitch, pitchGraph, pitchPosition);

    QString tags, tagsBrief;
    buildTags(term.tags, tags, tagsBrief);


    /* Find and replace markers with data */
    QStringList fields = m_currentConfig->termFields.keys();
    QJsonArray fieldsWithAudio;
    for (const QString &field : fields)
    {
        QString value = fieldsObj[field].toString();

        if (value.contains(REPLACE_AUDIO))
        {
            fieldsWithAudio.append(field);
        }
        value.replace(REPLACE_AUDIO, "");

        value.replace(REPLACE_EXPRESSION,       expression);
        value.replace(REPLACE_FURIGANA,         furigana);
        value.replace(REPLACE_FURIGANA_PLAIN,   furiganaPlain);
        value.replace(REPLACE_GLOSSARY,         glossary);
        value.replace(REPLACE_GLOSSARY_BRIEF,   glossaryBrief);
        value.replace(REPLACE_GLOSSARY_COMPACT, glossaryCompact);
        value.replace(REPLACE_PITCH,            pitch);
        value.replace(REPLACE_PITCH_GRAPHS,     pitchGraph);
        value.replace(REPLACE_PITCH_POSITIONS,  pitchPosition);
        value.replace(REPLACE_READING,          reading);
        value.replace(REPLACE_TAGS,             tags);
        value.replace(REPLACE_TAGS_BRIEF,       tagsBrief);

        fieldsObj[field] = value;
    }
    note[ANKI_NOTE_FIELDS] = fieldsObj;

    /* Add {audio} marker to the note */
    if (media)
    {
        QJsonArray audio = note[ANKI_NOTE_AUDIO].toArray();
        if (!fieldsWithAudio.isEmpty())
        {
            QJsonObject audObj;

            audObj[ANKI_NOTE_URL]      = QString(term.audioURL)
                .replace(REPLACE_EXPRESSION, term.expression)
                .replace(REPLACE_READING, reading);
            audObj[ANKI_NOTE_FILENAME] = audioFile;
            audObj[ANKI_NOTE_FIELDS]   = fieldsWithAudio;
            audObj[ANKI_NOTE_SKIPHASH] = term.audioSkipHash;
            audio.append(audObj);
        }

        if (!audio.isEmpty())
        {
            note[ANKI_NOTE_AUDIO] = audio;
        }
    }

    return note;
}

QJsonObject AnkiClient::createAnkiNoteObject(const Kanji &kanji, bool media)
{
    /* Build common parts of a note */
    QJsonObject note;
    QJsonObject fieldsObj;
    buildCommonNote(
        m_currentConfig->kanjiFields, kanji, media, note, fieldsObj
    );

    /* Set Term and Model */
    note[ANKI_NOTE_DECK] = m_currentConfig->kanjiDeck;
    note[ANKI_NOTE_MODEL] = m_currentConfig->kanjiModel;

    /* Build Note Fields */
    QString glossary;
    QString tags, tagsBrief;
    QString strokeCount;
    QString onyomi;
    QString kunyomi;
    if (!kanji.definitions.isEmpty())
    {
        glossary += "<ol>";
        for (const KanjiDefinition &def : kanji.definitions)
        {
            /* Build glossary */
            if (!def.glossary.isEmpty())
            {
                glossary += "<li><i>(";
                for (const Tag &tag : def.tags)
                {
                    glossary += tag.name;
                    glossary += ", ";
                }
                glossary += def.dictionary;
                glossary += ")</i></li>";
                glossary += "<ol>";
                for (const QString &glos : def.glossary)
                {
                    glossary += "<li>";
                    glossary += glos;
                    glossary += "</li>";
                }
                glossary += "</ol>";
            }

            /* Build tags */
            buildTags(def.tags, tags, tagsBrief);

            /* Find the stroke count if we haven't already */
            if (strokeCount.isEmpty())
            {
                for (auto &pair : def.stats)
                {
                    if (pair.first.notes == "Stroke count")
                    {
                        strokeCount = pair.second;
                    }
                }
            }

            /* Build onyomi */
            for (const QString &str : def.onyomi)
            {
                onyomi += str;
                onyomi += ", ";
            }

            /* Build kunyomi */
            for (const QString &str : def.kunyomi)
            {
                kunyomi += str;
                kunyomi += ", ";
            }
        }
        glossary += "</ol>";
        onyomi.chop(2);
        kunyomi.chop(2);

        /* Clear out empty results */
        if (glossary == "<ol></ol>")
            glossary.clear();
    }

    /* Replace Markers */
    QJsonArray fieldsWithAudio;
    QStringList fields = fieldsObj.keys();
    for (const QString &field : fields)
    {
        QString value = fieldsObj[field].toString();

        value.replace(REPLACE_CHARACTER,    kanji.character);
        value.replace(REPLACE_KUNYOMI,      kunyomi);
        value.replace(REPLACE_ONYOMI,       onyomi);
        value.replace(REPLACE_STROKE_COUNT, strokeCount);
        value.replace(REPLACE_GLOSSARY,     glossary);
        value.replace(REPLACE_TAGS,         tags);
        value.replace(REPLACE_TAGS_BRIEF,   tagsBrief);

        fieldsObj[field] = value;
    }

    /* Add Fields */
    note[ANKI_NOTE_FIELDS] = fieldsObj;

    return note;
}

void AnkiClient::buildCommonNote(
    const QJsonObject &configFields,
    const CommonExpFields &exp,
    const bool media,
    QJsonObject &note,
    QJsonObject &fieldObj)
{
    /* Set Duplicate Policy */
    switch (m_currentConfig->duplicatePolicy)
    {
    case AnkiConfig::DuplicatePolicy::None:
        note[ANKI_NOTE_OPTIONS] = QJsonObject{
            {ANKI_NOTE_OPTIONS_ALLOW_DUP, false},
        };
        break;
    case AnkiConfig::DuplicatePolicy::DifferentDeck:
        note[ANKI_NOTE_OPTIONS] = QJsonObject{
            {ANKI_NOTE_OPTIONS_SCOPE, ANKI_NOTE_OPTIONS_SCOPE_CHECK_DECK}
        };
        break;
    case AnkiConfig::DuplicatePolicy::SameDeck:
        note[ANKI_NOTE_OPTIONS] = QJsonObject{
            {ANKI_NOTE_OPTIONS_ALLOW_DUP, true}
        };
        break;
    }

    /* Add Card Tags */
    note[ANKI_NOTE_TAGS] = m_currentConfig->tags;

    /* Find and replace markers with processed data */
    QString clipboard =
        QString(exp.clipboard).replace('\n', m_currentConfig->newlineReplacer);
    QString clozeBody =
        QString(exp.clozeBody).replace('\n', m_currentConfig->newlineReplacer);
    QString clozePrefix =
        QString(exp.clozePrefix).replace('\n', m_currentConfig->newlineReplacer);
    QString clozeSuffix =
        QString(exp.clozeSuffix).replace('\n', m_currentConfig->newlineReplacer);
    QString sentence =
        QString(exp.sentence).replace('\n', m_currentConfig->newlineReplacer);
    QString sentence2 =
        QString(exp.sentence2).replace('\n', m_currentConfig->newlineReplacer);
    QString context =
        QString(exp.context).replace('\n', m_currentConfig->newlineReplacer);
    QString context2 =
        QString(exp.context2).replace('\n', m_currentConfig->newlineReplacer);

    QString frequencies = buildFrequencies(exp.frequencies);
    const int frequencyHarmonic = getFrequencyHarmonic(exp.frequencies);
    const int frequencyAverage = getFrequencyAverage(exp.frequencies);

    QJsonArray fieldsWithAudioMedia;
    QJsonArray fieldsWithAudioContext;
    QJsonArray fieldsWithScreenshot;
    QJsonArray fieldWithScreenshotVideo;
    QStringList fields = configFields.keys();
    for (const QString &field : fields)
    {
        QString value = configFields[field].toString();

        if (value.contains(REPLACE_AUDIO_MEDIA))
        {
            fieldsWithAudioMedia.append(field);
        }
        value.replace(REPLACE_AUDIO_MEDIA, "");

        if (value.contains(REPLACE_AUDIO_CONTEXT))
        {
            fieldsWithAudioContext.append(field);
        }
        value.replace(REPLACE_AUDIO_CONTEXT, "");

        if (value.contains(REPLACE_SCREENSHOT))
        {
            fieldsWithScreenshot.append(field);
        }
        value.replace(REPLACE_SCREENSHOT, "");

        if (value.contains(REPLACE_SCREENSHOT_VIDEO))
        {
            fieldWithScreenshotVideo.append(field);
        }
        value.replace(REPLACE_SCREENSHOT_VIDEO, "");

        value.replace(REPLACE_TITLE,        exp.title);
        value.replace(REPLACE_CLIPBOARD,    clipboard);
        value.replace(REPLACE_CLOZE_BODY,   clozeBody);
        value.replace(REPLACE_CLOZE_PREFIX, clozePrefix);
        value.replace(REPLACE_CLOZE_SUFFIX, clozeSuffix);
        value.replace(REPLACE_FREQUENCIES,  frequencies);

        /* If the term never occurs in the corpus of any loaded frequency
         * dictionary, assume it is a very rare word.
         * (The higher the ranking, the rarer the term) */
        constexpr int default_freq_rank = 9999999;
        constexpr int default_freq_occurrence = 0;

        value.replace(REPLACE_FREQ_HARMONIC_RANK, 
                positiveIntToQString(frequencyHarmonic, default_freq_rank));

        value.replace(REPLACE_FREQ_HARMONIC_OCCU, 
                positiveIntToQString(frequencyHarmonic, default_freq_occurrence));

        value.replace(REPLACE_FREQ_AVERAGE_RANK,  
                positiveIntToQString(frequencyAverage, default_freq_rank));

        value.replace(REPLACE_FREQ_AVERAGE_OCCU,  
                positiveIntToQString(frequencyAverage, default_freq_occurrence));

        value.replace(REPLACE_SENTENCE,     sentence);
        value.replace(REPLACE_SENTENCE_SEC, sentence2);
        value.replace(REPLACE_CONTEXT,      context);
        value.replace(REPLACE_CONTEXT_SEC,  context2);

        fieldObj[field] = value;
    }

    /* Add the requested Media Sections */
    if (media)
    {
        QJsonArray audio;
        if (!fieldsWithAudioMedia.isEmpty())
        {
            QJsonObject audObj;
            PlayerAdapter *player =
                GlobalMediator::getGlobalMediator()->getPlayerAdapter();

            double startTime = exp.startTime - m_currentConfig->audioPadStart;
            double endTime = exp.endTime + m_currentConfig->audioPadEnd;

            QString path;
            if (startTime >= 0 && endTime >= 0 && startTime < endTime)
            {
                path = player->tempAudioClip(
                    startTime,
                    endTime,
                    m_currentConfig->audioNormalize,
                    m_currentConfig->audioDb
                );
            }
            if (!path.isEmpty()) {
                audObj[ANKI_NOTE_DATA] = fileToBase64(path);
                QString filename = FileUtils::calculateMd5(path) + ".aac";
                audObj[ANKI_NOTE_FILENAME] = filename;
                audObj[ANKI_NOTE_FIELDS] = fieldsWithAudioMedia;
                audio.append(audObj);

                QFile(path).remove();
            }
        }

        if (!fieldsWithAudioContext.isEmpty())
        {
            QJsonObject audObj;
            PlayerAdapter *player =
                GlobalMediator::getGlobalMediator()->getPlayerAdapter();

            double startTime =
                exp.startTimeContext - m_currentConfig->audioPadStart;
            double endTime = exp.endTimeContext + m_currentConfig->audioPadEnd;

            QString path;
            if (startTime >= 0 && endTime >= 0 && startTime < endTime)
            {
                path = player->tempAudioClip(
                    startTime,
                    endTime,
                    m_currentConfig->audioNormalize,
                    m_currentConfig->audioDb
                );
            }
            if (!path.isEmpty()) {
                audObj[ANKI_NOTE_DATA] = fileToBase64(path);
                QString filename = FileUtils::calculateMd5(path) + ".aac";
                audObj[ANKI_NOTE_FILENAME] = filename;
                audObj[ANKI_NOTE_FIELDS] = fieldsWithAudioContext;
                audio.append(audObj);

                QFile(path).remove();
            }
        }

        if (!audio.isEmpty())
        {
            note[ANKI_NOTE_AUDIO] = audio;
        }

        QString imageExt;
        switch (m_currentConfig->screenshotType)
        {
        case AnkiConfig::FileType::jpg:
            imageExt = ".jpg";
            break;
        case AnkiConfig::FileType::png:
            imageExt = ".png";
            break;
        case AnkiConfig::FileType::webp:
            imageExt = ".webp";
            break;
        default:
            imageExt = ".jpg";
        }

        QJsonArray images;
        if (!fieldsWithScreenshot.isEmpty())
        {
            QJsonObject image;

            PlayerAdapter *player =
                GlobalMediator::getGlobalMediator()->getPlayerAdapter();
            const bool visibility = player->getSubVisibility();
            player->setSubVisiblity(true);
            QString path = player->tempScreenshot(true, imageExt);
            image[ANKI_NOTE_DATA] = fileToBase64(path);
            player->setSubVisiblity(visibility);

            QString filename = FileUtils::calculateMd5(path) + imageExt;
            image[ANKI_NOTE_FILENAME] = filename;

            image[ANKI_NOTE_FIELDS] = fieldsWithScreenshot;

            if (filename != imageExt)
            {
                images.append(image);
            }
            QFile(path).remove();
        }

        if (!fieldWithScreenshotVideo.isEmpty())
        {
            QJsonObject image;
            QString path = GlobalMediator::getGlobalMediator()
                ->getPlayerAdapter()->tempScreenshot(false, imageExt);
            image[ANKI_NOTE_DATA] = fileToBase64(path);

            QString filename = FileUtils::calculateMd5(path) + imageExt;
            image[ANKI_NOTE_FILENAME] = filename;

            image[ANKI_NOTE_FIELDS] = fieldWithScreenshotVideo;

            if (filename != imageExt)
            {
                images.append(image);
            }
            QFile(path).remove();
        }

        if (!images.isEmpty())
        {
            note[ANKI_NOTE_PICTURE] = images;
        }
    }
}

#define HL_STYLE        (QString("border-top: solid; border-right: solid;"))
#define H_STYLE         (QString("border-top: solid;"))

#define PITCH_FORMAT    (QString("<span style=\"%1\">%2</span>"))

void AnkiClient::buildPitchInfo(const QList<Pitch> &pitches,
                                QString            &pitch,
                                QString            &pitchGraph,
                                QString            &pitchPosition)
{
    if (pitches.isEmpty())
    {
        return;
    }

    pitch         += "<span class=\"memento-pitch\">";
    pitchGraph    += "<span>";
    pitchPosition += "<span>";

    const bool multipleDicts = pitches.size() > 1;

    if (multipleDicts)
    {
        pitch         += "<ul>";
        pitchGraph    += "<ul>";
        pitchPosition += "<ul>";
    }

    for (const Pitch &p : pitches)
    {
        const bool multiplePitches = p.position.size() > 1;

        /* Header */
        if (multipleDicts)
        {
            pitch         += "<li><i>" + p.dictionary + "</i>";
            pitchGraph    += "<li><i>" + p.dictionary + "</i>";
            pitchPosition += "<li><i>" + p.dictionary + "</i>";
        }
        if(multiplePitches)
        {
            pitch         += "<ol>";
            pitchGraph    += "<ol>";
            pitchPosition += "<ol>";
        }

        /* Body */
        for (const uint8_t pos : p.position)
        {
            if (p.mora.isEmpty())
            {
                continue;
            }

            if (multiplePitches)
            {
                pitch         += "<li style=\"padding: 2px 0px 2px 0px;\">";
                pitchGraph    += "<li>";
                pitchPosition += "<li>";
            }

            /* Build {pitch} marker */
            switch (pos)
            {
            case 0:
                pitch += p.mora.first();
                if (p.mora.size() > 1)
                {
                    pitch += PITCH_FORMAT.arg(H_STYLE)
                                         .arg(p.mora.join("")
                                         .remove(0, p.mora.first().size()));
                }
                break;
            case 1:
                pitch += PITCH_FORMAT.arg(HL_STYLE).arg(p.mora.first());
                if (p.mora.size() > 1)
                {
                    pitch += p.mora.join("").remove(0, p.mora.first().size());
                }
                break;
            default:
            {
                QString text = p.mora.first();
                pitch += text;

                text.clear();
                for (size_t i = 1; i < pos; ++i)
                {
                    text += p.mora[i];
                }
                if (!text.isEmpty())
                {
                    pitch += PITCH_FORMAT.arg(HL_STYLE).arg(text);
                }

                text.clear();
                for (int i = pos; i < p.mora.size(); ++i)
                {
                    text += p.mora[i];
                }
                if (!text.isEmpty())
                {
                    pitch += text;
                }
            }
            }

            /* Build {pitch-graph}s */
            pitchGraph = GraphicUtils::generatePitchGraph(
                p.mora.size(), pos, "rgba(0,0,0,0)", "currentColor"
            );

            /* Build {pitch-posititon}s */
            pitchPosition += "[" + QString::number(pos) + "]";

            if (multiplePitches)
            {
                pitch         += "</li>";
                pitchGraph    += "</li>";
                pitchPosition += "</li>";
            }
        }

        /* Trailer */
        if(multiplePitches)
        {
            pitch         += "</ol>";
            pitchGraph    += "</ol>";
            pitchPosition += "</ol>";
        }
        if (multipleDicts)
        {
            pitch         += "</li>";
            pitchGraph    += "</li>";
            pitchPosition += "</li>";
        }
    }

    if (multipleDicts)
    {
        pitch         += "</ul>";
        pitchGraph    += "</ul>";
        pitchPosition += "</ul>";
    }

    pitch         += "</span>";
    pitchGraph    += "</span>";
    pitchPosition += "</span>";
}

#undef HL_STYLE
#undef H_STYLE

#undef PITCH_FORMAT

QList<QPair<QString, QString>> AnkiClient::buildGlossary(
    const QList<TermDefinition> &definitions,
    QString &glossary,
    QString &glossaryBrief,
    QString &glossaryCompact)
{
    QString basepath = DirectoryUtils::getDictionaryResourceDir() + SLASH;

    glossary += "<div style=\"text-align: left;\"><ol>";
    glossaryBrief += "<div style=\"text-align: left;\"><ol>";
    glossaryCompact += "<div style=\"text-align: left;\"><ol>";
    QList<QPair<QString, QString>> filemap;

    for (const TermDefinition &def : definitions)
    {
        glossary += "<li>";
        glossaryCompact += "<li>";

        glossary += "<i>(";
        glossaryCompact += "<i>(";
        for (const Tag &tag : def.tags)
        {
            glossary += tag.name + ", ";
            glossaryCompact += tag.name + ", ";
        }
        glossary += def.dictionary;
        glossary += ")</i>";
        glossaryCompact += def.dictionary;
        glossaryCompact += ")</i>";

        glossary += "<ul>";
        glossaryCompact += ' ';

        QStringList items = GlossaryBuilder::buildGlossary(
            def.glossary, basepath + def.dictionary, filemap
        );
        for (const QString &item : items)
        {
            glossary += "<li>";
            glossary += item;
            glossary += "</li>";

            glossaryBrief += "<li>";
            glossaryBrief += item;
            glossaryBrief += "</li>";
        }
        glossaryCompact += items.join(" | ");

        glossary += "</ul></li>";
        glossaryCompact += "</li>";
    }

    glossary += "</ol></div>";
    glossaryBrief += "</ol></div>";
    glossaryCompact += "</ol></div>";
    return filemap;
}

QString AnkiClient::buildFrequencies(const QList<Frequency> &frequencies)
{
    if (frequencies.isEmpty())
        return "";

    QString freqStr;
    freqStr += "<ul>";
    for (const Frequency &freq : frequencies)
    {
        freqStr += "<li>";
        freqStr += freq.dictionary;
        freqStr += freq.dictionary.endsWith(':') ? " " : ": ";
        freqStr += freq.freq;
        freqStr += "</li>";
    }
    freqStr += "</ul>";

    if (freqStr == "<ul></ul>")
        freqStr.clear();

    return freqStr;
}

std::vector<int> AnkiClient::getFrequencyNumbers(
        const QList<Frequency> &frequencies)
{
    QString previousDictionary;
    std::vector<int> frequencyNumbers;

    for (const Frequency &frequencyEntry : frequencies)
    {
        if (frequencyEntry.dictionary == previousDictionary 
                || frequencyEntry.freq.isNull())
        {
            continue;
        }
        previousDictionary = frequencyEntry.dictionary;

        /* This regular expression only catches numbers in base 10 and 
         * would not catch negative or decimal numbers because we make
         * the assumption that these special types of numbers will not
         * appear in frequency dictionaries. */
        QRegularExpression numberPattern("\\d+");
        QRegularExpressionMatch match = numberPattern.match(frequencyEntry.freq);

        if (match.hasMatch()) 
        {
            /* Only save the first number to avoid counting secondary frequency 
             * information (e.g. frequency for the full kana orthography) in the
             * aggregate measures to align with Yomitan's behavior. */
            frequencyNumbers.push_back(match.captured(0).toInt());
            continue;
        }
    }

    return frequencyNumbers;
}

QString AnkiClient::positiveIntToQString(const int value, const int defaultValue)
{
    return (value < 0) ? QString::number(defaultValue) : QString::number(value);
}

int AnkiClient::getFrequencyHarmonic(const QList<Frequency> &frequencies) 
{
    const std::vector<int> frequencyNumbers = getFrequencyNumbers(frequencies);

    if (frequencyNumbers.empty()) 
    {
        return -1;
    }

    double total = 0.0;
    for (int frequencyNum : frequencyNumbers) 
    {
        if (frequencyNum != 0) 
        {
            total += 1.0 / frequencyNum;
        }
    }

    return std::floor(frequencyNumbers.size() / total);
}

int AnkiClient::getFrequencyAverage(const QList<Frequency> &frequencies)
{
    const std::vector<int> frequencyNumbers = getFrequencyNumbers(frequencies);

    if (frequencyNumbers.empty()) 
    {
        return -1;
    }

    /* Sum the elements in the vector */
    double total = std::accumulate(frequencyNumbers.begin(), 
            frequencyNumbers.end(), 0);

    return std::floor(total / frequencyNumbers.size());
}

void AnkiClient::buildTags(
    const QList<Tag> &tags,
    QString &tagStr,
    QString &tagBriefStr)
{
    if (tags.isEmpty())
    {
        tagStr.clear();
        tagBriefStr.clear();
        return;
    }

    tagStr      = "<ul>";
    tagBriefStr = "<ul>";
    for (const Tag &tag : tags)
    {
        tagStr      += "<li>";
        tagStr      += tag.name;
        tagBriefStr += "<li>";
        tagBriefStr += tag.name;
        if (!tag.notes.isEmpty())
        {
            tagStr += ": ";
            tagStr += tag.notes;
        }
        tagStr      += "</li>";
        tagBriefStr += "</li>";
    }
    tagStr      += "</ul>";
    tagBriefStr += "</ul>";
}

QString AnkiClient::fileToBase64(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        return {};
    }
    return file.readAll().toBase64();
}

/* End Note Helpers */
