#include "word.h"

#include "characterutil.h"

Word::Word(QString &kanji, QString &kana, QString &description, bool name) : m_kanji(new QString(kanji)),
                                                                             m_kana(new QString(kana)),
                                                                             m_description(new QString(description)),
                                                                             m_name(name)
{
    m_common = m_description->contains("(P)"));

    m_kanjiCount = 0;
    for (auto it = m_kanji->constBegin(); it != m_kanji->constEnd(), ++it)
    {
        if (CharacterUtil::isKanji(*it))
            ++m_kanjiCount;
    }
}

Word::~Word()
{
    delete m_kanji;
    delete m_kana;
    delete m_description;
}

bool Word::operator==(const Word &rhs)
{
    return *m_kanji == *rhs.m_kanji && *m_kana == *rhs.m_kana;
}