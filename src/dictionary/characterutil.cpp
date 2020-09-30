#include "characterutil.h"

#include <QStringBuilder>

bool CharacterUtil::isHiragana(const QChar &c)
{
    if (c == VERTICAL_BAR)
    {
        return true;
    }
    return c >= HIRAGANA_LOWER && c <= HIRAGANA_UPPER;
}

bool CharacterUtil::isKatakana(const QChar &c)
{
    if (c == VERTICAL_BAR)
    {
        return true;
    }
    return c >= KATAKANA_LOWER && c <= KATAKANA_UPPER;
}

bool CharacterUtil::isKana(const QChar &c)
{
    return isHiragana(c) || isKatakana(c);
}

bool CharacterUtil::isKanji(const QChar &c)
{
    if (c == ITERATION_MARK)
    {
        return true;
    }
    return c >= KANJI_LOWER && c <= KANJI_UPPER;
}

QChar CharacterUtil::toHiragana(const QChar &c)
{
    if (!isKatakana(c))
        return c;
    if (c > SMALL_KE)
        return c;
    return QChar(c.unicode() - KANA_UNICODE_DIFFERENCE);
}

QString CharacterUtil::toCanonical(const QString &str)
{
    QString converted;
    for (auto it = str.begin(); it != str.end(); ++it)
        converted.push_front(toCanonical(*it));
    return converted;
}

QChar CharacterUtil::toCanonical(const QChar &c)
{
    QChar converted = toHiragana(c);
    switch (converted.unicode())
    {
        case 'っ':
            return 'つ';
        case 'ゃ':
            return 'や';
        case 'ゅ':
            return 'ゆ';
        case 'ょ':
            return 'よ';
        case 'タ':
            return '夕';
        case 'ロ':
            return '口';
        case '|':
            return '一';
        case '｜':
            return '一';
        case 'ー':
            return '一';
        default:
            return converted;
    }
}

QString CharacterUtil::removeTrailingKana(const QString &str)
{
    int endIndex = str.length();
    for (unsigned int i = str.length() - 1; i >= 0; --i)
    {
        if (isKana(str[i]))
            --endIndex;
        else
            break;
    }
    if (endIndex > 0)
        return str.chopped(endIndex);
    return str;
}

bool CharacterUtil::hasTrailingKana(const QString &str)
{
    unsigned int trailingKana = 0;
    for (unsigned int i = str.length() - 1; i >= 0; --i)
    {
        if (isKana(str[i]))
            ++trailingKana;
        else
            break;
    }
    if (trailingKana == 0 || trailingKana == str.length())
        return false;
    return true;
}