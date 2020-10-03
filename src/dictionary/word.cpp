#include "word.h"

#include "characterutil.h"

Word::Word() : m_kanji(0), m_kana(0), m_description(0) {}

Word::Word(const QString &kanji, const QString &kana, const QString &description, const bool name) : m_kanji(new QString(kanji)),
                                                                                                     m_kana(new QString(kana)),
                                                                                                     m_description(new QString(description)),
                                                                                                     m_name(name)
{
    m_common = m_description->contains("(P)");

    m_kanjiCount = 0;
    for (auto it = m_kanji->constBegin(); it != m_kanji->constEnd(); ++it)
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

QDataStream &operator<<(QDataStream &dataStream, const Word &word)
{
    dataStream << *word.m_kanji
               << *word.m_kana
               << *word.m_description
               << word.m_common
               << word.m_name
               << word.m_kanjiCount;
}

QDataStream &operator>>(QDataStream &dataStream, Word &word)
{
    delete word.m_kanji;
    delete word.m_kana;
    delete word.m_description;

    word.m_kanji = new QString;
    word.m_kana = new QString;
    word.m_description = new QString;

    dataStream >> *word.m_kanji;
    dataStream >> *word.m_kana;
    dataStream >> *word.m_description;
    dataStream >> word.m_common;
    dataStream >> word.m_name;
    dataStream >> word.m_kanjiCount;
}