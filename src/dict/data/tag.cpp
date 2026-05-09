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

#include "dict/data/tag.h"

Tag::Tag(QObject *parent) : QObject(parent)
{

}

Tag::~Tag()
{

}

Tag *Tag::clone(QObject *parent) const
{
    Tag *copy = new Tag(parent);
    copy->setDictionaryInfo(dictionaryInfo()->clone(copy));
    copy->setName(name());
    copy->setCategory(category());
    copy->setNotes(notes());
    copy->setOrder(order());
    copy->setScore(score());
    copy->setValue(value());
    return copy;
}

DictionaryInfo *Tag::dictionaryInfo() const noexcept
{
    return m_dictionaryInfo;
}

void Tag::setDictionaryInfo(DictionaryInfo *value)
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

const QString &Tag::name() const noexcept
{
    return m_name;
}

void Tag::setName(const QString &value)
{
    if (m_name == value)
    {
        return;
    }
    m_name = value;
    emit nameChanged(m_name);
}

const QString &Tag::category() const noexcept
{
    return m_category;
}

void Tag::setCategory(const QString &value)
{
    if (m_category == value)
    {
        return;
    }
    m_category = value;
    emit categoryChanged(m_category);
}

const QString &Tag::notes() const noexcept
{
    return m_notes;
}

void Tag::setNotes(const QString &value)
{
    if (m_notes == value)
    {
        return;
    }
    m_notes = value;
    emit notesChanged(m_notes);
}

int Tag::order() const noexcept
{
    return m_order;
}

void Tag::setOrder(int value)
{
    if (m_order == value)
    {
        return;
    }
    m_order = value;
    emit orderChanged(m_order);
}

int Tag::score() const noexcept
{
    return m_score;
}

void Tag::setScore(int value)
{
    if (m_score == value)
    {
        return;
    }
    m_score = value;
    emit scoreChanged(m_score);
}

const QString &Tag::value() const noexcept
{
    return m_value;
}

void Tag::setValue(const QString &value)
{
    if (m_value == value)
    {
        return;
    }
    m_value = value;
    emit valueChanged(m_value);
}
