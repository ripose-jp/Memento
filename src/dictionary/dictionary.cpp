#include "dictionary.h"

#include <thread>

Dictionary::Dictionary(DictionaryType type) : m_type(type),
                                              m_kanjiStartWith(new QHash<QChar, QList<Word>>),
                                              m_kanjiContains(new QHash<QChar, QList<Word>>),
                                              m_digramStartsWith(new QHash<Digram, QList<Word>>),
                                              m_digramContains(new QHash<Digram, QList<Word>>),
                                              m_descriptionIndex(new QHash<Trigram, QList<Word>>),
                                              m_kanjiIndexingDone(false),
                                              m_digramIndexingDone(false),
                                              m_descriptionIndexingDone(false) {}

Dictionary::~Dictionary()
{
    delete m_kanjiStartWith;
    delete m_kanjiContains;
    delete m_digramStartsWith;
    delete m_digramContains;
    delete m_descriptionIndex;
}

QList<Word> *Dictionary::getWordsKanji(const QChar &kanji, const bool startsWith) const
{
    if (!indexingDone(m_kanjiIndexingDone))
        return nullptr;

    QList<Word> *result = new QList<Word>();

    QHash<QChar, QList<Word>>::iterator kanjiFoundList = m_kanjiStartWith->find(kanji);
    if (kanjiFoundList != m_kanjiStartWith->end())
        result->append(*kanjiFoundList);
    
    if (!startsWith) {
        QHash<QChar, QList<Word>>::iterator kanjiContainsList = m_kanjiContains->find(kanji);
        if (kanjiContainsList != m_kanjiContains->end())
            result->append(*kanjiContainsList);
    }

    return result;
}

QList<Word> *Dictionary::getWordsDigram(const Digram &dg, const bool startsWith) const
{
    if (!indexingDone(m_digramIndexingDone))
        return nullptr;

    QList<Word> *result = new QList<Word>();
    
    QHash<Digram, QList<Word>>::iterator digramFoundList = m_digramStartsWith->find(dg);
    if (digramFoundList != m_digramStartsWith->end())
        result->append(*digramFoundList);
    
    if (!startsWith) {
        QHash<Digram, QList<Word>>::iterator digramContainsList = m_digramContains->find(dg);
        if (digramContainsList != m_digramContains->end())
            result->append(*digramContainsList);
    }

    return result;
}

QList<Word> *Dictionary::getWordsDescription(const Trigram &tg) const
{
    if (!indexingDone(m_descriptionIndexingDone))
        return nullptr;

    QList<Word> *result = new QList<Word>();
    
    QHash<Trigram, QList<Word>>::iterator descriptionFoundList = m_descriptionIndex->find(tg);
    if (descriptionFoundList != m_descriptionIndex->end())
        result->append(*descriptionFoundList);

    return result;
}

bool Dictionary::indexingDone(const std::atomic_bool &waitCondition) const
{
    unsigned int waitTime = 0;
    while (!waitCondition)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_RESOLUTION));
        waitTime += WAIT_RESOLUTION;
        if (waitTime > MAX_WAIT_TIME)
            return false;
    }
    return true;
}