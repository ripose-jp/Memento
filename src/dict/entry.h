////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#ifndef ENTRY_H
#define ENTRY_H

#include <string>
#include <vector>

struct Entry
{
    Entry() : m_kanji(new std::string),
              m_altkanji(new std::string),
              m_kana(new std::string), 
              m_altkana(new std::string),
              m_descriptions(new std::vector<std::vector<std::string>>) {}
    
    ~Entry()
    {
        delete m_kanji;
        delete m_altkanji;
        delete m_kana;
        delete m_altkana;
        delete m_descriptions;
    }

    std::string *m_kanji;
    std::string *m_altkanji;
    std::string *m_kana;
    std::string *m_altkana;
    std::vector<std::vector<std::string>> *m_descriptions;
} typedef Entry;

#endif // ENTRY_H