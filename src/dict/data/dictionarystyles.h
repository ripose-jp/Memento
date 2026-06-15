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

#pragma once

#include <QHash>
#include <QList>
#include <QSet>
#include <QString>

/**
 * @brief Class holding dictionary style information.
 */
class DictionaryStyles
{
public:
    /**
     * @brief Create a DictionaryStyles from a styles.css content string.
     *
     * @param stylesheet The text of the styles.css file.
     */
    DictionaryStyles(const QString &stylesheet);

    /**
     * @brief Relationship between a selector part and the preceding part.
     */
    enum class CssCombinator
    {
        Descendant,
        Child,
        AdjacentSibling
    };

    /**
     * @brief A single simple selector in a CSS selector chain.
     */
    struct CssSelectorPart
    {
        /* Relationship to the preceding selector part */
        CssCombinator combinator{CssCombinator::Descendant};

        /* Optional tag name */
        QString tag;

        /* Required class names */
        QSet<QString> classNames;

        /* Attributes that must equal a specific value */
        QHash<QString, QString> attributes;

        /* Attributes that only need to be present */
        QSet<QString> presentAttributes;

        /* True if the element must be its parent's first element child */
        bool firstChild{false};
    };

    /**
     * @brief One CSS declaration in source order.
     */
    struct CssDeclaration
    {
        /* Lowercase CSS property name */
        QString property;

        /* Trimmed CSS property value */
        QString value;
    };

    /**
     * @brief A parsed CSS rule that can be applied to structured content.
     */
    struct CssRule
    {
        /* The selector parts, ordered from ancestor to target */
        QList<CssSelectorPart> selector;

        /* The CSS declarations in source order */
        QList<CssDeclaration> declarations;

        /* True if this rule applies to ::before content */
        bool before{false};

        /* The selector specificity */
        int specificity{0};

        /* The rule's source order */
        int order{0};
    };

    /**
     * @brief Parsed dictionary CSS that can be shared by definitions.
     */
    struct ParsedStylesheet
    {
        /* The parsed CSS rules */
        QList<CssRule> rules;

        /* Rule indexes whose target selector has no tag restriction */
        QList<qsizetype> universalRuleIndexes;

        /* Ordered candidate rule indexes for each target tag */
        QHash<QString, QList<qsizetype>> ruleIndexesByTargetTag;
    };

    /**
     * @brief Get the raw stylesheet text.
     *
     * @return The raw stylesheet text.
     */
    [[nodiscard]]
    const QString &stylesheet() const noexcept;

    /**
     * @brief Get the parsed stylesheet.
     *
     * @return The parsed stylesheet.
     */
    [[nodiscard]]
    const ParsedStylesheet &parsedStylesheet() const noexcept;

    /**
     * @brief Get ordered CSS rule candidates for an element tag.
     *
     * @param tag The original structured element tag.
     * @return Rule indexes that can match the target tag.
     */
    [[nodiscard]]
    const QList<qsizetype> &candidateRuleIndexes(
        const QString &tag) const noexcept;

private:
    /**
     * @brief Parse dictionary CSS rules supported by Qt rich text.
     *
     * @param css The dictionary stylesheet.
     * @return The parsed stylesheet.
     */
    [[nodiscard]]
    static ParsedStylesheet parseStylesheet(const QString &css);

    /**
     * @brief Parse a CSS selector into selector parts.
     *
     * @param selector The selector to parse.
     * @param before Set true if the selector targets ::before.
     * @param specificity Set to the selector specificity.
     * @return The parsed selector parts.
     */
    [[nodiscard]]
    static QList<CssSelectorPart> parseCssSelector(
        const QString &selector,
        bool &before,
        int &specificity);

    /**
     * @brief Parse CSS declarations into a declaration list.
     *
     * @param body The CSS declaration body.
     * @return The parsed declarations in source order.
     */
    [[nodiscard]]
    static QList<CssDeclaration> parseCssDeclarations(const QString &body);

    /* Raw content of the CSS stylesheet */
    const QString m_stylesheet;

    /* The parsed stylesheet */
    const ParsedStylesheet m_parsedStylesheet{};
};
