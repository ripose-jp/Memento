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

#include <QList>
#include <QPair>
#include <QHash>
#include <QString>

/**
 * @brief Stores the parsed form of one supported dictionary CSS selector.
 */
struct DictionaryCssSelector
{
    /* The optional tag name matched by the selector. */
    QString tag;

    /* The optional attribute name matched by the selector. */
    QString attributeName;

    /* The optional attribute value matched by the selector. */
    QString attributeValue;

    /* true if the selector only checks that the attribute exists. */
    bool attributeExists{false};
};

/**
 * @brief Stores one parsed dictionary CSS rule.
 */
struct DictionaryCssRule
{
    /* The selectors that share this declaration block. */
    QList<DictionaryCssSelector> selectors;

    /* The declaration block stored in source order. */
    QList<QPair<QString, QString>> declarations;
};

/**
 * @brief Stores one immutable parsed dictionary stylesheet.
 */
class DictionaryStylesheet
{
public:
    /**
     * @brief Construct an empty parsed stylesheet.
     */
    DictionaryStylesheet();

    /**
     * @brief Construct a parsed stylesheet.
     *
     * @param source The source stylesheet text.
     */
    explicit DictionaryStylesheet(const QString &source);

    /**
     * @brief Get the original stylesheet text.
     *
     * @return The original stylesheet text.
     */
    [[nodiscard]]
    const QString &source() const noexcept;

    /**
     * @brief Get the supported parsed stylesheet rules.
     *
     * @return The parsed rules in source order.
     */
    [[nodiscard]]
    const QList<DictionaryCssRule> &rules() const noexcept;

    /**
     * @brief Get CSS rule indexes that may match one structured node.
     *
     * @param tag The source tag name.
     * @param dataContent The data-sc-content value.
     * @param hasDataContent true if data-sc-content exists.
     * @param dataClass The data-sc-class value.
     * @param hasDataClass true if data-sc-class exists.
     * @param hasTitle true if the node has a title attribute.
     * @return Candidate rule indexes in source order.
     */
    [[nodiscard]]
    QList<int> candidateRuleIndexes(
        const QString &tag,
        const QString &dataContent,
        bool hasDataContent,
        const QString &dataClass,
        bool hasDataClass,
        bool hasTitle) const;

private:
    /**
     * @brief Parse supported stylesheet rules.
     *
     * @param source The stylesheet text to parse.
     * @return The supported parsed rules in source order.
     */
    [[nodiscard]]
    static QList<DictionaryCssRule> parse(const QString &source);

    /**
     * @brief Build selector indexes for fast candidate-rule lookup.
     */
    void buildIndexes();

    /**
     * @brief Append one index to an index list if it is missing.
     *
     * @param list The index list to update.
     * @param index The rule index to append.
     */
    static void appendIndex(QList<int> &list, int index);

    /**
     * @brief Append indexes to an index list if they are missing.
     *
     * @param list The index list to update.
     * @param indexes The indexes to append.
     */
    static void appendIndexes(
        QList<int> &list,
        const QList<int> &indexes);

    /* The original stylesheet text. */
    QString m_source;

    /* The supported parsed stylesheet rules. */
    QList<DictionaryCssRule> m_rules;

    /* Rules with selectors that match any supported node. */
    QList<int> m_globalRuleIndexes;

    /* Rules indexed by source tag name. */
    QHash<QString, QList<int>> m_tagRuleIndexes;

    /* Rules indexed by data-sc-content value. */
    QHash<QString, QList<int>> m_contentRuleIndexes;

    /* Rules that test for the presence of data-sc-content. */
    QList<int> m_contentExistsRuleIndexes;

    /* Rules indexed by data-sc-class value. */
    QHash<QString, QList<int>> m_classRuleIndexes;

    /* Rules that test for the presence of data-sc-class. */
    QList<int> m_classExistsRuleIndexes;

    /* Rules that test for the presence of title. */
    QList<int> m_titleRuleIndexes;
};
