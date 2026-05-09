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

#include "anki/ankiprofile.h"

AnkiProfile::AnkiProfile(QObject *parent) : QObject(parent)
{

}

AnkiProfile *AnkiProfile::clone(QObject *parent) const
{
    AnkiProfile *copy = new AnkiProfile(parent);
    copy->setName(name());
    copy->setHostname(hostname());
    copy->setPort(port());
    copy->setDuplicatePolicy(duplicatePolicy());
    copy->setNewlineReplacer(newlineReplacer());
    copy->setScreenshotType(screenshotType());
    copy->setAudioPadStart(audioPadStart());
    copy->setAudioPadEnd(audioPadEnd());
    copy->setAudioNormalize(audioNormalize());
    copy->setAudioDb(audioDb());
    copy->setExcludeGlossaries(excludeGlossaries());
    copy->setTags(tags());
    copy->setTermDeck(termDeck());
    copy->setTermModel(termModel());
    copy->termFields()->setItems(termFields()->items());
    copy->setKanjiDeck(kanjiDeck());
    copy->setKanjiModel(kanjiModel());
    copy->kanjiFields()->setItems(kanjiFields()->items());
    return copy;
}

void AnkiProfile::defaults()
{
    setHostname();
    setPort();
    setDuplicatePolicy();
    setNewlineReplacer();
    setScreenshotType();
    setAudioPadStart();
    setAudioPadEnd();
    setAudioNormalize();
    setAudioDb();
    setExcludeGlossaries();
    setTags();
    termFields()->clearValues();
    kanjiFields()->clearValues();
}

const QString &AnkiProfile::name() const noexcept
{
    return m_name;
}

void AnkiProfile::setName(const QString &value)
{
    if (m_name == value)
    {
        return;
    }
    m_name = value;
    emit nameChanged(m_name);
}

const QString &AnkiProfile::hostname() const noexcept
{
    return m_hostname;
}

void AnkiProfile::setHostname(const QString &value)
{
    if (m_hostname == value)
    {
        return;
    }
    m_hostname = value;
    emit hostnameChanged(m_hostname);
}

const QString &AnkiProfile::port() const noexcept
{
    return m_port;
}

void AnkiProfile::setPort(const QString &value)
{
    if (m_port == value)
    {
        return;
    }
    m_port = value;
    emit portChanged(m_port);
}

Anki::DuplicatePolicy AnkiProfile::duplicatePolicy() const noexcept
{
    return m_duplicatePolicy;
}

void AnkiProfile::setDuplicatePolicy(Anki::DuplicatePolicy value)
{
    if (m_duplicatePolicy == value)
    {
        return;
    }
    m_duplicatePolicy = value;
    emit duplicatePolicyChanged(m_duplicatePolicy);
}

const QString &AnkiProfile::newlineReplacer() const noexcept
{
    return m_newlineReplacer;
}

void AnkiProfile::setNewlineReplacer(const QString &value)
{
    if (m_newlineReplacer == value)
    {
        return;
    }
    m_newlineReplacer = value;
    emit newlineReplacerChanged(m_newlineReplacer);
}

Anki::FileType AnkiProfile::screenshotType() const noexcept
{
    return m_screenshotType;
}

void AnkiProfile::setScreenshotType(Anki::FileType value)
{
    if (m_screenshotType == value)
    {
        return;
    }
    m_screenshotType = value;
    emit screenshotTypeChanged(m_screenshotType);
}

double AnkiProfile::audioPadStart() const noexcept
{
    return m_audioPadStart;
}

void AnkiProfile::setAudioPadStart(double value)
{
    if (m_audioPadStart == value)
    {
        return;
    }
    m_audioPadStart = value;
    emit audioPadStartChanged(m_audioPadStart);
}

double AnkiProfile::audioPadEnd() const noexcept
{
    return m_audioPadEnd;
}

void AnkiProfile::setAudioPadEnd(double value)
{
    if (m_audioPadEnd == value)
    {
        return;
    }
    m_audioPadEnd = value;
    emit audioPadEndChanged(m_audioPadEnd);
}

bool AnkiProfile::audioNormalize() const noexcept
{
    return m_audioNormalize;
}

void AnkiProfile::setAudioNormalize(bool value)
{
    if (m_audioNormalize == value)
    {
        return;
    }
    m_audioNormalize = value;
    emit audioNormalizedChanged(m_audioNormalize);
}

double AnkiProfile::audioDb() const noexcept
{
    return m_audioDb;
}

void AnkiProfile::setAudioDb(double value)
{
    if (m_audioDb == value)
    {
        return;
    }
    m_audioDb = value;
    emit audioDbChanged(m_audioDb);
}

const QStringList &AnkiProfile::excludeGlossaries() const noexcept
{
    return m_excludeGlossaries;
}

void AnkiProfile::setExcludeGlossaries(const QStringList &value)
{
    if (m_excludeGlossaries == value)
    {
        return;
    }
    m_excludeGlossaries = value;
    emit excludeGlossariesChanged(m_excludeGlossaries);
}

const QStringList &AnkiProfile::tags() const noexcept
{
    return m_tags;
}

void AnkiProfile::setTags(const QStringList &value)
{
    if (m_tags == value)
    {
        return;
    }
    m_tags = value;
    emit tagsChanged(m_tags);
}

const QString &AnkiProfile::termDeck() const noexcept
{
    return m_termDeck;
}

void AnkiProfile::setTermDeck(const QString &value)
{
    if (m_termDeck == value)
    {
        return;
    }
    m_termDeck = value;
    emit termDeckChanged(m_termDeck);
}

const QString &AnkiProfile::termModel() const noexcept
{
    return m_termModel;
}

void AnkiProfile::setTermModel(const QString &value)
{
    if (m_termModel == value)
    {
        return;
    }
    m_termModel = value;
    emit termModelChanged(m_termModel);
}

AnkiFieldListModel *AnkiProfile::termFields() const noexcept
{
    return m_termFields;
}

const QString &AnkiProfile::kanjiDeck() const noexcept
{
    return m_kanjiDeck;
}

void AnkiProfile::setKanjiDeck(const QString &value)
{
    if (m_kanjiDeck == value)
    {
        return;
    }
    m_kanjiDeck = value;
    emit kanjiDeckChanged(m_kanjiDeck);
}

const QString &AnkiProfile::kanjiModel() const noexcept
{
    return m_kanjiModel;
}

void AnkiProfile::setKanjiModel(const QString &value)
{
    if (m_kanjiModel == value)
    {
        return;
    }
    m_kanjiModel = value;
    emit kanjiModelChanged(m_kanjiModel);
}

AnkiFieldListModel *AnkiProfile::kanjiFields() const noexcept
{
    return m_kanjiFields;
}
