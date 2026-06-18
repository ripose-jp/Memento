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

#include "dict/data/dictionarystyles.h"

DictionaryStyles::DictionaryStyles(const QString &stylesheet) :
    m_stylesheet(stylesheet),
    m_parsedStylesheet(parseStylesheet(m_stylesheet))
{

}

const QString &DictionaryStyles::stylesheet() const noexcept
{
    return m_stylesheet;
}

const DictionaryStyles::ParsedStylesheet &
DictionaryStyles::parsedStylesheet() const noexcept
{
    return m_parsedStylesheet;
}

const QList<qsizetype> &DictionaryStyles::candidateRuleIndexes(
    const QString &tag) const noexcept
{
    const auto rules =
        m_parsedStylesheet.ruleIndexesByTargetTag.constFind(tag);
    return rules == m_parsedStylesheet.ruleIndexesByTargetTag.cend() ?
        m_parsedStylesheet.universalRuleIndexes :
        rules.value();
}

DictionaryStyles::ParsedStylesheet DictionaryStyles::parseStylesheet(
    const QString &css)
{
    QString source;
    bool inComment = false;
    for (qsizetype i = 0; i < css.size(); ++i)
    {
        if (!inComment && i + 1 < css.size() &&
            css[i] == '/' && css[i + 1] == '*')
        {
            inComment = true;
            ++i;
            continue;
        }
        if (inComment && i + 1 < css.size() &&
            css[i] == '*' && css[i + 1] == '/')
        {
            inComment = false;
            ++i;
            continue;
        }
        if (!inComment)
        {
            source += css[i];
        }
    }

    QList<CssRule> rules;
    int order = 0;

    const auto findMatchingBrace =
    [] (const QString &str, qsizetype open) -> qsizetype
    {
        qsizetype depth = 1;
        for (qsizetype i = open + 1; i < str.size(); ++i)
        {
            if (str[i] == '{')
            {
                ++depth;
            }
            else if (str[i] == '}' && --depth == 0)
            {
                return i;
            }
        }
        return -1;
    };

    /* Finds the end of a top-level CSS at-rule without parsing its body */
    const auto skipAtRule =
    [&findMatchingBrace] (const QString &str, qsizetype start) -> qsizetype
    {
        const qsizetype semicolon = str.indexOf(';', start);
        const qsizetype brace = str.indexOf('{', start);
        if (brace >= 0 && (semicolon < 0 || brace < semicolon))
        {
            const qsizetype close = findMatchingBrace(str, brace);
            return close < 0 ? str.size() : close;
        }
        if (semicolon >= 0)
        {
            return semicolon;
        }
        return str.size();
    };

    const auto splitSelectors =
    [] (const QString &selector) -> QStringList
    {
        QStringList selectors;
        QString current;
        bool inQuote = false;
        QChar quote;
        int bracketDepth = 0;

        for (const QChar ch : selector)
        {
            if (inQuote)
            {
                current += ch;
                if (ch == quote)
                {
                    inQuote = false;
                }
            }
            else if (ch == '"' || ch == '\'')
            {
                inQuote = true;
                quote = ch;
                current += ch;
            }
            else if (ch == '[')
            {
                ++bracketDepth;
                current += ch;
            }
            else if (ch == ']')
            {
                --bracketDepth;
                current += ch;
            }
            else if (ch == ',' && bracketDepth == 0)
            {
                selectors.emplaceBack(current.trimmed());
                current.clear();
            }
            else
            {
                current += ch;
            }
        }

        if (!current.trimmed().isEmpty())
        {
            selectors.emplaceBack(current.trimmed());
        }
        return selectors;
    };

    std::function<void(const QStringList &, const QString &)> parseBody =
    [&] (const QStringList &selectors, const QString &body)
    {
        QString declarationsText;
        qsizetype last = 0;

        for (qsizetype i = 0; i < body.size(); ++i)
        {
            if (body[i] != '{')
            {
                continue;
            }

            qsizetype selectorStart = body.lastIndexOf(';', i);
            selectorStart = selectorStart < last ? last : selectorStart + 1;
            declarationsText += body.sliced(last, selectorStart - last);

            const QString nestedSelector =
                body.sliced(selectorStart, i - selectorStart).trimmed();
            const qsizetype close = findMatchingBrace(body, i);
            if (close < 0)
            {
                break;
            }

            QStringList combined;
            for (const QString &parent : selectors)
            {
                for (const QString &nested : splitSelectors(nestedSelector))
                {
                    if (nested.contains('&'))
                    {
                        combined.emplaceBack(nested.trimmed().replace(
                            "&", parent
                        ));
                    }
                    else
                    {
                        combined.emplaceBack(parent + ' ' + nested);
                    }
                }
            }

            parseBody(combined, body.sliced(i + 1, close - i - 1));
            i = close;
            last = close + 1;
        }

        declarationsText += body.sliced(last);
        const QList<CssDeclaration> declarations =
            parseCssDeclarations(declarationsText);
        if (declarations.isEmpty())
        {
            return;
        }

        for (const QString &selector : selectors)
        {
            CssPseudoElement pseudoElement = CssPseudoElement::None;
            int specificity = 0;
            QList<CssSelectorPart> parts = parseCssSelector(
                selector,
                pseudoElement,
                specificity
            );
            if (parts.isEmpty())
            {
                continue;
            }

            rules.emplaceBack(CssRule{
                parts,
                declarations,
                pseudoElement,
                specificity,
                order++
            });
        }
    };

    for (qsizetype i = 0; i < source.size(); ++i)
    {
        while (i < source.size() && source[i].isSpace())
        {
            ++i;
        }
        if (i >= source.size())
        {
            break;
        }
        if (source[i] == '@')
        {
            i = skipAtRule(source, i);
            continue;
        }

        const qsizetype open = source.indexOf('{', i);
        if (open < 0)
        {
            break;
        }

        const qsizetype close = findMatchingBrace(source, open);
        if (close < 0)
        {
            break;
        }

        parseBody(
            splitSelectors(source.sliced(i, open - i).trimmed()),
            source.sliced(open + 1, close - open - 1)
        );
        i = close;
    }

    std::sort(
        std::begin(rules), std::end(rules),
        [] (const CssRule &lhs, const CssRule &rhs) -> bool
        {
            return lhs.specificity < rhs.specificity ||
                (
                    lhs.specificity == rhs.specificity &&
                    lhs.order < rhs.order
                );
        }
    );

    ParsedStylesheet parsed;
    parsed.rules = std::move(rules);

    QSet<QString> targetTags;
    for (qsizetype i = 0; i < parsed.rules.size(); ++i)
    {
        const CssRule &rule = parsed.rules[i];
        for (const CssSelectorPart &part : rule.selector)
        {
            for (const CssPseudoClassSelector &pseudo :
                 part.pseudoClasses)
            {
                parsed.usesElementChildCount =
                    parsed.usesElementChildCount ||
                    pseudo.type == CssPseudoClass::LastChild;
            }
        }
        const QString targetTag =
            rule.selector.isEmpty() ? "" : rule.selector.back().tag;
        if (targetTag.isEmpty())
        {
            parsed.universalRuleIndexes.emplaceBack(i);
        }
        else
        {
            targetTags.insert(targetTag);
        }
    }

    for (const QString &tag : targetTags)
    {
        QList<qsizetype> &indexes =
            parsed.ruleIndexesByTargetTag[tag];
        indexes.reserve(parsed.rules.size());
        for (qsizetype i = 0; i < parsed.rules.size(); ++i)
        {
            const CssRule &rule = parsed.rules[i];
            const QString targetTag =
                rule.selector.isEmpty() ? "" : rule.selector.back().tag;
            if (targetTag.isEmpty() || targetTag == tag)
            {
                indexes.emplaceBack(i);
            }
        }
    }

    return parsed;
}

QList<DictionaryStyles::CssSelectorPart> DictionaryStyles::parseCssSelector(
    const QString &selector,
    CssPseudoElement &pseudoElement,
    int &specificity)
{
    /* Specificity weight for one class, attribute, or pseudo-class */
    constexpr int CLASS_SPECIFICITY = 10;

    /* Specificity weight for one tag or pseudo-element */
    constexpr int TAG_SPECIFICITY = 1;

    /* Character count for the attribute prefix-match operator */
    constexpr qsizetype PREFIX_ATTRIBUTE_OPERATOR_LENGTH = 2;

    /* Character count for the attribute equality operator */
    constexpr qsizetype EQUAL_ATTRIBUTE_OPERATOR_LENGTH = 1;

    /* Character count for a CSS pseudo-element prefix */
    constexpr qsizetype PSEUDO_ELEMENT_PREFIX_LENGTH = 2;

    /* Prefix that begins a supported exact numeric :nth-child() selector */
    const QString nthChildPrefix = ":nth-child(";

    /* Prefix that begins a supported simple :not() wrapper */
    const QString notPseudoPrefix = ":not(";

    pseudoElement = CssPseudoElement::None;
    specificity = 0;

    QString normalized = selector.trimmed();
    if (normalized.contains("::"))
    {
        const qsizetype pseudoStart = normalized.indexOf("::");
        qsizetype pseudoEnd =
            pseudoStart + PSEUDO_ELEMENT_PREFIX_LENGTH;
        while (pseudoEnd < normalized.size() &&
               (normalized[pseudoEnd].isLetter() ||
                normalized[pseudoEnd] == '-'))
        {
            ++pseudoEnd;
        }

        const QString pseudo =
            normalized.sliced(pseudoStart, pseudoEnd - pseudoStart);
        if (pseudo == "::before")
        {
            pseudoElement = CssPseudoElement::Before;
        }
        else if (pseudo == "::after")
        {
            pseudoElement = CssPseudoElement::After;
        }
        else
        {
            return {};
        }
        normalized.remove(pseudoStart, pseudo.size());
        if (normalized.contains("::"))
        {
            return {};
        }
        specificity += TAG_SPECIFICITY;
    }

    QList<QString> rawParts;
    QList<CssCombinator> combinators;
    QString current;
    CssCombinator combinator = CssCombinator::Descendant;
    bool inQuote = false;
    QChar quote;
    int bracketDepth = 0;

    const auto flushPart = [&] ()
    {
        if (current.trimmed().isEmpty())
        {
            return;
        }
        rawParts.emplaceBack(current.trimmed());
        combinators.emplaceBack(combinator);
        current.clear();
        combinator = CssCombinator::Descendant;
    };

    for (const QChar ch : normalized)
    {
        if (inQuote)
        {
            current += ch;
            if (ch == quote)
            {
                inQuote = false;
            }
        }
        else if (ch == '"' || ch == '\'')
        {
            inQuote = true;
            quote = ch;
            current += ch;
        }
        else if (ch == '[')
        {
            ++bracketDepth;
            current += ch;
        }
        else if (ch == ']')
        {
            --bracketDepth;
            current += ch;
        }
        else if (ch == '>' && bracketDepth == 0)
        {
            flushPart();
            combinator = CssCombinator::Child;
        }
        else if (ch == '+' && bracketDepth == 0)
        {
            flushPart();
            combinator = CssCombinator::AdjacentSibling;
        }
        else if (ch == '~' && bracketDepth == 0)
        {
            flushPart();
            combinator = CssCombinator::GeneralSibling;
        }
        else if (ch.isSpace() && bracketDepth == 0)
        {
            if (!current.trimmed().isEmpty())
            {
                flushPart();
            }
        }
        else
        {
            current += ch;
        }
    }

    flushPart();

    QList<CssSelectorPart> parts;
    for (qsizetype i = 0; i < rawParts.size(); ++i)
    {
        CssSelectorPart part;
        part.combinator = combinators[i];
        QString raw = rawParts[i];

        qsizetype attrIndex = raw.indexOf('[');
        while (attrIndex >= 0)
        {
            const qsizetype attrEnd = raw.indexOf(']', attrIndex);
            if (attrEnd < 0)
            {
                return {};
            }

            QString attr = raw.sliced(
                attrIndex + 1,
                attrEnd - attrIndex - 1
            ).trimmed();
            CssAttributeSelector selector;
            qsizetype opIndex = attr.indexOf("^=");
            if (opIndex >= 0)
            {
                selector.op = CssAttributeOperator::StartsWith;
            }
            else
            {
                opIndex = attr.indexOf('=');
                selector.op = opIndex < 0 ?
                    CssAttributeOperator::Exists :
                    CssAttributeOperator::Equals;
            }

            if (opIndex < 0)
            {
                selector.name = attr.trimmed();
            }
            else
            {
                selector.name = attr.first(opIndex).trimmed();
                const qsizetype valueOffset =
                    selector.op == CssAttributeOperator::StartsWith ?
                        PREFIX_ATTRIBUTE_OPERATOR_LENGTH :
                        EQUAL_ATTRIBUTE_OPERATOR_LENGTH;
                selector.value = attr.sliced(opIndex + valueOffset).trimmed();
                if (selector.value.size() >= 2 &&
                    ((selector.value.front() == '"' &&
                      selector.value.back() == '"') ||
                     (selector.value.front() == '\'' &&
                      selector.value.back() == '\'')))
                {
                    selector.value =
                        selector.value.sliced(1, selector.value.size() - 2);
                }
            }
            if (selector.name.isEmpty())
            {
                return {};
            }
            part.attributes.emplaceBack(std::move(selector));
            raw.remove(attrIndex, attrEnd - attrIndex + 1);
            attrIndex = raw.indexOf('[', attrIndex);
            specificity += CLASS_SPECIFICITY;
        }

        const auto parsePseudoClass =
        [&part, &specificity, nthChildPrefix]
        (const QString &rawPseudo, bool negated) -> bool
        {
            CssPseudoClassSelector selector;
            selector.negated = negated;
            if (rawPseudo == ":first-child")
            {
                selector.type = CssPseudoClass::FirstChild;
            }
            else if (rawPseudo == ":last-child")
            {
                selector.type = CssPseudoClass::LastChild;
            }
            else if (rawPseudo.startsWith(nthChildPrefix))
            {
                const qsizetype valueStart = nthChildPrefix.size();
                const qsizetype valueEnd =
                    rawPseudo.indexOf(')', valueStart);
                if (valueEnd != rawPseudo.size() - 1)
                {
                    return false;
                }

                const QString value = rawPseudo.sliced(
                    valueStart,
                    valueEnd - valueStart
                ).trimmed();
                if (value.isEmpty())
                {
                    return false;
                }
                for (const QChar ch : value)
                {
                    if (!ch.isDigit())
                    {
                        return false;
                    }
                }

                bool ok = false;
                const int childIndex = value.toInt(&ok);
                if (!ok || childIndex <= 0)
                {
                    return false;
                }
                selector.type = CssPseudoClass::NthChild;
                selector.childIndex = childIndex;
            }
            else
            {
                return false;
            }

            part.pseudoClasses.emplaceBack(std::move(selector));
            specificity += CLASS_SPECIFICITY;
            return true;
        };
        const auto pseudoClassTokenEnd =
        [&nthChildPrefix] (const QString &text, qsizetype start) -> qsizetype
        {
            if (text.sliced(start).startsWith(nthChildPrefix))
            {
                const qsizetype valueStart =
                    start + nthChildPrefix.size();
                const qsizetype valueEnd = text.indexOf(')', valueStart);
                return valueEnd < 0 ? -1 : valueEnd + 1;
            }

            qsizetype end = start + 1;
            while (end < text.size() &&
                   (text[end].isLetter() || text[end] == '-'))
            {
                ++end;
            }
            return end == start + 1 ? -1 : end;
        };

        qsizetype notIndex = raw.indexOf(notPseudoPrefix);
        while (notIndex >= 0)
        {
            const qsizetype pseudoStart =
                notIndex + notPseudoPrefix.size();
            const qsizetype pseudoEnd =
                pseudoClassTokenEnd(raw, pseudoStart);
            if (pseudoEnd < 0 ||
                pseudoEnd >= raw.size() ||
                raw[pseudoEnd] != ')')
            {
                return {};
            }

            const QString rawPseudo =
                raw.sliced(pseudoStart, pseudoEnd - pseudoStart);
            if (!parsePseudoClass(rawPseudo.trimmed(), true))
            {
                return {};
            }
            raw.remove(notIndex, pseudoEnd - notIndex + 1);
            notIndex = raw.indexOf(notPseudoPrefix);
        }

        qsizetype pseudoIndex = raw.indexOf(':');
        while (pseudoIndex >= 0)
        {
            const qsizetype pseudoEnd =
                pseudoClassTokenEnd(raw, pseudoIndex);
            if (pseudoEnd < 0)
            {
                return {};
            }
            const qsizetype length = pseudoEnd - pseudoIndex;
            const QString rawPseudo = raw.sliced(pseudoIndex, length);
            if (!parsePseudoClass(rawPseudo.trimmed(), false))
            {
                return {};
            }
            raw.remove(pseudoIndex, length);
            pseudoIndex = raw.indexOf(':');
        }

        qsizetype classIndex = raw.indexOf('.');
        while (classIndex >= 0)
        {
            qsizetype end = classIndex + 1;
            while (end < raw.size() &&
                   (raw[end].isLetterOrNumber() || raw[end] == '-' ||
                    raw[end] == '_'))
            {
                ++end;
            }
            if (end == classIndex + 1)
            {
                return {};
            }
            part.classNames.insert(
                raw.sliced(classIndex + 1, end - classIndex - 1)
            );
            raw.remove(classIndex, end - classIndex);
            specificity += CLASS_SPECIFICITY;
            classIndex = raw.indexOf('.', classIndex);
        }
        if (raw.contains(':'))
        {
            return {};
        }

        raw = raw.trimmed();
        if (!raw.isEmpty() && raw != "*")
        {
            part.tag = raw.toLower();
            specificity += TAG_SPECIFICITY;
        }
        parts.emplaceBack(std::move(part));
    }

    return parts;
}

QList<DictionaryStyles::CssDeclaration> DictionaryStyles::parseCssDeclarations(
    const QString &body)
{
    QList<CssDeclaration> declarations;
    QString property;
    QString value;
    bool inProperty = true;
    bool inQuote = false;
    QChar quote;
    int parenDepth = 0;

    const auto flush = [&] ()
    {
        const QString name = property.trimmed().toLower();
        QString cssValue = value.trimmed();
        if (name.isEmpty() || cssValue.isEmpty())
        {
            property.clear();
            value.clear();
            inProperty = true;
            return;
        }

        cssValue.replace('"', '\'');
        if (cssValue.size() >= 2 &&
            ((cssValue.front() == '\'' && cssValue.back() == '\'') ||
             (cssValue.front() == '"' && cssValue.back() == '"')))
        {
            cssValue = cssValue.sliced(1, cssValue.size() - 2);
        }

        declarations.emplaceBack(CssDeclaration{name, cssValue});
        property.clear();
        value.clear();
        inProperty = true;
    };

    for (const QChar ch : body)
    {
        if (inQuote)
        {
            (inProperty ? property : value) += ch;
            if (ch == quote)
            {
                inQuote = false;
            }
        }
        else if (ch == '"' || ch == '\'')
        {
            inQuote = true;
            quote = ch;
            (inProperty ? property : value) += ch;
        }
        else if (ch == '(')
        {
            ++parenDepth;
            (inProperty ? property : value) += ch;
        }
        else if (ch == ')')
        {
            --parenDepth;
            (inProperty ? property : value) += ch;
        }
        else if (ch == ':' && inProperty)
        {
            inProperty = false;
        }
        else if (ch == ';' && parenDepth == 0)
        {
            flush();
        }
        else
        {
            (inProperty ? property : value) += ch;
        }
    }
    flush();

    return declarations;
}
