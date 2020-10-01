#ifndef DICTIONARYINDEXER_H
#define DICTIONARYINDEXER_H

#include "dictionary.h"

#include <QList>
#include <QSet>
#include <QRunnable>

class DictionaryIndexer
{
public:
    void buildIndex(Dictionary *dictionary, QList<Word> *words);

    enum ThreadRole
    {
        KanjiIndex,
        DigramIndex,
        DescriptionIndex
    };
    const ThreadRole roles[] = { KanjiIndex, DigramIndex, DescriptionIndex };

private:
    Dictionary *m_dictionary;
    QList<Word> *m_words;

    class IndexBuilder : public QRunnable
    {
    public:
        IndexBuilder(DictionaryIndexer *parent, const ThreadRole role) : m_parent(parent), m_role(role) {}
        void run() Q_DECL_OVERRIDE;

    private:
        const ThreadRole m_role;
        DictionaryIndexer *m_parent;
    };

    void addKanjiIndex(const Word &word);
    void addDigramIndex(const Word &word);
    void addDigramIndex(const Word &word, const QString &str, QSet<Digram> &added);
    void addDescriptionIndex(const Word &word);
};

#endif // DICTIONARYINDEXER_H