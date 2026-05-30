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

#include "anki/ankiconfig.h"

#include <QFile>
#include <QIODevice>
#include <QJsonDocument>

AnkiConfig::AnkiConfig(const QString &path, QObject *parent) :
    QObject(parent),
    m_path(path)
{
    setValid(load());
}

AnkiConfig::~AnkiConfig()
{
    write();
}

#define PREFIX_ERR_STR "Could not load Anki config: "

bool AnkiConfig::load()
{
    /* Read in the config file */
    QFile configFile(m_path);
    if (!configFile.exists())
    {
        /* Use the defaults if there is no config file */
        qDeleteAll(m_profiles);
        m_profiles.clear();
        m_profiles.emplaceBack(new AnkiProfile(this));
        m_profile = m_profiles.first();
        emit profilesChanged();
        emit profileChanged(m_profile);
        return true;
    }

    if (!configFile.open(QIODevice::ReadOnly))
    {
        setError(tr("Anki config file exists, but it could not be read."));
        return false;
    }

    /* Read the file into memory */
    QJsonDocument jsonDoc = QJsonDocument::fromJson(configFile.readAll());
    configFile.close();

    /* Error check the JSON */
    if (jsonDoc.isNull())
    {
        setError(
            tr(PREFIX_ERR_STR "%1 is not JSON").arg(m_path)
        );
        return false;
    }
    else if (!jsonDoc.isObject())
    {
        setError(tr(PREFIX_ERR_STR "%1 is not a JSON object").arg(m_path));
        return false;
    }

    /* Get the config */
    QJsonObject jsonObj = jsonDoc.object();

    /* Clear out existing profiles */
    qDeleteAll(m_profiles);
    m_profiles.clear();
    m_profile = nullptr;

    /* Load new profiles */
    m_enabled = jsonObj[Anki::Keys::ENABLED]
        .toBool(Anki::Keys::ENABLED_DEFAULT);
    QString currentProfile = jsonObj[Anki::Keys::SET_PROFILE].toString();
    QJsonArray profiles = jsonObj[Anki::Keys::PROFILES].toArray();
    for (const QJsonValue &val : profiles)
    {
        QJsonObject profile = val.toObject();

        AnkiProfile *ankiProfile = new AnkiProfile(this);
        ankiProfile->setName(
            profile[Anki::Keys::NAME]
                .toString(Anki::Keys::NAME_DEFAULT)
        );
        ankiProfile->setHostname(
            profile[Anki::Keys::HOSTNAME]
                .toString(Anki::Keys::HOSTNAME_DEFAULT)
        );
        /* Make sure that hostnames include http:// or https:// */
        if (!ankiProfile->hostname().startsWith("http://") &&
            !ankiProfile->hostname().startsWith("https://"))
        {
            ankiProfile->setHostname("http://" + ankiProfile->hostname());
        }
        ankiProfile->setPort(
            profile[Anki::Keys::PORT]
                .toString(Anki::Keys::PORT_DEFAULT)
        );
        ankiProfile->setUseApiKey(
            profile[Anki::Keys::USE_API_KEY]
                .toBool(Anki::Keys::USE_API_KEY_DEFAULT)
        );
        ankiProfile->setApiKey(
            profile[Anki::Keys::API_KEY]
                .toString(Anki::Keys::API_KEY_DEFAULT)
        );
        ankiProfile->setDuplicatePolicy(static_cast<Anki::DuplicatePolicy>(
                profile[Anki::Keys::DUPLICATE_POLICY]
                    .toInt(Anki::Keys::DUPLICATE_POLICY_DEFAULT)
        ));
        ankiProfile->setNewlineReplacer(
            profile[Anki::Keys::NEWLINE_REPLACER]
                .toString(Anki::Keys::NEWLINE_REPLACER_DEFAULT)
        );
        ankiProfile->setScreenshotType(static_cast<Anki::FileType>(
            profile[Anki::Keys::SCREENSHOT].toInt(
                static_cast<int>(Anki::Keys::SCREENSHOT_DEFAULT))
        ));
        ankiProfile->setAudioPadStart(
            profile[Anki::Keys::AUDIO_PAD_START]
                .toDouble(Anki::Keys::AUDIO_PAD_START_DEFAULT)
        );
        ankiProfile->setAudioPadEnd(
            profile[Anki::Keys::AUDIO_PAD_END]
                .toDouble(Anki::Keys::AUDIO_PAD_END_DEFAULT)
        );
        ankiProfile->setAudioNormalize(
            profile[Anki::Keys::AUDIO_NORMALIZE]
                .toBool(Anki::Keys::AUDIO_NORMALIZE_DEFAULT)
        );
        ankiProfile->setAudioDb(
            profile[Anki::Keys::AUDIO_DB].toDouble(Anki::Keys::AUDIO_DB_DEFAULT)
        );
        if (profile[Anki::Keys::TAGS].isArray())
        {
            QStringList tags;
            for (const QJsonValue &val : profile[Anki::Keys::TAGS].toArray())
            {
                if (val.isString())
                {
                    tags.emplaceBack(val.toString());
                }
            }
            ankiProfile->setTags(tags);
        }
        if (profile[Anki::Keys::EXCLUDE_GLOSSARIES].isArray())
        {
            QStringList excludeGlossaries;
            for (const QJsonValue &val :
                profile[Anki::Keys::EXCLUDE_GLOSSARIES].toArray())
            {
                if (val.isString())
                {
                    excludeGlossaries.emplaceBack(val.toString());
                }
            }
            ankiProfile->setExcludeGlossaries(excludeGlossaries);
        }

        QJsonObject obj = profile[Anki::Keys::TERM].toObject();
        ankiProfile->setTermDeck(obj[Anki::Keys::DECK].toString());
        ankiProfile->setTermModel(obj[Anki::Keys::MODEL].toString());
        ankiProfile->termFields()->setItems(obj[Anki::Keys::FIELDS]);

        obj = profile[Anki::Keys::KANJI].toObject();
        ankiProfile->setKanjiDeck(obj[Anki::Keys::DECK].toString());
        ankiProfile->setKanjiModel(obj[Anki::Keys::MODEL].toString());
        ankiProfile->kanjiFields()->setItems(obj[Anki::Keys::FIELDS]);

        if (ankiProfile->name() == currentProfile)
        {
            m_profile = ankiProfile;
        }
        m_profiles.emplaceBack(ankiProfile);
    }
    std::ranges::sort(
        m_profiles,
        [] (const AnkiProfile *lhs, const AnkiProfile *rhs) -> bool
        {
            return lhs->name() < rhs->name();
        }
    );
    if (m_profiles.isEmpty())
    {
        m_profiles.emplaceBack(new AnkiProfile{this});
    }
    if (m_profile == nullptr)
    {
        m_profile = m_profiles.first();
    }
    for (AnkiProfile *profile : m_profiles)
    {
        connect(
            profile, &AnkiProfile::nameChanged,
            this, &AnkiConfig::sortProfiles
        );
    }

    emit enabledChanged(m_enabled);
    emit profilesChanged();
    emit profileChanged(m_profile);

    return true;
}

#undef PREFIX_ERR_STR

bool AnkiConfig::write()
{
    QFile configFile(m_path);
    if (!configFile.open(QIODevice::ReadWrite |
                         QIODevice::Truncate |
                         QIODevice::Text))
    {
        setError(tr("Could not write Anki config file at %1").arg(m_path));
        return false;
    }

    QJsonObject jsonObj;
    jsonObj[Anki::Keys::ENABLED] = enabled();
    if (profile())
    {
        jsonObj[Anki::Keys::SET_PROFILE] = profile()->name();
    }
    QJsonArray configArr;
    for (const AnkiProfile *profile : m_profiles)
    {
        QJsonObject configObj;
        configObj[Anki::Keys::NAME] = profile->name();
        configObj[Anki::Keys::HOSTNAME] = profile->hostname();
        configObj[Anki::Keys::PORT] = profile->port();
        configObj[Anki::Keys::USE_API_KEY] = profile->useApiKey();
        configObj[Anki::Keys::API_KEY] = profile->apiKey();
        configObj[Anki::Keys::DUPLICATE_POLICY] = profile->duplicatePolicy();
        configObj[Anki::Keys::NEWLINE_REPLACER] = profile->newlineReplacer();
        configObj[Anki::Keys::SCREENSHOT] = profile->screenshotType();
        configObj[Anki::Keys::AUDIO_PAD_START] = profile->audioPadStart();
        configObj[Anki::Keys::AUDIO_PAD_END] = profile->audioPadEnd();
        configObj[Anki::Keys::AUDIO_NORMALIZE] = profile->audioNormalize();
        configObj[Anki::Keys::AUDIO_DB] = profile->audioDb();

        QJsonArray tags;
        for (const QString &tag : profile->tags())
        {
            tags.append(tag);
        }
        configObj[Anki::Keys::TAGS] = std::move(tags);

        QJsonArray excludeGlossaries;
        for (const QString &dict : profile->excludeGlossaries())
        {
            excludeGlossaries.append(dict);
        }
        configObj[Anki::Keys::EXCLUDE_GLOSSARIES] =
            std::move(excludeGlossaries);

        QJsonObject obj;
        obj[Anki::Keys::DECK] = profile->termDeck();
        obj[Anki::Keys::MODEL] = profile->termModel();
        obj[Anki::Keys::FIELDS] = profile->termFields()->toJsonArray();
        configObj[Anki::Keys::TERM] = obj;

        obj[Anki::Keys::DECK] = profile->kanjiDeck();
        obj[Anki::Keys::MODEL] = profile->kanjiModel();
        obj[Anki::Keys::FIELDS] = profile->kanjiFields()->toJsonArray();
        configObj[Anki::Keys::KANJI] = obj;

        configArr.append(configObj);
    }
    jsonObj[Anki::Keys::PROFILES] = configArr;
    QJsonDocument jsonDoc(jsonObj);

    configFile.write(jsonDoc.toJson());
    configFile.close();

    return true;
}

bool AnkiConfig::valid() const noexcept
{
    return m_valid;
}

void AnkiConfig::setValid(bool value)
{
    if (m_valid == value)
    {
        return;
    }
    m_valid = value;
    emit validChanged(m_valid);
}

const QString &AnkiConfig::error() const noexcept
{
    return m_error;
}

void AnkiConfig::setError(const QString &value)
{
    if (m_error == value)
    {
        return;
    }
    m_error = value;
    emit errorChanged(m_error);
}

bool AnkiConfig::enabled() const noexcept
{
    return m_enabled;
}

void AnkiConfig::setEnabled(bool value)
{
    if (m_enabled == value)
    {
        return;
    }
    m_enabled = value;
    emit enabledChanged(m_enabled);
}

QQmlListProperty<AnkiProfile> AnkiConfig::profiles()
{
    return QQmlListProperty<AnkiProfile>(this, &m_profiles);
}

qsizetype AnkiConfig::profileIndex(const QString &name) const
{
    auto it = std::lower_bound(
        std::begin(m_profiles),
        std::end(m_profiles),
        name,
        [] (const AnkiProfile *profile, const QString &name) -> bool
        {
            return profile->name() < name;
        }
    );
    if (it == std::end(m_profiles) || (*it)->name() != name)
    {
        return -1;
    }
    return std::distance(std::begin(m_profiles), it);
}

bool AnkiConfig::profileExists(const QString &name) const
{
    return profileIndex(name) != -1;
}

bool AnkiConfig::addProfile(const QString &name, AnkiProfile *profile)
{
    auto it = std::lower_bound(
        std::begin(m_profiles),
        std::end(m_profiles),
        name,
        [] (const AnkiProfile *profile, const QString &name) -> bool
        {
            return profile->name() < name;
        }
    );
    if (it != std::end(m_profiles) && (*it)->name() == name)
    {
        setError(tr("A profile with that name already exists."));
        return false;
    }

    if (profile == nullptr)
    {
        profile = new AnkiProfile(this);
    }
    else
    {
        profile = profile->clone(this);
    }
    profile->setName(name);
    connect(
        profile, &AnkiProfile::nameChanged,
        this, &AnkiConfig::sortProfiles
    );
    m_profiles.insert(it, profile);
    emit profilesChanged();
    return true;
}

bool AnkiConfig::removeProfile(const QString &name)
{
    auto it = std::lower_bound(
        std::begin(m_profiles),
        std::end(m_profiles),
        name,
        [] (const AnkiProfile *profile, const QString &name) -> bool
        {
            return profile->name() < name;
        }
    );
    if (it == std::end(m_profiles) || (*it)->name() != name)
    {
        setError(tr("Could not find a profile with that name."));
        return false;
    }

    if (profile() == *it)
    {
        m_profile = nullptr;
    }
    (*it)->deleteLater();
    m_profiles.erase(it);
    if (m_profiles.isEmpty())
    {
        m_profiles.emplaceBack(new AnkiProfile(this));
    }
    if (profile() == nullptr)
    {
        m_profile = m_profiles.first();
    }
    emit profilesChanged();
    emit profileChanged(m_profile);
    return true;
}

void AnkiConfig::sortProfiles()
{
    std::ranges::sort(
        m_profiles,
        [] (const AnkiProfile *lhs, const AnkiProfile *rhs) -> bool
        {
            return lhs->name() < rhs->name();
        }
    );
    emit profilesChanged();
}

AnkiProfile *AnkiConfig::profile() const noexcept
{
    return m_profile;
}

bool AnkiConfig::setProfile(const QString &name)
{
    auto it = std::lower_bound(
        std::begin(m_profiles),
        std::end(m_profiles),
        name,
        [] (const AnkiProfile *profile, const QString &name) -> bool
        {
            return profile->name() < name;
        }
    );
    if (it == std::end(m_profiles) || (*it)->name() != name)
    {
        setError(tr("Could not find a profile with that name."));
        return false;
    }
    m_profile = *it;
    emit profileChanged(m_profile);
    return true;
}
