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

#include "../dict/entry.h"
#include "../gui/playeradapter.h"

#define REPLACE_AUDIO "{audio}"
#define REPLACE_AUDIO_MEDIA "{audio-media}"
#define REPLACE_CLOZE_BODY "{cloze-body}"
#define REPLACE_CLOZE_PREFIX "{cloze-prefix}"
#define REPLACE_CLOZE_SUFFIX "{cloze-suffix}"
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

class QNetworkReply;
class QNetworkAccessManager;

class AnkiClient : public QObject
{
    Q_OBJECT

public:
    AnkiClient(QObject *parent, PlayerAdapter *player);
    ~AnkiClient();
    
    void setServer(const QString &address, const QString &port);

    QString getProfile() const;
    QStringList getProfiles() const;
    const AnkiConfig *getConfig(const QString &profile) const;
    QHash<QString, AnkiConfig *> *getConfigs() const;
    bool setProfile(const QString &profile);
    void addProfile(const QString &profile, const AnkiConfig &config);
    void clearProfiles();

    bool isEnabled() const;
    void setEnabled(const bool value);

    void writeChanges();

    void testConnection(
        std::function<void(const bool, const QString &)> callback);
    void getDeckNames(
        std::function<void(const QStringList *, const QString &)> callback);
    void getModelNames(
        std::function<void(const QStringList *, const QString &)> callback);
    void getFieldNames(
        std::function<void(const QStringList *, const QString &)> callback,
        const QString &model);
    void entriesAddable(
        std::function<void(const QList<bool> *, const QString &)> callback,
        const QList<Entry *> *entries);
    void addEntry(
        std::function<void(const int, const QString &)> callback,
        const Entry *entry);

Q_SIGNALS:
    void settingsChanged() const;

private:
    bool m_enabled;

    QHash<QString, const AnkiConfig *> *m_configs;
    const AnkiConfig *m_currentConfig;
    QString m_currentProfile;

    QString m_address;
    QString m_port;

    PlayerAdapter *m_player;

    QNetworkAccessManager *m_manager;
    
    void readConfigFromFile(const QString &filename);
    bool writeConfigToFile(const QString &filename);
    QNetworkReply *makeRequest(const QString &action,
                               const QJsonObject &params = QJsonObject());
    QJsonObject processReply(QNetworkReply *reply, QString &error);
    void requestStringList(
        std::function<void(const QStringList *, const QString &)> callback,
        const QString &action,
        const QJsonObject &params = QJsonObject());
    QJsonObject createAnkiNoteObject(const Entry &entry,
                                     const bool media = false);
    QString buildGlossary(const QList<QList<QString>> &definitions);
    QString generateMD5(const QString &filename);
};

#endif // ANKICLIENT_H