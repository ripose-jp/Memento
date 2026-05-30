////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
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

#pragma once

#include <QObject>

#include <QJsonArray>
#include <QJsonObject>
#include <QSet>
#include <QString>

#include "anki/ankiconfigkeys.h"
#include "anki/ankifieldlistmodel.h"

/**
 * @brief Model for a single Anki profile.
 */
class AnkiProfile : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QString name
        READ name
        WRITE setName
        NOTIFY nameChanged
    )

    Q_PROPERTY(
        QString hostname
        READ hostname
        WRITE setHostname
        NOTIFY hostnameChanged
    )

    Q_PROPERTY(
        QString port
        READ port
        WRITE setPort
        NOTIFY portChanged
    )

    Q_PROPERTY(
        bool useApiKey
        READ useApiKey
        WRITE setUseApiKey
        NOTIFY useApiKeyChanged
    )

    Q_PROPERTY(
        QString apiKey
        READ apiKey
        WRITE setApiKey
        NOTIFY apiKeyChanged
    )

    Q_PROPERTY(
        Anki::DuplicatePolicy duplicatePolicy
        READ duplicatePolicy
        WRITE setDuplicatePolicy
        NOTIFY duplicatePolicyChanged
    )

    Q_PROPERTY(
        QString newlineReplacer
        READ newlineReplacer
        WRITE setNewlineReplacer
        NOTIFY newlineReplacerChanged
    )

    Q_PROPERTY(
        Anki::FileType screenshotType
        READ screenshotType
        WRITE setScreenshotType
        NOTIFY screenshotTypeChanged
    )

    Q_PROPERTY(
        double audioPadStart
        READ audioPadStart
        WRITE setAudioPadStart
        NOTIFY audioPadStartChanged
    )

    Q_PROPERTY(
        double audioPadEnd
        READ audioPadEnd
        WRITE setAudioPadEnd
        NOTIFY audioPadEndChanged
    )

    Q_PROPERTY(
        bool audioNormalize
        READ audioNormalize
        WRITE setAudioNormalize
        NOTIFY audioNormalizedChanged
    )

    Q_PROPERTY(
        double audioDb
        READ audioDb
        WRITE setAudioDb
        NOTIFY audioDbChanged
    )

    Q_PROPERTY(
        QStringList excludeGlossaries
        READ excludeGlossaries
        WRITE setExcludeGlossaries
        NOTIFY excludeGlossariesChanged
    )

    Q_PROPERTY(
        QStringList tags
        READ tags
        WRITE setTags
        NOTIFY tagsChanged
    )

    Q_PROPERTY(
        QString termDeck
        READ termDeck
        WRITE setTermDeck
        NOTIFY termDeckChanged
    )

    Q_PROPERTY(
        QString termModel
        READ termModel
        WRITE setTermModel
        NOTIFY termModelChanged
    )

    Q_PROPERTY(
        AnkiFieldListModel *termFields
        READ termFields
        NOTIFY termFieldsChanged
    )

    Q_PROPERTY(
        QString kanjiDeck
        READ kanjiDeck
        WRITE setKanjiDeck
        NOTIFY kanjiDeckChanged
    )

    Q_PROPERTY(
        QString kanjiModel
        READ kanjiModel
        WRITE setKanjiModel
        NOTIFY kanjiModelChanged
    )

    Q_PROPERTY(
        AnkiFieldListModel *kanjiFields
        READ kanjiFields
        NOTIFY kanjiFieldsChanged
    )

public:
    AnkiProfile(QObject *parent = nullptr);
    virtual ~AnkiProfile() = default;

    /**
     * @brief Create a copy of this AnkiProfile.
     *
     * @param parent The parent of the new profile.
     * @return A deep copy of this profile.
     */
    [[nodiscard]]
    AnkiProfile *clone(QObject *parent = nullptr) const;

    /**
     * @brief Set all values to their defaults.
     */
    Q_INVOKABLE void defaults();

    /**
     * @brief Get the name of this profile.
     *
     * @return The name of this profile.
     */
    [[nodiscard]]
    const QString &name() const noexcept;

    /**
     * @brief Set the name of this profile.
     *
     * @param value The new name.
     */
    void setName(const QString &value = Anki::Keys::NAME_DEFAULT);

    /**
     * @brief Get the hostname of the AnkiConnect server.
     *
     * @return The host name of the AnkiConnect server.
     */
    [[nodiscard]]
    const QString &hostname() const noexcept;

    /**
     * @brief Set the hostname of the AnkiConnect server.
     *
     * @param value The new hostname.
     */
    void setHostname(const QString &value = Anki::Keys::HOSTNAME_DEFAULT);

    /**
     * @brief Get the port of the AnkiConnect server.
     *
     * @return The port of the AnkiConnect server.
     */
    [[nodiscard]]
    const QString &port() const noexcept;

    /**
     * @brief Set the port of the AnkiConnect server.
     *
     * @param value The new port of the server.
     */
    void setPort(const QString &value = Anki::Keys::PORT_DEFAULT);

    /**
     * @brief Get if an API key should be used.
     *
     * @return true to use an API key,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool useApiKey() const noexcept;

    /**
     * @brief Set if an API key should be used.
     *
     * @param value true to use an API key, false otherwise.
     */
    void setUseApiKey(bool value = Anki::Keys::USE_API_KEY_DEFAULT);

    /**
     * @brief Get the API key to use to connect.
     *
     * @return The API key.
     */
    [[nodiscard]]
    const QString &apiKey() const noexcept;

    /**
     * @brief Set the API key to use to connect.
     *
     * @param value The API key to use.
     */
    void setApiKey(const QString &value = Anki::Keys::API_KEY_DEFAULT);

    /**
     * @brief Get the duplicate policy of this profile.
     *
     * @return The duplicate policy of this profile.
     */
    [[nodiscard]]
    Anki::DuplicatePolicy duplicatePolicy() const noexcept;

    /**
     * @brief Set the duplicate policy of this profile.
     *
     * @param value The new policy.
     */
    void setDuplicatePolicy(
        Anki::DuplicatePolicy value = Anki::Keys::DUPLICATE_POLICY_DEFAULT);

    /**
     * @brief Get the newline replacement string.
     *
     * @return The newline replacement string.
     */
    [[nodiscard]]
    const QString &newlineReplacer() const noexcept;

    /**
     * @brief Set the newline replacement string.
     *
     * @param value The new newline replacement string.
     */
    void setNewlineReplacer(
        const QString &value = Anki::Keys::NEWLINE_REPLACER_DEFAULT);

    /**
     * @brief Get the screenshot file type.
     *
     * @return The screenshot file type.
     */
    [[nodiscard]]
    Anki::FileType screenshotType() const noexcept;

    /**
     * @brief Set the screenshot file type.
     *
     * @param value The new file type.
     */
    void setScreenshotType(
        Anki::FileType value = Anki::Keys::SCREENSHOT_DEFAULT);

    /**
     * @brief Get the padding duration added to the start of audio clips.
     *
     * @return The padding duration added to the start of audio clips.
     */
    [[nodiscard]]
    double audioPadStart() const noexcept;

    /**
     * @brief Set the padding duration added to the start of audio clips.
     *
     * @param value The padding duration added to the start of audio clips.
     */
    void setAudioPadStart(double value = Anki::Keys::AUDIO_PAD_START_DEFAULT);

    /**
     * @brief Get the padding duration added to the end of audio clips.
     *
     * @return The padding duration added to the end of audio clips.
     */
    [[nodiscard]]
    double audioPadEnd() const noexcept;

    /**
     * @brief Set the padding duration added to the end of audio clips.
     *
     * @param value The padding duration added to the end of audio clips.
     */
    void setAudioPadEnd(double value = Anki::Keys::AUDIO_PAD_END_DEFAULT);

    /**
     * @brief Get if audio clips should be normalized.
     *
     * @return true to normalize audio,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool audioNormalize() const noexcept;

    /**
     * @brief Set if audio clips should be normalized.
     *
     * @param value true to normalize, false otherwise.
     */
    void setAudioNormalize(bool value = Anki::Keys::AUDIO_NORMALIZE_DEFAULT);

    /**
     * @brief Get the dB to normalize audio to.
     *
     * @return dB to normalize to.
     */
    [[nodiscard]]
    double audioDb() const noexcept;

    /**
     * @brief Set the dB to normalize audio to.
     *
     * @param value dB to normalize to.
     */
    void setAudioDb(double value = Anki::Keys::AUDIO_DB_DEFAULT);

    /**
     * @brief Get the glossaries that should be excluded by default.
     *
     * @return The glossaries that should be excluded by default.
     */
    [[nodiscard]]
    const QStringList &excludeGlossaries() const noexcept;

    /**
     * @brief Set the glossaries that should be excluded by default.
     *
     * @param value The glossaries that should be excluded by default.
     */
    void setExcludeGlossaries(const QStringList &value = {});

    /**
     * @brief Get the tags to add to notes.
     *
     * @return The tags to add to notes.
     */
    [[nodiscard]]
    const QStringList &tags() const noexcept;

    /**
     * @brief Set the tags to add to notes.
     *
     * @param value The tags to add to notes.
     */
    void setTags(const QStringList &value = Anki::Keys::TAGS_DEFAULT);

    /**
     * @brief Get the deck to add terms to.
     *
     * @return The deck to add terms to.
     */
    [[nodiscard]]
    const QString &termDeck() const noexcept;

    /**
     * @brief Set the deck to add terms to.
     *
     * @param value The deck to add terms to.
     */
    void setTermDeck(const QString &value = {});

    /**
     * @brief Get the model to use for term notes.
     *
     * @return The model to use for term notes.
     */
    [[nodiscard]]
    const QString &termModel() const noexcept;

    /**
     * @brief Set the model to use for term notes.
     *
     * @param value The model to use for term notes.
     */
    void setTermModel(const QString &value = {});

    /**
     * @brief Get the term note template.
     *
     * @return The term note template.
     */
    [[nodiscard]]
    AnkiFieldListModel *termFields() const noexcept;

    /**
     * @brief Get the deck to add kanji to.
     *
     * @return The deck to add kanji to.
     */
    [[nodiscard]]
    const QString &kanjiDeck() const noexcept;

    /**
     * @brief Set the deck to add kanji to.
     *
     * @param value The deck to add kanji to.
     */
    void setKanjiDeck(const QString &value = {});

    /**
     * @brief Get the model to use for kanji notes.
     *
     * @return The model to use for kanji notes.
     */
    [[nodiscard]]
    const QString &kanjiModel() const noexcept;

    /**
     * @brief Set the model to use for kanji notes.
     *
     * @param value The model to use for kanji notes.
     */
    void setKanjiModel(const QString &value = {});

    /**
     * @brief Get the kanji note template.
     *
     * @return The kanji note template.
     */
    [[nodiscard]]
    AnkiFieldListModel *kanjiFields() const noexcept;

signals:
    /**
     * @brief Emitted when the name is changed.
     *
     * @param value The new name.
     */
    void nameChanged(const QString &value);

    /**
     * @brief Emitted when the hostname is changed.
     *
     * @param value The new hostname.
     */
    void hostnameChanged(const QString &value);

    /**
     * @brief Emitted when the port is changed.
     *
     * @param value The new port.
     */
    void portChanged(const QString &value);

    /**
     * @brief Emitted when the use API key setting is changed.
     *
     * @param value The new use api key value.
     */
    void useApiKeyChanged(bool value);

    /**
     * @brief Emitted when the API key is changed.
     *
     * @param value The new API key.
     */
    void apiKeyChanged(const QString &value);

    /**
     * @brief Emitted when the duplicate policy is changed.
     *
     * @param value The new duplicate policy.
     */
    void duplicatePolicyChanged(Anki::DuplicatePolicy value);

    /**
     * @brief Emitted when the newline replacer is changed.
     *
     * @param value The new newline replacement string.
     */
    void newlineReplacerChanged(const QString &value);

    /**
     * @brief Emitted when the screenshot file type is changed.
     *
     * @param value The new screenshot file type.
     */
    void screenshotTypeChanged(Anki::FileType value);

    /**
     * @brief Emitted when the start audio padding is changed.
     *
     * @param value The new start padding time.
     */
    void audioPadStartChanged(double value);

    /**
     * @brief Emitted when the end audio padding is changed.
     *
     * @param value The new end padding time.
     */
    void audioPadEndChanged(double value);

    /**
     * @brief Emitted when audio normalization is changed.
     *
     * @param value The new audio normalization value.
     */
    void audioNormalizedChanged(bool value);

    /**
     * @brief Emitted when audio normalization dB is changed.
     *
     * @param value The new audio normalization dB.
     */
    void audioDbChanged(double value);

    /**
     * @brief Emitted when excluded glossaries are changed.
     *
     * @param value The new excluded glossaries.
     */
    void excludeGlossariesChanged(const QStringList &value);

    /**
     * @brief Emitted when note tags are changed.
     *
     * @param value The tags to add to notes.
     */
    void tagsChanged(const QStringList &value);

    /**
     * @brief Emitted when the term deck is changed.
     *
     * @param value The new term deck.
     */
    void termDeckChanged(const QString &value);

    /**
     * @brief Emitted when the term model is changed.
     *
     * @param value The new term model.
     */
    void termModelChanged(const QString &value);

    /**
     * @brief Emitted when the term fields are changed.
     */
    void termFieldsChanged();

    /**
     * @brief Emitted when the kanji deck is changed.
     *
     * @param value The new kanji deck.
     */
    void kanjiDeckChanged(const QString &value);

    /**
     * @brief Emitted when the kanji model is changed.
     *
     * @param value The new kanji model.
     */
    void kanjiModelChanged(const QString &value);

    /**
     * @brief Emitted when the kanji fields are changed.
     */
    void kanjiFieldsChanged();

private:
    /* Name of this profile */
    QString m_name{Anki::Keys::NAME_DEFAULT};

    /* Hostname of the AnkiConnect server */
    QString m_hostname{Anki::Keys::HOSTNAME_DEFAULT};

    /* Port of the AnkiConnect server */
    QString m_port{Anki::Keys::PORT_DEFAULT};

    /* true to use the API key, false otherwise */
    bool m_useApiKey{Anki::Keys::USE_API_KEY_DEFAULT};

    /* API key for the AnkiConnect server */
    QString m_apiKey{Anki::Keys::API_KEY_DEFAULT};

    /* Note duplicate policy */
    Anki::DuplicatePolicy m_duplicatePolicy{
        Anki::Keys::DUPLICATE_POLICY_DEFAULT
    };

    /* String to substitute newlines with */
    QString m_newlineReplacer{Anki::Keys::NEWLINE_REPLACER_DEFAULT};

    /* File type to save screenshots as */
    Anki::FileType m_screenshotType{Anki::Keys::SCREENSHOT_DEFAULT};

    /* Amount of padding to add (in seconds) to the start of an audio clip */
    double m_audioPadStart{Anki::Keys::AUDIO_PAD_START_DEFAULT};

    /* Amount of padding to add (in seconds) to the end of an audio clip*/
    double m_audioPadEnd{Anki::Keys::AUDIO_PAD_END_DEFAULT};

    /* true if audio should be normalized, false otherwise */
    bool m_audioNormalize{Anki::Keys::AUDIO_NORMALIZE_DEFAULT};

    /* dB value to normalized audio to */
    double m_audioDb{Anki::Keys::AUDIO_DB_DEFAULT};

    /* Set of dictionary names that should not be added to Anki by default */
    QStringList m_excludeGlossaries;

    /* List of tags to add to Anki notes */
    QStringList m_tags{Anki::Keys::TAGS_DEFAULT};

    /* Deck to add term cards to */
    QString m_termDeck;

    /* Model to use for term notes */
    QString m_termModel;

    /* JSON object mapping field names to their templates for term cards */
    AnkiFieldListModel *m_termFields{new AnkiFieldListModel(this)};

    /* Deck to add kanji cards to */
    QString m_kanjiDeck;

    /* Model to use for kanji notes */
    QString m_kanjiModel;

    /* JSON object mapping field names to their templates for kanji cards */
    AnkiFieldListModel *m_kanjiFields{new AnkiFieldListModel(this)};
};
