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

#include "dict/data/dictionarystylesheet.h"

#include <algorithm>
#include <optional>

#include <QRegularExpression>
#include <QStringList>

namespace
{
/**
 * @brief Remove block comments from a stylesheet.
 *
 * @param source The stylesheet to clean.
 * @return The stylesheet without block comments.
 */
[[nodiscard]]
static QString removeCssComments(const QString &source)
{
    static const QRegularExpression COMMENT_PATTERN{
        QStringLiteral(R"(/\*[\s\S]*?\*/)")};
    QString result{source};
    result.remove(COMMENT_PATTERN);
    return result;
}

/**
 * @brief Read a balanced block beginning at an opening brace.
 *
 * @param text The source text.
 * @param openIndex The index of the opening brace.
 * @param closeIndex The output index of the matching closing brace.
 * @return The text inside the balanced block.
 */
[[nodiscard]]
static QString readBalancedBlock(
    const QString &text,
    qsizetype openIndex,
    qsizetype &closeIndex)
{
    int depth{0};
    for (qsizetype i{openIndex}; i < text.size(); ++i)
    {
        if (text[i] == '{')
        {
            ++depth;
        }
        else if (text[i] == '}')
        {
            --depth;
            if (depth == 0)
            {
                closeIndex = i;
                return text.mid(openIndex + 1, i - openIndex - 1);
            }
        }
    }

    closeIndex = text.size();
    return text.mid(openIndex + 1);
}

/**
 * @brief Keep only top-level declarations from a declaration block.
 *
 * @param block The declaration block to filter.
 * @return Top-level declarations without nested rule bodies.
 */
[[nodiscard]]
static QString topLevelDeclarations(const QString &block)
{
    QString result;
    int depth{0};
    for (const QChar ch : block)
    {
        if (ch == '{')
        {
            ++depth;
        }
        else if (ch == '}')
        {
            --depth;
        }
        else if (depth == 0)
        {
            result += ch;
        }
    }
    return result;
}

/**
 * @brief Parse one selector supported by the native renderer.
 *
 * @param selector The selector string to parse.
 * @return The parsed selector, or std::nullopt if unsupported.
 */
[[nodiscard]]
static std::optional<DictionaryCssSelector> parseSelector(QString selector)
{
    selector = selector.trimmed();
    if (selector.isEmpty() ||
        selector.contains('&') ||
        selector.contains('>') ||
        selector.contains('+') ||
        selector.contains('~') ||
        selector.contains(':') ||
        selector.contains(' '))
    {
        return std::nullopt;
    }

    static const QRegularExpression SELECTOR_PATTERN{
        QStringLiteral(
            "^([a-zA-Z0-9_-]+)?(?:\\[([a-zA-Z0-9_-]+)"
            "(?:=\"([^\"]*)\")?\\])?$")};
    const QRegularExpressionMatch match{
        SELECTOR_PATTERN.match(selector)};
    if (!match.hasMatch())
    {
        return std::nullopt;
    }

    DictionaryCssSelector parsed;
    parsed.tag = match.captured(1);
    parsed.attributeName = match.captured(2);
    parsed.attributeValue = match.captured(3);
    parsed.attributeExists = !parsed.attributeName.isEmpty() &&
        parsed.attributeValue.isEmpty();
    return parsed;
}

/**
 * @brief Parse supported declarations from one CSS block.
 *
 * @param block The CSS declaration block.
 * @return The parsed declarations in source order.
 */
[[nodiscard]]
static QList<QPair<QString, QString>> parseDeclarations(const QString &block)
{
    QList<QPair<QString, QString>> result;
    const QStringList declarations{
        topLevelDeclarations(block).split(';', Qt::SkipEmptyParts)};
    for (const QString &declaration : declarations)
    {
        const qsizetype separator{declaration.indexOf(':')};
        if (separator < 0)
        {
            continue;
        }
        result.append({
            declaration.left(separator).trimmed().toLower(),
            declaration.mid(separator + 1).trimmed()
        });
    }
    return result;
}
} // namespace

DictionaryStylesheet::DictionaryStylesheet() = default;

DictionaryStylesheet::DictionaryStylesheet(const QString &source) :
    m_source{source},
    m_rules{parse(source)}
{
    buildIndexes();
}

const QString &DictionaryStylesheet::source() const noexcept
{
    return m_source;
}

const QList<DictionaryCssRule> &DictionaryStylesheet::rules() const noexcept
{
    return m_rules;
}

QList<int> DictionaryStylesheet::candidateRuleIndexes(
    const QString &tag,
    const QString &dataContent,
    bool hasDataContent,
    const QString &dataClass,
    bool hasDataClass,
    bool hasTitle) const
{
    QList<int> indexes;
    appendIndexes(indexes, m_globalRuleIndexes);
    appendIndexes(indexes, m_tagRuleIndexes.value(tag));
    if (hasDataContent)
    {
        appendIndexes(indexes, m_contentRuleIndexes.value(dataContent));
        appendIndexes(indexes, m_contentExistsRuleIndexes);
    }
    if (hasDataClass)
    {
        appendIndexes(indexes, m_classRuleIndexes.value(dataClass));
        appendIndexes(indexes, m_classExistsRuleIndexes);
    }
    if (hasTitle)
    {
        appendIndexes(indexes, m_titleRuleIndexes);
    }

    std::sort(indexes.begin(), indexes.end());
    return indexes;
}

QList<DictionaryCssRule> DictionaryStylesheet::parse(const QString &source)
{
    QList<DictionaryCssRule> rules;
    const QString stylesheet{removeCssComments(source)};
    qsizetype index{0};
    while (index < stylesheet.size())
    {
        const qsizetype openBrace{stylesheet.indexOf('{', index)};
        if (openBrace < 0)
        {
            break;
        }
        const QString selectorText{
            stylesheet.mid(index, openBrace - index).trimmed()};
        qsizetype closeBrace{openBrace};
        const QString block{
            readBalancedBlock(stylesheet, openBrace, closeBrace)};
        index = closeBrace + 1;

        DictionaryCssRule rule;
        const QStringList selectors{
            selectorText.split(',', Qt::SkipEmptyParts)};
        for (const QString &selectorText : selectors)
        {
            const std::optional<DictionaryCssSelector> selector{
                parseSelector(selectorText)};
            if (selector.has_value())
            {
                rule.selectors.append(*selector);
            }
        }
        rule.declarations = parseDeclarations(block);
        if (!rule.selectors.isEmpty() && !rule.declarations.isEmpty())
        {
            rules.append(rule);
        }
    }
    return rules;
}

void DictionaryStylesheet::buildIndexes()
{
    for (int i{0}; i < m_rules.size(); ++i)
    {
        for (const DictionaryCssSelector &selector : m_rules[i].selectors)
        {
            if (selector.attributeName == QStringLiteral("data-sc-content"))
            {
                if (selector.attributeExists)
                {
                    appendIndex(m_contentExistsRuleIndexes, i);
                }
                else
                {
                    appendIndex(
                        m_contentRuleIndexes[selector.attributeValue], i);
                }
            }
            else if (selector.attributeName ==
                     QStringLiteral("data-sc-class"))
            {
                if (selector.attributeExists)
                {
                    appendIndex(m_classExistsRuleIndexes, i);
                }
                else
                {
                    appendIndex(
                        m_classRuleIndexes[selector.attributeValue], i);
                }
            }
            else if (selector.attributeName == QStringLiteral("title"))
            {
                appendIndex(m_titleRuleIndexes, i);
            }
            else if (selector.attributeName.isEmpty() &&
                     !selector.tag.isEmpty())
            {
                appendIndex(m_tagRuleIndexes[selector.tag], i);
            }
            else if (selector.attributeName.isEmpty())
            {
                appendIndex(m_globalRuleIndexes, i);
            }
        }
    }
}

void DictionaryStylesheet::appendIndex(QList<int> &list, int index)
{
    if (!list.contains(index))
    {
        list.append(index);
    }
}

void DictionaryStylesheet::appendIndexes(
    QList<int> &list,
    const QList<int> &indexes)
{
    for (int index : indexes)
    {
        appendIndex(list, index);
    }
}
