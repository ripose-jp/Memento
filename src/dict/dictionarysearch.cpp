////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include "dict/dictionarysearch.h"

#include "dict/dictionarysearchcontroller.h"

/* Begin Constructor/Destructor */

DictionarySearch::DictionarySearch(QObject *parent) : QObject(parent)
{

}

DictionarySearch::~DictionarySearch()
{
    clearResults();
}

/* End Constructor/Destructor */
/* Begin Search Methods */

void DictionarySearch::searchTerms(
    const QString &query, const QString &text, qsizetype index)
{
    searchTermsAsync(query, text, index);
}

QCoro::Task<void> DictionarySearch::searchTermsAsync(
    const QString &query, const QString &text, qsizetype index)
{
    QPointer<DictionarySearch> dictionarySearch{this};
    const quint64 termsSearchId = ++m_termsSearchId;

    QList<Term *> terms = co_await DictionarySearchController::instance()
        ->searchTermsAsync(query, text, index);

    /* Make sure this object hasn't been deleted and the search isn't stale */
    if (dictionarySearch == nullptr || termsSearchId != m_termsSearchId)
    {
        qDeleteAll(terms);
        terms.clear();
        co_return;
    }

    for (Term *term : terms)
    {
        term->setParent(this);
    }

    std::swap(m_terms, terms);
    emit termsChanged();

    for (Term *term : terms)
    {
        term->deleteLater();
    }
    terms.clear();
}

void DictionarySearch::searchKanji(
    const QString &character, const QString &text, qsizetype index)
{
    searchKanjiAsync(character, text, index);
}

QCoro::Task<void> DictionarySearch::searchKanjiAsync(
    const QString &character, const QString &text, qsizetype index)
{
    QPointer<DictionarySearch> dictionarySearch{this};
    const quint64 kanjiSearchId = ++m_kanjiSearchId;

    Kanji *kanji = co_await DictionarySearchController::instance()
        ->searchKanjiAsync(character, text, index);

    /* Make sure this object hasn't been deleted and the search isn't stale */
    if (dictionarySearch == nullptr || kanjiSearchId != m_kanjiSearchId)
    {
        delete kanji;
        kanji = nullptr;
        co_return;
    }

    if (kanji != nullptr)
    {
        kanji->setParent(this);
    }

    std::swap(m_kanji, kanji);
    emit kanjiChanged();

    if (kanji != nullptr)
    {
        kanji->deleteLater();
        kanji = nullptr;
    }
}

/* End Search Methods */
/* Begin Properties */
/* Begin Clear Methods */

void DictionarySearch::clearResults()
{
    clearTerms();
    clearKanji();
}

void DictionarySearch::clearTerms()
{
    ++m_termsSearchId;
    clearTermsLater();
}

void DictionarySearch::clearTermsLater()
{
    if (m_terms.isEmpty())
    {
        return;
    }
    QList<Term *> terms = std::move(m_terms);
    m_terms.clear();
    emit termsChanged();
    for (Term *term : terms)
    {
        term->deleteLater();
    }
}

void DictionarySearch::clearKanji()
{
    ++m_kanjiSearchId;
    clearKanjiLater();
}

void DictionarySearch::clearKanjiLater()
{
    if (m_kanji)
    {
        m_kanji->deleteLater();
        m_kanji = nullptr;
        emit kanjiChanged();
    }
}

/* End Clear Methods */
/* Begin Properties */

QQmlListProperty<Term> DictionarySearch::termsQml()
{
    return QQmlListProperty<Term>(this, &m_terms);
}

Kanji *DictionarySearch::kanji() const noexcept
{
    return m_kanji;
}

/* End Properties */
