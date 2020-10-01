#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "dictionarytype.h"
#include "word.h"
#include "digram.h"
#include "trigram.h"

#include <QHash>
#include <QChar>
#include <QList>

/**
 * How long to wait for indexing until throwing timeout error (in milliseconds).
 * This should be long enough that even the slowest computer should have enough time to
 * finish indexing.
 */
#define MAX_WAIT_TIME 10000
/**
 * How often is indexing status polled (in milliseconds)
 */
#define WAIT_RESOLUTION 50

class Dictionary
{
public:
    Dictionary(DictionaryType type);
    ~Dictionary();
    const DictionaryType m_type;

    /**
	 * Gets all words that contain the kanji
	 * 
	 * @param startsWith Only words that start with the kanji are returned, else all words
	 * that contain kanji are returned
	 */
    QList<Word> *getWordsKanji(const QChar &kanji, const bool startsWith) const;

    /**
	 * Gets all words that contain the digram in kanji or kana field
	 * 
	 * @param startsWith Only words that start with the dg are returned, else all words
	 * that contain dg are returned
	 */
    QList<Word> *getWordsDigram(const Digram &dg, const bool startsWith) const;

    /**
	 * Gets all words that contain the trigram in description field
	 */
    QList<Word> *getWordsDescription(const Trigram &tg) const;

    std::atomic_bool m_kanjiIndexingDone;
    std::atomic_bool m_digramIndexingDone;
    std::atomic_bool m_descriptionIndexingDone;

	/**
	 * Kanji -> List of words that start with kanji monogram. 
	 * Kana monograms are not indexed and seach for single kana is not supported. 
	 */
    QHash<QChar, QList<Word>> *m_kanjiStartWith;

    /**
	 * Kanji -> List of words that contain kanji monogram (not in first position) 
	 * Kana monograms are not indexed and seach for single kana is not supported. 
	 */
    QHash<QChar, QList<Word>> *m_kanjiContains;

    /**
	 * Digram -> List of words that start with digram
	 * This is the most commonly used index since pair of kanji characters is
	 * very selective without consuming too much memory.
	 */
    QHash<Digram, QList<Word>> *m_digramStartsWith;

    /**
	 * Digram -> List of words that start with digram (not in first position)
	 * This is the most commonly used index since pair of kanji characters is
	 * very selective without consuming too much memory.
	 */
    QHash<Digram, QList<Word>> *m_digramContains;

    /**
	 * Trigram in description -> List of words containing the alphabet trigram.
	 */
    QHash<Trigram, QList<Word>> *m_descriptionIndex;
    
private:
	bool indexingDone(const std::atomic_bool &waitCondition) const;
};

#endif // DICTIONARY_H