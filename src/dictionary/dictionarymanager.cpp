#include "dictionarymanager.h"

void DictionaryManager::prepareDictionary(DictionaryType type)
{
    if (dictionaries[type])
        return;
    
    dictionaries[type] = nullptr; // TODO implement Dictionary Loader
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