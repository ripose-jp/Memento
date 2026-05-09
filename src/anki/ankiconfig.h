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

#include <QQmlListProperty>

#include "anki/ankiconfigkeys.h"
#include "anki/ankiprofile.h"

/**
 * @brief Manages read and writing Anki configuration files as well as modeling
 * their contents.
 */
class AnkiConfig : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        bool valid
        READ valid
        NOTIFY validChanged
    )

    Q_PROPERTY(
        QString error
        READ error
        NOTIFY errorChanged
    )

    Q_PROPERTY(
        bool enabled
        READ enabled
        WRITE setEnabled
        NOTIFY enabledChanged
    )

    Q_PROPERTY(
        QQmlListProperty<AnkiProfile> profiles
        READ profiles
        NOTIFY profilesChanged
    )

    Q_PROPERTY(
        AnkiProfile *profile
        READ profile
        NOTIFY profileChanged
    )

public:
    /**
     * @brief Construct a new AnkiConfig.
     *
     * @param path The path to the config file. It will be created if it doesn't
     * exist.
     * @param parent The parent of this object.
     */
    AnkiConfig(const QString &path, QObject *parent = nullptr);
    virtual ~AnkiConfig();

    /**
     * @brief Load settings from the given config file.
     *
     * @return true if the config was loaded,
     * @return false on error.
     */
    Q_INVOKABLE bool load();

    /**
     * @brief Write setting to the given config file.
     *
     * @return true if the config was written,
     * @return false on error.
     */
    Q_INVOKABLE bool write();

    /**
     * @brief Get if this config is valid.
     *
     * @return true if this config is valid,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool valid() const noexcept;

private:
    /**
     * @brief Set if this config is valid.
     *
     * @param value true if this config is valid, false otherwise.
     */
    void setValid(bool value);

public:
    /**
     * @brief Get the current human-readable error.
     *
     * @return The current human-readable error.
     */
    [[nodiscard]]
    const QString &error() const noexcept;

private:
    /**
     * @brief Set the current human-readable error.
     *
     * @param value The new human-readable error.
     */
    void setError(const QString &value);

public:
    /**
     * @brief Set if Anki Integration is enabled.
     *
     * @return true if enabled,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool enabled() const noexcept;

    /**
     * @brief Set if Anki Integration is enabled.
     *
     * @param value true if enabled, false otherwise.
     */
    void setEnabled(bool value);

    /**
     * @brief Get the current list of profiles.
     *
     * @return The current list of profiles.
     */
    [[nodiscard]]
    QQmlListProperty<AnkiProfile> profiles();

    /**
     * @brief Get the index of a profile.
     *
     * @param name The name of the profile.
     * @return The index of the profile if it exists, -1 if it doesn't.
     */
    [[nodiscard]]
    Q_INVOKABLE qsizetype profileIndex(const QString &name) const;

    /**
     * @brief Get if a profile with the given name exists.
     *
     * @param name The name to look for.
     * @return true if the profile exists,
     * @return false if the profile does not exist.
     */
    [[nodiscard]]
    Q_INVOKABLE bool profileExists(const QString &name) const;

    /**
     * @brief Add a profile to the config.
     *
     * @param name The name of the profile to add.
     * @param profile The profile to copy settings from. nullptr to default use
     * default values.
     * @return true if the profile was added,
     * @return false if it already exists.
     */
    Q_INVOKABLE bool addProfile(
        const QString &name, AnkiProfile *profile = nullptr);

    /**
     * @brief Remove a profile from the config.
     *
     * @param name The name of the profile to remove.
     * @return true if the profile was removed.
     * @return false if the profile never existed.
     */
    Q_INVOKABLE bool removeProfile(const QString &name);

    /**
     * @brief Get the currently set profile.
     *
     * @return The current profile.
     */
    [[nodiscard]]
    AnkiProfile *profile() const noexcept;

    /**
     * @brief Set the current profile.
     *
     * @param name The name of the current profile to set
     * @return true if the profile exists and was set,
     * @return false otherwise.
     */
    Q_INVOKABLE bool setProfile(const QString &name);

signals:
    /**
     * @brief Emitted when config validity changes.
     *
     * @param value The new validity value.
     */
    void validChanged(bool value);

    /**
     * @brief Emitted when a new error string is available.
     *
     * @param value The latest error.
     */
    void errorChanged(const QString &value);

    /**
     * @brief Emitted when Anki integration enabled is changed.
     *
     * @param value The new enabled value.
     */
    void enabledChanged(bool value);

    /**
     * @brief Emitted when the profile set changes.
     */
    void profilesChanged();

    /**
     * @brief Emitted when the current profile changes.
     *
     * @param value The new current profile.
     */
    void profileChanged(AnkiProfile *value);

private slots:
    /**
     * @brief Resorts all profiles.
     */
    void sortProfiles();

private:
    /* Path to the config file */
    const QString m_path;

    /* true if loaded successfully loaded, false if error */
    bool m_valid{false};

    /* The last error string */
    QString m_error;

    /* true if Anki integration is enabled */
    bool m_enabled{Anki::Keys::ENABLED_DEFAULT};

    /* All available Anki profiles */
    QList<AnkiProfile *> m_profiles = {new AnkiProfile(this)};

    /* The current active Anki profile */
    AnkiProfile *m_profile{m_profiles.first()};
};
