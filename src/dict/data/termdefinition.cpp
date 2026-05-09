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

#include "dict/data/termdefinition.h"

TermDefinition::TermDefinition(QObject *parent) : QObject(parent)
{

}

TermDefinition::~TermDefinition()
{

}

TermDefinition *TermDefinition::clone(QObject *parent) const
{
    TermDefinition *copy = new TermDefinition(parent);
    copy->setDictionaryInfo(dictionaryInfo()->clone(copy));
    for (Tag *tag : tags())
    {
        copy->appendTags(tag->clone(copy));
    }
    copy->setRules(rules());
    copy->setGlossary(glossary());
    copy->setScore(score());
    copy->setSelected(selected());
    return copy;
}

DictionaryInfo *TermDefinition::dictionaryInfo() const noexcept
{
    return m_dictionaryInfo;
}

void TermDefinition::setDictionaryInfo(DictionaryInfo *value)
{
    if (m_dictionaryInfo == value)
    {
        return;
    }
    if (value)
    {
        value->setParent(this);
    }
    std::swap(m_dictionaryInfo, value);
    emit dictionaryInfoChanged(m_dictionaryInfo);
    if (value)
    {
        value->deleteLater();
    }
}

QQmlListProperty<Tag> TermDefinition::tagsQml()
{
    return QQmlListProperty<Tag>(this, &m_tags);
}

const QList<Tag *> &TermDefinition::tags() const noexcept
{
    return m_tags;
}

void TermDefinition::setTags(const QList<Tag *> &value)
{
    qDeleteAll(m_tags);
    m_tags.clear();
    for (Tag *tag : value)
    {
        tag->setParent(this);
        m_tags.emplaceBack(tag);
    }
    emit tagsChanged();
}

void TermDefinition::appendTags(Tag *value)
{
    value->setParent(this);
    m_tags.emplaceBack(value);
    emit tagsChanged();
}

const QStringList &TermDefinition::rules() const noexcept
{
    return m_rules;
}

void TermDefinition::setRules(const QStringList &value)
{
    if (m_rules == value)
    {
        return;
    }
    m_rules = value;
    emit rulesChanged();
}

const QJsonArray &TermDefinition::glossary() const noexcept
{
    return m_glossary;
}

void TermDefinition::setGlossary(const QJsonArray &value)
{
    if (m_glossary == value)
    {
        return;
    }
    m_glossary = value;
    emit glossaryChanged();
}

int TermDefinition::score() const noexcept
{
    return m_score;
}

void TermDefinition::setScore(int value)
{
    if (m_score == value)
    {
        return;
    }
    m_score = value;
    emit scoreChanged(m_score);
}

bool TermDefinition::selected() const noexcept
{
    return m_selected;
}

void TermDefinition::setSelected(bool value)
{
    if (m_selected == value)
    {
        return;
    }
    m_selected = value;
    emit selectedChanged(m_selected);
}
