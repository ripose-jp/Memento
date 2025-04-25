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

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QStringList>

#include "ankiconfig.h"
#include "markertokenizer.h"

#include "anki/glossarybuilder.h"
#include "dict/expression.h"

/* Default AnkiConfig Values */
#define DEFAULT_PROFILE                 "Default"
#define DEFAULT_HOST                    "localhost"
#define DEFAULT_PORT                    "8765"
#define DEFAULT_SCREENSHOT              AnkiConfig::FileType::jpg
#define DEFAULT_DUPLICATE_POLICY        AnkiConfig::DuplicatePolicy::DifferentDeck
#define DEFAULT_NEWLINE_REPLACER        "<br>"
#define DEFAULT_TAGS                    "memento"
#define DEFAULT_AUDIO_PAD_START         0.0
#define DEFAULT_AUDIO_PAD_END           0.0
#define DEFAULT_AUDIO_NORMALIZE         false
#define DEFAULT_AUDIO_DB                (-20.0)

class QNetworkAccessManager;
class QNetworkReply;

/**
 * Object used for sending replies to from one thread to another.
 */
class AnkiReply : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;

/* Overloaded signals suck to use, so this is the best alternative. */
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

    /**
     * Destructor for all allocated AnkiConfigs.
     */
    void clearProfiles();

    /**
     * Writes the current configuration to the configuration file.
     */
    void writeChanges();

    /**
     * Gets the name of the current profile.
     * @return The name of the current profile.
     */
    QString getProfile() const;

    /**
     * Gets the names of all the profiles.
     * @return A list of all the profiles.
     */
    QStringList getProfiles() const;

    /**
     * Changes to the current profile.
     * @param profile The name of the profile to switch to.
     * @return true if the profile exists and was set, false otherwise.
     */
    bool setProfile(const QString &profile);

    /**
     * Adds a profile. Replace the old profile if it existed.
     * @param profile The name of the profile to add.
     * @param config  The configuration of the profile.
     */
    void addProfile(const QString &profile, const AnkiConfig &config);

    /**
     * Gets the AnkiConfig corresponding to the profile.
     * @param profile The name of the profile.
     * @return The config corresponding to the profile, nullptr if it doesn't
     *         exist.
     */
    QSharedPointer<const AnkiConfig> getConfig(const QString &profile) const;

    /**
     * Gets the current active AnkiConfig.
     * @return The current AnkiConfig.
     */
    QSharedPointer<const AnkiConfig> getConfig() const;

    /**
     * Gets a mapping of all the profiles to their corresponding configuration.
     * AnkiConfigs are deep copied.
     * @return A map that maps profile names to their AnkiConfigs.
     */
    QHash<QString, QSharedPointer<AnkiConfig>> getConfigs() const;

    /**
     * Gets if a configuration exists.
     * @return true if a config file exists, false otherwise.
     */
    bool configExists() const;

    /**
     * Gets if the AnkiClient is enabled.
     * @return true if enabled, false otherwise.
     */
    bool isEnabled() const;

    /**
     * Sets the enabled value of the AnkiClient.
     * @param value true if enabled, false otherwise.
     */
    void setEnabled(const bool value);

    /**
     * Sets the current AnkiConnect server.
     * @param address The hostname of the server.
     * @param port    The port of the server.
     */
    void setServer(const QString &address, const QString &port);

    /**
     * Tests the connection to the server.
     * @return An AnkiReply that emits the finishedBool() signal. Caller does
     *         not have ownership. Emits true if connection was successful,
     *         false and an error string otherwise.
     */
    AnkiReply *testConnection();

    /**
     * Gets the names of all Anki decks.
     * @return An AnkiReply that emits the finishedStringList() signal. Deck
     *         names are values in the list. Caller does not have ownership.
     */
    AnkiReply *getDeckNames();

    /**
     * Gets the names of all Anki models.
     * @return An AnkiReply that emits the finishedStringList() signal. Model
     *         names are values in the list. Caller does not have ownership.
     */
    AnkiReply *getModelNames();

    /**
     * Gets the field names for the model.
     * @param model The name of the model.
     * @return An AnkiReply that emits the finishedStringList() signal. Fields
     *         are values in the list. Caller does not have ownership.
     */
    AnkiReply *getFieldNames(const QString &model);

    /**
     * Gets if the list of terms are addable given the current configuration.
     * @param terms The list of terms to check if are addable.
     * @return An AnkiReply that emits the finishedBoolList() signal. The term
     *         at the same index is addable if true, not addable otherwise.
     *         Caller does not have ownership.
     */
    AnkiReply *notesAddable(QList<QSharedPointer<const Term>> terms);

    /**
     * Gets if the list of kanji are addable given the current configuration.
     * @param kanji The list of kanji to check if are addable.
     * @return An AnkiReply that emits the finishedBoolList() signal. The term
     *         at the same index is addable if true, not addable otherwise.
     *         Caller does not have ownership.
     */
    AnkiReply *notesAddable(QList<QSharedPointer<const Kanji>> kanji);

    /**
     * Adds a term note to Anki.
     * @param term The term to add a note for.
     * @return An AnkiReply that emits the finishedInt() signal. The integer
     *         value is the identifer of the card. Caller does not have
     *         ownership.
     */
    AnkiReply *addNote(const Term *term);

    /**
     * Adds a kanji note to Anki.
     * @param kanji The kanji to add a note for.
     * @return An AnkiReply that emits the finishedInt() signal. The integer
     *         value is the identifer of the card. Caller does not have
     *         ownership.
     */
    AnkiReply *addNote(const Kanji *kanji);

    /**
     * Opens an Anki GUI window searching for a query in a deck.
     * @param deck  The name of the deck to search in.
     * @param query The query to search.
     * @return An AnkiReply that emits the finishedIntList() signal. Integer
     *         values are card IDs.
     */
    AnkiReply *openBrowse(const QString &deck, const QString &query);

public Q_SLOTS:
    /**
     * Adds media file to Anki.
     * @param files A mapping of file paths to file names.
     * @return An AnkiReply that emits the finishedStringList() signal. Strings
     *         are added filenames.
     */
    AnkiReply *addMedia(const QList<GlossaryBuilder::FileInfo> &fileMap);

Q_SIGNALS:
    /**
     * Sends an add media request.
     * This is a hack to get the media request on the main thread.
     * @param fileMap The filemap to add.
     */
    void requestAddMedia(const QList<GlossaryBuilder::FileInfo> &fileMap) const;

    /**
     * Sends a request to issue a command to Anki that returns an integer.
     * This is a hack to make sure that the QNetworkAccessManager is used from
     * the correct thread when multithreading.
     * @param action    The AnkiConnect 'verb'.
     * @param params    Parameters for the action.
     * @param ankiReply The AnkiReply to emit the finishedInt() signal on.
     */
    void sendIntRequest(const QString     &action,
                        const QJsonObject &params,
                        AnkiReply         *ankiReply);

    /**
     * Sends a request to issue a command to Anki that returns a list of
     * booleans.
     * This is a hack to make sure that the QNetworkAccessManager is used from
     * the correct thread when multithreading.
     * @param action    The AnkiConnect 'verb'.
     * @param params    Parameters for the action.
     * @param ankiReply The AnkiReply to emit the finishedBoolList() signal on.
     */
    void sendBoolListRequest(const QString     &action,
                             const QJsonObject &params,
                             AnkiReply         *ankiReply);

private Q_SLOTS:
    /**
     * Issues a command to Anki that returns an integer.
     * This is a hack to make sure that the QNetworkAccessManager is used from
     * the correct thread when multithreading.
     * @param action    The AnkiConnect 'verb'.
     * @param params    Parameters for the action.
     * @param ankiReply The AnkiReply to emit the finishedInt() signal on.
     */
    void receiveIntRequest(const QString     &action,
                           const QJsonObject &params,
                           AnkiReply         *ankiReply);

    /**
     * Issues a command to Anki that returns a list of bools.
     * This is a hack to make sure that the QNetworkAccessManager is used from
     * the correct thread when multithreading.
     * @param action    The AnkiConnect 'verb'.
     * @param params    Parameters for the action.
     * @param ankiReply The AnkiReply to emit the finishedBoolList() signal on.
     */
    void receiveBoolListRequest(const QString     &action,
                                const QJsonObject &params,
                                AnkiReply         *ankiReply);

private:
    /**
     * Loads an Anki Integration configuration file.
     * @param filename The name of the configuration file in the config
     *                 directory.
     * @return true on success, false on error.
     */
    bool readConfigFromFile(const QString &filename);

    /**
     * Writes the current configuration to a configuration file.
     * @param filename The name of the configuration file in the config
     *                 directory.
     * @return true on success, false on error.
     */
    bool writeConfigToFile(const QString &filename);

    /**
     * Sets the current configuration to be the default configuration. Overrides
     * the "Default" profile if it has been modified.
     */
    void setDefaultConfig();

    /**
     * Makes a request to AnkiConnect.
     * @param action The AnkiConnection 'verb' to execute.
     * @param params The parameters of the action.
     * @return A QNetworkReply where the result will be received. Caller takes
     *         ownership.
     */
    QNetworkReply *makeRequest(const QString     &action,
                               const QJsonObject &params = QJsonObject());

    /**
     * Error checks the AnkiConnect reply.
     * @param reply      The reply to error check.
     * @param[out] error The reason the command failed. Empty string if no
     *                   error.
     * @return The JSON object AnkiConnect replied with.
     */
    QJsonObject processReply(QNetworkReply *reply, QString &error);

    /**
     * Makes a request to AnkiConnect that returns a list of strings.
     * @param action The AnkiConnect 'verb'.
     * @param params The parameters of the action if applicable.
     * @return An AnkiReply that emits the finishedStringList signal. Caller
     *         does not take ownership.
     */
    AnkiReply *requestStringList(const QString     &action,
                                 const QJsonObject &params = QJsonObject());

    /* true if a config exists, false otherwise */
    bool m_configExists = false;

    /* The enabled state of the AnkiClient. */
    bool m_enabled = false;

    /* A mapping of profile names to their configurations. */
    QHash<QString, QSharedPointer<const AnkiConfig>> m_configs;

    /* The reference to the active AnkiConfig. */
    QSharedPointer<const AnkiConfig> m_currentConfig = nullptr;

    /* The name of the active profile. */
    QString m_currentProfile;

    /* The current AnkiConnect hostname. */
    QString m_address;

    /* The current AnkiConnect port. */
    QString m_port;

    /* The Network Manager for this object. */
    QNetworkAccessManager *m_manager;
};

#endif // ANKICLIENT_H
