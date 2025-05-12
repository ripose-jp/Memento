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

#include <memory>

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QStringList>

#include <qcoro/network/qcoronetworkreply.h>
#include <qcoro/qcorotask.h>

#include "anki/ankiconfig.h"
#include "anki/glossarybuilder.h"
#include "anki/markertokenizer.h"
#include "dict/expression.h"

/* Default AnkiConfig Values */
static constexpr const char *DEFAULT_PROFILE = "Default";
static constexpr const char *DEFAULT_HOST = "localhost";
static constexpr const char *DEFAULT_PORT = "8765";
static constexpr AnkiConfig::FileType DEFAULT_SCREENSHOT = AnkiConfig::FileType::jpg;
static constexpr AnkiConfig::DuplicatePolicy DEFAULT_DUPLICATE_POLICY = AnkiConfig::DuplicatePolicy::DifferentDeck;
static constexpr const char *DEFAULT_NEWLINE_REPLACER = "<br>";
static constexpr const char *DEFAULT_TAGS = "memento";
static constexpr double DEFAULT_AUDIO_PAD_START = 0.0;
static constexpr double DEFAULT_AUDIO_PAD_END = 0.0;
static constexpr bool DEFAULT_AUDIO_NORMALIZE = false;
static constexpr double DEFAULT_AUDIO_DB = -20.0;

/**
 * Contains information about an AnkiConnect reply.
 */
template<typename T>
struct AnkiReply
{
    /* The data returned by Anki */
    T value{};

    /* An error string describing what went wrong */
    QString error;
};

class AnkiClient : public QObject
{
    Q_OBJECT

public:
    AnkiClient(QObject *parent = nullptr);

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
     * @return A reply containing true if a connection was established, false
     *         otherwise.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<bool>> testConnection();

    /**
     * Gets the names of all Anki decks.
     * @return An AnkiReply that emits the finishedStringList() signal. Deck
     *         names are values in the list. Caller does not have ownership.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QStringList>> getDeckNames();

    /**
     * Gets the names of all Anki models.
     * @return An AnkiReply containing a string list on success, error message
     *         on failure.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QStringList>> getModelNames();

    /**
     * Gets the field names for the model.
     * @param model The name of the model.
     * @return An AnkiReply containing a string list on success, error message
     *         on failure.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QStringList>> getFieldNames(const QString &model);

    /**
     * Gets if the list of terms are addable given the current configuration.
     * @param terms The list of terms to check if are addable.
     * @return An AnkiReply containing a boolean list of addable terms. Each
     *         term is represented by two booleans. The first determines if the
     *         expression is addable, the second if the reading is addable.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QList<bool>>> notesAddable(
        QList<QSharedPointer<const Term>> terms);

    /**
     * Gets if the list of kanji are addable given the current configuration.
     * @param kanji The list of kanji to check if are addable.
     * @return An AnkiReply containing a boolean list of addable terms. The
     *         kanji at the same index is addable if true, not addable
     *         otherwise.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QList<bool>>> notesAddable(
        QList<QSharedPointer<const Kanji>> kanji);

    /**
     * Adds a term note to Anki.
     * @param term The term to add a note for.
     * @return An AnkiReply containing the ID of the added card. An error string
     *         otherwise.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<int>> addNote(std::unique_ptr<const Term> term);

    /**
     * Adds a kanji note to Anki.
     * @param kanji The kanji to add a note for.
     * @return An AnkiReply containing the ID of the added card. An error string
     *         otherwise.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<int>> addNote(std::unique_ptr<const Kanji> kanji);

    /**
     * Adds media file to Anki.
     * @param files A mapping of file paths to file names.
     * @return An AnkiReply containing a list of added filenames. An error
     *         string on failure.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QStringList>> addMedia(
        QList<GlossaryBuilder::FileInfo> fileMap);

    /**
     * Opens an Anki GUI window searching for a query in a deck.
     * @param deck  The name of the deck to search in.
     * @param query The query to search.
     * @return An AnkiReply containing a list of card IDs on success, an error
     *         string otherwise.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QList<int>>> openBrowse(
        const QString &deck, const QString &query);

    /**
     * Opens a window for all cards that might be detected as duplicates for the
     * given term.
     * @param term The term to check for duplicates.
     * @return The IDs of the cards that may be duplicates.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QList<int>>> openDuplicates(
        std::unique_ptr<Term> term);

    /**
     * Opens a window for all cards that might be detected as duplicates for the
     * given kanji.
     * @param kanji The kanji to check for duplicates.
     * @return The IDs of the cards that may be duplicates.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QList<int>>> openDuplicates(
        std::unique_ptr<Kanji> kanji);

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
     * Makes a request to AnkiConnect that returns a list of strings.
     * @param action The AnkiConnect 'verb'.
     * @param params The parameters of the action if applicable.
     * @return An AnkiReply containing the string list on success, error message
     *         on error.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QStringList>> requestStringList(
        const QString &action,
        const QJsonObject &params = QJsonObject());

    /**
     * Issues a command to Anki that returns a list of bools.
     * @param action The AnkiConnect 'verb'.
     * @param params Parameters for the action.
     * @return An AnkiReply containing the bool list on success, error message
     *         on error.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<QList<bool>>> requestBoolList(
        const QString &action,
        const QJsonObject &params = QJsonObject());

    /**
     * Issues a command to Anki that returns an integer.
     * @param action The AnkiConnect 'verb'.
     * @param params Parameters for the action.
     * @return An AnkiReply containing the int on success, error message on
     *         error.
     */
    [[nodiscard]]
    QCoro::Task<AnkiReply<int>> requestInt(
        const QString &action,
        const QJsonObject &params = QJsonObject());

    /**
     * Makes a request to AnkiConnect.
     * @param action The AnkiConnection 'verb' to execute.
     * @param params The parameters of the action.
     * @return A QNetworkReply where the result will be received.
     */
    [[nodiscard]]
    QCoro::Task<std::unique_ptr<QNetworkReply>> makeRequest(
        const QString &action,
        const QJsonObject &params = QJsonObject());

    /**
     * Error checks the AnkiConnect reply.
     * @param      reply The reply to error check.
     * @param[out] error The reason the command failed. Empty string if no
     *                   error.
     * @return The JSON object AnkiConnect replied with.
     */
    [[nodiscard]]
    QJsonObject processReply(QNetworkReply &reply, QString &error);

    /* The Network Manager for this object. */
    QNetworkAccessManager m_manager;

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
};

#endif // ANKICLIENT_H
