#include "dictionarymanager.h"
#include "dictionaryloader.h"

Dictionary *DictionaryManager::dictionaries[DICTIONARY_TYPES] = { nullptr, nullptr};

void DictionaryManager::prepareDictionary(DictionaryType type)
{
    if (dictionaries[type])
        return;
     
    DictionaryLoader loader(type);
    dictionaries[type] = loader.loadDictionary();
}

Dictionary *DictionaryManager::getDictionary(DictionaryType type)
{
    prepareDictionary(type);
    return dictionaries[type];
}

void DictionaryManager::freeDictionaries()
{
    for (Dictionary *dic : dictionaries)
        delete dic;
}