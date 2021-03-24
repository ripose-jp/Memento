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

#ifndef ANKICLIENT_H
#define ANKICLIENT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonDocument>

#include "ankiconfig.h"

#include "../dict/expression.h"

#define REPLACE_AUDIO "{audio}"
#define REPLACE_AUDIO_MEDIA "{audio-media}"
#define REPLACE_CLOZE_BODY "{cloze-body}"
#define REPLACE_CLOZE_PREFIX "{cloze-prefix}"
#define REPLACE_CLOZE_SUFFIX "{cloze-suffix}"
#define REPLACE_CONTEXT "{context}"
#define REPLACE_DOCUMENT_TITLE "{document-title}"
#define REPLACE_EXPRESSION "{expression}"
#define REPLACE_ALT_EXPRESSION "{expression-alt}"
#define REPLACE_FURIGANA "{furigana}"
#define REPLACE_FURIGANA_PLAIN "{furigana-plain}"
#define REPLACE_GLOSSARY "{glossary}"
#define REPLACE_READING "{reading}"
#define REPLACE_ALT_READING "{reading-alt}"
#define REPLACE_SCREENSHOT "{screenshot}"
#define REPLACE_SCREENSHOT_VIDEO "{screenshot-video}"
#define REPLACE_SENTENCE "{sentence}"
#define REPLACE_TAGS "{tags}"

class QNetworkAccessManager;
class QNetworkReply;

class AnkiReply : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

Q_SIGNALS:
    void finishedBool(const bool value, const QString &error);
    void finishedStringList(const QStringList &value, const QString &error);
    void finishedBoolList(const QList<bool> &value, const QString &error);
    void finishedInt(const int, const QString &error);
};

class AnkiClient : public QObject
{
    Q_OBJECT

public:
    AnkiClient(QObject *parent = nullptr);
    ~AnkiClient();
    
    void setServer(const QString &address, const QString &port);

    QString     getProfile() const;
    QStringList getProfiles() const;
    bool        setProfile(const QString &profile);
    void        addProfile(const QString &profile, const AnkiConfig &config);

    const AnkiConfig             *getConfig(const QString &profile) const;
    QHash<QString, AnkiConfig *> *getConfigs()                      const;
    
    void clearProfiles();

    bool isEnabled() const;
    void setEnabled(const bool value);

    void writeChanges();

    AnkiReply *testConnection();
    AnkiReply *getDeckNames();
    AnkiReply *getModelNames();
    AnkiReply *getFieldNames(const QString &model);
    AnkiReply *termsAddable (const QList<Term *> *terms);
    AnkiReply *addTerm      (const Term *term);

Q_SIGNALS:
    void sendIntRequest(const QString &action, const QJsonObject &params, AnkiReply *ankiReply);

private Q_SLOTS:
    void recieveIntRequest(const QString &action, const QJsonObject &params, AnkiReply *ankiReply);

private:
    bool m_enabled;

    QHash<QString, const AnkiConfig *> *m_configs;
    const AnkiConfig *m_currentConfig;
    QString m_currentProfile;

    QString m_address;
    QString m_port;

    QNetworkAccessManager *m_manager;
    
    void readConfigFromFile(const QString &filename);
    bool writeConfigToFile (const QString &filename);

    QNetworkReply *makeRequest(const QString &action, const QJsonObject &params = QJsonObject());

    QJsonObject processReply(QNetworkReply *reply, QString &error);

    AnkiReply *requestStringList(const QString &action, const QJsonObject &params = QJsonObject());

    QJsonObject createAnkiNoteObject(const Term &term, const bool media = false);

    QString buildGlossary(const QList<TermDefinition> &definitions);

    QString generateMD5(const QString &filename);
};

#endif // ANKICLIENT_H