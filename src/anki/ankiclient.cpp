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

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonArray>
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

QNetworkAccessManager *m_manager;

AnkiClient::AnkiClient(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
    m_manager->setTransferTimeout(TIMEOUT);
}

AnkiClient::~AnkiClient()
{
    delete m_manager;
}

void AnkiClient::setServer(const QString &address, const QString &port)
{
    m_address = address;
    m_port = port;
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
