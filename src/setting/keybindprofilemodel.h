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

#include "setting/keybindprofile.h"

/**
 * @brief Object holding information about all keybind profiles.
 */
class KeybindProfileModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        KeybindProfile *profile
        READ profile
        WRITE setProfile
        NOTIFY profileChanged
    )

    Q_PROPERTY(
        QQmlListProperty<KeybindProfile> profiles
        READ profilesQml
        NOTIFY profilesChanged
    )

public:
    KeybindProfileModel(QObject *parent = nullptr);

    /**
     * @brief Get the current profile.
     *
     * @return The current profile.
     */
    [[nodiscard]]
    KeybindProfile *profile() const noexcept;

    /**
     * @brief Set the current profile.
     *
     * @param value The profile to set.
     */
    void setProfile(KeybindProfile *value);

    /**
     * @brief Set the current profile by name.
     *
     * @param name The name of the profile to set.
     * @return true if the profile exists and was set,
     * @return false if the profile did not exist.
     */
    Q_INVOKABLE bool setProfileByName(const QString &name);

    /**
     * @brief Get the QML list property for all profiles.
     *
     * @return The QML list property for all profiles.
     */
    [[nodiscard]]
    QQmlListProperty<KeybindProfile> profilesQml();

    /**
     * @brief Get all profiles.
     *
     * @return A list of all profiles.
     */
    [[nodiscard]]
    const QList<KeybindProfile *> &profiles() const noexcept;

    /**
     * @brief Add a new profile with the given name.
     *
     * @param name The name of the profile to add.
     * @param base The profile to copy, nullptr to use default values.
     * @return true if the profile was added,
     * @return false if a profile with that name already exists.
     */
    Q_INVOKABLE bool addProfile(
        const QString &name, const KeybindProfile *base = nullptr);

    /**
     * @brief Remove a profile with the given name.
     *
     * @param name The profile to remove.
     * @return true if the profile was removed,
     * @return false if the profile doesn't exist.
     */
    Q_INVOKABLE bool removeProfile(const QString &name);

    /**
     * @brief Get if a profile with a given name exists.
     *
     * @param name The name of the profile to search for.
     * @return true if the profile exists,
     * @return false otherwise.
     */
    [[nodiscard]]
    Q_INVOKABLE bool profileExists(const QString &name) const;

    /**
     * @brief Set all profiles.
     *
     * @param profiles The list of profiles to set. Takes ownership.
     */
    void setProfiles(QList<KeybindProfile *> &&profiles);

signals:
    /**
     * @brief Emitted when the current profile is changed.
     *
     * @param value The new current profile.
     */
    void profileChanged(KeybindProfile *value);

    /**
     * @brief Emitted when the profiles are changed.
     */
    void profilesChanged();

private slots:
    /**
     * @brief Sort the profiles alphabetically by name.
     */
    void sortProfiles();

private:
    /**
     * @brief Get the ideal position of the profile with the name in m_profiles.
     *
     * @param name Name of the profile.
     * @return An iterator to the ideal position in m_profiles.
     */
    [[nodiscard]]
    QList<KeybindProfile *>::const_iterator profilePosition(
        const QString &name) const;

    /* Current keybind profile */
    KeybindProfile *m_profile{new KeybindProfile(this)};

    /* List of available profiles */
    QList<KeybindProfile *> m_profiles = {m_profile};
};
