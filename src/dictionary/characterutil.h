#ifndef CHARACTERUTIL_H
#define CHARACTERUTIL_H

#include <QChar>
#include <QString>

#define VERTICAL_BAR '｜'

#define HIRAGANA_LOWER 0x3040
#define HIRAGANA_UPPER 0x309F

#define KATAKANA_LOWER 0x30A0
#define KATAKANA_UPPER 0x30FF

#define ITERATION_MARK 0x3005
#define KANJI_LOWER 0x4E00
#define KANJI_UPPER 0x9FAF

#define SMALL_KE 0x30F6
#define KANA_UNICODE_DIFFERENCE 0x60

/**
 * Utility methods for classifying characters
 */
class CharacterUtil
{
public:
    static bool isHiragana(const QChar &c);
    static bool isKatakana(const QChar &c);
    static bool isKana(const QChar &c);
    static bool isKanji(const QChar &c);

    /**
     * Converts katakana to hiragana, replaces synonyms and converts
     * to upper case (if alphabet).
     */
    static QString toCanonical(const QString &str);

    /**
     * Converts katakana to hiragana, replaces synonyms and converts
     * to upper case (if alphabet).
     */
    static QChar toCanonical(const QChar &c);

    /**
     * Removes kana characters from the end of str.
     * Does nothing if all characters are kana.
     */
    static QString removeTrailingKana(const QString &str);

    /**
     * Returns true if str has trailing kana.
     */
    static bool hasTrailingKana(const QString &str);

private:
    CharacterUtil() {}

    /** 
     * Converts katakana to hiragana.
     * Returns the same character if not katakana.
     */
    static QChar toHiragana(const QChar &c);
};

#endif // CHARACTERUTIL_H