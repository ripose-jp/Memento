#include "dictionaryloader.h"
#include "../util/directoryutils.h"
#include "dictionaryindexer.h"

#include <QFileInfo>
#include <QDateTime>
#include <QDataStream>
#include <QDebug>

DictionaryLoader::DictionaryLoader(DictionaryType type) : m_type(type),
                                                          m_words(new QList<Word>) {}

DictionaryLoader::~DictionaryLoader()
{
    delete m_words;
}

Dictionary *DictionaryLoader::loadDictionary()
{
    QFile cacheFile(getCacheFile());
    if (!isCacheOutdated() && fromDataStream(cacheFile))
    {
        indexWords();
        return m_dictionary;
    }
    
    if (!readFromSource())
    {
        return nullptr;
    }
    
    return loadDictionary();
}

bool DictionaryLoader::isCacheOutdated()
{
    QFile sourceFile(getDictionaryFile());
    if (!sourceFile.exists())
    {
        qDebug() << DICT_MISSING_ERR;
        return false;
    }
    QFileInfo sourceFileInfo(sourceFile);
    QDateTime sourceModified = sourceFileInfo.lastModified();

    QFile cacheFile(getCacheFile());
    if (!sourceFile.exists())
    {
        qDebug() << CACHE_MISSING_ERR;
        return false;
    }
    QFileInfo cacheFileInfo(cacheFile);
    QDateTime cacheModified = cacheFileInfo.lastModified();

    return sourceModified.currentMSecsSinceEpoch() > cacheModified.currentMSecsSinceEpoch();
}

QString DictionaryLoader::getDictionaryFile()
{
    QString path = DirectoryUtils::getDictionaryDir();
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
    return path;
}

QString DictionaryLoader::getCacheFile()
{
    QString path = DirectoryUtils::getCacheDir();
    switch (m_type)
    {
        case JAPANESE_DEFAULT:
            path += DEFAULT_CACHE_FILENAME;
            break;
        case JAPANESE_NAMES:
            path += NAMES_CACHE_FILENAME;
            break;
        default:
            qDebug() << "Dictionary not selected";
            break;
    }
    return path;
}

bool DictionaryLoader::readFromSource()
{
    m_dictionary = new Dictionary(m_type);
    QFile dictFile(getDictionaryFile());
    if (!dictFile.exists())
    {
        qDebug() << DICT_MISSING_ERR;
        return false;
    }

    if (!dictFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Can't open dictionary file!";
        return false;
    }

    while (!dictFile.atEnd())
    {
        QString line = dictFile.readLine();
        processLine(line);
    }

    // Remove (...)
    for (const Word word : *m_words)
    {
        int pos = word.m_kanji->indexOf('(');
        if (pos != -1)
            word.m_kanji->truncate(pos);
    }

    return true;
}

void DictionaryLoader::processLine(QString &line)
{
    QString kanji;
    QString kana;
    bool readKana;
    uint i = 0;
    while (true)
    {
        QChar c = line[i++];

        if (c == '/')
        {
            break;
        }
        else if (c == '[')
        {
            readKana = true;
        }
        else if (readKana && ']')
        {
            readKana = false;
        }
        else if (readKana)
        {
            kana.append(c);
        }
        else if (c == ' ')
        {
            continue;
        }
        else
        {
            kanji.append(c);
        }
    }

    QString description;
    while (true)
    {
        QChar c = line[i++];
        
        if (c == '/')
        {
            c = '\n';
        }

        description.append(c);

        if (i == line.length() - 1)
        {
            break;
        }
    }

    Word word(kanji,
              kana.length() > 0 ? kana : kanji,
              description,
              m_type == JAPANESE_NAMES);
    QList<Word> splitWords = word.m_name ? splitKanji(word) : splitWithMap(word);
    for (const Word splitWord : splitWords)
    {
        m_words->append(splitKana(splitWord));
    }
}

QList<Word> DictionaryLoader::splitKanji(const Word &word)
{
    QList<Word> words;
    for (QString kanji : word.m_kanji->split(';'))
        words.append(Word(cleanStr(kanji), *word.m_kana, *word.m_description, word.m_name));
    return words;
}

QList<Word> DictionaryLoader::splitKana(const Word &word)
{
    QList<Word> words;
    for (QString kana : word.m_kana->split(';'))
        words.append(Word(*word.m_kanji, cleanStr(kana), *word.m_description, word.m_name));
    return words;
}

QList<Word> DictionaryLoader::splitWithMap(const Word &word)
{
    QHash<QString, QString> *kanaMap = buildKanjiKanaMap(*word.m_kana);
    QList<Word> words;
    for (QString kanji : word.m_kanji->split(';'))
    {
        QString kana;
        if (kanaMap->contains(kanji))
        {
            kana = *kanaMap->find(kanji);
        }
        else if (kanaMap->contains("default"))
        {
            kana = *kanaMap->find("default");
        }
        else
        {
            kana = *word.m_kana;
        }
        
        words.append(Word(cleanStr(kanji), kana, *word.m_description, word.m_name));
    }
    delete kanaMap;

    return words;
}

QHash<QString, QString> *DictionaryLoader::buildKanjiKanaMap(const QString &multikana)
{
    QString defaultStr;
    QHash<QString, QString> *map = new QHash<QString, QString>();
    QString queryStr(multikana);
    for (const QString snip : cleanStr(queryStr).split(';'))
    {
        if (snip.contains(m_kanaMapExp))
        {
            QStringList matches = snip.split(m_kanaMapExp);
            const QString kana = matches[1];
            for (const QString kanji : matches[2].split(','))
                map->insert(kanji, kana);
        }
        else if (defaultStr.isEmpty())
        {
            defaultStr = snip;
        }
        else
        {
            defaultStr += ';' + snip;
        }
    }
    map->insert("default", defaultStr);
    return map;
}

QString &DictionaryLoader::cleanStr(QString &str)
{
    return str.remove("\\(P\\)")
              .remove("\\(ok\\)")
              .remove("\\(oK\\)")
              .remove("\\(ik\\)")
              .remove("\\(iK\\)")
              .remove("\\(\\)")
              .remove("\\(gikun\\)");
}

void DictionaryLoader::toDataStream(QFile &file)
{
    QDataStream out(&file);
    out << *m_words;
}

bool DictionaryLoader::fromDataStream(QFile &file)
{
    if (!file.exists())
    {
        qDebug() << CACHE_MISSING_ERR;
        return false;
    }

    QDataStream in(&file);
    delete m_words;
    m_words = new QList<Word>();
    in >> *m_words;

    return true;
}

void DictionaryLoader::indexWords()
{
    DictionaryIndexer indexer;
    indexer.buildIndex(m_dictionary, m_words);
}