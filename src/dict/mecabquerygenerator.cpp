////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2024 Ripose
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

#include "mecabquerygenerator.h"

#include <QDebug>
#include <QtGlobal>

#include "util/utils.h"

/* Begin Static Helpers */

#if defined(Q_OS_WIN)
/**
 * This whole section is necessary on Windows because MeCab has a bug that
 * prevents it from loading dictionaries if there are spaces in the path on
 * Windows. If Memento is to be install in "Program Files", this quickly
 * becomes an issue. This workaround turns all long paths into space-less
 * short paths.
 */
#ifndef NOMINMAX
#define NOMINMAX 1
#endif // NOMINMAX
#include <Windows.h>

#include <fileapi.h>

/**
 * Takes a Windows long path and returns an 8.3/short path.
 * @param path The Window long path to convert.
 * @return A Windows short path, or the empty string on error.
 */
static QByteArray toWindowsShortPath(const QString &path)
{
    QByteArray pathArr = path.toUtf8();
    DWORD length = 0;

    length = GetShortPathNameA(pathArr.constData(), NULL, 0);
    if (length == 0)
    {
        return "";
    }

    QByteArray buf(length, '\0');
    length = GetShortPathNameA(pathArr, buf.data(), length);
    if (length == 0)
    {
        return "";
    }
    buf.chop(1);
    return buf;
}

/**
 * Generates the MeCab argument on Windows.
 * @return An argument to pass MeCab so it uses the install's ipadic.
 */
static QByteArray genMecabArg()
{
    QByteArray arg = "-r ";
    arg += toWindowsShortPath(
        DirectoryUtils::getDictionaryDir() + "ipadic" + SLASH + "dicrc"
    );
    arg += " -d ";
    arg += toWindowsShortPath(DirectoryUtils::getDictionaryDir() + "ipadic");
    return arg;
}
#endif

/* End Static Helpers */
/* Begin Constructor */

MeCabQueryGenerator::MeCabQueryGenerator()
{
#if defined(Q_OS_WIN)
    QByteArray mecabArg = genMecabArg();
#elif defined(APPIMAGE) || defined(APPBUNDLE)
    QByteArray mecabArg = ( \
        "-r " + DirectoryUtils::getDictionaryDir() + "ipadic" + SLASH + "dicrc " \
        "-d " + DirectoryUtils::getDictionaryDir() + "ipadic" \
    ).toUtf8();
#else
    QByteArray mecabArg = "";
#endif
    m_tagger.reset(MeCab::createTagger(mecabArg));
    if (m_tagger == nullptr)
    {
        qDebug() << MeCab::getTaggerError();
    }
}

/* End Constructor */
/* Begin Query Generator */

std::vector<SearchQuery> MeCabQueryGenerator::generateQueries(
    const QString &text) const
{
    if (!valid() || text.isEmpty())
    {
        return {};
    }

    std::unique_ptr<MeCab::Lattice> lattice(MeCab::createLattice());
    QByteArray textArr = text.toUtf8();
    lattice->set_sentence(textArr);
    if (!m_tagger->parse(lattice.get()))
    {
        qDebug() << "Cannot access MeCab";
        qDebug() << MeCab::getLastError();
        return {};
    }
    std::vector<MeCabQuery> mecabQueries =
        generateQueriesHelper(lattice->bos_node()->next);

    std::vector<SearchQuery> queries;
    queries.reserve(mecabQueries.size());
    std::copy(
        std::begin(mecabQueries), std::end(mecabQueries),
        std::back_inserter(queries)
    );
    return queries;
}

std::vector<MeCabQueryGenerator::MeCabQuery>
MeCabQueryGenerator::generateQueriesHelper(const MeCab::Node *node)
{
    std::vector<MeCabQuery> queries;
    while (node)
    {
        QString deconj = extractDeconjugation(node);
        QString surface = extractSurface(node);
        QString surfaceClean = extractCleanSurface(node);
        if (deconj != "*")
        {
            MeCabQuery query;
            query.deconj = deconj;
            query.surface = surface;
            query.surfaceClean = surfaceClean;
            query.source = SearchQuery::Source::mecab;
            queries.emplace_back(std::move(query));
        }

        if (node->next)
        {
            std::vector<MeCabQuery> subQueries =
                generateQueriesHelper(node->next);
            for (MeCabQuery &p : subQueries)
            {
                p.deconj.prepend(surfaceClean);
                p.surface.prepend(surface);
                p.surfaceClean.prepend(surfaceClean);
                queries.emplace_back(std::move(p));
            }
        }

        node = node->bnext;
    }
    return queries;
}

inline QString MeCabQueryGenerator::extractDeconjugation(
    const MeCab::Node *node)
{
    constexpr int WORD_INDEX{6};
    QStringList features = QString::fromUtf8(node->feature).split(',');
    if (features.size() <= WORD_INDEX)
    {
        return "";
    }
    return features[WORD_INDEX];
}

inline QString MeCabQueryGenerator::extractSurface(const MeCab::Node *node)
{
    const char *rawText = node->surface;
    rawText -= node->rlength - node->length;
    return QString::fromUtf8(rawText, node->rlength);
}

inline QString MeCabQueryGenerator::extractCleanSurface(const MeCab::Node *node)
{
    return QString::fromUtf8(node->surface, node->length);
}

/* End Query Generator */
