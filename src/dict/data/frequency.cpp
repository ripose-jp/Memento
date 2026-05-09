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

#include "dict/data/frequency.h"

Frequency::Frequency(QObject *parent) : QObject(parent)
{

}

Frequency::~Frequency()
{

}

Frequency *Frequency::clone(QObject *parent) const
{
    Frequency *copy = new Frequency(parent);
    copy->setDictionaryInfo(dictionaryInfo()->clone(copy));
    copy->setFrequency(frequency());
    return copy;
}

DictionaryInfo *Frequency::dictionaryInfo() const noexcept
{
    return m_dictionaryInfo;
}

void Frequency::setDictionaryInfo(DictionaryInfo *value)
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

const QString &Frequency::frequency() const noexcept
{
    return m_frequency;
}

void Frequency::setFrequency(const QString &value)
{
    if (m_frequency == value)
    {
        return;
    }
    m_frequency = value;
    emit frequencyChanged(m_frequency);
}
