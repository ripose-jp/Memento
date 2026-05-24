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
        const QHash<QString, QString> declarations =
            parseCssDeclarations(declarationsText);
        if (declarations.isEmpty())
        {
            return;
        }

        for (const QString &selector : selectors)
        {
            bool before = false;
            int specificity = 0;
            QList<CssSelectorPart> parts = parseCssSelector(
                selector,
                before,
                specificity
            );
            if (parts.isEmpty())
            {
                continue;
            }

            rules.emplaceBack(CssRule{
                parts,
                declarations,
                before,
                specificity,
                order++
            });
        }
    };

    for (qsizetype i = 0; i < source.size(); ++i)
    {
        if (source[i] == '@')
        {
            const qsizetype brace = source.indexOf('{', i);
            if (brace < 0)
            {
                break;
            }
            const qsizetype close = findMatchingBrace(source, brace);
            i = close < 0 ? source.size() : close;
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

    return ParsedStylesheet{rules};
}

QList<DictionaryStyles::CssSelectorPart> DictionaryStyles::parseCssSelector(
    const QString &selector,
    bool &before,
    int &specificity)
{
    before = false;
    specificity = 0;

    QString normalized = selector.trimmed();
    if (normalized.contains("::before"))
    {
        before = true;
        normalized.replace("::before", "");
    }
    if (normalized.contains(':'))
    {
        return {};
    }

    QList<QString> rawParts;
    QList<bool> directParents;
    QString current;
    bool directParent = false;
    bool inQuote = false;
    QChar quote;
    int bracketDepth = 0;

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
            if (!current.trimmed().isEmpty())
            {
                rawParts.emplaceBack(current.trimmed());
                directParents.emplaceBack(directParent);
                current.clear();
            }
            directParent = true;
        }
        else if (ch.isSpace() && bracketDepth == 0)
        {
            if (!current.trimmed().isEmpty())
            {
                rawParts.emplaceBack(current.trimmed());
                directParents.emplaceBack(directParent);
                current.clear();
                directParent = false;
            }
        }
        else
        {
            current += ch;
        }
    }

    if (!current.trimmed().isEmpty())
    {
        rawParts.emplaceBack(current.trimmed());
        directParents.emplaceBack(directParent);
    }

    QList<CssSelectorPart> parts;
    for (qsizetype i = 0; i < rawParts.size(); ++i)
    {
        CssSelectorPart part;
        part.directParent = directParents[i];
        QString raw = rawParts[i];

        const qsizetype classIndex = raw.indexOf('.');
        if (classIndex >= 0)
        {
            qsizetype end = classIndex + 1;
            while (end < raw.size() &&
                   (raw[end].isLetterOrNumber() || raw[end] == '-' ||
                    raw[end] == '_'))
            {
                ++end;
            }
            part.className = raw.sliced(classIndex + 1, end - classIndex - 1);
            raw.remove(classIndex, end - classIndex);
            specificity += 10;
        }

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
            const qsizetype equal = attr.indexOf('=');
            if (equal < 0)
            {
                return {};
            }

            QString name = attr.first(equal).trimmed();
            QString value = attr.sliced(equal + 1).trimmed();
            if (value.size() >= 2 &&
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\'')))
            {
                value = value.sliced(1, value.size() - 2);
            }
            part.attributes[name] = value;
            raw.remove(attrIndex, attrEnd - attrIndex + 1);
            attrIndex = raw.indexOf('[', attrIndex);
            specificity += 10;
        }

        raw = raw.trimmed();
        if (!raw.isEmpty() && raw != "*")
        {
            part.tag = raw.toLower();
            ++specificity;
        }
        parts.emplaceBack(std::move(part));
    }

    return parts;
}

QHash<QString, QString> DictionaryStyles::parseCssDeclarations(
    const QString &body)
{
    QHash<QString, QString> declarations;
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

        declarations[name] = cssValue;
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
