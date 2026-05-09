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

#include "setting/keybindprofilemodel.h"

KeybindProfileModel::KeybindProfileModel(QObject *parent) : QObject(parent)
{
    for (KeybindProfile *profile : m_profiles)
    {
        connect(
            profile, &KeybindProfile::nameChanged,
            this, &KeybindProfileModel::sortProfiles
        );
    }
}

KeybindProfile *KeybindProfileModel::profile() const noexcept
{
    return m_profile;
}

void KeybindProfileModel::setProfile(KeybindProfile *value)
{
    if (m_profile == value)
    {
        return;
    }
    if (m_profiles.contains(value))
    {
        m_profile = value;
    }
    emit profileChanged(m_profile);
}

bool KeybindProfileModel::setProfileByName(const QString &name)
{
    auto it = profilePosition(name);
    if (it == std::end(m_profiles))
    {
        return false;
    }
    setProfile(*it);
    return true;
}

QQmlListProperty<KeybindProfile> KeybindProfileModel::profilesQml()
{
    return QQmlListProperty<KeybindProfile>(this, &m_profiles);
}

const QList<KeybindProfile *> &KeybindProfileModel::profiles() const noexcept
{
    return m_profiles;
}

bool KeybindProfileModel::addProfile(
    const QString &name, const KeybindProfile *base)
{
    auto insertPosition = profilePosition(name);
    if (insertPosition != std::end(m_profiles) &&
        (*insertPosition)->name() == name)
    {
        return false;
    }

    KeybindProfile *newProfile =
        base ? base->clone(this) : new KeybindProfile(this);
    newProfile->setName(name);
    connect(
        newProfile, &KeybindProfile::nameChanged,
        this, &KeybindProfileModel::sortProfiles
    );
    m_profiles.insert(insertPosition, newProfile);
    emit profilesChanged();
    return true;
}

bool KeybindProfileModel::removeProfile(const QString &name)
{
    auto removePosition = profilePosition(name);
    if (removePosition == std::end(m_profiles) ||
        (*removePosition)->name() != name)
    {
        return false;
    }

    KeybindProfile *removedProfile = *removePosition;
    removedProfile->deleteLater();
    m_profiles.erase(removePosition);
    if (m_profiles.isEmpty())
    {
        m_profiles.emplaceBack(new KeybindProfile(this));
    }
    if (removedProfile == profile())
    {
        setProfile(m_profiles.first());
    }
    emit profilesChanged();
    return true;
}

bool KeybindProfileModel::profileExists(const QString &name) const
{
    auto it = profilePosition(name);
    return it != std::end(m_profiles) && (*it)->name() == name;
}

void KeybindProfileModel::setProfiles(QList<KeybindProfile *> &&profiles)
{
    if (profiles.isEmpty())
    {
        return;
    }

    for (KeybindProfile *profile : profiles)
    {
        profile->setParent(this);
    }
    std::ranges::sort(
        profiles,
        [] (const KeybindProfile *lhs, const KeybindProfile *rhs) -> bool
        {
            return lhs->name() < rhs->name();
        }
    );

    m_profile = profiles.first();
    emit profileChanged(m_profile);

    qDeleteAll(m_profiles);
    m_profiles = std::move(profiles);
    emit profilesChanged();
}

QList<KeybindProfile *>::const_iterator KeybindProfileModel::profilePosition(
    const QString &name) const
{
    return std::lower_bound(
        std::begin(m_profiles), std::end(m_profiles), name,
        [] (const KeybindProfile *profile, const QString &name) -> bool
        {
            return profile->name() < name;
        }
    );
}

void KeybindProfileModel::sortProfiles()
{
    std::ranges::sort(
        m_profiles,
        [] (const KeybindProfile *lhs, const KeybindProfile *rhs) -> bool
        {
            return lhs->name() < rhs->name();
        }
    );
    emit profilesChanged();
}
