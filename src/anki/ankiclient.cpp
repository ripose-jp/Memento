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

#include "ankiclient.h"

#include "../util/directoryutils.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonArray>
#include <QFile>
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
#define ANKI_NOTE_AUDIO_URL "url"
#define ANKI_NOTE_AUDIO_FILENAME "filename"
#define ANKI_NOTE_AUDIO_FIELDS "fields"
#define ANKI_NOTE_AUDIO_SKIPHASH "skipHash"

#define MIN_ANKICONNECT_VERSION 6
#define TIMEOUT 5000
#define CONFIG_FILE "anki_connect.json"
#define FURIGANA_FORMAT_STRING (QString("<ruby>%1<rt>%2</rt></ruby>"))
#define AUDIO_URL_FORMAT_STRING (QString("http://assets.languagepod101.com/dictionary/japanese/audiomp3.php?kanji=%1&kana=%2"))
#define JAPANESE_POD_STUB_MD5 "7e2c2f954ef6051373ba916f000168dc"
#define AUDIO_FILENAME_FORMAT_STRING (QString("memento_%1_%2.mp3"))

// Config file fields
#define CONFIG_ENABLED "enabled"
#define CONFIG_HOST "host"
#define CONFIG_PORT "port"
#define CONFIG_TAGS "tags"
#define CONFIG_DECK "deck"
#define CONFIG_MODEL "model"
#define CONFIG_FIELDS "fields"

QNetworkAccessManager *m_manager;

AnkiClient::AnkiClient(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    m_manager->setTransferTimeout(TIMEOUT);

    readConfigFromFile(CONFIG_FILE);
}

AnkiClient::~AnkiClient()
{
    delete m_manager;
}

void AnkiClient::readConfigFromFile(const QString &filename)
{
    QFile configFile(DirectoryUtils::getConfigDir() + filename);
    if (configFile.exists())
    {
        if (!configFile.open(QIODevice::ReadOnly))
        {
            qDebug() << "AnkiConnect config file exists, can't open it";
        }
        else
        {
            QTextStream stream(&configFile);
            QJsonDocument jsonDoc = QJsonDocument::fromJson(
                stream.readAll().toLocal8Bit());
            configFile.close();

            if (jsonDoc.isNull())
            {
                qDebug() << filename << "is not JSON";
            }
            else if (!jsonDoc.isObject())
            {
                qDebug() << filename << "is not an object";
            }
            else
            {
                QJsonObject jsonObj = jsonDoc.object();
                if (!jsonObj[CONFIG_ENABLED].isBool())
                {
                    qDebug() << CONFIG_ENABLED << "is not a boolean";
                }
                else if (!jsonObj[CONFIG_HOST].isString())
                {
                    qDebug() << CONFIG_HOST << "is not a string";
                }
                else if (!jsonObj[CONFIG_PORT].isString())
                {
                    qDebug() << CONFIG_PORT << "is not a string";
                }
                else if (!jsonObj[CONFIG_TAGS].isArray())
                {
                    qDebug() << CONFIG_TAGS << "is not an array";
                }
                else if (!jsonObj[CONFIG_DECK].isString())
                {
                    qDebug() << CONFIG_DECK << "is not a string";
                }
                else if (!jsonObj[CONFIG_MODEL].isString())
                {
                    qDebug() << CONFIG_MODEL << "is not a string";
                }
                else if (!jsonObj[CONFIG_FIELDS].isObject())
                {
                    qDebug() << CONFIG_FIELDS << "is not an object";
                }
                else
                {
                    m_config.enabled = jsonObj[CONFIG_ENABLED].toBool();
                    m_config.address = jsonObj[CONFIG_HOST].toString();
                    m_config.port = jsonObj[CONFIG_PORT].toString();
                    QJsonArray tagArray = jsonObj[CONFIG_TAGS].toArray();
                    for (auto it = tagArray.begin(); it != tagArray.end(); ++it)
                    {
                        m_config.tags.append(it->toString());
                    }
                    m_config.deck = jsonObj[CONFIG_DECK].toString();
                    m_config.model = jsonObj[CONFIG_MODEL].toString();
                    m_config.fields = jsonObj[CONFIG_FIELDS].toObject();
                    setServer(m_config.address, m_config.port);
                }
            }
        }
    }
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
    jsonObj[CONFIG_ENABLED] = m_config.enabled;
    jsonObj[CONFIG_HOST] = m_config.address;
    jsonObj[CONFIG_PORT] = m_config.port;
    QJsonArray jsonTags;
    for (auto it = m_config.tags.begin(); it != m_config.tags.end(); ++it)
    {
        jsonTags.append(QJsonValue(*it));
    }
    jsonObj[CONFIG_TAGS] = jsonTags;
    jsonObj[CONFIG_DECK] = m_config.deck;
    jsonObj[CONFIG_MODEL] = m_config.model;
    jsonObj[CONFIG_FIELDS] = m_config.fields;
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

AnkiConfig AnkiClient::getConfig() const
{
    return m_config;
}

void AnkiClient::setConfig(const AnkiConfig &config)
{
    m_config = config;
    setServer(config.address, config.port);
    writeConfigToFile(CONFIG_FILE);
}

bool AnkiClient::isEnabled() const
{
    return m_config.enabled;
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
    params[ANKI_ADD_NOTE_PARAM] = createAnkiNoteObject(*entry);
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

QJsonObject AnkiClient::createAnkiNoteObject(const Entry &entry)
{
    QJsonObject note;
    note[ANKI_NOTE_DECK] = m_config.deck;
    note[ANKI_NOTE_MODEL] = m_config.model;

    // Processed fields
    QString audioFile = AUDIO_FILENAME_FORMAT_STRING.arg(*entry.m_kana)
                                                    .arg(*entry.m_kanji);
    QString furigana = FURIGANA_FORMAT_STRING.arg(*entry.m_kanji)
                                             .arg(*entry.m_kana);
    QString furiganaPlain;
    if (entry.m_kanji->isEmpty())
        furiganaPlain = *entry.m_kana;
    else
        furiganaPlain = *entry.m_altkanji + "[" + *entry.m_kana + "]";
    QString glossary = buildGlossary(*entry.m_descriptions);

    // Find and replace fields with processed fields
    QStringList fields = m_config.fields.keys();
    QJsonArray fieldsWithAudio;
    QJsonObject fieldsObj;
    for (auto it = fields.begin(); it != fields.end(); ++it)
    {
        QString value = m_config.fields[*it].toString();

        if (value.contains(REPLACE_AUDIO))
        {
            fieldsWithAudio.append(*it);
        }
        value = value.replace(REPLACE_AUDIO, "");
        value = value.replace(REPLACE_CLOZE_BODY, *entry.m_clozeBody);
        value = value.replace(REPLACE_CLOZE_PREFIX, *entry.m_clozePrefix);
        value = value.replace(REPLACE_CLOZE_SUFFIX, *entry.m_clozeSuffix);
        value = value.replace(
            REPLACE_EXPRESSION, 
            entry.m_kanji->isEmpty() ? *entry.m_kana : *entry.m_kanji);
        value = value.replace(REPLACE_ALT_EXPRESSION, *entry.m_altkanji);
        value = value.replace(REPLACE_FURIGANA, furigana);
        value = value.replace(REPLACE_FURIGANA_PLAIN, furiganaPlain);
        value = value.replace(REPLACE_GLOSSARY, glossary);
        value = value.replace(REPLACE_READING, *entry.m_kana);
        value = value.replace(REPLACE_ALT_READING, *entry.m_altkana);
        value = value.replace(REPLACE_SENTENCE, *entry.m_sentence);

        fieldsObj[*it] = value;
    }
    note[ANKI_NOTE_FIELDS] = fieldsObj;
    
    QJsonArray tags;
    for (auto it = m_config.tags.begin(); it != m_config.tags.end(); ++it)
        tags.append(*it);
    note[ANKI_NOTE_TAGS] = tags;

    if (!fieldsWithAudio.isEmpty())
    {
        QJsonObject audio;
        audio[ANKI_NOTE_AUDIO_URL] = 
            AUDIO_URL_FORMAT_STRING.arg(*entry.m_kanji).arg(*entry.m_kana);
        audio[ANKI_NOTE_AUDIO_FILENAME] = audioFile;
        audio[ANKI_NOTE_AUDIO_FIELDS] = fieldsWithAudio;
        audio[ANKI_NOTE_AUDIO_SKIPHASH] = JAPANESE_POD_STUB_MD5;
        note[ANKI_NOTE_AUDIO] = audio;
    }
    
    return note;
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
    if (reply->error() == QNetworkReply::NoError) {
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
