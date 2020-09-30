#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "ngram.h"

#include <QChar>

/**
 * Pair of characters
 */
class Digram : public NGram
{
public:
    Digram(const QChar &first, const QChar &second)
        : m_first(first), m_second(second) {}
    const QChar m_first;
    const QChar m_second;
};

inline bool operator==(const Digram &lhs, const Digram &rhs)
{
    return lhs.m_first == rhs.m_first && lhs.m_second == rhs.m_second;
}

inline uint qHash(const Digram &key, uint seed)
{
    return qHash(key.m_first, seed) ^ qHash(key.m_second, seed);
}

#endif // DIAGRAM_H