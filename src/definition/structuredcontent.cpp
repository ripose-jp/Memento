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

#include "definition/structuredcontent.h"

#include <algorithm>
#include <memory>
#include <optional>

#include <QColor>
#include <QGuiApplication>
#include <QJsonObject>
#include <QRegularExpression>
#include <QScreen>
#include <QSet>
#include <QStringList>
#include <QVariantList>

#include "dict/data/dictionaryinfo.h"
#include "dict/data/dictionarystylesheet.h"
#include "util/utils.h"

namespace
{
static constexpr int MAX_IMAGE_WIDTH{350};

/**
 * @brief Stores supported presentation properties for one node.
 */
struct ResolvedStyle
{
    /* The inherited foreground color, if one was specified. */
    QString color;

    /* The non-inherited background color, if one was specified. */
    QString backgroundColor;

    /* The non-inherited border color, if one was specified. */
    QString borderColor;

    /* The inherited font scale relative to the glossary base font. */
    double fontScale{1.0};

    /* The non-inherited top margin in pixels. */
    double marginTop{0.0};

    /* The non-inherited left margin in pixels. */
    double marginLeft{0.0};

    /* The non-inherited right margin in pixels. */
    double marginRight{0.0};

    /* The non-inherited bottom margin in pixels. */
    double marginBottom{0.0};

    /* The non-inherited top padding in pixels. */
    double paddingTop{0.0};

    /* The non-inherited left padding in pixels. */
    double paddingLeft{0.0};

    /* The non-inherited right padding in pixels. */
    double paddingRight{0.0};

    /* The non-inherited bottom padding in pixels. */
    double paddingBottom{0.0};

    /* The non-inherited border width in pixels. */
    double borderWidth{0.0};

    /* The non-inherited border radius in pixels. */
    double borderRadius{0.0};

    /* true if the node should shrink-wrap its content when possible. */
    bool fitContent{false};

    /* true if only the left edge of the border should be visible. */
    bool leftBorderOnly{false};

    /* true if text should use the semantic example-keyword color. */
    bool exampleKeyword{false};

    /* The inherited list marker string. */
    QString listMarker;

    /* The inherited horizontal text alignment value. */
    QString textAlign;

    /* The non-inherited vertical alignment value. */
    QString verticalAlign;

    /* The inherited word-break value. */
    QString wordBreak;

    /* true if the current font weight is bold. */
    bool bold{false};

    /* true if the current font style is italic. */
    bool italic{false};

    /* true if text should be underlined. */
    bool underline{false};

    /* true if text should be overlined. */
    bool overline{false};

    /* true if text should be struck out. */
    bool strikeout{false};
};

/**
 * @brief Stores data needed while recursively building one document.
 */
class DocumentBuilder
{
public:
    /**
     * @brief Construct a document builder.
     *
     * @param dictionaryInfo The dictionary metadata.
     * @param glossaryStyle The requested glossary display style.
     * @param font The base glossary font.
     */
    DocumentBuilder(
        const DictionaryInfo *dictionaryInfo,
        Setting::GlossaryStyle glossaryStyle,
        const QFont &font);

    /**
     * @brief Build a complete glossary document.
     *
     * @param content The glossary content to parse.
     * @return The native document description.
     */
    [[nodiscard]]
    QVariantMap build(const QJsonArray &content);

private:
    /**
     * @brief Build one structured-content value.
     *
     * @param value The JSON value to parse.
     * @param parentStyle The inherited style.
     * @param siblingIndex The zero-based position among siblings.
     * @return The native node description.
     */
    [[nodiscard]]
    QVariantMap buildValue(
        const QJsonValue &value,
        const ResolvedStyle &parentStyle,
        int siblingIndex);

    /**
     * @brief Build one structured-content object.
     *
     * @param object The JSON object to parse.
     * @param parentStyle The inherited style.
     * @param siblingIndex The zero-based position among siblings.
     * @return The native node description.
     */
    [[nodiscard]]
    QVariantMap buildObject(
        const QJsonObject &object,
        const ResolvedStyle &parentStyle,
        int siblingIndex);

    /**
     * @brief Build one plain text node.
     *
     * @param text The text to store.
     * @param parentStyle The inherited style.
     * @return The native text node description.
     */
    [[nodiscard]]
    QVariantMap buildText(
        const QString &text,
        const ResolvedStyle &parentStyle);

    /**
     * @brief Build one list node for plain-text bullet glossaries.
     *
     * @param content The glossary content to parse.
     * @param parentStyle The inherited style.
     * @return The native unordered-list node description.
     */
    [[nodiscard]]
    QVariantMap buildPlainTextList(
        const QJsonArray &content,
        const ResolvedStyle &parentStyle);

    /**
     * @brief Build one legacy top-level image glossary object.
     *
     * @param object The image object.
     * @param parentStyle The inherited style.
     * @return The native image node description.
     */
    [[nodiscard]]
    QVariantMap buildLegacyImage(
        const QJsonObject &object,
        const ResolvedStyle &parentStyle);

    /**
     * @brief Build one table node with flattened cell metadata.
     *
     * @param object The table object.
     * @param style The resolved table style.
     * @return The native table node description.
     */
    [[nodiscard]]
    QVariantMap buildTable(
        const QJsonObject &object,
        const ResolvedStyle &style);

    /**
     * @brief Build all child values for one node.
     *
     * @param value The JSON content value.
     * @param parentStyle The inherited style.
     * @return The native child-node list.
     */
    [[nodiscard]]
    QVariantList buildChildren(
        const QJsonValue &value,
        const ResolvedStyle &parentStyle);

    /**
     * @brief Build a native node shell shared by every node kind.
     *
     * @param tag The source structured-content tag.
     * @param kind The native render kind.
     * @param style The resolved node style.
     * @return A partially populated native node.
     */
    [[nodiscard]]
    QVariantMap makeNode(
        const QString &tag,
        const QString &kind,
        const ResolvedStyle &style);

    /**
     * @brief Group consecutive inline children into flow fragments.
     *
     * @param children The child nodes to normalize.
     * @param style The parent style for generated fragment nodes.
     * @return The child list with inline runs grouped together.
     */
    [[nodiscard]]
    QVariantList groupInlineRuns(
        const QVariantList &children,
        const ResolvedStyle &style);

    /**
     * @brief Resolve stylesheet and inline styles for one object.
     *
     * @param object The source structured-content object.
     * @param parentStyle The inherited style.
     * @return The resolved node style.
     */
    [[nodiscard]]
    ResolvedStyle resolveStyle(
        const QJsonObject &object,
        const ResolvedStyle &parentStyle) const;

    /**
     * @brief Determine if a native render kind participates in inline flow.
     *
     * @param kind The native render kind to inspect.
     * @return true if the kind is inline, false otherwise.
     */
    [[nodiscard]]
    static bool isInlineKind(const QString &kind);

    /**
     * @brief Determine if a node list can be rendered inline.
     *
     * @param children The child-node list to inspect.
     * @return true if every child is inline, false otherwise.
     */
    [[nodiscard]]
    static bool isInlineOnly(const QVariantList &children);

    /**
     * @brief Determine if a node list contains styled-text-safe content.
     *
     * @param nodes The child-node list to inspect.
     * @return true if the nodes can be flattened into styled text.
     */
    [[nodiscard]]
    static bool canUseStyledText(const QVariantList &nodes);

    /**
     * @brief Escape plain text for Qt's limited styled-text subset.
     *
     * @param text The plain text to escape.
     * @return The escaped styled-text fragment.
     */
    [[nodiscard]]
    static QString escapeStyledText(const QString &text);

    /**
     * @brief Build one styled-text fragment from an inline node.
     *
     * @param node The inline node to serialize.
     * @return The styled-text fragment.
     */
    [[nodiscard]]
    static QString buildStyledText(const QVariantMap &node);

    /**
     * @brief Build styled text from inline children.
     *
     * @param nodes The inline child nodes to serialize.
     * @return The styled text for the inline run.
     */
    [[nodiscard]]
    static QString buildStyledText(const QVariantList &nodes);

    /**
     * @brief Convert one structured tag into a native render kind.
     *
     * @param tag The structured-content tag.
     * @return The native render kind.
     */
    [[nodiscard]]
    static QString kindForTag(const QString &tag);

    /**
     * @brief Get if an array contains a structured-content object.
     *
     * @param content The glossary content to inspect.
     * @return true if structured content exists, false otherwise.
     */
    [[nodiscard]]
    static bool containsStructuredContent(const QJsonArray &content);

    /**
     * @brief Convert the dictionary name into its resource URL prefix.
     *
     * @param name The dictionary name.
     * @return The resource URL prefix for dictionary files.
     */
    [[nodiscard]]
    static QString makeBasePath(const QString &name);

    /**
     * @brief Convert a style object into a QML-friendly map.
     *
     * @param style The resolved style to expose.
     * @return The style as a QVariantMap.
     */
    [[nodiscard]]
    static QVariantMap toVariantMap(const ResolvedStyle &style);

    /* The dictionary resource URL prefix. */
    QString m_basePath;

    /* The requested glossary style. */
    Setting::GlossaryStyle m_glossaryStyle;

    /* The immutable parsed stylesheet shared with dictionary metadata. */
    std::shared_ptr<const DictionaryStylesheet> m_stylesheet;

    /* The base font size in pixels for CSS length resolution. */
    double m_baseFontPixels{14.0};

    /* The next stable node ID assigned during document generation. */
    int m_nextNodeId{0};
};

/**
 * @brief Convert a point size into logical pixels.
 *
 * @param pointSize The point size to convert.
 * @return The equivalent logical pixel size.
 */
[[nodiscard]]
static double pointSizeToPixelSize(double pointSize)
{
    static constexpr double POINTS_IN_INCH{72.0};
    return pointSize / POINTS_IN_INCH *
        QGuiApplication::primaryScreen()->logicalDotsPerInch();
}

/**
 * @brief Test if one selector matches a structured-content object.
 *
 * @param selector The selector to test.
 * @param tag The source structured-content tag.
 * @param dataContent The data-sc-content value.
 * @param hasDataContent true if data-sc-content exists.
 * @param dataClass The data-sc-class value.
 * @param hasDataClass true if data-sc-class exists.
 * @param hasTitle true if the node has a title attribute.
 * @return true if the selector matches, false otherwise.
 */
[[nodiscard]]
static bool selectorMatches(
    const DictionaryCssSelector &selector,
    const QString &tag,
    const QString &dataContent,
    bool hasDataContent,
    const QString &dataClass,
    bool hasDataClass,
    bool hasTitle)
{
    if (!selector.tag.isEmpty() && selector.tag != tag)
    {
        return false;
    }
    if (selector.attributeName.isEmpty())
    {
        return true;
    }
    if (selector.attributeName == QStringLiteral("title"))
    {
        return hasTitle;
    }
    if (selector.attributeName == QStringLiteral("data-sc-content"))
    {
        if (!hasDataContent)
        {
            return false;
        }
        return selector.attributeExists ||
            dataContent == selector.attributeValue;
    }
    if (selector.attributeName == QStringLiteral("data-sc-class"))
    {
        if (!hasDataClass)
        {
            return false;
        }
        return selector.attributeExists ||
            dataClass == selector.attributeValue;
    }
    return false;
}

/**
 * @brief Parse a CSS color value that Qt Quick can render directly.
 *
 * @param value The CSS value to inspect.
 * @return The color string, or an empty string if unsupported.
 */
[[nodiscard]]
static QString parseSimpleColor(const QString &value)
{
    if (value.contains('('))
    {
        return {};
    }
    const QColor color{value};
    return color.isValid() ? value : QString{};
}

/**
 * @brief Parse a supported CSS font-size value.
 *
 * @param value The CSS value to parse.
 * @param currentScale The current inherited font scale.
 * @param basePixels The root font size in pixels.
 * @return The resolved root-relative font scale.
 */
[[nodiscard]]
static double parseFontScale(
    QString value,
    double currentScale,
    double basePixels)
{
    value = value.trimmed().toLower();
    bool ok{false};
    if (value.endsWith('%'))
    {
        const double number{value.chopped(1).toDouble(&ok)};
        return ok ? currentScale * number / 100.0 : currentScale;
    }
    if (value.endsWith(QStringLiteral("em")))
    {
        const double number{value.chopped(2).toDouble(&ok)};
        return ok ? currentScale * number : currentScale;
    }
    if (value.endsWith(QStringLiteral("rem")))
    {
        const double number{value.chopped(3).toDouble(&ok)};
        return ok ? number : currentScale;
    }
    if (value.endsWith(QStringLiteral("px")))
    {
        const double number{value.chopped(2).toDouble(&ok)};
        return ok && basePixels > 0.0 ? number / basePixels : currentScale;
    }
    return currentScale;
}

/**
 * @brief Parse one supported CSS length.
 *
 * @param value The CSS value to parse.
 * @param fontScale The current font scale.
 * @param basePixels The root font size in pixels.
 * @return The resolved pixel value, or std::nullopt if unsupported.
 */
[[nodiscard]]
static std::optional<double> parseLength(
    QString value,
    double fontScale,
    double basePixels)
{
    value = value.trimmed().toLower();
    if (value.contains('(') || value.isEmpty())
    {
        return std::nullopt;
    }

    bool ok{false};
    if (value.endsWith(QStringLiteral("rem")))
    {
        const double number{value.chopped(3).toDouble(&ok)};
        return ok ? std::optional<double>{number * basePixels} :
            std::nullopt;
    }
    if (value.endsWith(QStringLiteral("em")))
    {
        const double number{value.chopped(2).toDouble(&ok)};
        return ok ? std::optional<double>{
            number * basePixels * fontScale} : std::nullopt;
    }
    if (value.endsWith(QStringLiteral("px")))
    {
        const double number{value.chopped(2).toDouble(&ok)};
        return ok ? std::optional<double>{number} : std::nullopt;
    }
    const double number{value.toDouble(&ok)};
    return ok ? std::optional<double>{number} : std::nullopt;
}

/**
 * @brief Apply a one-to-four-value CSS box shorthand.
 *
 * @param value The shorthand value.
 * @param fontScale The current font scale.
 * @param basePixels The root font size in pixels.
 * @param top The output top value.
 * @param right The output right value.
 * @param bottom The output bottom value.
 * @param left The output left value.
 */
static void applyBoxShorthand(
    const QString &value,
    double fontScale,
    double basePixels,
    double &top,
    double &right,
    double &bottom,
    double &left)
{
    static const QRegularExpression WHITESPACE{
        QStringLiteral(R"(\s+)")};
    const QStringList values{
        value.split(WHITESPACE, Qt::SkipEmptyParts)};
    if (values.isEmpty() || values.size() > 4)
    {
        return;
    }

    QList<double> parsed;
    for (const QString &part : values)
    {
        const std::optional<double> length{
            parseLength(part, fontScale, basePixels)};
        if (!length.has_value())
        {
            return;
        }
        parsed.append(*length);
    }

    switch (parsed.size())
    {
    case 1:
        top = right = bottom = left = parsed[0];
        break;
    case 2:
        top = bottom = parsed[0];
        right = left = parsed[1];
        break;
    case 3:
        top = parsed[0];
        right = left = parsed[1];
        bottom = parsed[2];
        break;
    case 4:
        top = parsed[0];
        right = parsed[1];
        bottom = parsed[2];
        left = parsed[3];
        break;
    default:
        break;
    }
}

/**
 * @brief Apply one supported declaration to a resolved style.
 *
 * @param property The CSS property name.
 * @param value The CSS property value.
 * @param basePixels The root font size in pixels.
 * @param style The style to mutate.
 */
static void applyDeclaration(
    const QString &property,
    const QString &value,
    double basePixels,
    ResolvedStyle &style)
{
    if (property == QStringLiteral("color"))
    {
        const QString color{parseSimpleColor(value)};
        if (!color.isEmpty())
        {
            style.color = color;
        }
    }
    else if (property == QStringLiteral("background") ||
             property == QStringLiteral("background-color"))
    {
        const QString color{parseSimpleColor(value)};
        if (!color.isEmpty())
        {
            style.backgroundColor = color;
        }
    }
    else if (property == QStringLiteral("font-size"))
    {
        style.fontScale = parseFontScale(
            value, style.fontScale, basePixels);
    }
    else if (property == QStringLiteral("font-style"))
    {
        style.italic = value == QStringLiteral("italic");
    }
    else if (property == QStringLiteral("font-weight"))
    {
        style.bold = value == QStringLiteral("bold");
    }
    else if (property == QStringLiteral("text-align"))
    {
        style.textAlign = value;
    }
    else if (property == QStringLiteral("vertical-align"))
    {
        style.verticalAlign = value;
    }
    else if (property == QStringLiteral("word-break"))
    {
        style.wordBreak = value;
    }
    else if (property == QStringLiteral("list-style-type"))
    {
        style.listMarker = value;
    }
    else if (property == QStringLiteral("border-color"))
    {
        const QString color{parseSimpleColor(value)};
        if (!color.isEmpty())
        {
            style.borderColor = color;
        }
    }
    else if (property == QStringLiteral("border-width"))
    {
        const std::optional<double> length{
            parseLength(value, style.fontScale, basePixels)};
        if (length.has_value())
        {
            style.borderWidth = *length;
        }
    }
    else if (property == QStringLiteral("border-radius"))
    {
        const std::optional<double> length{
            parseLength(value, style.fontScale, basePixels)};
        if (length.has_value())
        {
            style.borderRadius = *length;
        }
    }
    else if (property == QStringLiteral("border-style"))
    {
        static const QRegularExpression WHITESPACE{
            QStringLiteral(R"(\s+)")};
        const QStringList values{
            value.split(WHITESPACE, Qt::SkipEmptyParts)};
        style.leftBorderOnly =
            values == QStringList{
                QStringLiteral("none"),
                QStringLiteral("none"),
                QStringLiteral("none"),
                QStringLiteral("solid")};
    }
    else if (property == QStringLiteral("width"))
    {
        style.fitContent = value == QStringLiteral("fit-content");
    }
    else if (property == QStringLiteral("margin"))
    {
        applyBoxShorthand(
            value, style.fontScale, basePixels,
            style.marginTop, style.marginRight,
            style.marginBottom, style.marginLeft);
    }
    else if (property == QStringLiteral("padding"))
    {
        applyBoxShorthand(
            value, style.fontScale, basePixels,
            style.paddingTop, style.paddingRight,
            style.paddingBottom, style.paddingLeft);
    }
    else if (property == QStringLiteral("text-decoration"))
    {
        style.underline = value.contains(QStringLiteral("underline"));
        style.overline = value.contains(QStringLiteral("overline"));
        style.strikeout = value.contains(QStringLiteral("line-through"));
    }

    double *lengthTarget{nullptr};
    if (property == QStringLiteral("margin-top"))
    {
        lengthTarget = &style.marginTop;
    }
    else if (property == QStringLiteral("margin-left"))
    {
        lengthTarget = &style.marginLeft;
    }
    else if (property == QStringLiteral("margin-right"))
    {
        lengthTarget = &style.marginRight;
    }
    else if (property == QStringLiteral("margin-bottom"))
    {
        lengthTarget = &style.marginBottom;
    }
    else if (property == QStringLiteral("padding-top"))
    {
        lengthTarget = &style.paddingTop;
    }
    else if (property == QStringLiteral("padding-left"))
    {
        lengthTarget = &style.paddingLeft;
    }
    else if (property == QStringLiteral("padding-right"))
    {
        lengthTarget = &style.paddingRight;
    }
    else if (property == QStringLiteral("padding-bottom"))
    {
        lengthTarget = &style.paddingBottom;
    }
    if (lengthTarget)
    {
        const std::optional<double> length{
            parseLength(value, style.fontScale, basePixels)};
        if (length.has_value())
        {
            *lengthTarget = *length;
        }
    }
}

/**
 * @brief Apply supported inline structured-content styles.
 *
 * @param object The inline style object.
 * @param basePixels The root font size in pixels.
 * @param style The style to mutate.
 */
static void applyInlineStyle(
    const QJsonObject &object,
    double basePixels,
    ResolvedStyle &style)
{
    static const QRegularExpression CAMEL_CASE{
        QStringLiteral("([a-z])([A-Z])")};
    for (auto it{object.begin()}; it != object.end(); ++it)
    {
        QString property{it.key()};
        property.replace(
            CAMEL_CASE,
            QStringLiteral("\\1-\\2"));
        property = property.toLower();

        if (it.value().isArray() &&
            property == QStringLiteral("text-decoration-line"))
        {
            QStringList values;
            for (const QJsonValue &value : it.value().toArray())
            {
                values.append(value.toString());
            }
            applyDeclaration(
                QStringLiteral("text-decoration"),
                values.join(' '),
                basePixels,
                style);
        }
        else if (it.value().isString())
        {
            applyDeclaration(
                property,
                it.value().toString(),
                basePixels,
                style);
        }
        else if (it.value().isDouble())
        {
            applyDeclaration(
                property,
                QString::number(it.value().toDouble()),
                basePixels,
                style);
        }
    }
}

/**
 * @brief Make a child style by inheriting only inheritable properties.
 *
 * @param parent The parent style.
 * @return A new child style initialized from inheritable properties.
 */
[[nodiscard]]
static ResolvedStyle inheritStyle(const ResolvedStyle &parent)
{
    ResolvedStyle child;
    child.color = parent.color;
    child.fontScale = parent.fontScale;
    child.listMarker = parent.listMarker;
    child.textAlign = parent.textAlign;
    child.wordBreak = parent.wordBreak;
    child.bold = parent.bold;
    child.italic = parent.italic;
    child.underline = parent.underline;
    child.overline = parent.overline;
    child.strikeout = parent.strikeout;
    child.exampleKeyword = parent.exampleKeyword;
    return child;
}

/**
 * @brief Make a subtle transparent fill from a border color.
 *
 * @param colorValue The source CSS color.
 * @return A translucent ARGB color string, or an empty string if invalid.
 */
[[nodiscard]]
static QString translucentBackground(const QString &colorValue)
{
    QColor color{colorValue};
    if (!color.isValid())
    {
        return {};
    }
    color.setAlphaF(0.08);
    return color.name(QColor::HexArgb);
}

/**
 * @brief Fill gaps left by browser-only Jitendex CSS features.
 *
 * Some important Jitendex boxes depend on CSS features that do not map cleanly
 * to Qt Quick, such as color-mix(), calc(), and fit-content. Preserve their
 * intent with a small semantic adapter instead of attempting a full CSS engine.
 *
 * @param object The source structured-content object.
 * @param style The style to update.
 */
static void applySemanticFallbacks(
    const QJsonObject &object,
    ResolvedStyle &style)
{
    const QJsonObject data =
        object[QStringLiteral("data")].toObject();
    const QString dataClass{
        data[QStringLiteral("class")].toString()};
    const QString dataContent{
        data[QStringLiteral("content")].toString()};

    if (dataClass == QStringLiteral("extra-box"))
    {
        style.fitContent = true;
        style.leftBorderOnly = true;
        style.borderWidth = std::max(style.borderWidth, 3.0);
    }

    if (dataContent == QStringLiteral("sense-note") &&
        style.borderColor.isEmpty())
    {
        style.borderColor = QStringLiteral("goldenrod");
    }
    else if (dataContent == QStringLiteral("example-sentence") &&
             style.borderColor.isEmpty())
    {
        style.borderColor = QStringLiteral("#d8d8d8");
    }
    else if (dataContent == QStringLiteral("example-keyword") &&
             style.color.isEmpty())
    {
        style.exampleKeyword = true;
    }
    if (style.backgroundColor.isEmpty() &&
        !style.borderColor.isEmpty() &&
        dataClass == QStringLiteral("extra-box") &&
        dataContent != QStringLiteral("example-sentence"))
    {
        style.backgroundColor = translucentBackground(style.borderColor);
    }
}

/**
 * @brief Parse a CSS list marker into a display string.
 *
 * @param marker The CSS marker value.
 * @param ordered true for ordered lists, false for unordered lists.
 * @param index The zero-based item index.
 * @return The marker string to display.
 */
[[nodiscard]]
static QString markerText(const QString &marker, bool ordered, int index)
{
    if (ordered)
    {
        return QString::number(index + 1) + '.';
    }
    if (marker == QStringLiteral("none"))
    {
        return {};
    }
    if (marker == QStringLiteral("circle"))
    {
        return QStringLiteral("◦");
    }
    if (marker == QStringLiteral("disc") || marker.isEmpty())
    {
        return QStringLiteral("•");
    }
    if ((marker.startsWith('"') && marker.endsWith('"')) ||
        (marker.startsWith('\'') && marker.endsWith('\'')))
    {
        return marker.mid(1, marker.size() - 2);
    }
    return QStringLiteral("•");
}

/**
 * @brief Get the generated marker text for a Jitendex forms cell.
 *
 * Jitendex uses CSS ::before pseudo-elements for these markers. The native
 * renderer does not implement pseudo-elements, so the small known vocabulary
 * gets translated into real text while unsupported classes still fall back.
 *
 * @param dataClass The structured-content data class.
 * @return The generated text, or an empty string if the class is unknown.
 */
[[nodiscard]]
static QString generatedFormsText(const QString &dataClass)
{
    static const QHash<QString, QString> MARKERS{
        {QStringLiteral("form-pri"), QStringLiteral("△")},
        {QStringLiteral("form-irr"), QStringLiteral("✕")},
        {QStringLiteral("form-out"), QStringLiteral("古")},
        {QStringLiteral("form-old"), QStringLiteral("旧")},
        {QStringLiteral("form-rare"), QStringLiteral("▽")},
        {QStringLiteral("form-valid"), QStringLiteral("◇")}
    };
    return MARKERS.value(dataClass);
}

/**
 * @brief Get if a native node list contains visible text.
 *
 * @param nodes The native child nodes to inspect.
 * @return true if any descendant contains non-empty text, false otherwise.
 */
[[nodiscard]]
static bool containsVisibleText(const QVariantList &nodes)
{
    return std::any_of(
        std::cbegin(nodes),
        std::cend(nodes),
        [] (const QVariant &value)
        {
            const QVariantMap node{value.toMap()};
            if (!node[QStringLiteral("text")].toString().isEmpty())
            {
                return true;
            }
            return containsVisibleText(
                node[QStringLiteral("children")].toList());
        });
}
} // namespace

DocumentBuilder::DocumentBuilder(
    const DictionaryInfo *dictionaryInfo,
    Setting::GlossaryStyle glossaryStyle,
    const QFont &font) :
    m_basePath{makeBasePath(
        dictionaryInfo ? dictionaryInfo->name() : QString{})},
    m_glossaryStyle{glossaryStyle},
    m_stylesheet{
        dictionaryInfo ?
            dictionaryInfo->stylesheet() :
            std::make_shared<const DictionaryStylesheet>()}
{
    if (font.pixelSize() > 0)
    {
        m_baseFontPixels = font.pixelSize();
    }
    else if (font.pointSizeF() > 0.0)
    {
        m_baseFontPixels = pointSizeToPixelSize(font.pointSizeF());
    }
}

QVariantMap DocumentBuilder::build(const QJsonArray &content)
{
    ResolvedStyle rootStyle;
    QVariantMap document{makeNode(
        QStringLiteral("document"),
        QStringLiteral("document"),
        rootStyle)};

    QVariantList children;
    const bool useBullets{
        m_glossaryStyle == Setting::GlossaryStyleBullet &&
        !containsStructuredContent(content)};
    if (useBullets)
    {
        children.append(buildPlainTextList(content, rootStyle));
    }
    else
    {
        for (qsizetype i{0}; i < content.size(); ++i)
        {
            const QJsonValue value{content[i]};
            if (value.isString())
            {
                children.append(buildText(value.toString(), rootStyle));
            }
            else if (value.isObject())
            {
                const QJsonObject object = value.toObject();
                const QString type{object[QStringLiteral("type")].toString()};
                if (type == QStringLiteral("structured-content"))
                {
                    children.append(buildValue(
                        object[QStringLiteral("content")], rootStyle, i));
                }
                else if (type == QStringLiteral("image"))
                {
                    children.append(buildLegacyImage(object, rootStyle));
                }
                else if (type == QStringLiteral("text"))
                {
                    children.append(buildText(
                        object[QStringLiteral("text")].toString(),
                        rootStyle));
                }
            }

            if (i + 1 >= content.size())
            {
                continue;
            }
            if (m_glossaryStyle == Setting::GlossaryStyleLineBreak)
            {
                children.append(buildText(QStringLiteral("\n"), rootStyle));
            }
            else if (m_glossaryStyle == Setting::GlossaryStylePipe)
            {
                children.append(buildText(QStringLiteral(" | "), rootStyle));
            }
        }
    }

    document[QStringLiteral("children")] = children;
    document[QStringLiteral("layout")] = QStringLiteral("column");
    return document;
}

QVariantMap DocumentBuilder::buildValue(
    const QJsonValue &value,
    const ResolvedStyle &parentStyle,
    int siblingIndex)
{
    if (value.isString())
    {
        return buildText(value.toString(), parentStyle);
    }
    if (value.isArray())
    {
        QVariantMap fragment{makeNode(
            QStringLiteral("fragment"),
            QStringLiteral("fragment"),
            parentStyle)};
        const QVariantList children = buildChildren(value, parentStyle);
        const bool inlineOnly{isInlineOnly(children)};
        fragment[QStringLiteral("children")] = inlineOnly ? children :
            groupInlineRuns(children, parentStyle);
        fragment[QStringLiteral("layout")] = inlineOnly ?
            QStringLiteral("flow") : QStringLiteral("column");
        if (inlineOnly && canUseStyledText(children))
        {
            fragment[QStringLiteral("styledText")] =
                buildStyledText(children);
        }
        return fragment;
    }
    if (value.isObject())
    {
        return buildObject(value.toObject(), parentStyle, siblingIndex);
    }
    return buildText({}, parentStyle);
}

QVariantMap DocumentBuilder::buildObject(
    const QJsonObject &object,
    const ResolvedStyle &parentStyle,
    int)
{
    const QString tag{object[QStringLiteral("tag")].toString()};
    if (tag.isEmpty())
    {
        return buildText({}, parentStyle);
    }

    const ResolvedStyle style{resolveStyle(object, parentStyle)};
    if (tag == QStringLiteral("table"))
    {
        return buildTable(object, style);
    }

    QVariantMap node{makeNode(tag, kindForTag(tag), style)};
    const QString href{object[QStringLiteral("href")].toString()};
    if (!href.isEmpty())
    {
        node[QStringLiteral("href")] = href;
    }

    const QJsonObject data =
        object[QStringLiteral("data")].toObject();
    const QString dataClass{data[QStringLiteral("class")].toString()};

    if (tag == QStringLiteral("br"))
    {
        node[QStringLiteral("text")] = QStringLiteral("\n");
        return node;
    }
    if (tag == QStringLiteral("img"))
    {
        const QString path{object[QStringLiteral("path")].toString()};
        const QString units{
            object[QStringLiteral("sizeUnits")].toString(
                QStringLiteral("px"))};
        double width{object[QStringLiteral("width")].toDouble(-1.0)};
        double height{object[QStringLiteral("height")].toDouble(-1.0)};
        if (units == QStringLiteral("em"))
        {
            width *= m_baseFontPixels * style.fontScale;
            height *= m_baseFontPixels * style.fontScale;
        }
        node[QStringLiteral("source")] = m_basePath + path;
        node[QStringLiteral("alt")] =
            object[QStringLiteral("alt")].toString();
        node[QStringLiteral("width")] = width;
        node[QStringLiteral("height")] = height;
        node[QStringLiteral("sizeUnits")] = QStringLiteral("px");
        return node;
    }

    const QVariantList children =
        buildChildren(object[QStringLiteral("content")], style);
    QVariantList nodeChildren = children;
    if ((tag == QStringLiteral("td") || tag == QStringLiteral("th")) &&
        !containsVisibleText(nodeChildren))
    {
        const QString generatedText{
            generatedFormsText(dataClass)};
        if (!generatedText.isEmpty())
        {
            nodeChildren.append(buildText(generatedText, style));
        }
    }
    const bool inlineOnly{isInlineOnly(nodeChildren)};
    node[QStringLiteral("children")] = inlineOnly ? nodeChildren :
        groupInlineRuns(nodeChildren, style);
    node[QStringLiteral("layout")] = inlineOnly ?
        QStringLiteral("flow") : QStringLiteral("column");
    if (inlineOnly && canUseStyledText(nodeChildren))
    {
        node[QStringLiteral("styledText")] =
            buildStyledText(nodeChildren);
    }
    if (tag == QStringLiteral("ol") || tag == QStringLiteral("ul"))
    {
        QVariantList listChildren = nodeChildren;
        for (qsizetype i{0}; i < listChildren.size(); ++i)
        {
            QVariantMap child{listChildren[i].toMap()};
            const QVariantMap childStyle =
                child[QStringLiteral("style")].toMap();
            child[QStringLiteral("marker")] = markerText(
                childStyle[QStringLiteral("listMarker")].toString(),
                tag == QStringLiteral("ol"),
                i);
            listChildren[i] = child;
        }
        node[QStringLiteral("children")] = listChildren;
    }
    else if (tag == QStringLiteral("td") || tag == QStringLiteral("th"))
    {
        node[QStringLiteral("columnSpan")] = std::max(
            1, object[QStringLiteral("colSpan")].toInt(1));
        node[QStringLiteral("rowSpan")] = std::max(
            1, object[QStringLiteral("rowSpan")].toInt(1));
    }
    return node;
}

QVariantMap DocumentBuilder::buildText(
    const QString &text,
    const ResolvedStyle &parentStyle)
{
    QVariantMap node{makeNode(
        QStringLiteral("text"),
        QStringLiteral("text"),
        parentStyle)};
    node[QStringLiteral("text")] = text;
    return node;
}

QVariantMap DocumentBuilder::buildPlainTextList(
    const QJsonArray &content,
    const ResolvedStyle &parentStyle)
{
    QVariantMap list{makeNode(
        QStringLiteral("ul"),
        QStringLiteral("list"),
        parentStyle)};
    QVariantList children;
    for (const QJsonValue &value : content)
    {
        const QStringList lines{
            value.toString().split('\n', Qt::KeepEmptyParts)};
        for (const QString &line : lines)
        {
            QVariantMap item{makeNode(
                QStringLiteral("li"),
                QStringLiteral("list-item"),
                parentStyle)};
            item[QStringLiteral("marker")] = QStringLiteral("•");
            item[QStringLiteral("children")] =
                QVariantList{buildText(line, parentStyle)};
            item[QStringLiteral("layout")] = QStringLiteral("flow");
            children.append(item);
        }
    }
    list[QStringLiteral("children")] = children;
    list[QStringLiteral("layout")] = QStringLiteral("column");
    return list;
}

QVariantMap DocumentBuilder::buildLegacyImage(
    const QJsonObject &object,
    const ResolvedStyle &parentStyle)
{
    QVariantMap node{makeNode(
        QStringLiteral("img"),
        QStringLiteral("image"),
        parentStyle)};
    node[QStringLiteral("source")] =
        m_basePath + object[QStringLiteral("path")].toString();
    node[QStringLiteral("width")] =
        object[QStringLiteral("width")].toDouble(MAX_IMAGE_WIDTH);
    node[QStringLiteral("height")] =
        object[QStringLiteral("height")].toDouble(-1.0);
    node[QStringLiteral("description")] =
        object[QStringLiteral("description")].toString();
    return node;
}

QVariantMap DocumentBuilder::buildTable(
    const QJsonObject &object,
    const ResolvedStyle &style)
{
    QVariantMap node{makeNode(
        QStringLiteral("table"),
        QStringLiteral("table"),
        style)};
    QVariantList cells;
    int rowIndex{0};
    int columnCount{0};
    QSet<QPair<int, int>> occupiedCells;
    const QVariantList rows =
        buildChildren(object[QStringLiteral("content")], style);
    for (const QVariant &rowValue : rows)
    {
        const QVariantMap row{rowValue.toMap()};
        if (row[QStringLiteral("tag")].toString() != QStringLiteral("tr"))
        {
            continue;
        }
        int columnIndex{0};
        const QVariantList rowChildren =
            row[QStringLiteral("children")].toList();
        for (const QVariant &cellValue : rowChildren)
        {
            while (occupiedCells.contains({rowIndex, columnIndex}))
            {
                ++columnIndex;
            }
            QVariantMap cell{cellValue.toMap()};
            cell[QStringLiteral("row")] = rowIndex;
            cell[QStringLiteral("column")] = columnIndex;
            const int columnSpan{
                std::max(1, cell[QStringLiteral("columnSpan")].toInt())};
            const int rowSpan{
                std::max(1, cell[QStringLiteral("rowSpan")].toInt())};
            for (int row{rowIndex}; row < rowIndex + rowSpan; ++row)
            {
                for (int column{columnIndex};
                     column < columnIndex + columnSpan;
                     ++column)
                {
                    occupiedCells.insert({row, column});
                }
            }
            columnIndex += columnSpan;
            cells.append(cell);
        }
        columnCount = std::max(columnCount, columnIndex);
        ++rowIndex;
    }
    node[QStringLiteral("cells")] = cells;
    node[QStringLiteral("columnCount")] = columnCount;
    node[QStringLiteral("rowCount")] = rowIndex;
    return node;
}

QVariantList DocumentBuilder::buildChildren(
    const QJsonValue &value,
    const ResolvedStyle &parentStyle)
{
    QVariantList children;
    if (value.isArray())
    {
        const QJsonArray array = value.toArray();
        for (qsizetype i{0}; i < array.size(); ++i)
        {
            children.append(buildValue(array[i], parentStyle, i));
        }
    }
    else if (!value.isUndefined() && !value.isNull())
    {
        children.append(buildValue(value, parentStyle, 0));
    }
    return children;
}

QVariantMap DocumentBuilder::makeNode(
    const QString &tag,
    const QString &kind,
    const ResolvedStyle &style)
{
    QVariantMap node;
    node[QStringLiteral("id")] = m_nextNodeId++;
    node[QStringLiteral("tag")] = tag;
    node[QStringLiteral("kind")] = kind;
    node[QStringLiteral("style")] = toVariantMap(style);
    return node;
}

QVariantList DocumentBuilder::groupInlineRuns(
    const QVariantList &children,
    const ResolvedStyle &style)
{
    QVariantList result;
    QVariantList inlineRun;
    const auto flushInlineRun =
        [this, &result, &inlineRun, &style] ()
        {
            if (inlineRun.isEmpty())
            {
                return;
            }
            QVariantMap fragment{makeNode(
                QStringLiteral("fragment"),
                QStringLiteral("fragment"),
                style)};
            fragment[QStringLiteral("children")] = inlineRun;
            fragment[QStringLiteral("layout")] = QStringLiteral("flow");
            result.append(fragment);
            inlineRun.clear();
        };

    for (const QVariant &child : children)
    {
        const QString kind{
            child.toMap()[QStringLiteral("kind")].toString()};
        if (isInlineKind(kind))
        {
            inlineRun.append(child);
            continue;
        }
        flushInlineRun();
        result.append(child);
    }
    flushInlineRun();
    return result;
}

ResolvedStyle DocumentBuilder::resolveStyle(
    const QJsonObject &object,
    const ResolvedStyle &parentStyle) const
{
    ResolvedStyle style{inheritStyle(parentStyle)};
    const QString tag{object[QStringLiteral("tag")].toString()};
    if (tag == QStringLiteral("ul"))
    {
        style.listMarker = QStringLiteral("disc");
    }
    else if (tag == QStringLiteral("ol"))
    {
        style.listMarker = QStringLiteral("decimal");
    }
    else if (tag == QStringLiteral("rt"))
    {
        style.fontScale *= 0.6;
    }
    else if (tag == QStringLiteral("a"))
    {
        style.color = QStringLiteral("#4da3ff");
        style.underline = true;
    }

    const QJsonObject data{object[QStringLiteral("data")].toObject()};
    const QString dataContent{data[QStringLiteral("content")].toString()};
    const bool hasDataContent{data.contains(QStringLiteral("content"))};
    const QString dataClass{data[QStringLiteral("class")].toString()};
    const bool hasDataClass{data.contains(QStringLiteral("class"))};
    const bool hasTitle{object.contains(QStringLiteral("title"))};
    const QList<int> ruleIndexes{m_stylesheet->candidateRuleIndexes(
        tag, dataContent, hasDataContent, dataClass, hasDataClass, hasTitle)};
    for (int ruleIndex : ruleIndexes)
    {
        const DictionaryCssRule &rule{m_stylesheet->rules()[ruleIndex]};
        const bool matched{std::any_of(
            std::cbegin(rule.selectors),
            std::cend(rule.selectors),
            [&tag,
             &dataContent,
             hasDataContent,
             &dataClass,
             hasDataClass,
             hasTitle] (const DictionaryCssSelector &selector)
            {
                return selectorMatches(
                    selector,
                    tag,
                    dataContent,
                    hasDataContent,
                    dataClass,
                    hasDataClass,
                    hasTitle);
            })};
        if (!matched)
        {
            continue;
        }
        for (const auto &[property, value] : rule.declarations)
        {
            applyDeclaration(property, value, m_baseFontPixels, style);
        }
    }

    const QJsonObject inlineStyle =
        object[QStringLiteral("style")].toObject();
    applyInlineStyle(inlineStyle, m_baseFontPixels, style);
    applySemanticFallbacks(object, style);
    return style;
}

bool DocumentBuilder::isInlineKind(const QString &kind)
{
    return kind == QStringLiteral("text") ||
        kind == QStringLiteral("inline") ||
        kind == QStringLiteral("link") ||
        kind == QStringLiteral("ruby") ||
        kind == QStringLiteral("ruby-text") ||
        kind == QStringLiteral("break");
}

bool DocumentBuilder::isInlineOnly(const QVariantList &children)
{
    return std::all_of(
        std::cbegin(children),
        std::cend(children),
        [] (const QVariant &child)
        {
            const QString kind{
                child.toMap()[QStringLiteral("kind")].toString()};
            return isInlineKind(kind);
        });
}

bool DocumentBuilder::canUseStyledText(const QVariantList &nodes)
{
    return std::all_of(
        std::cbegin(nodes),
        std::cend(nodes),
        [] (const QVariant &value)
        {
            const QVariantMap node{value.toMap()};
            const QString kind{node[QStringLiteral("kind")].toString()};
            if (kind == QStringLiteral("text") ||
                kind == QStringLiteral("break"))
            {
                return true;
            }
            if (kind != QStringLiteral("inline"))
            {
                return false;
            }

            const QVariantMap style{node[QStringLiteral("style")].toMap()};
            const bool hasBoxStyle =
                !style[QStringLiteral("backgroundColor")].toString()
                    .isEmpty() ||
                !style[QStringLiteral("borderColor")].toString()
                    .isEmpty() ||
                style[QStringLiteral("borderWidth")].toDouble() > 0.0 ||
                style[QStringLiteral("marginTop")].toDouble() > 0.0 ||
                style[QStringLiteral("marginLeft")].toDouble() > 0.0 ||
                style[QStringLiteral("marginRight")].toDouble() > 0.0 ||
                style[QStringLiteral("marginBottom")].toDouble() > 0.0 ||
                style[QStringLiteral("paddingTop")].toDouble() > 0.0 ||
                style[QStringLiteral("paddingLeft")].toDouble() > 0.0 ||
                style[QStringLiteral("paddingRight")].toDouble() > 0.0 ||
                style[QStringLiteral("paddingBottom")].toDouble() > 0.0;
            return !hasBoxStyle && canUseStyledText(
                node[QStringLiteral("children")].toList());
        });
}

QString DocumentBuilder::escapeStyledText(const QString &text)
{
    QString escaped{text.toHtmlEscaped()};
    escaped.replace('\n', QStringLiteral("<br/>"));
    return escaped;
}

QString DocumentBuilder::buildStyledText(const QVariantMap &node)
{
    const QString kind{node[QStringLiteral("kind")].toString()};
    if (kind == QStringLiteral("text") ||
        kind == QStringLiteral("break"))
    {
        return escapeStyledText(node[QStringLiteral("text")].toString());
    }

    QString text{buildStyledText(
        node[QStringLiteral("children")].toList())};
    const QVariantMap style{node[QStringLiteral("style")].toMap()};
    if (style[QStringLiteral("bold")].toBool())
    {
        text = QStringLiteral("<b>") + text + QStringLiteral("</b>");
    }
    if (style[QStringLiteral("italic")].toBool())
    {
        text = QStringLiteral("<i>") + text + QStringLiteral("</i>");
    }
    if (style[QStringLiteral("underline")].toBool())
    {
        text = QStringLiteral("<u>") + text + QStringLiteral("</u>");
    }
    const QString color{style[QStringLiteral("color")].toString()};
    if (!color.isEmpty())
    {
        text = QStringLiteral("<font color=\"") +
            color.toHtmlEscaped() +
            QStringLiteral("\">") +
            text +
            QStringLiteral("</font>");
    }
    return text;
}

QString DocumentBuilder::buildStyledText(const QVariantList &nodes)
{
    QString text;
    for (const QVariant &value : nodes)
    {
        text += buildStyledText(value.toMap());
    }
    return text;
}

QString DocumentBuilder::kindForTag(const QString &tag)
{
    if (tag == QStringLiteral("span"))
    {
        return QStringLiteral("inline");
    }
    if (tag == QStringLiteral("a"))
    {
        return QStringLiteral("link");
    }
    if (tag == QStringLiteral("details"))
    {
        return QStringLiteral("details");
    }
    if (tag == QStringLiteral("summary"))
    {
        return QStringLiteral("summary");
    }
    if (tag == QStringLiteral("ruby"))
    {
        return QStringLiteral("ruby");
    }
    if (tag == QStringLiteral("rt") || tag == QStringLiteral("rp"))
    {
        return QStringLiteral("ruby-text");
    }
    if (tag == QStringLiteral("ul") || tag == QStringLiteral("ol"))
    {
        return QStringLiteral("list");
    }
    if (tag == QStringLiteral("li"))
    {
        return QStringLiteral("list-item");
    }
    if (tag == QStringLiteral("td") || tag == QStringLiteral("th"))
    {
        return QStringLiteral("cell");
    }
    if (tag == QStringLiteral("img"))
    {
        return QStringLiteral("image");
    }
    if (tag == QStringLiteral("br"))
    {
        return QStringLiteral("break");
    }
    return QStringLiteral("block");
}

bool DocumentBuilder::containsStructuredContent(const QJsonArray &content)
{
    return std::any_of(
        std::cbegin(content),
        std::cend(content),
        [] (const QJsonValue &value)
        {
            return value.isObject();
        });
}

QString DocumentBuilder::makeBasePath(const QString &name)
{
    QString basePath{DirectoryUtils::getDictionaryResourceDir()};
#if defined(Q_OS_WIN)
    basePath.prepend('/');
    basePath.replace('\\', '/');
#endif
    basePath.prepend(QStringLiteral("file://"));
    basePath += '/';
    basePath += name;
    basePath += '/';
    return basePath;
}

QVariantMap DocumentBuilder::toVariantMap(const ResolvedStyle &style)
{
    QVariantMap map;
    const auto insertString =
        [&map] (const QString &key, const QString &value)
        {
            if (!value.isEmpty())
            {
                map[key] = value;
            }
        };
    const auto insertNumber =
        [&map] (const QString &key, double value, double defaultValue)
        {
            if (value != defaultValue)
            {
                map[key] = value;
            }
        };
    const auto insertBool =
        [&map] (const QString &key, bool value)
        {
            if (value)
            {
                map[key] = true;
            }
        };

    insertString(QStringLiteral("color"), style.color);
    insertString(QStringLiteral("backgroundColor"), style.backgroundColor);
    insertString(QStringLiteral("borderColor"), style.borderColor);
    insertNumber(QStringLiteral("fontScale"), style.fontScale, 1.0);
    insertNumber(QStringLiteral("marginTop"), style.marginTop, 0.0);
    insertNumber(QStringLiteral("marginLeft"), style.marginLeft, 0.0);
    insertNumber(QStringLiteral("marginRight"), style.marginRight, 0.0);
    insertNumber(QStringLiteral("marginBottom"), style.marginBottom, 0.0);
    insertNumber(QStringLiteral("paddingTop"), style.paddingTop, 0.0);
    insertNumber(QStringLiteral("paddingLeft"), style.paddingLeft, 0.0);
    insertNumber(QStringLiteral("paddingRight"), style.paddingRight, 0.0);
    insertNumber(QStringLiteral("paddingBottom"), style.paddingBottom, 0.0);
    insertNumber(QStringLiteral("borderWidth"), style.borderWidth, 0.0);
    insertNumber(QStringLiteral("borderRadius"), style.borderRadius, 0.0);
    insertBool(QStringLiteral("fitContent"), style.fitContent);
    insertBool(QStringLiteral("leftBorderOnly"), style.leftBorderOnly);
    insertBool(QStringLiteral("exampleKeyword"), style.exampleKeyword);
    insertString(QStringLiteral("listMarker"), style.listMarker);
    insertString(QStringLiteral("textAlign"), style.textAlign);
    insertString(QStringLiteral("verticalAlign"), style.verticalAlign);
    insertString(QStringLiteral("wordBreak"), style.wordBreak);
    insertBool(QStringLiteral("bold"), style.bold);
    insertBool(QStringLiteral("italic"), style.italic);
    insertBool(QStringLiteral("underline"), style.underline);
    insertBool(QStringLiteral("overline"), style.overline);
    insertBool(QStringLiteral("strikeout"), style.strikeout);
    return map;
}

StructuredContent::StructuredContent(QObject *parent) : QObject(parent)
{

}

StructuredContent::~StructuredContent()
{

}

QVariantMap StructuredContent::document(
    DictionaryInfo *dictionaryInfo,
    const QJsonArray &content,
    Setting::GlossaryStyle glossaryStyle,
    const QFont &font)
{
    DocumentBuilder builder{dictionaryInfo, glossaryStyle, font};
    return builder.build(content);
}
