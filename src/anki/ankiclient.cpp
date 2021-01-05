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

#include <QDebug>

#define ANKI_ACTION "action"
#define ANKI_RESULT "result"
#define ANKI_ERROR "error"

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
    QNetworkRequest request;
    request.setUrl(QUrl("http://" + m_address + ":" + m_port));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject jsonMsg;
    jsonMsg[ANKI_ACTION] = "version";
    jsonMsg["version"] = 6;
    QJsonDocument jsonDoc(jsonMsg);

    QNetworkReply *reply = m_manager->post(request, jsonDoc.toJson());
    connect(reply, &QNetworkReply::finished, [=] {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonObject replyObj = replyToJson(reply);
            if (replyObj.empty())
            {
                qDebug() << "Reply from Anki is empty";
                callback(false);
            }
            else if (!replyObj[ANKI_ERROR].isNull())
            {
                qDebug() << "Error reported: " 
                         << replyObj[ANKI_ERROR].toString();
                callback(false);
            }
            else if (replyObj[ANKI_RESULT].toInt() < MIN_ANKICONNECT_VERSION)
            {
                qDebug() << "AnkiConnect version " 
                         << replyObj[ANKI_RESULT].toInt()
                         << " <= "
                         << MIN_ANKICONNECT_VERSION;
                callback(false);
            }
            else 
            {
                qDebug() << "Version number: "
                         << replyObj["result"].toInt();
                callback(true);
            }
        }
        else
        {
            qDebug() << reply->errorString();
            callback(false);
        }
        delete reply;
    });
}

QJsonObject AnkiClient::replyToJson(QNetworkReply *reply)
{
    QJsonDocument replyDoc = QJsonDocument::fromJson(reply->readAll());
    if (!replyDoc.isNull())
    {
        if (replyDoc.isObject())
        {
            return replyDoc.object();
        }
        else
        {
            qDebug() << "Reply was not an object";
        }
    }
    else
    {
        qDebug() << "Reply was not JSON";
        qDebug() << reply->readAll();
    }

    return QJsonObject();
}