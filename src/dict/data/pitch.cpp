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

#include "dict/data/pitch.h"

Pitch::Pitch(QObject *parent) : QObject(parent)
{

}

Pitch::~Pitch()
{

}

Pitch *Pitch::clone(QObject *parent) const
{
    Pitch *copy = new Pitch(parent);
    copy->setDictionaryInfo(dictionaryInfo()->clone(copy));
    copy->setMora(mora());
    copy->setPositions(positions());
    return copy;
}

DictionaryInfo *Pitch::dictionaryInfo() const noexcept
{
    return m_dictionaryInfo;
}

void Pitch::setDictionaryInfo(DictionaryInfo *value)
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

const QStringList &Pitch::mora() const noexcept
{
    return m_mora;
}

void Pitch::setMora(const QStringList &value)
{
    if (m_mora == value)
    {
        return;
    }
    m_mora = value;
    emit moraChanged();
}

const QList<int> &Pitch::positions() const noexcept
{
    return m_positions;
}

void Pitch::setPositions(const QList<int> &value)
{
    if (m_positions == value)
    {
        return;
    }
    m_positions = value;
    emit positionsChanged();
}
