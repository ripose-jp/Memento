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

#include "quick/paths.h"

Paths::Paths(QObject *parent) : QObject(parent)
{

}

const QString &Paths::program() const noexcept
{
    return m_program;
}

const QString &Paths::ankiConfig() const noexcept
{
    return m_ankiConfig;
}

const QString &Paths::config() const noexcept
{
    return m_config;
}

const QString &Paths::dictionaryDb() const noexcept
{
    return m_dictionaryDb;
}

const QString &Paths::dictionaryResource() const noexcept
{
    return m_dictionaryResource;
}

const QString &Paths::mecabDictionary() const noexcept
{
    return m_mecabDictionary;
}

const QString &Paths::mpvInputConfig() const noexcept
{
    return m_mpvInputConfig;
}

const QString &Paths::historyDir() const noexcept
{
    return m_historyDir;
}
