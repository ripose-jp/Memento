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

#include "anki/ankiconnect.h"
#include "anki/notebuilder.h"
#include "util/globalmediator.h"
#include "util/utils.h"

static constexpr int MIN_ANKICONNECT_VERSION = 6;
static constexpr int TIMEOUT = 5000;
static constexpr const char *CONFIG_FILE = "anki_connect.json";

/* Config file fields */
static constexpr const char *CONFIG_ENABLED = "enabled";
static constexpr const char *CONFIG_PROFILES = "profiles";
static constexpr const char *CONFIG_SET_PROFILE = "setProfile";
static constexpr const char *CONFIG_NAME = "name";
static constexpr const char *CONFIG_HOST = "host";
static constexpr const char *CONFIG_PORT = "port";
static constexpr const char *CONFIG_DUPLICATE = "duplicate";
static constexpr const char *CONFIG_NEWLINE_REPLACER = "newline-replace";
static constexpr const char *CONFIG_SCREENSHOT = "screenshot";
static constexpr const char *CONFIG_AUDIO_PAD_START = "audio-pad-start";
static constexpr const char *CONFIG_AUDIO_PAD_END = "audio-pad-end";
static constexpr const char *CONFIG_AUDIO_NORMALIZE = "audio-normalize";
static constexpr const char *CONFIG_AUDIO_DB = "audio-db";
static constexpr const char *CONFIG_TERM = "term";
static constexpr const char *CONFIG_KANJI = "kanji";
static constexpr const char *CONFIG_TAGS = "tags";
static constexpr const char *CONFIG_DECK = "deck";
static constexpr const char *CONFIG_MODEL = "model";
static constexpr const char *CONFIG_FIELDS = "fields";
static constexpr const char *CONFIG_EXCLUDE_GLOSSARY = "ex-glos";

/* Begin Constructor/Destructors */

AnkiClient::AnkiClient(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    m_manager->setTransferTimeout(TIMEOUT);

    if (!readConfigFromFile(CONFIG_FILE) || m_currentConfig == nullptr)
    {
        setDefaultConfig();
    }

    qRegisterMetaType<QList<GlossaryBuilder::FileInfo>>("FileMap");

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
    QNetworkReply *reply = makeRequest(AnkiConnect::Action::VERSION);
    AnkiReply *ankiReply = new AnkiReply;
    connect(reply, &QNetworkReply::finished, this,
        [this, reply, ankiReply]
        {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedBool(false, error);
            }
            else if (!replyObj[AnkiConnect::Req::RESULT].isDouble())
            {
                Q_EMIT ankiReply->finishedBool(
                    false, "AnkiConnect result is not a number"
                );
            }
            else if (replyObj[AnkiConnect::Req::RESULT].toInt() <
                MIN_ANKICONNECT_VERSION)
            {
                error = "AnkiConnect version %1 < %2";
                error = error.arg(
                    QString::number(replyObj[AnkiConnect::Req::RESULT].toInt()),
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
    return requestStringList(AnkiConnect::Action::DECK_NAMES);
}

AnkiReply *AnkiClient::getModelNames()
{
    return requestStringList(AnkiConnect::Action::MODEL_NAMES);
}

AnkiReply *AnkiClient::getFieldNames(const QString &model)
{
    QJsonObject params;
    params[AnkiConnect::Param::MODEL_NAME] = model;
    return requestStringList(AnkiConnect::Action::FIELD_NAMES, params);
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
            Anki::Note::Context ctx =
                Anki::Note::build(*m_currentConfig, termCopy, false);
            notes.append(ctx.ankiObject);
            if (!term->reading.isEmpty())
            {
                termCopy.readingAsExpression = true;
                Anki::Note::Context ctx =
                    Anki::Note::build(*m_currentConfig, termCopy, false);
                notes.append(ctx.ankiObject);
            }
        }
        QJsonObject params;
        params[AnkiConnect::Note::CAN_ADD_NOTES_PARAM] = notes;

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
            AnkiConnect::Action::CAN_ADD_NOTES, params, proxyReply
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
                Anki::Note::Context ctx =
                    Anki::Note::build(*m_currentConfig, *kanji, false);
                notes.append(ctx.ankiObject);
            }
            QJsonObject params;
            params[AnkiConnect::Note::CAN_ADD_NOTES_PARAM] = notes;
            Q_EMIT sendBoolListRequest(
                AnkiConnect::Action::CAN_ADD_NOTES, params, ankiReply
            );
        }
    );

    return ankiReply;
}

AnkiReply *AnkiClient::addNote(const Term *term)
{
    AnkiReply *ankiReply = new AnkiReply;

    QThreadPool::globalInstance()->start(
        [this, term, ankiReply]
        {
            QJsonObject params;
            Anki::Note::Context ctx =
                Anki::Note::build(*m_currentConfig, *term, true);
            if (!ctx.fileMap.isEmpty())
            {
                Q_EMIT requestAddMedia(ctx.fileMap);
            }
            delete term;
            params[AnkiConnect::Param::ADD_NOTE] = ctx.ankiObject;
            Q_EMIT sendIntRequest(
                AnkiConnect::Action::ADD_NOTE, params, ankiReply
            );
        }
    );

    return ankiReply;
}

AnkiReply *AnkiClient::addNote(const Kanji *kanji)
{
    AnkiReply *ankiReply = new AnkiReply;

    QThreadPool::globalInstance()->start(
        [this, kanji, ankiReply]
        {
            QJsonObject params;
            Anki::Note::Context ctx =
                Anki::Note::build(*m_currentConfig, *kanji, true);
            delete kanji;
            params[AnkiConnect::Param::ADD_NOTE] = ctx.ankiObject;
            Q_EMIT sendIntRequest(
                AnkiConnect::Action::ADD_NOTE, params, ankiReply
            );
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
    params[AnkiConnect::Param::QUERY] = queryStr;
    QNetworkReply *reply = makeRequest(AnkiConnect::Action::GUI_BROWSE, params);
    AnkiReply *ankiReply = new AnkiReply;
    connect(reply, &QNetworkReply::finished, this,
        [this, reply, ankiReply]
        {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedIntList(QList<int>(), error);
            }
            else if (!replyObj[AnkiConnect::Req::RESULT].isArray())
            {
                Q_EMIT ankiReply->finishedIntList(
                    QList<int>(), "Result is not an array"
                );
            }
            else
            {
                QList<int> response;
                QJsonArray resultArray =
                    replyObj[AnkiConnect::Req::RESULT].toArray();
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

AnkiReply *AnkiClient::addMedia(const QList<GlossaryBuilder::FileInfo> &fileMap)
{
    QJsonObject params;
    QJsonArray actions;
    for (const GlossaryBuilder::FileInfo &info : fileMap)
    {
        QJsonObject command;
        command[AnkiConnect::Req::ACTION] =
            AnkiConnect::Action::STORE_MEDIA_FILE;
        QJsonObject fileParams;
        fileParams[AnkiConnect::Note::DATA] = FileUtils::toBase64(info.path);
        fileParams[AnkiConnect::Note::FILENAME] = info.name;
        command[AnkiConnect::Req::PARAMS] = fileParams;

        actions << command;
    }
    params[AnkiConnect::Param::ACTIONS] = actions;

    QNetworkReply *reply = makeRequest(AnkiConnect::Action::MULTI, params);
    AnkiReply *ankiReply = new AnkiReply;
    connect(reply, &QNetworkReply::finished, this,
        [this, reply, ankiReply]
        {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedStringList(QStringList(), error);
            }
            else if (!replyObj[AnkiConnect::Req::RESULT].isArray())
            {
                Q_EMIT ankiReply->finishedStringList(
                    QStringList(), "Result is not an array"
                );
            }
            else
            {
                QStringList filenames;
                QJsonArray resultObjects =
                    replyObj[AnkiConnect::Req::RESULT].toArray();
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
                    if (!obj[AnkiConnect::Reply::ERROR].isNull())
                    {
                        Q_EMIT ankiReply->finishedStringList(
                            QStringList(),
                            obj[AnkiConnect::Reply::ERROR].toString()
                        );
                        goto exit;
                    }

                    if (!obj[AnkiConnect::Req::RESULT].isString())
                    {
                        Q_EMIT ankiReply->finishedStringList(
                            QStringList(), "A result is not a string"
                        );
                        goto exit;
                    }

                    filenames << obj[AnkiConnect::Req::RESULT].toString();
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
    jsonMsg[AnkiConnect::Req::ACTION] = action;
    jsonMsg[AnkiConnect::Reply::VERSION] = MIN_ANKICONNECT_VERSION;
    if (!params.isEmpty())
    {
        jsonMsg[AnkiConnect::Req::PARAMS] = params;
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
        else if (!replyObj.contains(AnkiConnect::Reply::ERROR))
        {
            error = "Anki response is missing error field";
        }
        else if (!replyObj.contains(AnkiConnect::Req::RESULT))
        {
            error = "Anki response is missing result field";
        }
        else if (!replyObj[AnkiConnect::Reply::ERROR].isNull())
        {
            error = replyObj[AnkiConnect::Reply::ERROR].toString();
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
        [this, reply, ankiReply]
        {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedInt(0, error);
            }
            else if (!replyObj[AnkiConnect::Req::RESULT].isDouble())
            {
                Q_EMIT ankiReply->finishedInt(
                    0, "AnkiConnect result is not a double"
                );
            }
            else
            {
                Q_EMIT ankiReply->finishedInt(
                    replyObj[AnkiConnect::Req::RESULT].toInt(), error
                );
            }
            ankiReply->deleteLater();
            reply->deleteLater();
        }
    );
}

void AnkiClient::receiveBoolListRequest(const QString     &action,
                                        const QJsonObject &params,
                                        AnkiReply         *ankiReply)
{
    QNetworkReply *reply = makeRequest(action, params);
    connect(reply, &QNetworkReply::finished, this,
        [this, reply, ankiReply]
        {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedBoolList(QList<bool>(), error);
            }
            else if (!replyObj[AnkiConnect::Req::RESULT].isArray())
            {
                Q_EMIT ankiReply->finishedBoolList(
                    QList<bool>(), "Result is not an array"
                );
            }
            else
            {
                QList<bool> response;
                QJsonArray resultArray =
                    replyObj[AnkiConnect::Req::RESULT].toArray();
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
        [this, reply, ankiReply]
        {
            QString error;
            QJsonObject replyObj = processReply(reply, error);
            if (replyObj.isEmpty())
            {
                Q_EMIT ankiReply->finishedStringList(QStringList(), error);
            }
            else if (!replyObj[AnkiConnect::Req::RESULT].isArray())
            {
                Q_EMIT ankiReply->finishedStringList(
                    QStringList(), "Result is not an array"
                );
            }
            else
            {
                QStringList decks;
                QJsonArray deckNames =
                    replyObj[AnkiConnect::Req::RESULT].toArray();
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
