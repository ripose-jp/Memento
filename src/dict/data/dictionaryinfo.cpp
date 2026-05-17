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

#include "dict/data/dictionaryinfo.h"

DictionaryInfo::DictionaryInfo(QObject *parent) : QObject(parent)
{

}

DictionaryInfo::~DictionaryInfo()
{

}

DictionaryInfo *DictionaryInfo::clone(QObject *parent) const
{
    DictionaryInfo *copy = new DictionaryInfo(parent);
    copy->setId(id());
    copy->setName(name());
    copy->setEnabled(enabled());
    copy->setStyles(styles());
    return copy;
}

int64_t DictionaryInfo::id() const noexcept
{
    return m_id;
}

void DictionaryInfo::setId(int64_t value)
{
    if (m_id == value)
    {
        return;
    }
    m_id = value;
    emit idChanged(m_id);
}

const QString &DictionaryInfo::name() const noexcept
{
    return m_name;
}

void DictionaryInfo::setName(const QString &value)
{
    if (m_name == value)
    {
        return;
    }
    m_name = value;
    emit nameChanged(m_name);
}

bool DictionaryInfo::enabled() const noexcept
{
    return m_enabled;
}

void DictionaryInfo::setEnabled(bool value)
{
    if (m_enabled == value)
    {
        return;
    }
    m_enabled = value;
    emit enabledChanged(m_enabled);
}

const QString &DictionaryInfo::styles() const noexcept
{
    return m_styles;
}

void DictionaryInfo::setStyles(const QString &value)
{
    if (m_styles == value)
    {
        return;
    }
    m_styles = value;
    emit stylesChanged(m_styles);
}
