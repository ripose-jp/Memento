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