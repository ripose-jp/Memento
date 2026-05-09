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

#include "dict/dictionary.h"

#include "util/utils.h"

Dictionary::Dictionary(QObject *parent) : QObject(parent)
{
    databaseInstance(this);
    setModifyingDatabase(m_db->modifyingDatabase());
    connect(
        m_db, &DatabaseManager::modifyingDatabaseChanged,
        this, &Dictionary::setModifyingDatabase,
        Qt::QueuedConnection
    );
}

Dictionary::~Dictionary()
{

}

DatabaseManager *Dictionary::databaseInstance(QObject *parent)
{
    if (m_db == nullptr)
    {
        m_db = new DatabaseManager(DirectoryUtils::getDictionaryDb(), parent);
    }
    return m_db;
}

bool Dictionary::modifyingDatabase() const noexcept
{
    return m_modifyingDatabase;
}

void Dictionary::setModifyingDatabase(bool value)
{
    if (m_modifyingDatabase == value)
    {
        return;
    }
    m_modifyingDatabase = value;
    emit modifyingDatabaseChanged(m_modifyingDatabase);
}
