#ifndef ENTRY_H
#define ENTRY_H

#include <QString>

struct Entry
{
    Entry(const QString &kanji, const QString &kana, const QString &description)
        : m_kanji(new QString(kanji)), m_kana(new QString(kana)), m_description(new QString(description)) {}
    
    ~Entry()
    {
        delete m_kanji;
        delete m_kana;
        delete m_description;
    }

    QString *m_kanji;
    QString *m_kana;
    QString *m_description;
} typedef Entry;

#endif // ENTRY_H