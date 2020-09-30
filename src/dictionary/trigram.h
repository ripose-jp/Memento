#ifndef TRIGRAM_H
#define TRIGRAM_H

#include "ngram.h"

#include <QChar>

/**
 * Three characters
 */
class Trigram : public NGram
{
public:
    Trigram(const QChar &first, const QChar &second, const QChar &third)
        : m_first(first), m_second(second), m_third(third) {}
    const QChar m_first;
    const QChar m_second;
    const QChar m_third;
};

inline bool operator==(const Trigram &lhs, const Trigram &rhs)
{
    return lhs.m_first == rhs.m_first && lhs.m_second == rhs.m_second 
        && lhs.m_third == rhs.m_third;
}

inline uint qHash(const Trigram &key, uint seed)
{
    return qHash(key.m_first, seed) ^ qHash(key.m_second, seed) 
        ^ qHash(key.m_third, seed);
}

#endif // TRIGRAM_H