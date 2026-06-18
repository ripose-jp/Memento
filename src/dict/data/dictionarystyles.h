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
        AdjacentSibling,
        GeneralSibling
    };

    /**
     * @brief Attribute selector comparison supported by the CSS matcher.
     */
    enum class CssAttributeOperator
    {
        Exists,
        Equals,
        StartsWith
    };

    /**
     * @brief Pseudo-element target supported by generated-content rules.
     */
    enum class CssPseudoElement
    {
        None,
        Before,
        After
    };

    /**
     * @brief Supported CSS pseudo-class condition.
     */
    enum class CssPseudoClass
    {
        FirstChild,
        LastChild,
        NthChild
    };

    /**
     * @brief A single CSS attribute selector.
     */
    struct CssAttributeSelector
    {
        /* Attribute name to test */
        QString name;

        /* Attribute value used by value-based operators */
        QString value;

        /* Attribute comparison performed by the selector */
        CssAttributeOperator op{CssAttributeOperator::Exists};
    };

    /**
     * @brief A single CSS pseudo-class selector.
     */
    struct CssPseudoClassSelector
    {
        /* Pseudo-class condition to test */
        CssPseudoClass type{CssPseudoClass::FirstChild};

        /* Exact one-based child index for :nth-child(), or 0 otherwise */
        int childIndex{0};

        /* True when this pseudo-class came from :not(...) */
        bool negated{false};
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

        /* Attribute selector tests required by this selector part */
        QList<CssAttributeSelector> attributes;

        /* Pseudo-class selector tests required by this selector part */
        QList<CssPseudoClassSelector> pseudoClasses;
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

        /* Pseudo-element target for generated-content rules */
        CssPseudoElement pseudoElement{CssPseudoElement::None};

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

        /* True when any selector needs a total element-child count */
        bool usesElementChildCount{false};
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
     * @param pseudoElement Set to the selector's generated-content target.
     * @param specificity Set to the selector specificity.
     * @return The parsed selector parts.
     */
    [[nodiscard]]
    static QList<CssSelectorPart> parseCssSelector(
        const QString &selector,
        CssPseudoElement &pseudoElement,
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
