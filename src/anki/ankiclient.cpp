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
#define ANKI_ACTION_VERSION "version"

#define MIN_ANKICONNECT_VERSION 6

QNetworkAccessManager *m_manager;

AnkiClient::AnkiClient(QObject *parent) : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
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

void AnkiClient::testConnection(void (*callback)(const bool))
{
    QNetworkReply *reply = makeRequest(ANKI_ACTION_VERSION);
    connect(reply, &QNetworkReply::finished, [=] {
        QJsonObject replyObj = processReply(reply);
        callback(!replyObj.isEmpty());
        delete reply;
    });
}

void AnkiClient::getDeckNames(void (*callback)(const QList<QString> *))
{
    QNetworkReply *reply = makeRequest(ANKI_DECK_NAMES);
    connect(reply, &QNetworkReply::finished, [=] {
        QJsonObject replyObj = processReply(reply);
        if (replyObj.isEmpty()) 
        {
            callback(0);
        }
        else 
        {
            QList<QString> *decks = new QList<QString>();
            QJsonArray deckNames = replyObj[ANKI_RESULT].toArray();
            for (auto it = deckNames.begin(); it != deckNames.end(); ++it)
                decks->append((*it).toString());
            callback(decks);
        }
        delete reply;
    });
}

QNetworkReply *AnkiClient::makeRequest(const QString &action,
                                       const QJsonObject &params = QJsonObject())
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

QJsonObject AnkiClient::processReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument replyDoc = QJsonDocument::fromJson(reply->readAll());
        if (replyDoc.isNull())
        {
            qDebug() << "Reply was not JSON";
            qDebug() << reply->readAll();
            return QJsonObject();
        }
        else if (!replyDoc.isObject())
        {
            qDebug() << "Reply was not an object";
            return QJsonObject();
        }

        QJsonObject replyObj = replyDoc.object();
        if (replyObj.length() != 2)
        {
            qDebug() << "Anki response has unexpected number of fields";
        }
        else if (!replyObj.contains(ANKI_ERROR))
        {
            qDebug() << "Anki response is missing error field";
        }
        else if (!replyObj.contains(ANKI_RESULT))
        {
            qDebug() << "Anki response is missing result field";
        }
        else if (!replyObj[ANKI_ERROR].isNull())
        {
            qDebug() << "Error reported:" 
                     << replyObj[ANKI_ERROR].toString();
        }
        else if (replyObj[ANKI_RESULT].toInt() < MIN_ANKICONNECT_VERSION)
        {
            qDebug() << "AnkiConnect version" 
                     << replyObj[ANKI_RESULT].toInt()
                     << "<"
                     << MIN_ANKICONNECT_VERSION;
        }
        else 
        {
            return replyObj;
        }
    }
    else
    {
        qDebug() << reply->errorString();
    }

    return QJsonObject();
}
