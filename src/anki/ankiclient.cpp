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

#include "../util/directoryutils.h"
extern "C"
{
    #include "../ffmpeg/transcode_aac.h"
}

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>
#include <QDebug>

// Anki request fields
#define ANKI_ACTION "action"
#define ANKI_RESULT "result"
#define ANKI_PARAMS "params"

// Anki reply fields
#define ANKI_ERROR "error"
#define ANKI_VERSION "version"

// Anki actions
#define ANKI_DECK_NAMES "deckNames"
#define ANKI_MODEL_NAMES "modelNames"
#define ANKI_FIELD_NAMES "modelFieldNames"
#define ANKI_ACTION_VERSION "version"
#define ANKI_CAN_ADD_NOTES "canAddNotes"
#define ANKI_ADD_NOTE "addNote"
#define ANKI_ADD_NOTE_PARAM "note"

// Anki param fields
#define ANKI_PARAM_MODEL_NAME "modelName"

// Anki note fields
#define ANKI_CAN_ADD_NOTES_PARAM "notes"
#define ANKI_NOTE_DECK "deckName"
#define ANKI_NOTE_MODEL "modelName"
#define ANKI_NOTE_FIELDS "fields"
#define ANKI_NOTE_TAGS "tags"
#define ANKI_NOTE_AUDIO "audio"
#define ANKI_NOTE_PICTURE "picture"
#define ANKI_NOTE_URL "url"
#define ANKI_NOTE_PATH "path"
#define ANKI_NOTE_FILENAME "filename"
#define ANKI_NOTE_FIELDS "fields"
#define ANKI_NOTE_SKIPHASH "skipHash"

#define MIN_ANKICONNECT_VERSION 6
#define TIMEOUT 5000
#define CONFIG_FILE "anki_connect.json"
#define FURIGANA_FORMAT_STRING (QString("<ruby>%1<rt>%2</rt></ruby>"))
#define AUDIO_URL_FORMAT_STRING (QString("http://assets.languagepod101.com/dictionary/japanese/audiomp3.php?kanji=%1&kana=%2"))
#define JAPANESE_POD_STUB_MD5 "7e2c2f954ef6051373ba916f000168dc"
#define AUDIO_FILENAME_FORMAT_STRING (QString("memento_%1_%2.mp3"))

// Config file fields
#define CONFIG_ENABLED "enabled"
#define CONFIG_PROFILES "profiles"
#define CONFIG_SET_PROFILE "setProfile"
#define CONFIG_NAME "name"
#define CONFIG_HOST "host"
#define CONFIG_PORT "port"
#define CONFIG_TAGS "tags"
#define CONFIG_DECK "deck"
#define CONFIG_MODEL "model"
#define CONFIG_FIELDS "fields"

QNetworkAccessManager *m_manager;

AnkiClient::AnkiClient(QObject *parent, PlayerAdapter *player)
    : QObject(parent),
      m_player(player),
      m_configs(new QHash<QString, const AnkiConfig *>),
      m_currentConfig(0)
{
    m_manager = new QNetworkAccessManager(this);
    m_manager->setTransferTimeout(TIMEOUT);

    readConfigFromFile(CONFIG_FILE);
}

AnkiClient::~AnkiClient()
{
    clearProfiles();
    delete m_configs;
    delete m_manager;
}

void AnkiClient::readConfigFromFile(const QString &filename)
{
    QFile configFile(DirectoryUtils::getConfigDir() + filename);
    if (!configFile.exists())
    {
        return;
    }

    if (!configFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "AnkiConnect config file exists, can't open it";
        return;
    }

    // Read the file into memory
    QTextStream stream(&configFile);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(
        stream.readAll().toLocal8Bit());
    configFile.close();

    // Error check the JSON
    if (jsonDoc.isNull())
    {
        qDebug() << filename << "is not JSON";
        return;
    }
    else if (!jsonDoc.isObject())
    {
        qDebug() << filename << "is not an object";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    // Error check the config
    if (!jsonObj[CONFIG_ENABLED].isBool())
    {
        qDebug() << CONFIG_ENABLED << "is not a boolean";
        return;
    }
    else if (!jsonObj[CONFIG_SET_PROFILE].isString())
    {
        qDebug() << CONFIG_SET_PROFILE << "is not a string";
        return;
    }
    else if (!jsonObj[CONFIG_PROFILES].isArray())
    {
        qDebug() << CONFIG_PROFILES << "is not an array";
        return;
    }
    QJsonArray profiles = jsonObj[CONFIG_PROFILES].toArray();
    for (auto it = profiles.begin(); it != profiles.end(); ++it)
    {
        if (!it->isObject())
        {
            qDebug() << CONFIG_PROFILES << "element is not an object";
            return;
        }
        QJsonObject profile = it->toObject();
        if (!profile[CONFIG_NAME].isString())
        {
            qDebug() << CONFIG_NAME << "is not a string";
            return;
        }
        else if (!profile[CONFIG_HOST].isString())
        {
            qDebug() << CONFIG_HOST << "is not a string";
            return;
        }
        else if (!profile[CONFIG_PORT].isString())
        {
            qDebug() << CONFIG_PORT << "is not a string";
            return;
        }
        else if (!profile[CONFIG_TAGS].isArray())
        {
            qDebug() << CONFIG_TAGS << "is not an array";
            return;
        }
        else if (!profile[CONFIG_DECK].isString())
        {
            qDebug() << CONFIG_DECK << "is not a string";
            return;
        }
        else if (!profile[CONFIG_MODEL].isString())
        {
            qDebug() << CONFIG_MODEL << "is not a string";
            return;
        }
        else if (!profile[CONFIG_FIELDS].isObject())
        {
            qDebug() << CONFIG_FIELDS << "is not an object";
            return;
        }
    }

    m_enabled = jsonObj[CONFIG_ENABLED].toBool();
    m_currentProfile = jsonObj[CONFIG_SET_PROFILE].toString();
    for (auto it = profiles.begin(); it != profiles.end(); ++it)
    {
        QJsonObject profile = it->toObject();
        AnkiConfig *config = new AnkiConfig;
        config->address = profile[CONFIG_HOST].toString();
        config->port = profile[CONFIG_PORT].toString();
        config->tags = profile[CONFIG_TAGS].toArray();
        config->deck = profile[CONFIG_DECK].toString();
        config->model = profile[CONFIG_MODEL].toString();
        config->fields = profile[CONFIG_FIELDS].toObject();

        QString profileName = profile[CONFIG_NAME].toString();
        m_configs->insert(profileName, config);
        if (profileName == m_currentProfile)
        {
            m_currentConfig = config;
        }
    }

    setServer(m_currentConfig->address, m_currentConfig->port);
}

bool AnkiClient::writeConfigToFile(const QString &filename)
{
    QFile configFile(DirectoryUtils::getConfigDir() + filename);
    if (!configFile.open(QIODevice::ReadWrite | QIODevice::Truncate |
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
    for (auto it = profiles.begin(); it != profiles.end(); ++it)
    {
        QJsonObject configObj;
        configObj[CONFIG_NAME] = *it;
        const AnkiConfig *config = m_configs->value(*it);
        configObj[CONFIG_HOST] = config->address;
        configObj[CONFIG_PORT] = config->port;
        QJsonArray tagsArr;
        for (size_t i = 0; i < config->tags.size(); ++i)
        {
            tagsArr.append(config->tags[i]);
        }
        configObj[CONFIG_TAGS] = tagsArr;
        configObj[CONFIG_DECK] = config->deck;
        configObj[CONFIG_MODEL] = config->model;
        configObj[CONFIG_FIELDS] = config->fields;

        configArr.append(configObj);
    }
    jsonObj[CONFIG_PROFILES] = configArr;
    QJsonDocument jsonDoc(jsonObj);

    QTextStream stream(&configFile);
    stream << jsonDoc.toJson();
    configFile.close();

    return true;
}

void AnkiClient::setServer(const QString &address, const QString &port)
{
    m_address = address;
    m_port = port;
}

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

const AnkiConfig *AnkiClient::getConfig(const QString &profile) const
{
    return m_configs->value(profile);
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

void AnkiClient::clearProfiles()
{
    for (auto it = m_configs->begin(); it != m_configs->end(); ++it)
        delete *it;
    m_configs->clear();
}

bool AnkiClient::isEnabled() const
{
    return m_enabled;
}

void AnkiClient::setEnabled(const bool value)
{
    m_enabled = value;
}

void AnkiClient::writeChanges()
{
    writeConfigToFile(CONFIG_FILE);
    Q_EMIT settingsChanged();
}

void AnkiClient::testConnection(
    std::function<void(const bool, const QString &)> callback)
{
    QNetworkReply *reply = makeRequest(ANKI_ACTION_VERSION);
    connect(reply, &QNetworkReply::finished, [=] {
        QString error;
        QJsonObject replyObj = processReply(reply, error);
        if (replyObj.isEmpty())
        {
            callback(false, error);
        }
        else if (!replyObj[ANKI_RESULT].isDouble())
        {
            callback(false, "AnkiConnect result is not a number");
        }
        else if (replyObj[ANKI_RESULT].toInt() < MIN_ANKICONNECT_VERSION)
        {
            error = "AnkiConnect version %1 < %2";
            error = error.arg(QString::number(replyObj[ANKI_RESULT].toInt()),
                              QString::number(MIN_ANKICONNECT_VERSION));
            callback(false, error);
        }
        else
        {
            callback(true, error);
        }
        delete reply;
    });
}

void AnkiClient::getDeckNames(
    std::function<void(const QStringList *, const QString &)> callback)
{
    requestStringList(callback, ANKI_DECK_NAMES);
}

void AnkiClient::getModelNames(
    std::function<void(const QStringList *, const QString &)> callback)
{
    requestStringList(callback, ANKI_MODEL_NAMES);
}

void AnkiClient::getFieldNames(
    std::function<void(const QStringList *, const QString &)> callback,
    const QString &model)
{
    QJsonObject params;
    params[ANKI_PARAM_MODEL_NAME] = model;
    requestStringList(callback, ANKI_FIELD_NAMES, params);
}

void AnkiClient::requestStringList(
    std::function<void(const QStringList *, const QString &)> callback,
    const QString &action,
    const QJsonObject &params)
{
    QNetworkReply *reply = makeRequest(action, params);
    connect(reply, &QNetworkReply::finished, [=] {
        QString error;
        QJsonObject replyObj = processReply(reply, error);
        if (replyObj.isEmpty())
        {
            callback(0, error);
        }
        else if (!replyObj[ANKI_RESULT].isArray())
        {
            callback(0, "Result is not an array");
        }
        else
        {
            QStringList *decks = new QStringList();
            QJsonArray deckNames = replyObj[ANKI_RESULT].toArray();
            for (auto it = deckNames.begin(); it != deckNames.end(); ++it)
                decks->append((*it).toString());
            callback(decks, error);
        }
        delete reply;
    });
}

void AnkiClient::entriesAddable(
    std::function<void(const QList<bool> *, const QString &)> callback,
    const QList<Entry *> *entries)
{
    QJsonArray notes;
    for (auto it = entries->begin(); it != entries->end(); ++it)
        notes.append(createAnkiNoteObject(**it));
    QJsonObject params;
    params[ANKI_CAN_ADD_NOTES_PARAM] = notes;
    QNetworkReply *reply = makeRequest(ANKI_CAN_ADD_NOTES, params);
    connect(reply, &QNetworkReply::finished, [=] {
        QString error;
        QJsonObject replyObj = processReply(reply, error);
        if (replyObj.isEmpty())
        {
            callback(0, error);
        }
        else if (!replyObj[ANKI_RESULT].isArray())
        {
            callback(0, "Result is not an array");
        }
        else
        {
            QList<bool> *response = new QList<bool>();
            QJsonArray resultArray = replyObj[ANKI_RESULT].toArray();
            for (auto it = resultArray.begin(); it != resultArray.end(); ++it)
            {
                if (it->isBool())
                {
                    response->append(it->toBool());
                }
                else
                {
                    delete response;
                    callback(0, "Response was not an array of bool");
                }
            }
            callback(response, error);
        }
        delete reply;
    });
}

void AnkiClient::addEntry(
    std::function<void(const int, const QString &)> callback,
    const Entry *entry)
{
    QJsonObject params;
    params[ANKI_ADD_NOTE_PARAM] = createAnkiNoteObject(*entry, true);
    QNetworkReply *reply = makeRequest(ANKI_ADD_NOTE, params);
    connect(reply, &QNetworkReply::finished, [=] {
        QString error;
        QJsonObject replyObj = processReply(reply, error);
        if (replyObj.isEmpty())
        {
            callback(false, error);
        }
        else if (!replyObj[ANKI_RESULT].isDouble())
        {
            callback(false, "AnkiConnect result is not a bool");
        }
        else
        {
            callback(replyObj[ANKI_RESULT].toInt(), error);
        }
        delete reply;
    });
}

QJsonObject AnkiClient::createAnkiNoteObject(const Entry &entry,
                                             const bool media)
{
    QJsonObject note;
    note[ANKI_NOTE_DECK] = m_currentConfig->deck;
    note[ANKI_NOTE_MODEL] = m_currentConfig->model;

    // Processed fields
    QString audioFile = AUDIO_FILENAME_FORMAT_STRING.arg(*entry.m_kana)
                            .arg(*entry.m_kanji);
    QString furigana =
        entry.m_kanji->isEmpty() ? *entry.m_kana :
                                   FURIGANA_FORMAT_STRING.arg(*entry.m_kanji)
                                   .arg(*entry.m_kana);
    QString furiganaPlain;
    if (entry.m_kanji->isEmpty())
        furiganaPlain = *entry.m_kana;
    else
        furiganaPlain = *entry.m_altkanji + "[" + *entry.m_kana + "]";
    QString glossary = buildGlossary(*entry.m_descriptions);

    // Find and replace fields with processed fields
    QStringList fields = m_currentConfig->fields.keys();
    QJsonArray fieldsWithAudio, fieldsWithAudioMedia, 
               fieldsWithScreenshot, fieldWithScreenshotVideo;
    QJsonObject fieldsObj;
    for (auto it = fields.begin(); it != fields.end(); ++it)
    {
        QString value = m_currentConfig->fields[*it].toString();

        if (value.contains(REPLACE_AUDIO))
        {
            fieldsWithAudio.append(*it);
        }
        value = value.replace(REPLACE_AUDIO, "");

        if (value.contains(REPLACE_AUDIO_MEDIA))
        {
            fieldsWithAudioMedia.append(*it);
        }
        value = value.replace(REPLACE_AUDIO_MEDIA, "");

        if (value.contains(REPLACE_SCREENSHOT))
        {
            fieldsWithScreenshot.append(*it);
        }
        value = value.replace(REPLACE_SCREENSHOT, "");

        if (value.contains(REPLACE_SCREENSHOT_VIDEO))
        {
            fieldWithScreenshotVideo.append(*it);
        }
        value = value.replace(REPLACE_SCREENSHOT_VIDEO, "");

        value = value.replace(REPLACE_CLOZE_BODY, *entry.m_clozeBody);
        value = value.replace(REPLACE_CLOZE_PREFIX, *entry.m_clozePrefix);
        value = value.replace(REPLACE_CLOZE_SUFFIX, *entry.m_clozeSuffix);
        value = value.replace(
            REPLACE_EXPRESSION,
            entry.m_kanji->isEmpty() ? *entry.m_kana : *entry.m_kanji
        );
        value = value.replace(REPLACE_ALT_EXPRESSION, *entry.m_altkanji);
        value = value.replace(REPLACE_FURIGANA, furigana);
        value = value.replace(REPLACE_FURIGANA_PLAIN, furiganaPlain);
        value = value.replace(REPLACE_GLOSSARY, glossary);
        value = value.replace(REPLACE_READING, *entry.m_kana);
        value = value.replace(REPLACE_ALT_READING, *entry.m_altkana);
        value = value.replace(REPLACE_SENTENCE, *entry.m_sentence);

        fieldsObj[*it] = value;
    }

    // Add media secions to the request
    if (media)
    {
        QJsonArray audio;
        if (!fieldsWithAudio.isEmpty())
        {
            QJsonObject audObj;
            audObj[ANKI_NOTE_URL] = AUDIO_URL_FORMAT_STRING.arg(*entry.m_kanji)
                                                           .arg(*entry.m_kana);
            audObj[ANKI_NOTE_FILENAME] = audioFile;
            audObj[ANKI_NOTE_FIELDS] = fieldsWithAudio;
            audObj[ANKI_NOTE_SKIPHASH] = JAPANESE_POD_STUB_MD5;
            audio.append(audObj);
        }

        if (!fieldsWithAudioMedia.isEmpty())
        {
            QJsonObject audObj;
            QTemporaryFile temp;
            if (temp.open()) {
                QString path = temp.fileName() + ".aac";
                temp.close();
                temp.remove();

                audObj[ANKI_NOTE_PATH] = path;

                transcode_aac(
                    m_player->getPath().toLatin1(), path.toLatin1(), 
                    m_player->getAudioTrack() - 1,
                    m_player->getSubStart(), m_player->getSubEnd()
                );

                QString filename = generateMD5(path) + ".aac";
                audObj[ANKI_NOTE_FILENAME] = audioFile;
                audObj[ANKI_NOTE_FIELDS] = fieldsWithAudioMedia;

                if (filename != ".aac")
                {
                    audio.append(audObj);
                }
            }
        }

        if (!audio.isEmpty())
        {
            note[ANKI_NOTE_AUDIO] = audio;
        }
        
        QJsonArray images;
        if (!fieldsWithScreenshot.isEmpty())
        {
            QJsonObject image;
            QString path = m_player->tempScreenshot(true);
            image[ANKI_NOTE_PATH] = path;

            QString filename = generateMD5(path) + ".png";
            image[ANKI_NOTE_FILENAME] = filename;

            image[ANKI_NOTE_FIELDS] = fieldsWithScreenshot;

            if (filename != ".png")
            {
                images.append(image);
            }
        }

        if (!fieldWithScreenshotVideo.isEmpty())
        {
            QJsonObject image;
            QString path = m_player->tempScreenshot(false);
            image[ANKI_NOTE_PATH] = path;

            QString filename = generateMD5(path) + ".png";
            image[ANKI_NOTE_FILENAME] = filename;
            
            image[ANKI_NOTE_FIELDS] = fieldWithScreenshotVideo;

            if (filename != ".png")
            {
                images.append(image);
            }
        }

        if (!images.isEmpty())
        {
            note[ANKI_NOTE_PICTURE] = images;
        }    
    }

    note[ANKI_NOTE_FIELDS] = fieldsObj;
    note[ANKI_NOTE_TAGS] = m_currentConfig->tags;
    
    return note;
}

QString AnkiClient::generateMD5(const QString &filename)
{
    QFile file(filename);
    if (file.open(QFile::ReadOnly))
    {
        QCryptographicHash hasher(QCryptographicHash::Md5);
        if (hasher.addData(&file))
        {
            return QString(hasher.result().toHex());
        }
    }
    return "";
}

QString AnkiClient::buildGlossary(const QList<QList<QString>> &definitions)
{
    QString glossary;
    glossary.append("<div style=\"text-align: left;\"><ol>");

    for (size_t i = 0; i < definitions.size(); ++i)
    {
        glossary += "<li>";

        glossary += "<i>(";
        glossary += definitions[i].front();
        glossary += ")</i>";

        glossary += "<ul>";
        for (size_t j = 1; j < definitions[i].size(); ++j)
        {
            glossary += "<li>";
            glossary += definitions[i][j];
            glossary += "</li>";
        }
        glossary += "</ul>";

        glossary += "</li>";
    }

    glossary.append("</ol></div>");

    return glossary;
}

QNetworkReply *AnkiClient::makeRequest(const QString &action,
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
    if (reply->error() == QNetworkReply::NoError)
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
    }
    else
    {
        error = reply->errorString();
    }

    return QJsonObject();
}
