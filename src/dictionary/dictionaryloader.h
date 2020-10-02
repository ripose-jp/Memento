#ifndef DICTIONARYLOADER_H
#define DICTIONARYLOADER_H

#define DEFAULT_DICT_FILENAME "edict2"
#define NAMES_DICT_FILENAME "enamdict"

#define DEFAULT_CACHE_FILENAME "JAPANESE_DEFAULT.cache"
#define NAMES_CACHE_FILENAME "JAPANESE_NAMES.cache"

#define DICT_MISSING_ERR "Dictionary file not found!"
#define CACHE_MISSING_ERR "Cache file not found!"

#include "dictionarytype.h"
#include "dictionary.h"

#include <QList>
#include <QFile>
#include <QRegExp>

class DictionaryLoader
{
public:
    DictionaryLoader(DictionaryType type);
    ~DictionaryLoader();

    /**
	 * Loads dictionary from cache if present, else from source 
	 */
    Dictionary *loadDictionary();

	/**
	 * Saves dictionary to serialized file
	 */
	void toDataStream(QFile &file);

	/**
	 * Retrieves dictionary from serialized file
	 */
	bool fromDataStream(QFile &file);

private:
    Dictionary *m_dictionary;
    DictionaryType m_type;
    QList<Word> *m_words;
    const QRegExp m_kanaMapExp = QRegExp("^(.+)\\((.+)\\)");

    /**
	 * True if dictionary cache file is outdated and needs to be recreated
	 * (source file is newer)
	 */
	bool isCacheOutdated();

    /**
	 * Gets dictionary file reference for current dictionary
	 */
    QFile getDictionaryFile();

    /**
	 * Gets cache file reference for current dictionary
	 */
    QFile getCacheFile();

    /**
	 * Reads dictionary from source file
	 */
    bool readFromSource();

	void processLine(QString &line);

    /**
	 * Some words contain multiple versions split with ; in kanji field.
	 * Return them as individual words.
	 * @return
	 */
    QList<Word> splitKanji(const Word &word);
    QList<Word> splitKana(const Word &word);
    QList<Word> splitWithMap(const Word &word);

    /**
	 * Some kanji and kana fields contain multiple options, like:
	 * 
	 * kanji:電気メーカー;電気メーカ;電機メーカー;電機メーカ
	 * kana:でんきメーカー(電気メーカー,電機メーカー);でんきメーカ(電気メーカ,電機メーカ)
	 * 
	 * Retuns a kanji -> kana map, for example: 電気メーカー -> でんきメーカー
	 * "default" -> defain kana (not mapped to any kanji) 
	 * 
	 * User is responsible for deleting the hashmap.  
	 */
    QHash<QString, QString> *buildKanjiKanaMap(const QString &multikana);

    QString &cleanStr(QString &str);

	/**
	 * Indexes words in parallel threads
	 */
	void indexWords();
};

#endif // DICTIONARYLOADER_H