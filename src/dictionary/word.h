#ifndef WORD_H
#define WORD_H

#include <QString>

struct Word
{
    Word(QString& kanji, QString& kana, QString &description, bool name);
    ~Word();
    bool operator==(const Word &rhs);

    /**
	 * Word in kanji form (might also contain kana characters)
	 */
    QString *m_kanji;

    /**
	 * Word in kana form
	 */
    QString *m_kana;

    /**
	 * English description
	 */
    QString *m_description;

    /**
	 * If true, this is a common word. 
	 */
    bool m_common;

    /**
	 * If true, this word is from names dictionary.
	 * If false, this word is from default dictionary.
	 */
    bool m_name;

    /**
	 * Number of kanji characters in the kanji field
	 */
    int m_kanjiCount; 
} typedef Word;

#endif // WORD_H