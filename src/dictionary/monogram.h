#ifndef MONOGRAM_H
#define MONOGRAM_H

#include "ngram.h"

#include <QChar>

/**
 * Single character
 */
class Monogram : public NGram
{
public:
    Monogram(const QChar &first) : m_first(first) {}
    const QChar m_first;
};

inline bool operator==(const Monogram &lhs, const Monogram &rhs)
{
    return lhs.m_first == rhs.m_first;
}

inline uint qHash(const Monogram &key, uint seed)
{
    return qHash(key.m_first, seed);
}

#endif // MONOGRAM_H