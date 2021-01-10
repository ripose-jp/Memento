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

#ifndef ANKICLIENT_H
#define ANKICLIENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>

#include "ankiconfig.h"

class AnkiClient : public QObject
{
    Q_OBJECT

public:
    AnkiClient(QObject *parent);
    ~AnkiClient();
    void setServer(const QString &address, const QString &port);
    AnkiConfig getConfig() const;
    void setConfig(const AnkiConfig &config);
    void testConnection(
        std::function<void(const bool, const QString &)> callback);
    void getDeckNames(
        std::function<void(const QStringList *, const QString &)> callback);
    void getModelNames(
        std::function<void(const QStringList *, const QString &)> callback);
    void getFieldNames(
        std::function<void(const QStringList *, const QString &)> callback,
        const QString &model);

private:
    AnkiConfig m_config;
    QString m_address;
    QString m_port;
    
    QNetworkReply *makeRequest(const QString &action,
                               const QJsonObject &params);
    QJsonObject processReply(QNetworkReply *reply, QString &error);
    void requestStringList(
        std::function<void(const QStringList *, const QString &)> callback,
        const QString &action,
        const QJsonObject &params);
    void setSettings(const bool enabled, const QString &address,
                     const QString &port, const QStringList &tags,
                     const QJsonObject &modelConfig);
    
    bool isEnabled() const;
    QString getAddress() const;
    QString getPort() const;
    QStringList getTags() const;
    QJsonObject getModelConfig() const;
};

#endif // ANKICLIENT_H