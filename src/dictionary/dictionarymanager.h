#ifndef DICTIONARYMANAGER_H
#define DICTIONARYMANAGER_H

#include "dictionarytype.h"
#include "dictionary.h"

class DictionaryManager
{
public:
    static void prepareDictionary(DictionaryType type);
    static Dictionary *getDictionary(DictionaryType type);
    static void freeDictionaries();

private:
    static Dictionary *dictionaries[DICTIONARY_TYPES];
    DictionaryManager() {}
};

#endif // DICTIONARYMANAGER_H