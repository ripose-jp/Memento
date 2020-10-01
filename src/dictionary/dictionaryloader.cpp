#include "dictionaryloader.h"
#include "../util/directoryutils.h"

#include <QDebug>

DictionaryLoader::DictionaryLoader(DictionaryType type) : m_type(type),
                                                          m_words(new QList<Word>) {}

DictionaryLoader::~DictionaryLoader()
{
    delete m_words;
}

Dictionary *DictionaryLoader::loadDictionary()
{
    
}

bool DictionaryLoader::isCacheOutdated()
{

}

QFile DictionaryLoader::getDictionaryFile()
{
    QString path = DirectoryUtils::getDictionaryDir() + "/";
    switch (m_type)
    {
        case JAPANESE_DEFAULT:
            path += DEFAULT_DICT_FILENAME;
            break;
        case JAPANESE_NAMES:
            path += NAMES_DICT_FILENAME;
            break;
        default:
            qDebug() << "Dictionary not selected";
            break;
    }
    return QFile(path);
}