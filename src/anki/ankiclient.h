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

/* Shared Markers */
#define REPLACE_AUDIO_MEDIA         "{audio-media}"
#define REPLACE_CLOZE_BODY          "{cloze-body}"
#define REPLACE_CLOZE_PREFIX        "{cloze-prefix}"
#define REPLACE_CLOZE_SUFFIX        "{cloze-suffix}"
#define REPLACE_CONTEXT             "{context}"
#define REPLACE_FREQUENCIES         "{frequencies}"
#define REPLACE_GLOSSARY            "{glossary}"
#define REPLACE_SCREENSHOT          "{screenshot}"
#define REPLACE_SCREENSHOT_VIDEO    "{screenshot-video}"
#define REPLACE_SENTENCE            "{sentence}"
#define REPLACE_TAGS                "{tags}"
#define REPLACE_TITLE               "{title}"


/* Term Markers */
#define REPLACE_AUDIO               "{audio}"
#define REPLACE_EXPRESSION          "{expression}"
#define REPLACE_FURIGANA            "{furigana}"
#define REPLACE_FURIGANA_PLAIN      "{furigana-plain}"
#define REPLACE_GLOSSARY_BRIEF      "{glossary-brief}"
#define REPLACE_PITCH               "{pitch}"
#define REPLACE_PITCH_GRAPHS        "{pitch-graph}"
#define REPLACE_PITCH_POSITIONS     "{pitch-position}"
#define REPLACE_READING             "{reading}"

/* Kanji Markers */
#define REPLACE_CHARACTER           "{character}"
#define REPLACE_KUNYOMI             "{kunyomi}"
#define REPLACE_ONYOMI              "{onyomi}"
#define REPLACE_STROKE_COUNT        "{stroke-count}"

/* Default AnkiConfig Values */
#define DEFAULT_PROFILE                 "Default"
#define DEFAULT_HOST                    "localhost"
#define DEFAULT_PORT                    "8765"
#define DEFAULT_SCREENSHOT              AnkiConfig::FileType::jpg
#define DEFAULT_DUPLICATE_POLICY        AnkiConfig::DuplicatePolicy::DifferentDeck
#define DEFAULT_TAGS                    "memento"
#define DEFAULT_AUDIO_URL               (QString("http://assets.languagepod101.com/dictionary/japanese/audiomp3.php?kanji={expression}&kana={reading}"))
#define DEFAULT_AUDIO_HASH              "7e2c2f954ef6051373ba916f000168dc"
#define DEFAULT_AUDIO_PAD_START         0.0
#define DEFAULT_AUDIO_PAD_END           0.0

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
    void finishedInt(const int value, const QString &error);
    void finishedIntList(const QList<int> &value, const QString &error);
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
    const AnkiConfig             *getConfig()                       const;
    QHash<QString, AnkiConfig *> *getConfigs()                      const;
    
    void clearProfiles();

    bool isEnabled() const;
    void setEnabled(const bool value);

    void writeChanges();

    AnkiReply *testConnection();
    AnkiReply *getDeckNames();
    AnkiReply *getModelNames();
    AnkiReply *getFieldNames(const QString &model);
    AnkiReply *notesAddable (const QList<Term *>        &terms);
    AnkiReply *notesAddable (const QList<const Kanji *> &kanjiList);
    AnkiReply *addNote      (const Term  *term);
    AnkiReply *addNote      (const Kanji *kanji);
    AnkiReply *openBrowse   (const QString &deck, const QString &query);

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
    
    bool readConfigFromFile(const QString &filename);
    bool writeConfigToFile (const QString &filename);

    void setDefaultConfig();

    QNetworkReply *makeRequest(const QString &action, const QJsonObject &params = QJsonObject());

    QJsonObject processReply(QNetworkReply *reply, QString &error);

    AnkiReply *requestStringList(const QString &action, const QJsonObject &params = QJsonObject());

    QJsonObject createAnkiNoteObject(const Term  &term,  const bool media = false);
    QJsonObject createAnkiNoteObject(const Kanji &kanji, const bool media = false);

    void     buildCommonNote (QJsonObject                  &note,
                              QJsonObject                  &fieldObj, 
                              const QJsonObject            &configFields,
                              const bool                    media);
    QString  buildFrequencies(const QList<Frequency>       &freq);
    void     buildPitchInfo  (const QList<Pitch>           &pitches,
                              QString &pitch, QString &pitchGraph, QString &pitchPosition);
    QString  buildGlossary   (const QList<TermDefinition>  &definitions);
    QString  buildGlossary   (const QList<KanjiDefinition> &definitions);
    QString &accumulateTags  (const QList<Tag>             &tags,         
                              QString                      &tagStr);

    QString generateMD5(const QString &filename);
};

#endif // ANKICLIENT_H