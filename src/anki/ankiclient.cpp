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

// Anki param fields
#define ANKI_PARAM_MODEL_NAME "modelName"

#define MIN_ANKICONNECT_VERSION 6
#define TIMEOUT 5000

#define CONFIG_FILE "anki_connect.json"

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

void AnkiClient::testConnection(
    std::function<void(const bool, const QString &)> callback)
{
    QNetworkReply *reply = makeRequest(ANKI_ACTION_VERSION, QJsonObject());
    connect(reply, &QNetworkReply::finished, [=] {
        QString error;
        QJsonObject replyObj = processReply(reply, error);
        if (replyObj.isEmpty())
        {
            callback(false, error);
        }
        else if (!replyObj[ANKI_RESULT].isDouble())
        {
            error = "AnkiConnect result is not a number";
            callback(false, error);
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
    requestStringList(callback, ANKI_DECK_NAMES, QJsonObject());
}

void AnkiClient::getModelNames(
    std::function<void(const QStringList *, const QString &)> callback)
{
    requestStringList(callback, ANKI_MODEL_NAMES, QJsonObject());
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
        if (replyObj.isEmpty() || !replyObj[ANKI_RESULT].isArray()) 
        {
            callback(0, error);
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
