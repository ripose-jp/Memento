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
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTemporaryFile>
#include <QThreadPool>

#include "../gui/playeradapter.h"
#include "../gui/widgets/subtitlelistwidget.h"
#include "../util/constants.h"
#include "../util/globalmediator.h"
#include "../util/utils.h"

/* Anki request fields */
#define ANKI_ACTION                     "action"
#define ANKI_RESULT                     "result"
#define ANKI_PARAMS                     "params"

/* Anki reply fields */
#define ANKI_ERROR                      "error"
#define ANKI_VERSION                    "version"

/* Anki actions */
#define ANKI_DECK_NAMES                 "deckNames"
#define ANKI_MODEL_NAMES                "modelNames"
#define ANKI_FIELD_NAMES                "modelFieldNames"
#define ANKI_ACTION_VERSION             "version"
#define ANKI_CAN_ADD_NOTES              "canAddNotes"
#define ANKI_ADD_NOTE                   "addNote"
#define ANKI_ADD_NOTE_PARAM             "note"
#define ANKI_GUI_BROWSE                 "guiBrowse"

/* Anki param fields */
#define ANKI_PARAM_MODEL_NAME           "modelName"
#define ANKI_PARAM_QUERY                "query"

/* Anki note fields */
#define ANKI_CAN_ADD_NOTES_PARAM        "notes"
#define ANKI_NOTE_DECK                  "deckName"
#define ANKI_NOTE_MODEL                 "modelName"
#define ANKI_NOTE_FIELDS                "fields"
#define ANKI_NOTE_OPTIONS               "options"
#define ANKI_NOTE_TAGS                  "tags"
#define ANKI_NOTE_AUDIO                 "audio"
#define ANKI_NOTE_PICTURE               "picture"
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
#define AUDIO_FILENAME_FORMAT_STRING    (QString("memento_%1_%2.mp3"))

/* Config file fields */
#define CONFIG_ENABLED          "enabled"
#define CONFIG_PROFILES         "profiles"
#define CONFIG_SET_PROFILE      "setProfile"
#define CONFIG_NAME             "name"
#define CONFIG_HOST             "host"
#define CONFIG_PORT             "port"
#define CONFIG_DUPLICATE        "duplicate"
#define CONFIG_SCREENSHOT       "screenshot"
#define CONFIG_AUDIO_NAME       "audio-name"
#define CONFIG_AUDIO_URL        "audio-url"
#define CONFIG_AUDIO_HASH       "audio-skiphash"
#define CONFIG_AUDIO_PAD_START  "audio-pad-start"
#define CONFIG_AUDIO_PAD_END    "audio-pad-end"
#define CONFIG_TERM             "term"
#define CONFIG_KANJI            "kanji"
#define CONFIG_TAGS             "tags"
#define CONFIG_DECK             "deck"
#define CONFIG_MODEL            "model"
#define CONFIG_FIELDS           "fields"

/* Begin Constructor/Destructors */

AnkiClient::AnkiClient(QObject *parent)
    : QObject(parent),
      m_configs(new QHash<QString, const AnkiConfig *>),
      m_currentConfig(0),
      m_enabled(false)
{
    m_manager = new QNetworkAccessManager(this);
    m_manager->setTransferTimeout(TIMEOUT);

    if (!readConfigFromFile(CONFIG_FILE) || m_currentConfig == nullptr)
    {
        setDefaultConfig();
    }

    connect(
        this, &AnkiClient::sendIntRequest,
        this, &AnkiClient::receiveIntRequest
    );

    GlobalMediator::getGlobalMediator()->setAnkiClient(this);
}

AnkiClient::~AnkiClient()
{
    for (const QString &path : m_tempFiles)
    {
        QFile(path).remove();
    }
    clearProfiles();
    delete m_configs;
    delete m_manager;
}

void AnkiClient::clearProfiles()
{
    for (const AnkiConfig *config : *m_configs)
        delete config;
    m_configs->clear();
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
    for (size_t i = 0; i < profiles.size(); ++i)
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
            profile[CONFIG_DUPLICATE] =
                AnkiConfig::DuplicatePolicy::DifferentDeck;
        }
        if (profile[CONFIG_SCREENSHOT].isNull())
        {
            modified = true;
            profile[CONFIG_SCREENSHOT] = AnkiConfig::FileType::jpg;
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
        if (profile[CONFIG_AUDIO_NAME].isNull())
        {
            modified = true;
            profile[CONFIG_AUDIO_NAME] = SETTINGS_AUDIO_SRC_NAME_DEFAULT;
            profile[CONFIG_AUDIO_URL]  = SETTINGS_AUDIO_SRC_URL_DEFAULT;
            profile[CONFIG_AUDIO_HASH] = SETTINGS_AUDIO_SRC_MD5_DEFAULT;
        }
        if (profile[CONFIG_AUDIO_URL].isNull())
        {
            modified = true;
            profile[CONFIG_AUDIO_URL] = SETTINGS_AUDIO_SRC_URL_DEFAULT;
        }
        if (profile[CONFIG_AUDIO_HASH].isNull())
        {
            modified = true;
            profile[CONFIG_AUDIO_HASH] = SETTINGS_AUDIO_SRC_MD5_DEFAULT;
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

        if (modified)
            profiles[i] = profile;

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
        else if (!profile[CONFIG_SCREENSHOT].isDouble())
        {
            qDebug() << CONFIG_SCREENSHOT << "is not a double";
            return false;
        }
        else if (!profile[CONFIG_AUDIO_URL].isString())
        {
            qDebug() << CONFIG_AUDIO_URL << "is not a string";
            return false;
        }
        else if (!profile[CONFIG_AUDIO_HASH].isString())
        {
            qDebug() << CONFIG_AUDIO_HASH << "is not a string";
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
    }

    m_enabled = jsonObj[CONFIG_ENABLED].toBool();
    m_currentProfile = jsonObj[CONFIG_SET_PROFILE].toString();
    for (const QJsonValue &val : profiles)
    {
        QJsonObject profile     = val.toObject();
        AnkiConfig *config      = new AnkiConfig;
        config->address         = profile[CONFIG_HOST].toString();
        config->port            = profile[CONFIG_PORT].toString();
        config->duplicatePolicy = (AnkiConfig::DuplicatePolicy)
            profile[CONFIG_DUPLICATE].toInt(
                AnkiConfig::DuplicatePolicy::DifferentDeck
            );
        config->screenshotType  = (AnkiConfig::FileType)
            profile[CONFIG_SCREENSHOT].toInt(AnkiConfig::FileType::jpg);
        config->audio.name      = profile[CONFIG_AUDIO_NAME].toString();
        config->audio.url       = profile[CONFIG_AUDIO_URL].toString();
        config->audio.md5       = profile[CONFIG_AUDIO_HASH].toString();
        config->audioPadStart   = profile[CONFIG_AUDIO_PAD_START].toDouble();
        config->audioPadEnd     = profile[CONFIG_AUDIO_PAD_END].toDouble();
        config->tags            = profile[CONFIG_TAGS].toArray();

        QJsonObject obj         = profile[CONFIG_TERM].toObject();
        config->termDeck        = obj[CONFIG_DECK].toString();
        config->termModel       = obj[CONFIG_MODEL].toString();
        config->termFields      = obj[CONFIG_FIELDS].toObject();

        obj                     = profile[CONFIG_KANJI].toObject();
        config->kanjiDeck       = obj[CONFIG_DECK].toString();
        config->kanjiModel      = obj[CONFIG_MODEL].toString();
        config->kanjiFields     = obj[CONFIG_FIELDS].toObject();

        QString profileName = profile[CONFIG_NAME].toString();
        m_configs->insert(profileName, config);
        if (profileName == m_currentProfile)
        {
            m_currentConfig = config;
        }
    }

    if (m_currentConfig)
        setServer(m_currentConfig->address, m_currentConfig->port);

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
    QList<QString> profiles = m_configs->keys();
    for (const QString &profile : profiles)
    {
        QJsonObject configObj;
        const AnkiConfig *config          = m_configs->value(profile);
        configObj[CONFIG_NAME]            = profile;
        configObj[CONFIG_HOST]            = config->address;
        configObj[CONFIG_PORT]            = config->port;
        configObj[CONFIG_DUPLICATE]       = config->duplicatePolicy;
        configObj[CONFIG_SCREENSHOT]      = config->screenshotType;
        configObj[CONFIG_AUDIO_NAME]      = config->audio.name;
        configObj[CONFIG_AUDIO_URL]       = config->audio.url;
        configObj[CONFIG_AUDIO_HASH]      = config->audio.md5;
        configObj[CONFIG_AUDIO_PAD_START] = config->audioPadStart;
        configObj[CONFIG_AUDIO_PAD_END]   = config->audioPadEnd;
        configObj[CONFIG_TAGS]            = config->tags;

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
    QList keys = m_configs->keys();
    QStringList result;
    for (auto it = keys.begin(); it != keys.end(); ++it)
        result.append(*it);
    return result;
}

bool AnkiClient::setProfile(const QString &profile)
{
    const AnkiConfig *config = m_configs->value(profile);
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
    const AnkiConfig *oldConfig = m_configs->value(profile);
    m_configs->insert(profile, new AnkiConfig(config));
    delete oldConfig;
}

const AnkiConfig *AnkiClient::getConfig(const QString &profile) const
{
    return m_configs->value(profile);
}

const AnkiConfig *AnkiClient::getConfig() const
{
    return m_currentConfig;
}

QHash<QString, AnkiConfig *> *AnkiClient::getConfigs() const
{
    QList<QString> keys = m_configs->keys();
    QHash<QString, AnkiConfig *> *configs = new QHash<QString, AnkiConfig *>();
    for (auto it = keys.begin(); it != keys.end(); ++it)
    {
        configs->insert(*it, new AnkiConfig(*m_configs->value(*it)));
    }
    return configs;
}

void AnkiClient::setDefaultConfig()
{
    AnkiConfig *config = new AnkiConfig;
    config->address         = DEFAULT_HOST;
    config->port            = DEFAULT_PORT;
    config->duplicatePolicy = DEFAULT_DUPLICATE_POLICY;
    config->screenshotType  = DEFAULT_SCREENSHOT;
    config->audio.name      = SETTINGS_AUDIO_SRC_NAME_DEFAULT;
    config->audio.url       = SETTINGS_AUDIO_SRC_URL_DEFAULT;
    config->audio.md5       = SETTINGS_AUDIO_SRC_MD5_DEFAULT;
    config->audioPadStart   = DEFAULT_AUDIO_PAD_START;
    config->audioPadEnd     = DEFAULT_AUDIO_PAD_END;
    config->tags.append(DEFAULT_TAGS);

    delete m_configs->value(DEFAULT_PROFILE);
    m_configs->insert(DEFAULT_PROFILE, config);
    m_currentConfig  = config;
    m_currentProfile = DEFAULT_PROFILE;

    setServer(config->address, config->port);
}

/* End Config/Profile Methods */
/* Begin Getter/Setters */

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
    return requestStringList(ANKI_DECK_NAMES);
}

AnkiReply *AnkiClient::getModelNames()
{
    return requestStringList(ANKI_MODEL_NAMES);
}

AnkiReply *AnkiClient::getFieldNames(const QString &model)
{
    QJsonObject params;
    params[ANKI_PARAM_MODEL_NAME] = model;
    return requestStringList(ANKI_FIELD_NAMES, params);
}

AnkiReply *AnkiClient::notesAddable(const QList<Term *> &terms)
{
    QJsonArray notes;
    for (const Term *term : terms)
    {
        notes.append(createAnkiNoteObject(*term));
    }
    QJsonObject params;
    params[ANKI_CAN_ADD_NOTES_PARAM] = notes;
    QNetworkReply *reply = makeRequest(ANKI_CAN_ADD_NOTES, params);
    AnkiReply *ankiReply = new AnkiReply;
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
    return ankiReply;
}

AnkiReply *AnkiClient::notesAddable(const QList<const Kanji *> &kanjiList)
{
    QJsonArray notes;
    for (const Kanji *kanji : kanjiList)
        notes.append(createAnkiNoteObject(*kanji));
    QJsonObject params;
    params[ANKI_CAN_ADD_NOTES_PARAM] = notes;
    QNetworkReply *reply = makeRequest(ANKI_CAN_ADD_NOTES, params);
    AnkiReply *ankiReply = new AnkiReply;
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
    return ankiReply;
}

AnkiReply *AnkiClient::addNote(const Term *term)
{
    AnkiReply *ankiReply = new AnkiReply;

    QThreadPool::globalInstance()->start(
        [=] {
            QJsonObject params;
            QJsonObject note = createAnkiNoteObject(*term, true);
            delete term;
            params[ANKI_ADD_NOTE_PARAM] = note;
            Q_EMIT sendIntRequest(ANKI_ADD_NOTE, params, ankiReply);
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
            params[ANKI_ADD_NOTE_PARAM] = note;
            Q_EMIT sendIntRequest(ANKI_ADD_NOTE, params, ankiReply);
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
    case AnkiConfig::None:
    default:
        queryStr += query;
    }
    params[ANKI_PARAM_QUERY] = queryStr;
    QNetworkReply *reply = makeRequest(ANKI_GUI_BROWSE, params);
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

QJsonObject AnkiClient::createAnkiNoteObject(const Term &term, const bool media)
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
    QString audioFile = AUDIO_FILENAME_FORMAT_STRING.arg(term.reading)
                                                    .arg(term.expression);

    QString clozeBody   = QString(term.clozeBody).replace('\n', "<br>");
    QString clozePrefix = QString(term.clozePrefix).replace('\n', "<br>");
    QString clozeSuffix = QString(term.clozeSuffix).replace('\n', "<br>");
    QString sentence    = QString(term.sentence).replace('\n', "<br>");

    QString frequencies = buildFrequencies(term.frequencies);

    QString furigana;
    QString furiganaPlain;
    QString reading;
    if (term.reading.isEmpty())
    {
        furigana      = term.expression;
        furiganaPlain = term.expression;
        reading       = term.expression;
    }
    else
    {
        furigana      = FURIGANA_FORMAT_STRING.arg(term.expression)
                                              .arg(term.reading);
        furiganaPlain = term.expression + "[" + term.reading + "]";
        reading       = term.reading;
    }

    QString glossary        = buildGlossary(term.definitions);
    QString glossaryCompact = "<ol>";

    QString pitch;
    QString pitchGraph;
    QString pitchPosition;
    buildPitchInfo(term.pitches, pitch, pitchGraph, pitchPosition);

    for (const TermDefinition &def : term.definitions)
    {
        for (QString glos : def.glossary)
        {
            glossaryCompact += "<li>";
            glossaryCompact += glos.replace('\n', "</li><li>");
            glossaryCompact += "</li>";
        }
        if (glossaryCompact.endsWith("<li></li>"))
            glossaryCompact.chop(9);
    }
    glossaryCompact        += "</ol>";

    QString tags = "<ul>";
    accumulateTags(term.tags, tags);
    tags        += "</ul>";
    if (tags == "<ul></ul>")
        tags.clear();

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

        value.replace(REPLACE_CLOZE_BODY,      clozeBody);
        value.replace(REPLACE_CLOZE_PREFIX,    clozePrefix);
        value.replace(REPLACE_CLOZE_SUFFIX,    clozeSuffix);
        value.replace(REPLACE_EXPRESSION,      term.expression);
        value.replace(REPLACE_FREQUENCIES,     frequencies);
        value.replace(REPLACE_FURIGANA,        furigana);
        value.replace(REPLACE_FURIGANA_PLAIN,  furiganaPlain);
        value.replace(REPLACE_GLOSSARY,        glossary);
        value.replace(REPLACE_GLOSSARY_BRIEF,  glossaryCompact);
        value.replace(REPLACE_PITCH,           pitch);
        value.replace(REPLACE_PITCH_GRAPHS,    pitchGraph);
        value.replace(REPLACE_PITCH_POSITIONS, pitchPosition);
        value.replace(REPLACE_READING,         reading);
        value.replace(REPLACE_SENTENCE,        sentence);
        value.replace(REPLACE_TAGS,            tags);

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

            audObj[ANKI_NOTE_URL]      = QString(m_currentConfig->audio.url)
                .replace(REPLACE_EXPRESSION, term.expression)
                .replace(REPLACE_READING,    reading);
            audObj[ANKI_NOTE_FILENAME] = audioFile;
            audObj[ANKI_NOTE_FIELDS]   = fieldsWithAudio;
            audObj[ANKI_NOTE_SKIPHASH] = m_currentConfig->audio.md5;
            audio.append(audObj);
        }

        if (!audio.isEmpty())
        {
            note[ANKI_NOTE_AUDIO] = audio;
        }
    }

    return note;
}

QJsonObject AnkiClient::createAnkiNoteObject(const Kanji &kanji,
                                             const bool   media)
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
    QString clozeBody   = QString(kanji.clozeBody).replace('\n', "<br>");
    QString clozePrefix = QString(kanji.clozePrefix).replace('\n', "<br>");
    QString clozeSuffix = QString(kanji.clozeSuffix).replace('\n', "<br>");
    QString sentence    = QString(kanji.sentence).replace('\n', "<br>");

    QString frequencies = buildFrequencies(kanji.frequencies);

    QString glossary;
    QString tags;
    QString strokeCount;
    QString onyomi;
    QString kunyomi;
    if (!kanji.definitions.isEmpty())
    {
        glossary += "<ol>";
        tags     += "<ul>";
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
            accumulateTags(def.tags, tags);

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
        tags     += "</ul>";
        onyomi.chop(2);
        kunyomi.chop(2);

        /* Clear out empty results */
        if (glossary == "<ol></ol>")
            glossary.clear();
            /* Clear out an empty tag */
        if (tags == "<ul></ul>")
            tags.clear();
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

        value.replace(REPLACE_CLOZE_BODY,   clozeBody);
        value.replace(REPLACE_CLOZE_PREFIX, clozePrefix);
        value.replace(REPLACE_CLOZE_SUFFIX, clozeSuffix);
        value.replace(REPLACE_FREQUENCIES,  frequencies);
        value.replace(REPLACE_GLOSSARY,     glossary);
        value.replace(REPLACE_SENTENCE,     sentence);
        value.replace(REPLACE_TAGS,         tags);

        fieldsObj[field] = value;
    }

    /* Add Fields */
    note[ANKI_NOTE_FIELDS] = fieldsObj;

    return note;
}

void AnkiClient::buildCommonNote(const QJsonObject     &configFields,
                                 const CommonExpFields &exp,
                                 const bool             media,
                                 QJsonObject           &note,
                                 QJsonObject           &fieldObj)
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
    QString context   = QString(exp.context).replace('\n', "<br>");
    QString context2  = QString(exp.context2).replace('\n', "<br>");
    QString sentence2 = QString(exp.sentence2).replace('\n', "<br>");
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
        value.replace(REPLACE_CONTEXT,      context);
        value.replace(REPLACE_CONTEXT_SEC,  context2);
        value.replace(REPLACE_SENTENCE_SEC, sentence2);

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
                path = player->tempAudioClip(startTime, endTime);
            }
            if (!path.isEmpty()) {
                audObj[ANKI_NOTE_PATH] = path;
                m_tempFiles.append(path);
                QString filename = FileUtils::calculateMd5(path) + ".aac";
                audObj[ANKI_NOTE_FILENAME] = filename;
                audObj[ANKI_NOTE_FIELDS] = fieldsWithAudioMedia;
                audio.append(audObj);
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
                path = player->tempAudioClip(startTime, endTime);
            }
            if (!path.isEmpty()) {
                audObj[ANKI_NOTE_PATH] = path;
                m_tempFiles.append(path);
                QString filename = FileUtils::calculateMd5(path) + ".aac";
                audObj[ANKI_NOTE_FILENAME] = filename;
                audObj[ANKI_NOTE_FIELDS] = fieldsWithAudioContext;
                audio.append(audObj);
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
            image[ANKI_NOTE_PATH] = path;
            m_tempFiles.append(path);
            player->setSubVisiblity(visibility);

            QString filename = FileUtils::calculateMd5(path) + imageExt;
            image[ANKI_NOTE_FILENAME] = filename;

            image[ANKI_NOTE_FIELDS] = fieldsWithScreenshot;

            if (filename != imageExt)
            {
                images.append(image);
            }
        }

        if (!fieldWithScreenshotVideo.isEmpty())
        {
            QJsonObject image;
            QString path = GlobalMediator::getGlobalMediator()
                ->getPlayerAdapter()->tempScreenshot(false, imageExt);
            image[ANKI_NOTE_PATH] = path;
            m_tempFiles.append(path);

            QString filename = FileUtils::calculateMd5(path) + imageExt;
            image[ANKI_NOTE_FILENAME] = filename;

            image[ANKI_NOTE_FIELDS] = fieldWithScreenshotVideo;

            if (filename != imageExt)
            {
                images.append(image);
            }
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
                for (size_t i = pos; i < p.mora.size(); ++i)
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
                    p.mora.size(), pos, "white", "black"
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

QString AnkiClient::buildGlossary(const QList<TermDefinition> &definitions)
{
    QString glossary;
    glossary.append("<div style=\"text-align: left;\"><ol>");

    for (const TermDefinition &def : definitions)
    {
        glossary += "<li>";

        glossary += "<i>(";
        for (const Tag &tag : def.tags)
        {
            glossary += tag.name + ", ";
        }
        for (const Tag &rule : def.rules)
        {
            if (def.tags.contains(rule))
            {
                continue;
            }
            glossary += rule.name + ", ";
        }
        glossary += def.dictionary;
        glossary += ")</i>";

        glossary += "<ul>";
        for (QString glos : def.glossary)
        {
            glossary += "<li>";
            glossary += glos.replace('\n', "</li><li>");
            glossary += "</li>";
        }
        if (glossary.endsWith("<li></li>"))
            glossary.chop(9);
        glossary += "</ul>";

        glossary += "</li>";
    }

    glossary.append("</ol></div>");

    return glossary;
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

QString &AnkiClient::accumulateTags(const QList<Tag> &tags, QString &tagStr)
{
    if (tags.isEmpty())
        return tagStr;

    for (const Tag &tag : tags)
    {
        tagStr += "<li>";
        tagStr += tag.name;
        if (!tag.notes.isEmpty())
        {
            tagStr += ": ";
            tagStr += tag.notes;
        }
        tagStr += "</li>";
    }

    return tagStr;
}

/* End Note Helpers */
