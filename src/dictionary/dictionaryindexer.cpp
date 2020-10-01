#include "dictionaryindexer.h"
#include "characterutil.h"

#include <QThreadPool>
#include <QDateTime>

void DictionaryIndexer::buildIndex(Dictionary *dictionary, QList<Word> *words)
{
    m_dictionary = dictionary;
    m_words = words;

    for (const ThreadRole role : roles)
    {
        IndexBuilder *thread = new IndexBuilder(this, role);
        QThreadPool::globalInstance()->start(thread);
    }
}

void DictionaryIndexer::IndexBuilder::run()
{
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    Q_FOREACH (Word word, *m_parent->m_words)
    {
        switch (m_role)
        {
            case (ThreadRole::KanjiIndex):
                m_parent->addKanjiIndex(word);
                break;
            case (ThreadRole::DigramIndex):
                m_parent->addDigramIndex(word);
                break;
            case (ThreadRole::DescriptionIndex):
                m_parent->addDescriptionIndex(word);
                break;
        }
    }
    switch (m_role)
    {
        case (ThreadRole::KanjiIndex):
            m_parent->m_dictionary->m_kanjiIndexingDone = true;
            break;
        case (ThreadRole::DigramIndex):
            m_parent->m_dictionary->m_digramIndexingDone = true;
            break;
        case (ThreadRole::DescriptionIndex):
            m_parent->m_dictionary->m_descriptionIndexingDone = true;
            break;
    }
}

void DictionaryIndexer::addKanjiIndex(const Word &word)
{
    // starts
    QChar c = CharacterUtil::toCanonical((*word.m_kanji)[0]);
    if (!CharacterUtil::isKanji(c))
        return;

    auto wordsIt = m_dictionary->m_kanjiStartWith->find(c);
    QList<Word> *words;
    if (wordsIt != m_dictionary->m_kanjiStartWith->end())
    {
        words = &(*wordsIt);
    }
    else
    {
        m_dictionary->m_kanjiStartWith->insert(c, QList<Word>());
        words = &(*m_dictionary->m_kanjiStartWith->find(c));
    }
    words->append(word);

    // contains
    QSet<QChar> added;
    QString str = CharacterUtil::toCanonical(*word.m_kanji);
    for (auto it = str.begin() + 1; it != str.end(); ++it)
    {
        c = *it;
        if (!CharacterUtil::isKanji(c) || added.contains(c))
            continue;
        
        wordsIt = m_dictionary->m_kanjiContains->find(c);
        if (wordsIt != m_dictionary->m_kanjiContains->end())
        {
            words = &(*wordsIt);
        }
        else
        {
            words = &(*m_dictionary->m_kanjiStartWith->insert(c, QList<Word>()));
        }
        words->append(word);
        added.insert(c);
    }
}

void DictionaryIndexer::addDigramIndex(const Word &word)
{
    QSet<Digram> added;
    addDigramIndex(word, CharacterUtil::toCanonical(*word.m_kanji), added);
    addDigramIndex(word, CharacterUtil::toCanonical(*word.m_kana), added);
}

void DictionaryIndexer::addDigramIndex(const Word &word, const QString &str, QSet<Digram> &added)
{
    if (str.length() < 2)
        return;
    
    // starts
    Digram dg(str[0], str[1]);
    auto digramIt = m_dictionary->m_digramStartsWith->find(dg);
    QList<Word> *words;
    if (digramIt != m_dictionary->m_digramStartsWith->end())
    {
        words = &(*digramIt);
    }
    else
    {
        m_dictionary->m_digramStartsWith->insert(dg, QList<Word>());
        words = &(*m_dictionary->m_digramStartsWith->find(dg));
    }
    words->append(word);

    // contains
    for (uint i = 1; i < str.length(); ++i)
    {
        Digram containsDg(str[i], str[i + 1]);
        if (added.contains(containsDg))
            continue;
        
        digramIt = m_dictionary->m_digramContains->find(containsDg);
        if (digramIt != m_dictionary->m_digramContains->end())
        {
            words = &(*digramIt);
        }
        else
        {
            words = &(*m_dictionary->m_digramContains->insert(containsDg, QList<Word>()));
        }
        words->append(word);
        added.insert(containsDg);
    }
}

void DictionaryIndexer::addDescriptionIndex(const Word &word)
{
    const QString &description = *word.m_description;

    for (uint i = 0; i < description.length() - 2; ++i)
    {
        Trigram trigram(description[i], description[i + 1], description[i + 2]);
        auto trigramIt = m_dictionary->m_descriptionIndex->find(trigram);
        QList<Word> *words;
        if (trigramIt != m_dictionary->m_descriptionIndex->end())
        {
            words = &(*trigramIt);
        }
        else
        {
            words = &(*m_dictionary->m_descriptionIndex->insert(trigram, QList<Word>()));
        }
        words->append(word);
    }
}