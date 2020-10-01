#ifndef DICTIONARYLOADER_H
#define DICTIONARYLOADER_H

#define DEFAULT_DICT_FILENAME "edict2"
#define NAMES_DICT_FILENAME "enamdict"

#include "dictionarytype.h"
#include "dictionary.h"

#include <QList>
#include <QFile>

class DictionaryLoader
{
public:
    DictionaryLoader(DictionaryType type);
    ~DictionaryLoader();

    /**
	 * Loads dictionary from cache if present, else from source 
	 */
    Dictionary *loadDictionary();

private:
    Dictionary *m_dictionary;
    DictionaryType m_type;
    QList<Word> *m_words;

    /**
	 * True if dictionary cache file is outdated and needs to be recreated
	 * (source file is newer)
	 */
	bool isCacheOutdated();

    /**
	 * Gets dictionary file reference for current dictionary
	 */
    QFile getDictionaryFile();
};

#endif // DICTIONARYLOADER_H