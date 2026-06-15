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

#include "definition/structuredrichtext.h"

#include <algorithm>
#include <array>
#include <cmath>

#include <QGuiApplication>
#include <QJsonObject>
#include <QLocale>
#include <QQuickWindow>
#include <QSet>
#include <QStringList>
#include <QUrl>

#include "util/utils.h"

/* Begin Local Functions */

namespace
{

/* Number of physical sides represented by CSS box arrays */
constexpr std::size_t BOX_SIDE_COUNT = 4;

/* Array index for the top side of a CSS box */
constexpr std::size_t TOP_SIDE = 0;

/* Array index for the right side of a CSS box */
constexpr std::size_t RIGHT_SIDE = 1;

/* Array index for the bottom side of a CSS box */
constexpr std::size_t BOTTOM_SIDE = 2;

/* Array index for the left side of a CSS box */
constexpr std::size_t LEFT_SIDE = 3;

/* Alpha channel value representing a fully opaque QColor */
constexpr int OPAQUE_ALPHA = 255;

/* Smallest pixel value treated as visible by compatibility rendering */
constexpr double MINIMUM_VISIBLE_PIXELS = 0.001;

/* Pixel width used for the CSS thin border keyword */
constexpr double THIN_BORDER_WIDTH_PIXELS = 1.0;

/* Pixel width used for an unspecified solid border and CSS medium */
constexpr double DEFAULT_BORDER_WIDTH_PIXELS = 3.0;

/* Pixel width used for the CSS thick border keyword */
constexpr double THICK_BORDER_WIDTH_PIXELS = 5.0;

/* Initial selector-node capacity for a structured-content render */
constexpr qsizetype SELECTOR_ARENA_RESERVE = 128;

/* Initial ancestor-stack capacity for selector matching */
constexpr qsizetype ELEMENT_STACK_RESERVE = 32;

/* Initial sibling-state stack capacity for margin collapsing */
constexpr qsizetype SIBLING_STACK_RESERVE = 32;

/* Initial nested-list stack capacity */
constexpr qsizetype LIST_STACK_RESERVE = 8;

/* Initial capacity for resolved CSS values cached during a render */
constexpr qsizetype CSS_VALUE_CACHE_RESERVE = 64;

/* Initial character capacity for generated rich-text HTML */
constexpr qsizetype OUTPUT_RESERVE = 4096;

/* CSS side names in top, right, bottom, left order */
constexpr std::array<const char *, BOX_SIDE_COUNT> SIDE_NAMES = {
    "top",
    "right",
    "bottom",
    "left"
};

/* Longhand padding properties in CSS box-side order */
constexpr std::array<const char *, BOX_SIDE_COUNT> PADDING_PROPERTIES = {
    "padding-top",
    "padding-right",
    "padding-bottom",
    "padding-left"
};

/* Longhand margin properties in CSS box-side order */
constexpr std::array<const char *, BOX_SIDE_COUNT> MARGIN_PROPERTIES = {
    "margin-top",
    "margin-right",
    "margin-bottom",
    "margin-left"
};

/**
 * @brief Components parsed from a CSS border shorthand.
 */
struct ParsedBorder
{
    /* Border width token */
    QString width;

    /* Border style token */
    QString style;

    /* Border color token */
    QString color;
};

/**
 * @brief Split a CSS value into whitespace-delimited top-level tokens.
 *
 * Whitespace inside quoted strings and function parentheses is preserved.
 *
 * @param value The CSS value to tokenize.
 * @return The top-level CSS tokens in source order.
 */
QStringList splitCssTokens(const QString &value)
{
    QStringList tokens;
    QString current;
    QChar quote;
    bool inQuote = false;
    int parenthesisDepth = 0;

    for (const QChar ch : value.trimmed())
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
        else if (ch == '(')
        {
            ++parenthesisDepth;
            current += ch;
        }
        else if (ch == ')')
        {
            --parenthesisDepth;
            current += ch;
        }
        else if (ch.isSpace() && parenthesisDepth == 0)
        {
            if (!current.isEmpty())
            {
                tokens.emplaceBack(std::move(current));
                current.clear();
            }
        }
        else
        {
            current += ch;
        }
    }
    if (!current.isEmpty())
    {
        tokens.emplaceBack(std::move(current));
    }
    return tokens;
}

/**
 * @brief Parse supported components from a CSS border shorthand.
 *
 * @param value The border shorthand value.
 * @return The discovered width, style, and color tokens.
 */
ParsedBorder parseBorderShorthand(const QString &value)
{
    /* CSS border styles recognized while classifying shorthand tokens */
    static const QSet<QString> BORDER_STYLES = {
        "none",
        "hidden",
        "dotted",
        "dashed",
        "solid",
        "double",
        "groove",
        "ridge",
        "inset",
        "outset"
    };

    ParsedBorder border;
    QStringList colorTokens;
    for (const QString &token : splitCssTokens(value))
    {
        const QString normalized = token.toLower();
        if (BORDER_STYLES.contains(normalized))
        {
            border.style = normalized;
        }
        else if (normalized == "thin" ||
                 normalized == "medium" ||
                 normalized == "thick" ||
                 normalized.front().isDigit() ||
                 normalized.front() == '.' ||
                 normalized.front() == '+' ||
                 normalized.front() == '-')
        {
            border.width = token;
        }
        else
        {
            colorTokens.emplaceBack(token);
        }
    }
    border.color = colorTokens.join(' ');
    return border;
}

/**
 * @brief Composite a foreground color over a background color.
 *
 * @param foreground The color painted over the background.
 * @param background The existing painted color.
 * @return The resulting premultiplied color, or an invalid color if clear.
 */
QColor compositeColor(const QColor &foreground, const QColor &background)
{
    const double foregroundAlpha = foreground.alphaF();
    const double backgroundAlpha = background.alphaF();
    const double outputAlpha =
        foregroundAlpha + backgroundAlpha * (1.0 - foregroundAlpha);
    if (outputAlpha <= 0.0)
    {
        return {};
    }

    return QColor::fromRgbF(
        (
            foreground.redF() * foregroundAlpha +
            background.redF() * backgroundAlpha *
                (1.0 - foregroundAlpha)
        ) / outputAlpha,
        (
            foreground.greenF() * foregroundAlpha +
            background.greenF() * backgroundAlpha *
                (1.0 - foregroundAlpha)
        ) / outputAlpha,
        (
            foreground.blueF() * foregroundAlpha +
            background.blueF() * backgroundAlpha *
                (1.0 - foregroundAlpha)
        ) / outputAlpha,
        outputAlpha
    );
}

/**
 * @brief Parse a CSS color supported by the compatibility renderer.
 *
 * QColor handles named and hexadecimal colors. This helper additionally
 * accepts comma-separated rgb() and rgba() values used by dictionaries.
 *
 * @param value The CSS color value.
 * @return The parsed color, or an invalid color when unsupported.
 */
QColor parseCssColor(const QString &value)
{
    QColor color = QColor::fromString(value.trimmed());
    if (color.isValid())
    {
        return color;
    }

    /* Pattern for comma-separated integer RGB channels and decimal alpha */
    static const QRegularExpression RGB_COLOR{
        R"(^rgba?\(\s*([+-]?\d+(?:\.\d+)?)\s*,\s*)"
        R"(([+-]?\d+(?:\.\d+)?)\s*,\s*)"
        R"(([+-]?\d+(?:\.\d+)?)\s*)"
        R"((?:,\s*([+-]?\d+(?:\.\d+)?)\s*)?\)$)",
        QRegularExpression::CaseInsensitiveOption
    };
    const QRegularExpressionMatch match =
        RGB_COLOR.match(value.trimmed());
    if (!match.hasMatch())
    {
        return {};
    }

    /* Maximum numeric value of an RGB color channel */
    constexpr double COLOR_CHANNEL_MAXIMUM = 255.0;
    const double red = std::clamp(
        match.captured(1).toDouble(),
        0.0,
        COLOR_CHANNEL_MAXIMUM
    );
    const double green = std::clamp(
        match.captured(2).toDouble(),
        0.0,
        COLOR_CHANNEL_MAXIMUM
    );
    const double blue = std::clamp(
        match.captured(3).toDouble(),
        0.0,
        COLOR_CHANNEL_MAXIMUM
    );
    const double alpha = match.captured(4).isEmpty() ?
        1.0 :
        std::clamp(match.captured(4).toDouble(), 0.0, 1.0);
    return QColor::fromRgbF(
        red / COLOR_CHANNEL_MAXIMUM,
        green / COLOR_CHANNEL_MAXIMUM,
        blue / COLOR_CHANNEL_MAXIMUM,
        alpha
    );
}

}

/* End Local Functions */
/* Begin Constructor/Destructor */

StructuredRichText::StructuredRichText(QObject *parent) : QObject(parent)
{

}

StructuredRichText::~StructuredRichText()
{

}

/* End Constructor/Destructor */
/* Begin Public Methods */

QString StructuredRichText::parse(
    const DictionaryInfo *info,
    const QJsonArray &content,
    Setting::GlossaryStyle style,
    const QQuickItem *item,
    const QFont &font,
    const QColor &color,
    const QColor &backgroundColor) const
{
    if (info == nullptr)
    {
        return "";
    }

    /* JSON keys and discriminator values for top-level glossary entries */
    constexpr const char *KEY_TYPE = "type";
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *VALUE_TYPE_IMAGE = "image";
    constexpr const char *VALUE_TYPE_STRUCTURED_CONTENT = "structured-content";
    constexpr const char *VALUE_TYPE_TEXT = "text";

    StructuredRichText::Context ctx;
    ctx.info = info;
    ctx.style = style;
    if (item && item->window())
    {
        ctx.screen = item->window()->screen();
    }
    ctx.font = font;
    ctx.dictionaryStyles = info->styles();

    ctx.rootFontPixelSize = fontPixelSize(font, ctx.screen);
    ctx.parentFontPixelSize = ctx.rootFontPixelSize;
    if (color.isValid())
    {
        ctx.glossaryTextColor = color.name(
            color.alpha() == OPAQUE_ALPHA ?
                QColor::HexRgb :
                QColor::HexArgb
        );
        ctx.textColor = ctx.glossaryTextColor;
    }
    if (backgroundColor.isValid())
    {
        ctx.glossaryBackgroundColor = backgroundColor.name(
            backgroundColor.alpha() == OPAQUE_ALPHA ?
                QColor::HexRgb :
                QColor::HexArgb
        );
        ctx.backgroundColor = ctx.glossaryBackgroundColor;
    }
    ctx.paintedBackgroundColor = ctx.backgroundColor;
    ctx.selectorElements.reserve(SELECTOR_ARENA_RESERVE);
    ctx.elements.reserve(ELEMENT_STACK_RESERVE);
    ctx.siblings.reserve(SIBLING_STACK_RESERVE);
    ctx.lists.reserve(LIST_STACK_RESERVE);
    ctx.resolvedCssValues.reserve(CSS_VALUE_CACHE_RESERVE);
    ctx.basepath = DirectoryUtils::getDictionaryResourceDir();
#if defined(Q_OS_WIN)
    ctx.basepath.prepend('/');
    ctx.basepath.replace('\\', '/');
#endif
    ctx.basepath.prepend("file://");
    ctx.basepath += '/';
    ctx.basepath += info->name();
    ctx.basepath += '/';

    const bool containsSc = containsStructuredContent(content);
    const bool shouldUseBullets =
        style == Setting::GlossaryStyleBullet && !containsSc;

    QString glossary;
    glossary.reserve(OUTPUT_RESERVE);
    glossary = "<html><head></head><body>";

    StructuredList fallbackList{"ul", "disc"};

    for (qsizetype i = 0; i < content.size(); ++i)
    {
        const QJsonValue &val = content[i];

        if (shouldUseBullets)
        {
            /* Marker-to-content gap relative to the inherited font size */
            constexpr double RELATIVE_INDENT_FACTOR = 0.35;

            ++fallbackList.item;
            glossary += "<table "
                "cellspacing=\"0\" "
                "width=\"100%\">"
                    "<tr><td "
                    "style=\""
                        "vertical-align: top; "
                        "white-space: nowrap; "
                        "padding-right: ";
            glossary += formatPixelSize(
                ctx.parentFontPixelSize * RELATIVE_INDENT_FACTOR
            );
            glossary += "px;\">";
            glossary += escapeHtml(listMarker(fallbackList, ""));
            glossary += "</td>"
                "<td width=\"100%\" style=\"vertical-align: top;\">";
        }

        switch (val.type())
        {
            case QJsonValue::Type::String:
                glossary += escapeHtml(val.toString()).replace('\n', "<br>");
                break;

            case QJsonValue::Type::Object:
            {
                QJsonObject obj = val.toObject();
                if (obj[KEY_TYPE] == VALUE_TYPE_STRUCTURED_CONTENT)
                {
                    addStructuredChildren(obj[KEY_CONTENT], ctx, glossary);
                }
                else if (obj[KEY_TYPE] == VALUE_TYPE_IMAGE)
                {
                    addImage(obj, ctx, glossary);
                }
                else if (obj[KEY_TYPE] == VALUE_TYPE_TEXT)
                {
                    addText(obj, glossary);
                }
                break;
            }

            default:
                break;
        }

        if (containsSc)
        {
            /* Ignore this check if structured content is detected */
        }
        else if (shouldUseBullets)
        {
            glossary += "</td></tr></table>";
        }
        else if (i >= content.size() - 1)
        {
            /* Avoid putting <br> or | after the fine line */
        }
        else if (style == Setting::GlossaryStyleLineBreak)
        {
            glossary += "<br>";
        }
        else if (style == Setting::GlossaryStylePipe)
        {
            glossary += " | ";
        }
    }

    glossary += "</body></html>";

    return glossary;
}

/* End Public Methods */
/* Begin Structured Content Parsing */

void StructuredRichText::addStructuredData(
    const QJsonObject &obj, QString &out) const
{
    for (const QString &key : obj.keys())
    {
        const QJsonValue &val = obj[key];
        if (!val.isString())
        {
            continue;
        }
        out += ' ';
        out += structuredDataAttributeName(key);
        out += "=\"";
        out += escapeHtml(val.toString());
        out += '"';
    }
}

void StructuredRichText::addStructuredStyle(
    const QJsonObject &obj,
    StructuredRichText::Context &ctx,
    StructuredRichText::ElementRenderState &state) const
{
    /* Structured style key that changes the inherited font size */
    constexpr const char *KEY_FONT_SIZE = "fontSize";

    const auto addString = [&obj, &ctx, &state, this]
    (const char *jsonKey, const char *cssProperty)
    {
        if (obj[jsonKey].isString())
        {
            addResolvedCssDeclaration(
                cssProperty,
                obj[jsonKey].toString(),
                ctx,
                state.declarations
            );
        }
    };
    const auto addSpacing = [&obj, &ctx, &state, this]
    (const char *jsonKey, const char *cssProperty)
    {
        if (obj[jsonKey].isString())
        {
            addResolvedCssDeclaration(
                cssProperty,
                obj[jsonKey].toString(),
                ctx,
                state.declarations
            );
        }
        else if (obj[jsonKey].isDouble())
        {
            addResolvedCssDeclaration(
                cssProperty,
                QString::number(obj[jsonKey].toDouble()) + "em",
                ctx,
                state.declarations
            );
        }
    };

    addString("fontStyle", "font-style");
    addString("fontWeight", "font-weight");
    if (obj[KEY_FONT_SIZE].isString())
    {
        const QString fontSize =
            resolveCssValue(obj[KEY_FONT_SIZE].toString(), ctx);
        if (!fontSize.isEmpty())
        {
            const double pixels = cssFontSizeToPixels(
                fontSize,
                ctx.screen,
                ctx.parentFontPixelSize,
                ctx.rootFontPixelSize
            );
            if (pixels >= 0.0)
            {
                state.fontPixelSize = pixels;
                addCssDeclaration(
                    "font-size",
                    formatPixelSize(pixels) + "px",
                    state.declarations
                );
            }
            else
            {
                addCssDeclaration("font-size", fontSize, state.declarations);
            }
        }
    }

    addString("color", "color");
    addString("background", "background");
    addString("backgroundColor", "background-color");
    if (obj["textDecorationLine"].isArray())
    {
        QStringList lines;
        const QJsonArray values =
            obj["textDecorationLine"].toArray();
        lines.reserve(values.size());
        for (const QJsonValue &value : values)
        {
            if (value.isString())
            {
                lines.emplaceBack(value.toString());
            }
        }
        addResolvedCssDeclaration(
            "text-decoration-line",
            lines.join(' '),
            ctx,
            state.declarations
        );
    }
    else
    {
        addString("textDecorationLine", "text-decoration-line");
    }
    addString("textDecorationStyle", "text-decoration-style");
    addString("textDecorationColor", "text-decoration-color");
    addString("borderColor", "border-color");
    addString("borderStyle", "border-style");
    addString("borderRadius", "border-radius");
    addString("borderWidth", "border-width");
    addString("clipPath", "clip-path");
    addString("verticalAlign", "vertical-align");
    addString("textAlign", "text-align");
    addString("textEmphasis", "text-emphasis");
    addString("textShadow", "text-shadow");
    addSpacing("margin", "margin");
    addSpacing("marginTop", "margin-top");
    addSpacing("marginRight", "margin-right");
    addSpacing("marginBottom", "margin-bottom");
    addSpacing("marginLeft", "margin-left");
    addString("padding", "padding");
    addString("paddingTop", "padding-top");
    addString("paddingRight", "padding-right");
    addString("paddingBottom", "padding-bottom");
    addString("paddingLeft", "padding-left");
    addString("wordBreak", "word-break");
    addString("whiteSpace", "white-space");
    addString("cursor", "cursor");
    addString("listStyleType", "list-style-type");
}

void StructuredRichText::addStructuredContentHelper(
    const QString &str, QString &out) const
{
    out += escapeHtml(str).replace('\n', "<br>");
}

void StructuredRichText::addStructuredContentHelper(
    const QString &str,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    flushPendingVerticalMargin(ctx, out);

    if (ctx.linkHref.isEmpty() && ctx.titleTooltip.isEmpty())
    {
        addStructuredContentHelper(str, out);
        return;
    }

    const bool titleOnly =
        ctx.linkHref.isEmpty() && !ctx.titleTooltip.isEmpty();
    const QString href = ctx.titleTooltip.isEmpty() ?
        ctx.linkHref :
        internalLinkHref(ctx.linkHref, ctx.titleTooltip, "", "title");
    addAnchorStart(href, titleOnly ? ctx.textColor : "", out);
    addStructuredContentHelper(str, out);
    out += "</a>";
}

void StructuredRichText::addStructuredContentHelper(
    const QJsonArray &arr,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    for (const QJsonValue &val : arr)
    {
        addStructuredContent(val, ctx, out);
    }
}

void StructuredRichText::addStructuredContentHelper(
    const QJsonObject &obj,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    /* Structured-content keys consumed by special element renderers */
    constexpr const char *KEY_ALT = "alt";
    constexpr const char *KEY_BACKGROUND = "background";
    constexpr const char *KEY_BORDER = "border";
    constexpr const char *KEY_BORDER_RADIUS = "borderRadius";
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *KEY_DATA = "data";
    constexpr const char *KEY_DESCRIPTION = "description";
    constexpr const char *KEY_HEIGHT = "height";
    constexpr const char *KEY_HREF = "href";
    constexpr const char *KEY_PATH = "path";
    constexpr const char *KEY_PIXELATED = "pixelated";
    constexpr const char *KEY_RENDERING = "imageRendering";
    constexpr const char *KEY_TAG = "tag";
    constexpr const char *KEY_TITLE = "title";
    constexpr const char *KEY_UNITS = "sizeUnits";
    constexpr const char *KEY_VERT_ALIGN = "verticalAlign";
    constexpr const char *KEY_WIDTH = "width";

    QString tag = obj[KEY_TAG].toString();
    if (!isSupportedStructuredTag(tag))
    {
        return;
    }
    else if (tag == "ruby")
    {
        flushPendingVerticalMargin(ctx, out);
        ctx.elements.emplaceBack(structuredElement(obj, ctx));
        addRuby(obj, ctx, out);
        ctx.elements.removeLast();
    }
    else if (obj[KEY_TAG].toString() == "a" &&
        anchorNeedsTooltipHandling(obj, ctx))
    {
        flushPendingVerticalMargin(ctx, out);
        ctx.elements.emplaceBack(structuredElement(obj, ctx));

        const QString oldLinkHref = ctx.linkHref;
        const QString oldTitleTooltip = ctx.titleTooltip;
        if (obj[KEY_HREF].isString())
        {
            ctx.linkHref = obj[KEY_HREF].toString();
        }
        if (obj[KEY_TITLE].isString())
        {
            ctx.titleTooltip = obj[KEY_TITLE].toString();
        }
        addStructuredChildren(obj[KEY_CONTENT], ctx, out);
        ctx.titleTooltip = oldTitleTooltip;
        ctx.linkHref = oldLinkHref;

        ctx.elements.removeLast();
    }
    else if (tag == "br")
    {
        flushPendingVerticalMargin(ctx, out);
        ctx.elements.emplaceBack(structuredElement(obj, ctx));
        out += '<';
        out += tag;
        if (obj[KEY_DATA].isObject())
        {
            addStructuredData(obj[KEY_DATA].toObject(), out);
        }
        out += '>';
        ctx.elements.removeLast();
    }
    else if (tag == "img")
    {
        flushPendingVerticalMargin(ctx, out);
        ctx.elements.emplaceBack(structuredElement(obj, ctx));

        QString filename = escapeHtml(ctx.basepath + obj[KEY_PATH].toString());

        const QString imageTitle = obj[KEY_TITLE].isString() ?
            obj[KEY_TITLE].toString() :
            ctx.titleTooltip;

        const bool imageTitleOnly = ctx.linkHref.isEmpty() &&
            !imageTitle.isEmpty();

        if (!imageTitle.isEmpty())
        {
            addAnchorStart(
                internalLinkHref(ctx.linkHref, imageTitle, "", "title"),
                imageTitleOnly ? ctx.textColor : "",
                out
            );
        }

        out += "<img src=\"";
        out += filename;
        out += '"';

        if (obj[KEY_DATA].isObject())
        {
            addStructuredData(obj[KEY_DATA].toObject(), out);
        }

        if (obj[KEY_ALT].isString())
        {
            out += " alt=\"";
            out += escapeHtml(obj[KEY_ALT].toString());
            out += '"';
        }

        if (obj[KEY_DESCRIPTION].isString())
        {
            out += " description=\"";
            out += escapeHtml(obj[KEY_DESCRIPTION].toString());
            out += '"';
        }

        QString units = obj[KEY_UNITS].toString("px");
        if (obj[KEY_WIDTH].isDouble())
        {
            QString cssSize = QString("%1%2")
                .arg(obj[KEY_WIDTH].toDouble())
                .arg(units);
            const double pixelSize = cssFontSizeToPixels(
                cssSize,
                ctx.screen,
                ctx.parentFontPixelSize,
                ctx.rootFontPixelSize
            );

            out += " width=\"";
            out += formatPixelSize(pixelSize);
            out += '"';
        }
        if (obj[KEY_HEIGHT].isDouble())
        {
            QString cssSize = QString("%1%2")
                .arg(obj[KEY_HEIGHT].toDouble())
                .arg(units);
            const double pixelSize = cssFontSizeToPixels(
                cssSize,
                ctx.screen,
                ctx.parentFontPixelSize,
                ctx.rootFontPixelSize
            );

            out += " height=\"";
            out += formatPixelSize(pixelSize);
            out += '"';
        }

        CssDeclarations declarations;
        addMatchingCssRules(ctx, declarations);

        if (obj[KEY_RENDERING].isString())
        {
            addCssDeclaration(
                "image-rendering",
                obj[KEY_RENDERING].toString("auto"),
                declarations
            );
        }
        else if (obj[KEY_PIXELATED].toBool(false))
        {
            addCssDeclaration("image-rendering", "pixelated", declarations);
        }

        if (obj[KEY_BACKGROUND].toBool(true))
        {
            addCssDeclaration(
                "background-color", "currentColor", declarations
            );
        }

        if (obj[KEY_VERT_ALIGN].isString())
        {
            addCssDeclaration(
                "vertical-align",
                obj[KEY_VERT_ALIGN].toString(),
                declarations
            );
        }

        if (obj[KEY_BORDER].isString())
        {
            addCssDeclaration(
                "border",
                obj[KEY_BORDER].toString(),
                declarations
            );
        }

        if (obj[KEY_BORDER_RADIUS].isString())
        {
            addCssDeclaration(
                "border-radius",
                obj[KEY_BORDER_RADIUS].toString(),
                declarations
            );
        }

        if (!declarations.isEmpty())
        {
            out += " style=\"";
            addCssDeclarations(declarations, out);
            out += '"';
        }
        out += '>';

        if (!imageTitle.isEmpty())
        {
            out += "</a>";
        }

        ctx.elements.removeLast();
    }
    else
    {
        addStructuredElement(obj, tag, ctx, out);
    }
}

void StructuredRichText::addStructuredChildren(
    const QJsonValue &val,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    ctx.siblings.emplaceBack();
    addStructuredContent(val, ctx, out);
    flushPendingVerticalMargin(ctx, out);
    ctx.siblings.removeLast();
}

void StructuredRichText::addStructuredElement(
    const QJsonObject &obj,
    const QString &tag,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    /* Structured-content key containing an element's children */
    constexpr const char *KEY_CONTENT = "content";

    ctx.elements.emplaceBack(structuredElement(obj, ctx));

    const QString oldTitleTooltip = ctx.titleTooltip;
    ElementRenderState state = elementRenderState(obj, tag, ctx);
    const bool blockElement = state.layout != ElementLayout::Inline;
    const bool collapsibleBlock = state.marginFlow == MarginFlow::Collapsible;
    const bool propagatesTrailingChildMargin =
        state.marginFlow == MarginFlow::PropagateLastChild;

    if (blockElement)
    {
        addPendingVerticalMargin(
            state.box.margins[static_cast<std::size_t>(BoxSide::Top)],
            state.fontPixelSize,
            ctx,
            ctx.siblings.back()
        );
        if (!collapsibleBlock)
        {
            flushPendingVerticalMargin(ctx, out);
        }
    }
    else
    {
        flushPendingVerticalMargin(ctx, out);
    }
    addStructuredElementStart(state, ctx, out);

    if (state.layout == ElementLayout::List)
    {
        ctx.lists.emplaceBack(StructuredList{tag, state.listMarkerType});
    }

    std::swap(ctx.parentFontPixelSize, state.fontPixelSize);
    std::swap(ctx.textColor, state.textColor);
    std::swap(ctx.titleTooltip, state.titleTooltip);
    std::swap(ctx.textDirection, state.textDirection);
    std::swap(ctx.paintedBackgroundColor, state.paintedBackgroundColor);

    SiblingState completedChildren;
    if (blockElement)
    {
        SiblingState childSiblings;
        if (collapsibleBlock)
        {
            mergePendingVerticalMargins(
                ctx.siblings.back(),
                childSiblings
            );
            clearPendingVerticalMargins(ctx.siblings.back());
        }

        ctx.siblings.emplaceBack(std::move(childSiblings));
        addStructuredContent(obj[KEY_CONTENT], ctx, out);
        if (!collapsibleBlock && !propagatesTrailingChildMargin)
        {
            flushPendingVerticalMargin(ctx, out);
        }
        completedChildren = ctx.siblings.takeLast();
    }
    else
    {
        addStructuredChildren(obj[KEY_CONTENT], ctx, out);
    }

    std::swap(ctx.paintedBackgroundColor, state.paintedBackgroundColor);
    std::swap(ctx.textDirection, state.textDirection);
    std::swap(ctx.titleTooltip, state.titleTooltip);
    std::swap(ctx.textColor, state.textColor);
    std::swap(ctx.parentFontPixelSize, state.fontPixelSize);
    ctx.titleTooltip = oldTitleTooltip;

    if (state.layout == ElementLayout::List)
    {
        ctx.lists.removeLast();
    }

    addStructuredElementEnd(state, ctx, out);
    if (blockElement)
    {
        if (collapsibleBlock || propagatesTrailingChildMargin)
        {
            mergePendingVerticalMargins(
                completedChildren,
                ctx.siblings.back()
            );
        }
        addPendingVerticalMargin(
            state.box.margins[static_cast<std::size_t>(BoxSide::Bottom)],
            state.fontPixelSize,
            ctx,
            ctx.siblings.back()
        );
    }
    ctx.elements.removeLast();
}

StructuredRichText::ElementRenderState
StructuredRichText::elementRenderState(
    const QJsonObject &obj,
    const QString &tag,
    StructuredRichText::Context &ctx) const
{
    ElementRenderState state;
    state.tag = tag;
    state.outputTag = tag;
    state.fontPixelSize = ctx.parentFontPixelSize;
    state.textColor = ctx.textColor;
    state.titleTooltip = ctx.titleTooltip;
    state.textDirection = ctx.textDirection;
    state.paintedBackgroundColor = ctx.paintedBackgroundColor;
    state.textOnlyContent = obj["content"].isString();
    if (obj["lang"].isString())
    {
        state.textDirection =
            QLocale(obj["lang"].toString()).textDirection();
    }

    addElementAttributes(obj, state);
    resolveElementStyles(obj, ctx, state);
    resolveElementLayout(ctx, state);
    applyElementCompatibility(ctx, state);

    return state;
}

void StructuredRichText::addElementAttributes(
    const QJsonObject &obj,
    StructuredRichText::ElementRenderState &state) const
{
    /* Structured-content keys emitted as HTML element attributes */
    constexpr const char *KEY_COLSPAN = "colSpan";
    constexpr const char *KEY_DATA = "data";
    constexpr const char *KEY_HREF = "href";
    constexpr const char *KEY_LANG = "lang";
    constexpr const char *KEY_ROWSPAN = "rowSpan";
    constexpr const char *KEY_TITLE = "title";

    if (obj[KEY_HREF].isString())
    {
        state.attributes += " href=\"";
        state.attributes += escapeHtml(obj[KEY_HREF].toString());
        state.attributes += '"';
    }

    if (obj[KEY_DATA].isObject())
    {
        addStructuredData(obj[KEY_DATA].toObject(), state.attributes);
    }

    if (obj[KEY_COLSPAN].isDouble())
    {
        state.attributes += " colspan=\"";
        state.attributes += QString::number(
            static_cast<int>(obj[KEY_COLSPAN].toDouble())
        );
        state.attributes += '"';
    }

    if (obj[KEY_ROWSPAN].isDouble())
    {
        state.attributes += " rowspan=\"";
        state.attributes += QString::number(
            static_cast<int>(obj[KEY_ROWSPAN].toDouble())
        );
        state.attributes += '"';
    }

    if (obj[KEY_TITLE].isString())
    {
        state.titleTooltip = obj[KEY_TITLE].toString();
        state.attributes += " title=\"";
        state.attributes += escapeHtml(obj[KEY_TITLE].toString());
        state.attributes += '"';
    }

    if (obj[KEY_LANG].isString())
    {
        state.attributes += " lang=\"";
        state.attributes += escapeHtml(obj[KEY_LANG].toString());
        state.attributes += '"';
    }
}

void StructuredRichText::resolveElementStyles(
    const QJsonObject &obj,
    StructuredRichText::Context &ctx,
    StructuredRichText::ElementRenderState &state) const
{
    /* Structured-content key containing direct style declarations */
    constexpr const char *KEY_STYLE = "style";

    if (state.tag == "table")
    {
        addCssDeclaration("border", "1px solid", state.declarations);
        addCssDeclaration(
            "border-collapse", "collapse", state.declarations
        );
    }
    else if (state.tag == "th" || state.tag == "td")
    {
        addCssDeclaration("border", "1px solid", state.declarations);
        addCssDeclaration(
            "border-collapse", "collapse", state.declarations
        );
        addCssDeclaration("padding", "5px", state.declarations);
    }

    addMatchingCssRules(
        ctx,
        state.declarations,
        &state.beforeContent
    );

    if (obj[KEY_STYLE].isObject())
    {
        addStructuredStyle(
            obj[KEY_STYLE].toObject(),
            ctx,
            state
        );
    }
    if (state.declarations.contains("font-size"))
    {
        const double pixelSize = cssFontSizeToPixels(
            state.declarations["font-size"],
            ctx.screen,
            ctx.parentFontPixelSize,
            ctx.rootFontPixelSize
        );
        if (pixelSize >= 0.0)
        {
            state.fontPixelSize = pixelSize;
        }
    }
    if (state.declarations.contains("color"))
    {
        state.textColor = state.declarations["color"];
    }
}

void StructuredRichText::resolveElementLayout(
    StructuredRichText::Context &ctx,
    StructuredRichText::ElementRenderState &state) const
{
    const QString listType =
        state.declarations.value("list-style-type");
    state.listMarkerType = normalizeListMarker(listType.isEmpty() ?
        defaultListMarker(state.tag) :
        listType
    );
    const bool isList = state.tag == "ul" || state.tag == "ol";
    const bool isListItem = state.tag == "li" && !ctx.lists.isEmpty();
    if (isListItem)
    {
        StructuredList &list = ctx.lists.back();
        ++list.item;
        state.listMarker = listMarker(list, listType);
    }

    const auto isBoxDeclaration = [] (const QString &property) -> bool
    {
        return property == "background-color" ||
            property.startsWith("border") ||
            property.startsWith("padding");
    };
    const bool styledBlock =
        (
            state.tag == "div" ||
            state.tag == "details" ||
            state.tag == "summary"
        ) &&
        std::any_of(
            state.declarations.keyBegin(),
            state.declarations.keyEnd(),
            isBoxDeclaration
        );
    state.paddedSpan =
        state.tag == "span" &&
        (
            ctx.selectorElements[ctx.elements.back()].attributes.value(
                "data-sc-class"
            ) == "tag" ||
            ctx.selectorElements[ctx.elements.back()].attributes.value(
                "data-sc-content"
            ) == "forms-label"
        );

    if (isList)
    {
        state.layout = ElementLayout::List;
        state.outputTag = "div";
    }
    else if (isListItem && !state.listMarker.isEmpty())
    {
        state.layout = ElementLayout::MarkedListItem;
        state.outputTag = "table";
        state.widthPolicy = WidthPolicy::Fill;
    }
    else if (isListItem)
    {
        state.layout = ElementLayout::MarkerlessListItem;
        state.outputTag = "div";
    }
    else if (state.tag == "details" || styledBlock)
    {
        state.layout = ElementLayout::Box;
        state.outputTag = "table";
        state.widthPolicy = WidthPolicy::Fill;
    }
    else if (state.tag == "div" || state.tag == "summary")
    {
        state.layout = ElementLayout::Block;
        state.outputTag = "div";
    }

    if (isList || isListItem)
    {
        state.declarations.remove("list-style-type");
    }
    if (state.listMarkerType == "none")
    {
        state.listMarkerType.clear();
    }

    const QString width = state.declarations.value("width");
    if (state.layout == ElementLayout::Box &&
        width.compare("fit-content", Qt::CaseInsensitive) == 0)
    {
        state.widthPolicy = WidthPolicy::FitContent;
        state.declarations.remove("width");
    }
    else if (state.layout == ElementLayout::Box && !width.isEmpty())
    {
        state.widthPolicy = WidthPolicy::Explicit;
    }
}

void StructuredRichText::applyElementCompatibility(
    const StructuredRichText::Context &ctx,
    StructuredRichText::ElementRenderState &state) const
{
    if (state.tag == "span")
    {
        const QString left = state.declarations.take("margin-left");
        const QString right = state.declarations.take("margin-right");
        if (state.textDirection == Qt::RightToLeft)
        {
            state.inlineSpacingBefore = right;
            state.inlineSpacingAfter = left;
        }
        else
        {
            state.inlineSpacingBefore = left;
            state.inlineSpacingAfter = right;
        }
    }

    if (state.layout == ElementLayout::Inline)
    {
        state.marginFlow = MarginFlow::Inline;
        return;
    }

    resolveBoxStyle(ctx, state);
    state.marginFlow = marginFlow(state);
}

void StructuredRichText::resolveBoxStyle(
    const StructuredRichText::Context &ctx,
    StructuredRichText::ElementRenderState &state) const
{
    const bool hasPadding =
        state.declarations.contains("padding") ||
        std::any_of(
            PADDING_PROPERTIES.begin(),
            PADDING_PROPERTIES.end(),
            [&state] (const char *property)
            {
                return state.declarations.contains(property);
            }
        );
    if (hasPadding)
    {
        const QString paddingShorthand =
            state.declarations.take("padding");
        for (std::size_t side = 0; side < BOX_SIDE_COUNT; ++side)
        {
            QString value =
                state.declarations.take(PADDING_PROPERTIES[side]);
            if (value.isEmpty())
            {
                value = cssBoxSideValue(
                    paddingShorthand,
                    static_cast<qsizetype>(side)
                );
            }
            const double pixels = cssFontSizeToPixels(
                value,
                ctx.screen,
                state.fontPixelSize,
                ctx.rootFontPixelSize
            );
            if (pixels > MINIMUM_VISIBLE_PIXELS)
            {
                state.box.padding[side] = pixels;
            }
        }
    }

    for (std::size_t side = 0; side < BOX_SIDE_COUNT; ++side)
    {
        state.box.margins[side] =
            state.declarations.take(MARGIN_PROPERTIES[side]);
    }

    if (state.layout == ElementLayout::Block &&
        state.textOnlyContent)
    {
        QString &left = state.box.margins[LEFT_SIDE];
        QString &right = state.box.margins[RIGHT_SIDE];
        if (state.textDirection == Qt::RightToLeft)
        {
            state.contentInlineSpacingBefore = std::move(right);
            state.contentInlineSpacingAfter = std::move(left);
        }
        else
        {
            state.contentInlineSpacingBefore = std::move(left);
            state.contentInlineSpacingAfter = std::move(right);
        }
    }

    if (state.declarations.contains("background-color"))
    {
        const QString background =
            state.declarations.take("background-color");
        QColor resolvedBackground = parseCssColor(background);
        if (resolvedBackground.isValid() &&
            resolvedBackground.alpha() < OPAQUE_ALPHA)
        {
            const QColor parent =
                parseCssColor(ctx.paintedBackgroundColor);
            if (parent.isValid())
            {
                resolvedBackground =
                    compositeColor(resolvedBackground, parent);
            }
        }
        if (resolvedBackground.isValid())
        {
            state.box.backgroundColor = resolvedBackground.name(
                resolvedBackground.alpha() == OPAQUE_ALPHA ?
                    QColor::HexRgb :
                    QColor::HexArgb
            );
            state.paintedBackgroundColor = state.box.backgroundColor;
        }
        else
        {
            state.box.backgroundColor = background;
        }
    }

    const bool hasBorder = std::any_of(
        state.declarations.keyBegin(),
        state.declarations.keyEnd(),
        [] (const QString &property)
        {
            return property == "border" ||
                property.startsWith("border-");
        }
    );
    if (!hasBorder)
    {
        state.declarations.remove("border-radius");
        state.box.enabled =
            state.layout == ElementLayout::Box ||
            !state.box.backgroundColor.isEmpty() ||
            hasPadding;
        const bool hasHorizontalMargins =
            !isZeroSpacing(state.box.margins[LEFT_SIDE]) ||
            !isZeroSpacing(state.box.margins[RIGHT_SIDE]);
        if (state.layout == ElementLayout::Block &&
            !state.textOnlyContent &&
            hasHorizontalMargins)
        {
            state.layout = ElementLayout::Box;
            state.outputTag = "table";
            state.widthPolicy = WidthPolicy::Fill;
            state.box.enabled = true;
        }
        return;
    }

    const ParsedBorder commonBorder =
        parseBorderShorthand(state.declarations.take("border"));
    const QString borderWidths =
        state.declarations.take("border-width");
    const QString borderStyles =
        state.declarations.take("border-style");
    const QString borderColors =
        state.declarations.take("border-color");

    std::size_t paintedBorderCount = 0;
    QString sharedBorderColor;
    bool sharedBorderColorMatches = true;
    for (std::size_t side = 0; side < BOX_SIDE_COUNT; ++side)
    {
        ParsedBorder border = commonBorder;
        const QString sideName = SIDE_NAMES[side];
        const QString boxWidth = cssBoxSideValue(
            borderWidths,
            static_cast<qsizetype>(side)
        );
        const QString boxStyle = cssBoxSideValue(
            borderStyles,
            static_cast<qsizetype>(side)
        );
        const QString boxColor = cssBoxSideValue(
            borderColors,
            static_cast<qsizetype>(side)
        );
        if (!boxWidth.isEmpty())
        {
            border.width = boxWidth;
        }
        if (!boxStyle.isEmpty())
        {
            border.style = boxStyle;
        }
        if (!boxColor.isEmpty())
        {
            border.color = boxColor;
        }

        const ParsedBorder sideBorder = parseBorderShorthand(
            state.declarations.take("border-" + sideName)
        );
        if (!sideBorder.width.isEmpty())
        {
            border.width = sideBorder.width;
        }
        if (!sideBorder.style.isEmpty())
        {
            border.style = sideBorder.style;
        }
        if (!sideBorder.color.isEmpty())
        {
            border.color = sideBorder.color;
        }

        const QString widthProperty = "border-" + sideName + "-width";
        const QString styleProperty = "border-" + sideName + "-style";
        const QString colorProperty = "border-" + sideName + "-color";
        if (state.declarations.contains(widthProperty))
        {
            border.width = state.declarations.take(widthProperty);
        }
        if (state.declarations.contains(styleProperty))
        {
            border.style = state.declarations.take(styleProperty);
        }
        if (state.declarations.contains(colorProperty))
        {
            border.color = state.declarations.take(colorProperty);
        }

        if (border.width.isEmpty() &&
            border.style.compare("solid", Qt::CaseInsensitive) == 0)
        {
            border.width =
                formatPixelSize(DEFAULT_BORDER_WIDTH_PIXELS) + "px";
        }
        double widthPixels = cssFontSizeToPixels(
            border.width,
            ctx.screen,
            state.fontPixelSize,
            ctx.rootFontPixelSize
        );
        if (border.width.compare("thin", Qt::CaseInsensitive) == 0)
        {
            widthPixels = THIN_BORDER_WIDTH_PIXELS;
        }
        else if (border.width.compare("medium", Qt::CaseInsensitive) == 0)
        {
            widthPixels = DEFAULT_BORDER_WIDTH_PIXELS;
        }
        else if (border.width.compare("thick", Qt::CaseInsensitive) == 0)
        {
            widthPixels = THICK_BORDER_WIDTH_PIXELS;
        }

        if (border.color.isEmpty() ||
            border.color.compare("currentcolor", Qt::CaseInsensitive) == 0)
        {
            border.color = state.textColor;
        }

        BorderSide &resolved = state.box.borders[side];
        resolved.color = border.color;
        resolved.style = border.style;
        resolved.widthPixels = std::max(0.0, widthPixels);
        const QColor color = parseCssColor(resolved.color);
        resolved.painted =
            resolved.style.compare("solid", Qt::CaseInsensitive) == 0 &&
            resolved.widthPixels > MINIMUM_VISIBLE_PIXELS &&
            color.isValid();
        if (resolved.painted)
        {
            ++paintedBorderCount;
            if (sharedBorderColor.isEmpty())
            {
                sharedBorderColor = resolved.color;
            }
            else if (sharedBorderColor != resolved.color)
            {
                sharedBorderColorMatches = false;
            }
        }
        else if (!resolved.style.isEmpty() &&
                 resolved.style.compare("none", Qt::CaseInsensitive) != 0 &&
                 resolved.widthPixels > MINIMUM_VISIBLE_PIXELS)
        {
            state.cellDeclarations["border-" + sideName] =
                formatPixelSize(resolved.widthPixels) + "px " +
                resolved.style + ' ' + resolved.color;
        }
    }
    state.declarations.remove("border-radius");

    state.box.compactBorderFrame =
        paintedBorderCount > 0 && sharedBorderColorMatches;
    state.box.enabled =
        state.layout == ElementLayout::Box ||
        paintedBorderCount > 0 ||
        !state.box.backgroundColor.isEmpty() ||
        std::any_of(
            state.box.padding.begin(),
            state.box.padding.end(),
            [] (double value)
            {
                return value > MINIMUM_VISIBLE_PIXELS;
            }
        );

    const bool hasHorizontalMargins =
        !isZeroSpacing(state.box.margins[LEFT_SIDE]) ||
        !isZeroSpacing(state.box.margins[RIGHT_SIDE]);
    if (state.layout == ElementLayout::Block &&
        !state.textOnlyContent &&
        hasHorizontalMargins)
    {
        state.layout = ElementLayout::Box;
        state.outputTag = "table";
        state.widthPolicy = WidthPolicy::Fill;
        state.box.enabled = true;
    }
}

StructuredRichText::MarginFlow StructuredRichText::marginFlow(
    const StructuredRichText::ElementRenderState &state) const
{
    if (state.layout == ElementLayout::Inline)
    {
        return MarginFlow::Inline;
    }

    const bool hasVerticalPadding =
        state.box.padding[TOP_SIDE] > MINIMUM_VISIBLE_PIXELS ||
        state.box.padding[BOTTOM_SIDE] > MINIMUM_VISIBLE_PIXELS;
    const bool hasPaintedBorder = std::any_of(
        state.box.borders.begin(),
        state.box.borders.end(),
        [] (const BorderSide &border)
        {
            return border.painted;
        }
    );
    if (state.box.enabled &&
        (!state.box.backgroundColor.isEmpty() ||
         hasVerticalPadding ||
         hasPaintedBorder))
    {
        return MarginFlow::Contained;
    }
    if (state.layout == ElementLayout::MarkedListItem)
    {
        return MarginFlow::PropagateLastChild;
    }
    return MarginFlow::Collapsible;
}

void StructuredRichText::addStructuredElementStart(
    const StructuredRichText::ElementRenderState &state,
    const StructuredRichText::Context &ctx,
    QString &out) const
{
    addInlineSpacer(
        state.inlineSpacingBefore,
        state.fontPixelSize,
        ctx,
        out
    );

    if (state.layout == ElementLayout::Box)
    {
        addBoxStart(state, ctx, out);
    }
    else
    {
        out += '<';
        out += state.outputTag;
        if (state.layout == ElementLayout::MarkedListItem)
        {
            out += " cellspacing=\"0\""
                " width=\"100%\"";
        }
        out += state.attributes;
        if (!state.declarations.isEmpty())
        {
            out += " style=\"";
            addCssDeclarations(state.declarations, out);
            out += '"';
        }
        out += '>';
    }

    if (state.layout == ElementLayout::MarkerlessListItem)
    {
        addVerticalPixelSpacer(state.box.padding[TOP_SIDE], out);
    }

    if (state.layout == ElementLayout::MarkedListItem)
    {
        /* Marker-to-content gap relative to the item's font size */
        constexpr double RELATIVE_INDENT_FACTOR = 0.35;
        const char *paddingProperty =
            state.textDirection == Qt::RightToLeft ?
                "padding-left" :
                "padding-right";

        out += "<tr><td style=\"vertical-align: top; white-space: nowrap; ";
        out += paddingProperty;
        out += ": ";
        out += formatPixelSize(
            state.fontPixelSize * RELATIVE_INDENT_FACTOR
        );
        out += "px;\">";
        out += escapeHtml(state.listMarker);
        out += "</td><td width=\"100%\" style=\"vertical-align: top;\">";
    }

    if (state.paddedSpan)
    {
        out += "&nbsp;";
    }

    addInlineSpacer(
        state.contentInlineSpacingBefore,
        state.fontPixelSize,
        ctx,
        out
    );

    if (!state.beforeContent.isEmpty())
    {
        out += escapeHtml(state.beforeContent);
    }
}

void StructuredRichText::addStructuredElementEnd(
    const StructuredRichText::ElementRenderState &state,
    const StructuredRichText::Context &ctx,
    QString &out) const
{
    if (state.paddedSpan)
    {
        out += "&nbsp;";
    }
    addInlineSpacer(
        state.contentInlineSpacingAfter,
        state.fontPixelSize,
        ctx,
        out
    );
    if (state.layout == ElementLayout::MarkerlessListItem)
    {
        addVerticalPixelSpacer(state.box.padding[BOTTOM_SIDE], out);
    }
    if (state.layout == ElementLayout::Box)
    {
        addBoxEnd(state, ctx, out);
    }
    else
    {
        if (state.layout == ElementLayout::MarkedListItem)
        {
            out += "</td></tr>";
        }

        out += "</";
        out += state.outputTag;
        out += '>';
    }

    addInlineSpacer(
        state.inlineSpacingAfter,
        state.fontPixelSize,
        ctx,
        out
    );

    if (state.paddedSpan)
    {
        out += ' ';
    }
}

void StructuredRichText::addBoxStart(
    const StructuredRichText::ElementRenderState &state,
    const StructuredRichText::Context &ctx,
    QString &out) const
{
    const auto marginPixels =
        [&state, &ctx, this] (std::size_t side) -> double
    {
        return std::max(
            0.0,
            cssFontSizeToPixels(
                state.box.margins[side],
                ctx.screen,
                state.fontPixelSize,
                ctx.rootFontPixelSize
            )
        );
    };
    const double leftMargin = marginPixels(LEFT_SIDE);
    const double rightMargin = marginPixels(RIGHT_SIDE);
    const bool hasMarginGrid =
        leftMargin > MINIMUM_VISIBLE_PIXELS ||
        rightMargin > MINIMUM_VISIBLE_PIXELS;

    if (hasMarginGrid)
    {
        out += "<table cellspacing=\"0\""
            " width=\"100%\"><tr>";
        if (leftMargin > MINIMUM_VISIBLE_PIXELS)
        {
            out += "<td width=\"";
            out += formatPixelSize(leftMargin);
            out += "\"></td>";
        }
        out += "<td width=\"100%\" style=\"vertical-align: top;\">";
    }
    if (!state.box.enabled)
    {
        return;
    }

    const auto addTableStart =
        [&state, &out, this] (
            const QString &background,
            bool outerTable)
    {
        out += "<table cellspacing=\"0\"";
        if (state.widthPolicy == WidthPolicy::Fill)
        {
            out += " width=\"100%\"";
        }
        if (!background.isEmpty())
        {
            out += " bgcolor=\"";
            out += background;
            out += '"';
        }
        if (outerTable)
        {
            out += state.attributes;
            if (!state.declarations.isEmpty())
            {
                out += " style=\"";
                addCssDeclarations(state.declarations, out);
                out += '"';
            }
        }
        out += "><tr><td style=\"vertical-align: top;";
    };

    const auto addBorderPadding =
        [&state, &out, this] (std::size_t side)
    {
        const BorderSide &border = state.box.borders[side];
        if (!border.painted)
        {
            return;
        }
        out += "padding-";
        out += SIDE_NAMES[side];
        out += ": ";
        out += formatPixelSize(border.widthPixels);
        out += "px;";
    };

    const auto closeOpeningCell = [&out] ()
    {
        out += "\">";
    };

    const auto firstPaintedBorder =
        std::find_if(
            state.box.borders.begin(),
            state.box.borders.end(),
            [] (const BorderSide &border)
            {
                return border.painted;
            }
        );
    const bool hasPaintedBorder =
        firstPaintedBorder != state.box.borders.end();
    const QString contentBackground =
        state.paintedBackgroundColor;

    bool outerTable = true;
    if (hasPaintedBorder && state.box.compactBorderFrame)
    {
        addTableStart(firstPaintedBorder->color, outerTable);
        for (std::size_t side = 0; side < BOX_SIDE_COUNT; ++side)
        {
            addBorderPadding(side);
        }
        closeOpeningCell();
        outerTable = false;
    }
    else if (hasPaintedBorder)
    {
        /* Stable nesting order for independently colored border layers */
        constexpr std::array<std::size_t, BOX_SIDE_COUNT> BORDER_LAYER_ORDER = {
            TOP_SIDE,
            RIGHT_SIDE,
            BOTTOM_SIDE,
            LEFT_SIDE
        };
        for (const std::size_t side : BORDER_LAYER_ORDER)
        {
            const BorderSide &border = state.box.borders[side];
            if (!border.painted)
            {
                continue;
            }
            addTableStart(border.color, outerTable);
            addBorderPadding(side);
            closeOpeningCell();
            outerTable = false;
        }
    }

    addTableStart(contentBackground, outerTable);
    /* Cell-padding emission order used by the Qt HTML table workaround */
    constexpr std::array<std::size_t, BOX_SIDE_COUNT> PADDING_ORDER = {
        BOTTOM_SIDE,
        LEFT_SIDE,
        RIGHT_SIDE,
        TOP_SIDE
    };
    for (const std::size_t side : PADDING_ORDER)
    {
        if (state.box.padding[side] <= MINIMUM_VISIBLE_PIXELS)
        {
            continue;
        }
        out += "padding-";
        out += SIDE_NAMES[side];
        out += ": ";
        out += formatPixelSize(state.box.padding[side]);
        out += "px;";
    }
    addCssDeclarations(state.cellDeclarations, out);
    closeOpeningCell();
}

void StructuredRichText::addBoxEnd(
    const StructuredRichText::ElementRenderState &state,
    const StructuredRichText::Context &ctx,
    QString &out) const
{
    if (state.box.enabled)
    {
        out += "</td></tr></table>";
        const std::size_t paintedBorderCount =
            static_cast<std::size_t>(std::count_if(
                state.box.borders.begin(),
                state.box.borders.end(),
                [] (const BorderSide &border)
                {
                    return border.painted;
                }
            ));
        const std::size_t borderLayerCount =
            state.box.compactBorderFrame && paintedBorderCount > 0 ?
                1 :
                paintedBorderCount;
        for (std::size_t layer = 0;
             layer < borderLayerCount;
             ++layer)
        {
            out += "</td></tr></table>";
        }
    }

    const auto marginPixels =
    [&state, &ctx, this] (std::size_t side) -> double
    {
        return std::max(
            0.0,
            cssFontSizeToPixels(
                state.box.margins[side],
                ctx.screen,
                state.fontPixelSize,
                ctx.rootFontPixelSize
            )
        );
    };
    const double leftMargin = marginPixels(LEFT_SIDE);
    const double rightMargin = marginPixels(RIGHT_SIDE);
    if (leftMargin > MINIMUM_VISIBLE_PIXELS ||
        rightMargin > MINIMUM_VISIBLE_PIXELS)
    {
        out += "</td>";
        if (rightMargin > MINIMUM_VISIBLE_PIXELS)
        {
            out += "<td width=\"";
            out += formatPixelSize(rightMargin);
            out += "\"></td>";
        }
        out += "</tr></table>";
    }
}

void StructuredRichText::addInlineSpacer(
    const QString &spacing,
    double fontPixelSize,
    const StructuredRichText::Context &ctx,
    QString &out) const
{
    const double pixelSize = cssFontSizeToPixels(
        spacing,
        ctx.screen,
        fontPixelSize,
        ctx.rootFontPixelSize
    );
    if (pixelSize <= 0.0)
    {
        return;
    }

    out += "<span style=\"font-size: 1px;letter-spacing: ";
    out += formatPixelSize(pixelSize);
    out += "px;\">&nbsp;</span>";
}

void StructuredRichText::addVerticalSpacer(
    const QString &spacing,
    double fontPixelSize,
    const StructuredRichText::Context &ctx,
    QString &out) const
{
    const QString value = spacing.trimmed();
    if (isZeroSpacing(value))
    {
        return;
    }

    const double pixelSize = cssFontSizeToPixels(
        value,
        ctx.screen,
        fontPixelSize,
        ctx.rootFontPixelSize
    );
    if (pixelSize < 0.0)
    {
        return;
    }

    addVerticalPixelSpacer(pixelSize, out);
}

void StructuredRichText::addVerticalPixelSpacer(
    double pixelSize, QString &out) const
{
    /* Threshold below which a spacer is omitted from generated HTML */
    constexpr double MINIMUM_VISIBLE_SPACING_PIXELS = 0.001;

    /* Line box height used to represent an arbitrary vertical spacer */
    constexpr double SPACER_LINE_HEIGHT_PIXELS = 1.0;

    if (std::abs(pixelSize) < MINIMUM_VISIBLE_SPACING_PIXELS)
    {
        return;
    }

    out += "<div style=\"font-size: ";
    out += formatPixelSize(SPACER_LINE_HEIGHT_PIXELS);
    out += "px;line-height: ";
    out += formatPixelSize(SPACER_LINE_HEIGHT_PIXELS);
    out += "px;margin-top: ";
    out += formatPixelSize(pixelSize - SPACER_LINE_HEIGHT_PIXELS);
    out += "px;\">&nbsp;</div>";
}

void StructuredRichText::addPendingVerticalMargin(
    const QString &spacing,
    double fontPixelSize,
    const StructuredRichText::Context &ctx,
    StructuredRichText::SiblingState &siblings) const
{
    /* CSS length syntax accepted by the margin-collapsing compatibility path */
    static const QRegularExpression CSS_LENGTH{
        R"(^[+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:px|%|em|rem|ex|rex|ch|rch|)"
        R"(cap|rcap|ic|ric|lh|rlh|vw|svw|lvw|dvw|vi|svi|lvi|dvi|vh|)"
        R"(svh|lvh|dvh|vb|svb|lvb|dvb|vmin|svmin|lvmin|dvmin|vmax|)"
        R"(svmax|lvmax|dvmax|pt|pc|in|cm|mm|q)$)",
        QRegularExpression::CaseInsensitiveOption
    };

    const QString value = spacing.trimmed();
    if (isZeroSpacing(value) || !CSS_LENGTH.match(value).hasMatch())
    {
        return;
    }

    const double pixelSize = cssFontSizeToPixels(
        value,
        ctx.screen,
        fontPixelSize,
        ctx.rootFontPixelSize
    );
    if (pixelSize > 0.0)
    {
        siblings.pendingPositiveMarginPixels = std::max(
            siblings.pendingPositiveMarginPixels,
            pixelSize
        );
    }
    else if (pixelSize < 0.0)
    {
        siblings.pendingNegativeMarginPixels = std::min(
            siblings.pendingNegativeMarginPixels,
            pixelSize
        );
    }
}

void StructuredRichText::mergePendingVerticalMargins(
    const StructuredRichText::SiblingState &source,
    StructuredRichText::SiblingState &destination) const
{
    destination.pendingPositiveMarginPixels = std::max(
        destination.pendingPositiveMarginPixels,
        source.pendingPositiveMarginPixels
    );
    destination.pendingNegativeMarginPixels = std::min(
        destination.pendingNegativeMarginPixels,
        source.pendingNegativeMarginPixels
    );
}

void StructuredRichText::flushPendingVerticalMargin(
    StructuredRichText::Context &ctx,
    QString &out) const
{
    if (ctx.siblings.isEmpty())
    {
        return;
    }

    SiblingState &siblings = ctx.siblings.back();
    const double margin =
        siblings.pendingPositiveMarginPixels +
        siblings.pendingNegativeMarginPixels;
    clearPendingVerticalMargins(siblings);
    addVerticalPixelSpacer(margin, out);
}

void StructuredRichText::clearPendingVerticalMargins(
    StructuredRichText::SiblingState &siblings) const
{
    siblings.pendingPositiveMarginPixels = 0.0;
    siblings.pendingNegativeMarginPixels = 0.0;
}

QString StructuredRichText::cssBoxSideValue(
    const QString &value, qsizetype side) const
{
    /* Side count and indexes used to expand CSS box shorthands */
    constexpr qsizetype BOX_SIDE_COUNT = 4;
    constexpr qsizetype TOP_SIDE = 0;
    constexpr qsizetype RIGHT_SIDE = 1;
    constexpr qsizetype BOTTOM_SIDE = 2;

    if (side < 0 || side >= BOX_SIDE_COUNT)
    {
        return "";
    }

    QStringList values;
    QString current;
    int parenthesisDepth = 0;
    for (const QChar ch : value.trimmed())
    {
        if (ch == '(')
        {
            ++parenthesisDepth;
        }
        else if (ch == ')')
        {
            --parenthesisDepth;
        }

        if (ch.isSpace() && parenthesisDepth == 0)
        {
            if (!current.isEmpty())
            {
                values.emplaceBack(current);
                current.clear();
            }
        }
        else
        {
            current += ch;
        }
    }
    if (!current.isEmpty())
    {
        values.emplaceBack(current);
    }

    switch (values.size())
    {
    case 1:
        return values[0];

    case 2:
        return side == TOP_SIDE || side == BOTTOM_SIDE ?
            values[0] :
            values[1];

    case 3:
        if (side == TOP_SIDE)
        {
            return values[0];
        }
        if (side == RIGHT_SIDE)
        {
            return values[1];
        }
        return side == BOTTOM_SIDE ? values[2] : values[1];

    case BOX_SIDE_COUNT:
        return values[side];

    default:
        return "";
    }
}

bool StructuredRichText::isZeroSpacing(const QString &spacing) const
{
    /* CSS numeric zero with an optional unit */
    static const QRegularExpression ZERO_SPACING{
        R"(^[+-]?(?:0+(?:\.0*)?|\.0+)(?:[a-z%]+)?$)",
        QRegularExpression::CaseInsensitiveOption
    };

    const QString value = spacing.trimmed();
    return value.isEmpty() || ZERO_SPACING.match(value).hasMatch();
}

void StructuredRichText::addStructuredContent(
    const QJsonValue &val,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    switch (val.type())
    {
    case QJsonValue::Type::String:
        addStructuredContentHelper(val.toString(), ctx, out);
        break;

    case QJsonValue::Type::Array:
        addStructuredContentHelper(val.toArray(), ctx, out);
        break;

    case QJsonValue::Type::Object:
        addStructuredContentHelper(val.toObject(), ctx, out);
        break;

    default:
        break;
    }
}

/* End Structured Content Parsing */
/* Begin Other Object Parsers */

void StructuredRichText::addImage(
    const QJsonObject &obj,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    /* Legacy glossary image object keys */
    constexpr const char *KEY_PATH = "path";
    constexpr const char *KEY_WIDTH = "width";
    constexpr const char *KEY_HEIGHT = "height";
    constexpr const char *KEY_TITLE = "title";
    constexpr const char *KEY_RENDERING = "imageRendering";
    constexpr const char *KEY_DESCRIPTION = "description";

    QString filename = escapeHtml(ctx.basepath + obj[KEY_PATH].toString());

    if (obj[KEY_TITLE].isString())
    {
        addAnchorStart(
            internalLinkHref("", obj[KEY_TITLE].toString(), "", "title"),
            ctx.textColor,
            out
        );
    }

    out += "<img src=\"";
    out += filename;
    out += '"';

    if (obj[KEY_WIDTH].isDouble())
    {
        out += " width=\"";
        out += QString::number(obj[KEY_WIDTH].toDouble(1.0));
        out += '"';
    }
    if (obj[KEY_HEIGHT].isDouble())
    {
        out += " height=\"";
        out += QString::number(obj[KEY_HEIGHT].toDouble(1.0));
        out += '"';
    }

    out += " style=\"display: inline-table; vertical-align: top;";
    if (obj[KEY_RENDERING].isString())
    {
        out += "image-rendering: ";
        out += obj[KEY_RENDERING].toString("auto");
        out += ";";
    }
    out += '"';

    out += '>';

    if (obj[KEY_TITLE].isString())
    {
        out += "</a>";
    }

    if (obj[KEY_DESCRIPTION].isString())
    {
        out += "<br>";
        out += escapeHtml(obj[KEY_DESCRIPTION].toString())
            .replace('\n', "<br>");
    }
}

void StructuredRichText::addText(const QJsonObject &obj, QString &out) const
{
    /* Legacy glossary text object key */
    constexpr const char *KEY_TEXT = "text";
    out += escapeHtml(obj[KEY_TEXT].toString()).replace('\n', "<br>");
}

/* End Other Object Parsers */
/* Begin Helpers */

bool StructuredRichText::containsStructuredContent(
    const QJsonArray &content) const
{
    return std::any_of(
        std::begin(content), std::end(content),
        [] (const QJsonValue &value) -> bool
        {
            /* Top-level discriminator for structured glossary entries */
            constexpr const char *KEY_TYPE = "type";
            constexpr const char *VALUE_STRUCTURED_CONTENT =
                "structured-content";

            return value.type() == QJsonValue::Object &&
                value.toObject()[KEY_TYPE] == VALUE_STRUCTURED_CONTENT;
        }
    );
}

QString StructuredRichText::escapeHtml(const QString &str) const
{
    return str.toHtmlEscaped();
}

void StructuredRichText::addRuby(
    const QJsonObject &obj,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    /* Structured-content key containing ruby base and reading children */
    constexpr const char *KEY_CONTENT = "content";

    QJsonArray base;
    QString reading;
    splitRubyContent(obj[KEY_CONTENT], base, reading);

    if (reading.isEmpty())
    {
        addStructuredChildren(base, ctx, out);
        return;
    }

    addAnchorStart(
        internalLinkHref(
            ctx.linkHref,
            reading,
            structuredContentText(base),
            "ruby"
        ),
        ctx.linkHref.isEmpty() ? ctx.textColor : "",
        out
    );

    const QString oldLinkHref = ctx.linkHref;
    const QString oldTitleTooltip = ctx.titleTooltip;
    ctx.linkHref.clear();
    ctx.titleTooltip.clear();
    addStructuredChildren(base, ctx, out);
    ctx.titleTooltip = oldTitleTooltip;
    ctx.linkHref = oldLinkHref;

    out += "</a>";
}

void StructuredRichText::splitRubyContent(
    const QJsonValue &val,
    QJsonArray &base,
    QString &reading) const
{
    /* Structured-content keys used to separate ruby base and reading nodes */
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *KEY_TAG = "tag";

    switch (val.type())
    {
    case QJsonValue::Type::Array:
    {
        const QJsonArray arr = val.toArray();
        for (const QJsonValue &child : arr)
        {
            if (child.isObject())
            {
                const QJsonObject obj = child.toObject();
                const QString tag = obj[KEY_TAG].toString();
                if (tag == "rt")
                {
                    reading += structuredContentText(obj[KEY_CONTENT]);
                    continue;
                }
                if (tag == "rp")
                {
                    continue;
                }
            }
            base.append(child);
        }
        break;
    }

    case QJsonValue::Type::Object:
    {
        const QJsonObject obj = val.toObject();
        const QString tag = obj[KEY_TAG].toString();
        if (tag == "rt")
        {
            reading += structuredContentText(obj[KEY_CONTENT]);
        }
        else if (tag != "rp")
        {
            base.append(val);
        }
        break;
    }

    default:
        base.append(val);
        break;
    }
}

QString StructuredRichText::structuredContentText(const QJsonValue &val) const
{
    /* Structured-content key recursively traversed for plain text */
    constexpr const char *KEY_CONTENT = "content";

    switch (val.type())
    {
    case QJsonValue::Type::String:
        return val.toString();

    case QJsonValue::Type::Array:
    {
        QString text;
        const QJsonArray arr = val.toArray();
        for (const QJsonValue &child : arr)
        {
            text += structuredContentText(child);
        }
        return text;
    }

    case QJsonValue::Type::Object:
        return structuredContentText(val.toObject()[KEY_CONTENT]);

    default:
        return "";
    }
}

bool StructuredRichText::containsRuby(const QJsonValue &val) const
{
    /* Structured-content keys used while searching for ruby descendants */
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *KEY_TAG = "tag";

    switch (val.type())
    {
    case QJsonValue::Type::Array:
    {
        const QJsonArray arr = val.toArray();
        return std::any_of(
            std::begin(arr), std::end(arr),
            [this] (const QJsonValue &child) -> bool
            {
                return containsRuby(child);
            }
        );
    }

    case QJsonValue::Type::Object:
    {
        const QJsonObject obj = val.toObject();
        if (obj[KEY_TAG].toString() == "ruby")
        {
            return true;
        }
        return containsRuby(obj[KEY_CONTENT]);
    }

    default:
        return false;
    }
}

bool StructuredRichText::anchorNeedsTooltipHandling(
    const QJsonObject &obj,
    const StructuredRichText::Context &ctx) const
{
    /* Structured-content keys that require custom anchor tooltip handling */
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *KEY_TITLE = "title";

    return containsRuby(obj[KEY_CONTENT]) ||
        obj[KEY_TITLE].isString() ||
        !ctx.titleTooltip.isEmpty();
}

QString StructuredRichText::internalLinkHref(
    const QString &target,
    const QString &tooltip,
    const QString &tooltipText,
    const QString &tooltipType) const
{
    QString out = "memento://glossary-link?";
    bool hasParam = false;

    if (!target.isEmpty())
    {
        out += "target=";
        out += QString::fromUtf8(QUrl::toPercentEncoding(target));
        hasParam = true;
    }

    if (!tooltip.isEmpty())
    {
        if (hasParam)
        {
            out += '&';
        }
        out += "tooltip=";
        out += QString::fromUtf8(QUrl::toPercentEncoding(tooltip));
        hasParam = true;
    }

    if (!tooltipText.isEmpty())
    {
        if (hasParam)
        {
            out += '&';
        }
        out += "tooltipText=";
        out += QString::fromUtf8(QUrl::toPercentEncoding(tooltipText));
        hasParam = true;
    }

    if (!tooltipType.isEmpty())
    {
        if (hasParam)
        {
            out += '&';
        }
        out += "tooltipType=";
        out += QString::fromUtf8(QUrl::toPercentEncoding(tooltipType));
    }

    return out;
}

void StructuredRichText::addAnchorStart(
    const QString &href,
    const QString &color,
    QString &out) const
{
    /* Quote substitution used to keep generated style attributes valid */
    constexpr const char QUOTE_SEARCH = '"';
    constexpr const char QUOTE_ESCAPE = '\'';

    out += "<a href=\"";
    out += escapeHtml(href);
    out += '"';
    if (!color.isEmpty())
    {
        out += " style=\"color: ";
        out += escapeHtml(QString(color).replace(QUOTE_SEARCH, QUOTE_ESCAPE));
        out += "; text-decoration: none;\"";
    }
    out += '>';
}

void StructuredRichText::addCssDeclaration(
    const QString &property,
    const QString &value,
    StructuredRichText::CssDeclarations &declarations) const
{
    const QString name = property.trimmed().toLower();
    QString cssValue = value.trimmed();

    if (name.isEmpty() || cssValue.isEmpty())
    {
        return;
    }

    cssValue.replace('"', '\'');

    if (name == "margin")
    {
        /* Maximum component count permitted by the CSS margin shorthand */
        constexpr qsizetype MAXIMUM_MARGIN_VALUES = 4;

        QStringList values;
        QString current;
        bool inQuote = false;
        QChar quote;
        int parenDepth = 0;

        for (const QChar ch : cssValue)
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
            else if (ch == '(')
            {
                ++parenDepth;
                current += ch;
            }
            else if (ch == ')')
            {
                --parenDepth;
                current += ch;
            }
            else if (ch.isSpace() && parenDepth == 0)
            {
                if (!current.isEmpty())
                {
                    values.emplaceBack(current);
                    current.clear();
                }
            }
            else
            {
                current += ch;
            }
        }
        if (!current.isEmpty())
        {
            values.emplaceBack(current);
        }
        if (values.isEmpty() ||
            values.size() > MAXIMUM_MARGIN_VALUES)
        {
            return;
        }

        const QString top = values[0];
        const QString right = values.size() > 1 ? values[1] : top;
        const QString bottom = values.size() > 2 ? values[2] : top;
        const QString left = values.size() > 3 ? values[3] : right;
        addCssDeclaration("margin-top", top, declarations);
        addCssDeclaration("margin-right", right, declarations);
        addCssDeclaration("margin-bottom", bottom, declarations);
        addCssDeclaration("margin-left", left, declarations);
        return;
    }

    if (name == "clip-path" &&
        cssValue.startsWith("circle(", Qt::CaseInsensitive))
    {
        declarations["border-radius"] = "50%";
        return;
    }

    /* Make sure the function is supported by Qt rich text before using it */
    QRegularExpressionMatchIterator functions =
        m_cssFunctionRegex.globalMatch(cssValue);
    while (functions.hasNext())
    {
        const QString functionName =
            functions.next().captured(1).toLower();
        if (!m_supportedCssFunctions.contains(functionName))
        {
            return;
        }
    }

    if (name == "text-decoration-line")
    {
        declarations["text-decoration"] = cssValue;
    }
    else if (name == "content")
    {
        /* Remove quotes before injecting text */
        if (cssValue.size() >= 2 &&
            ((cssValue.front() == '"' && cssValue.back() == '"') ||
             (cssValue.front() == '\'' && cssValue.back() == '\'')))
        {
            cssValue = cssValue.sliced(1, cssValue.size() - 2);
        }
        declarations[name] = cssValue;
    }
    else if (name == "background")
    {
        if (!cssValue.contains(' ') && !cssValue.contains("url("))
        {
            declarations["background-color"] = cssValue;
        }
    }
    else if (m_supportedCssProperties.contains(name))
    {
        declarations[name] = cssValue;
    }
}

void StructuredRichText::addResolvedCssDeclaration(
    const QString &property,
    const QString &value,
    StructuredRichText::Context &ctx,
    StructuredRichText::CssDeclarations &declarations) const
{
    const QString resolved = resolveCssValue(value, ctx);
    if (resolved.isEmpty())
    {
        return;
    }
    addCssDeclaration(property, resolved, declarations);
}

void StructuredRichText::addCssDeclarations(
    const StructuredRichText::CssDeclarations &declarations,
    QString &out) const
{
    for (const auto &[key, value] : declarations.asKeyValueRange())
    {
        /* These are stored as metadata, not handled directly by styles */
        if (key == "content" || key == "list-style-type")
        {
            continue;
        }

        out += key;
        out += ": ";
        out += value;
        out += ';';
    }
}

QString StructuredRichText::resolveCssValue(
    const QString &value,
    StructuredRichText::Context &ctx) const
{
    const auto cached = ctx.resolvedCssValues.constFind(value);
    if (cached != ctx.resolvedCssValues.cend())
    {
        return cached.value();
    }

    QString resolved = resolveCssVariables(value.trimmed(), ctx);
    if (resolved.isEmpty())
    {
        ctx.resolvedCssValues[value] = "";
        return "";
    }

    resolved = resolveCssCalculations(resolved);
    if (resolved.isEmpty())
    {
        ctx.resolvedCssValues[value] = "";
        return "";
    }

    if (resolved.startsWith("color-mix(", Qt::CaseInsensitive))
    {
        resolved = resolveColorMix(resolved);
    }
    else if (resolved.contains("gradient(", Qt::CaseInsensitive))
    {
        resolved = cssGradientFallback(resolved);
    }
    ctx.resolvedCssValues[value] = resolved;
    return resolved;
}

QString StructuredRichText::resolveCssVariables(
    QString value,
    const StructuredRichText::Context &ctx) const
{
    /* Recursion guard for nested or cyclic CSS variable substitution */
    constexpr int MAX_REPLACEMENTS = 32;

    /* Character count of the opening "var(" token */
    constexpr qsizetype CSS_VARIABLE_PREFIX_LENGTH = 4;

    for (int replacementCount = 0;
         replacementCount < MAX_REPLACEMENTS;
         ++replacementCount)
    {
        const qsizetype start = value.indexOf(
            "var(",
            0,
            Qt::CaseInsensitive
        );
        if (start < 0)
        {
            return value;
        }

        qsizetype end = -1;
        int depth = 1;
        for (qsizetype i = start + CSS_VARIABLE_PREFIX_LENGTH;
             i < value.size();
             ++i)
        {
            if (value[i] == '(')
            {
                ++depth;
            }
            else if (value[i] == ')' && --depth == 0)
            {
                end = i;
                break;
            }
        }
        if (end < 0)
        {
            return "";
        }

        const QStringList arguments = splitCssArguments(
            value.sliced(
                start + CSS_VARIABLE_PREFIX_LENGTH,
                end - start - CSS_VARIABLE_PREFIX_LENGTH
            )
        );
        if (arguments.isEmpty())
        {
            return "";
        }

        const QString name = arguments[0].trimmed().toLower();
        QString fallback;
        if (arguments.size() > 1)
        {
            fallback = arguments.sliced(1).join(',');
        }

        QString replacement;
        if (name == "--text-color" || name == "--fg")
        {
            replacement = ctx.glossaryTextColor.isEmpty() ?
                fallback :
                ctx.glossaryTextColor;
        }
        else if (name == "--font-size-no-units")
        {
            replacement = formatPixelSize(ctx.rootFontPixelSize);
        }
        else if (name == "--background-color" || name == "--canvas")
        {
            replacement = ctx.glossaryBackgroundColor.isEmpty() ?
                fallback :
                ctx.glossaryBackgroundColor;
        }
        else
        {
            replacement = fallback;
        }

        if (replacement.isEmpty())
        {
            return "";
        }
        replacement = resolveCssVariables(replacement, ctx);
        if (replacement.isEmpty())
        {
            return "";
        }
        value.replace(start, end - start + 1, replacement);
    }

    return "";
}

QString StructuredRichText::resolveCssCalculations(QString value) const
{
    /* Recursion guard for nested CSS calc() substitutions */
    constexpr int MAX_REPLACEMENTS = 16;

    /* Character count of the opening "calc(" token */
    constexpr qsizetype CSS_CALC_PREFIX_LENGTH = 5;

    for (int replacementCount = 0;
         replacementCount < MAX_REPLACEMENTS;
         ++replacementCount)
    {
        const qsizetype start = value.indexOf(
            "calc(",
            0,
            Qt::CaseInsensitive
        );
        if (start < 0)
        {
            return value;
        }

        qsizetype end = -1;
        int depth = 1;
        for (qsizetype i = start + CSS_CALC_PREFIX_LENGTH;
             i < value.size();
             ++i)
        {
            if (value[i] == '(')
            {
                ++depth;
            }
            else if (value[i] == ')' && --depth == 0)
            {
                end = i;
                break;
            }
        }
        if (end < 0)
        {
            return "";
        }

        const QString replacement = resolveCssCalc(
            value.sliced(start, end - start + 1)
        );
        if (replacement.isEmpty())
        {
            return "";
        }
        value.replace(start, end - start + 1, replacement);
    }

    return "";
}

QString StructuredRichText::resolveColorMix(const QString &value) const
{
    /* Number of colors supported by the compatibility color-mix parser */
    constexpr qsizetype COLOR_COUNT = 2;

    /* Character count of the opening "color-mix(" token */
    constexpr qsizetype COLOR_MIX_PREFIX_LENGTH = 10;

    /* Character count of a CSS function's closing parenthesis */
    constexpr qsizetype CSS_FUNCTION_SUFFIX_LENGTH = 1;

    /* Equal weighting used when neither mixed color specifies a percentage */
    constexpr double DEFAULT_COLOR_PERCENTAGE = 50.0;

    /* Percentage total used to infer one omitted color weight */
    constexpr double TOTAL_COLOR_PERCENTAGE = 100.0;

    /* Sentinel identifying a color weight omitted from color-mix() */
    constexpr double UNSPECIFIED_PERCENTAGE = -1.0;

    if (!value.endsWith(')'))
    {
        return "";
    }

    QStringList arguments = splitCssArguments(
        value.sliced(
            COLOR_MIX_PREFIX_LENGTH,
            value.size() -
                COLOR_MIX_PREFIX_LENGTH -
                CSS_FUNCTION_SUFFIX_LENGTH
        )
    );
    if (!arguments.isEmpty() &&
        arguments.front().startsWith("in ", Qt::CaseInsensitive))
    {
        arguments.removeFirst();
    }
    if (arguments.size() != COLOR_COUNT)
    {
        return "";
    }

    std::array<QColor, COLOR_COUNT> colors;
    std::array<double, COLOR_COUNT> percentages = {
        UNSPECIFIED_PERCENTAGE,
        UNSPECIFIED_PERCENTAGE
    };
    /* Trailing percentage attached to an individual color-mix argument */
    static const QRegularExpression PERCENTAGE{
        R"(\s+([0-9]+(?:\.[0-9]+)?)%\s*$)"
    };

    for (qsizetype i = 0; i < COLOR_COUNT; ++i)
    {
        QString colorValue = arguments[i].trimmed();
        const QRegularExpressionMatch match = PERCENTAGE.match(colorValue);
        if (match.hasMatch())
        {
            percentages[i] = match.captured(1).toDouble();
            colorValue = colorValue.first(match.capturedStart()).trimmed();
        }
        colors[i] = QColor::fromString(colorValue);
        if (!colors[i].isValid())
        {
            return "";
        }
    }

    if (percentages[0] < 0.0 && percentages[1] < 0.0)
    {
        percentages[0] = DEFAULT_COLOR_PERCENTAGE;
        percentages[1] = DEFAULT_COLOR_PERCENTAGE;
    }
    else if (percentages[0] < 0.0)
    {
        percentages[0] = TOTAL_COLOR_PERCENTAGE - percentages[1];
    }
    else if (percentages[1] < 0.0)
    {
        percentages[1] = TOTAL_COLOR_PERCENTAGE - percentages[0];
    }

    const double total = percentages[0] + percentages[1];
    if (total <= 0.0)
    {
        return "";
    }
    const double firstWeight = percentages[0] / total;
    const double secondWeight = percentages[1] / total;
    const double alpha =
        colors[0].alphaF() * firstWeight +
        colors[1].alphaF() * secondWeight;
    if (alpha <= 0.0)
    {
        return QColor(Qt::transparent).name(QColor::HexArgb);
    }

    return QColor::fromRgbF(
        (
            colors[0].redF() * colors[0].alphaF() * firstWeight +
            colors[1].redF() * colors[1].alphaF() * secondWeight
        ) / alpha,
        (
            colors[0].greenF() * colors[0].alphaF() * firstWeight +
            colors[1].greenF() * colors[1].alphaF() * secondWeight
        ) / alpha,
        (
            colors[0].blueF() * colors[0].alphaF() * firstWeight +
            colors[1].blueF() * colors[1].alphaF() * secondWeight
        ) / alpha,
        alpha
    ).name(QColor::HexArgb);
}

QString StructuredRichText::resolveCssCalc(const QString &value) const
{
    /* Simple dimension-by-number division supported inside calc() */
    static const QRegularExpression DIVISION{
        R"(^calc\(\s*([+-]?(?:\d+(?:\.\d*)?|\.\d+))([a-z%]+)\s*/\s*)"
        R"(([+-]?(?:\d+(?:\.\d*)?|\.\d+))\s*\)$)",
        QRegularExpression::CaseInsensitiveOption
    };

    const QRegularExpressionMatch match = DIVISION.match(value);
    if (!match.hasMatch())
    {
        return "";
    }

    const double denominator = match.captured(3).toDouble();
    if (denominator == 0.0)
    {
        return "";
    }
    return formatPixelSize(
        match.captured(1).toDouble() / denominator
    ) + match.captured(2);
}

QString StructuredRichText::cssGradientFallback(const QString &value) const
{
    const qsizetype open = value.indexOf('(');
    if (open < 0 || !value.endsWith(')'))
    {
        return "";
    }

    const QStringList arguments = splitCssArguments(
        value.sliced(open + 1, value.size() - open - 2)
    );
    if (arguments.isEmpty())
    {
        return "";
    }

    QString colorValue = arguments.front().trimmed();
    /* Trailing percentage position attached to a gradient color stop */
    static const QRegularExpression COLOR_STOP{
        R"(\s+[0-9]+(?:\.[0-9]+)?%\s*$)"
    };
    const QRegularExpressionMatch match = COLOR_STOP.match(colorValue);
    if (match.hasMatch())
    {
        colorValue = colorValue.first(match.capturedStart()).trimmed();
    }

    const QColor color = QColor::fromString(colorValue);
    return color.isValid() ? color.name(QColor::HexArgb) : "";
}

QStringList StructuredRichText::splitCssArguments(
    const QString &arguments) const
{
    QStringList result;
    QString current;
    bool inQuote = false;
    QChar quote;
    int parenDepth = 0;

    for (const QChar ch : arguments)
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
        else if (ch == '(')
        {
            ++parenDepth;
            current += ch;
        }
        else if (ch == ')')
        {
            --parenDepth;
            current += ch;
        }
        else if (ch == ',' && parenDepth == 0)
        {
            result.emplaceBack(current.trimmed());
            current.clear();
        }
        else
        {
            current += ch;
        }
    }
    result.emplaceBack(current.trimmed());
    return result;
}

QString StructuredRichText::normalizeListMarker(QString marker) const
{
    marker = marker.trimmed();
    marker.replace('"', '\'');
    if (marker.size() >= 2 &&
        ((marker.front() == '\'' && marker.back() == '\'') ||
         (marker.front() == '"' && marker.back() == '"')))
    {
        marker = marker.sliced(1, marker.size() - 2);
    }
    return marker;
}

QString StructuredRichText::defaultListMarker(const QString &tag) const
{
    if (tag == "ol")
    {
        return "1";
    }
    if (tag == "ul")
    {
        return "disc";
    }
    return "";
}

QString StructuredRichText::listMarker(
    const StructuredList &list, const QString &marker) const
{
    QString normalized = normalizeListMarker(marker);
    if (normalized.isEmpty())
    {
        normalized = list.marker;
    }
    if (normalized == "none")
    {
        return "";
    }
    if (!isBuiltInListMarker(normalized))
    {
        return normalized;
    }
    if (normalized == "disc")
    {
        return QString{QChar(0x2022)};
    }
    if (normalized == "circle")
    {
        return QString{QChar(0x25E6)};
    }
    if (normalized == "square")
    {
        return QString{QChar(0x25AA)};
    }
    if (normalized == "1")
    {
        return QString::number(list.item) + '.';
    }
    if (normalized == "a" || normalized == "A")
    {
        /* Number of symbols in the Latin alphabetic list-marker sequence */
        constexpr int LETTERS_IN_ALPHABET = 26;

        QString alpha;
        qsizetype value = list.item;
        while (value > 0)
        {
            --value;
            const QChar ch{
                'a' + static_cast<char>(value % LETTERS_IN_ALPHABET)
            };
            alpha.prepend(normalized == "A" ? ch.toUpper() : ch);
            value /= LETTERS_IN_ALPHABET;
        }
        return alpha + '.';
    }
    return list.marker;
}

bool StructuredRichText::isBuiltInListMarker(const QString &marker) const
{
    const QString normalized = normalizeListMarker(marker);
    return normalized.isEmpty() ||
        normalized == "disc" ||
        normalized == "circle" ||
        normalized == "square" ||
        normalized == "1" ||
        normalized == "a" ||
        normalized == "A";
}

void StructuredRichText::addMatchingCssRules(
    StructuredRichText::Context &ctx,
    StructuredRichText::CssDeclarations &declarations,
    QString *beforeContent) const
{
    if (ctx.dictionaryStyles == nullptr || ctx.elements.isEmpty())
    {
        return;
    }

    const ParsedStylesheet &stylesheet =
        ctx.dictionaryStyles->parsedStylesheet();
    const QList<qsizetype> &candidateIndexes =
        ctx.dictionaryStyles->candidateRuleIndexes(
            ctx.selectorElements[ctx.elements.back()].tag
        );
    for (const qsizetype index : candidateIndexes)
    {
        const CssRule &rule = stylesheet.rules[index];
        if (!cssRuleMatches(rule, ctx))
        {
            continue;
        }
        if (rule.before)
        {
            if (beforeContent != nullptr)
            {
                for (const CssDeclaration &declaration : rule.declarations)
                {
                    if (declaration.property == "content")
                    {
                        *beforeContent = declaration.value;
                    }
                }
            }
            continue;
        }

        for (const CssDeclaration &declaration : rule.declarations)
        {
            QString value = resolveCssValue(declaration.value, ctx);
            if (value.isEmpty())
            {
                continue;
            }
            if (declaration.property == "font-size")
            {
                const double pixelSize = cssFontSizeToPixels(
                    value,
                    ctx.screen,
                    ctx.parentFontPixelSize,
                    ctx.rootFontPixelSize
                );
                if (pixelSize >= 0.0)
                {
                    value = formatPixelSize(pixelSize) + "px";
                }
            }
            addCssDeclaration(declaration.property, value, declarations);
        }
    }
}

bool StructuredRichText::cssRuleMatches(
    const CssRule &rule,
    const StructuredRichText::Context &ctx) const
{
    if (rule.selector.isEmpty() || ctx.elements.isEmpty())
    {
        return false;
    }

    const qsizetype stackIndex = ctx.elements.size() - 1;
    return cssRuleMatchesAt(
        rule,
        ctx,
        rule.selector.size() - 1,
        stackIndex,
        ctx.elements[stackIndex]
    );
}

bool StructuredRichText::cssRuleMatchesAt(
    const CssRule &rule,
    const StructuredRichText::Context &ctx,
    qsizetype partIndex,
    qsizetype stackIndex,
    qsizetype selectorElementIndex) const
{
    const StructuredElement &element =
        ctx.selectorElements[selectorElementIndex];
    if (partIndex < 0 ||
        !cssSelectorPartMatches(rule.selector[partIndex], element))
    {
        return false;
    }
    if (partIndex == 0)
    {
        return true;
    }

    switch (rule.selector[partIndex].combinator)
    {
    case DictionaryStyles::CssCombinator::Child:
        if (stackIndex <= 0)
        {
            return false;
        }
        return cssRuleMatchesAt(
            rule,
            ctx,
            partIndex - 1,
            stackIndex - 1,
            ctx.elements[stackIndex - 1]
        );

    case DictionaryStyles::CssCombinator::AdjacentSibling:
        if (element.previousSibling < 0)
        {
            return false;
        }
        return cssRuleMatchesAt(
            rule,
            ctx,
            partIndex - 1,
            stackIndex,
            element.previousSibling
        );

    case DictionaryStyles::CssCombinator::Descendant:
        for (qsizetype i = stackIndex - 1; i >= 0; --i)
        {
            if (cssRuleMatchesAt(
                    rule,
                    ctx,
                    partIndex - 1,
                    i,
                    ctx.elements[i]
                ))
            {
                return true;
            }
        }
        return false;
    }

    return false;
}

bool StructuredRichText::cssSelectorPartMatches(
    const CssSelectorPart &part,
    const StructuredElement &element) const
{
    if (!part.tag.isEmpty() && part.tag != element.tag)
    {
        return false;
    }
    for (const QString &className : part.classNames)
    {
        if (!element.classes.contains(className))
        {
            return false;
        }
    }
    for (const auto &[key, value] : part.attributes.asKeyValueRange())
    {
        if (element.attributes.value(key) != value)
        {
            return false;
        }
    }
    for (const QString &attribute : part.presentAttributes)
    {
        if (!element.attributes.contains(attribute))
        {
            return false;
        }
    }
    if (part.firstChild && element.previousSibling >= 0)
    {
        return false;
    }
    return true;
}

qsizetype StructuredRichText::structuredElement(
    const QJsonObject &obj,
    StructuredRichText::Context &ctx) const
{
    /* Structured-content keys recorded for CSS selector matching */
    constexpr const char *KEY_DATA = "data";
    constexpr const char *KEY_HREF = "href";
    constexpr const char *KEY_LANG = "lang";
    constexpr const char *KEY_TAG = "tag";
    constexpr const char *KEY_TITLE = "title";

    StructuredElement element;
    element.tag = obj[KEY_TAG].toString().toLower();
    element.classes.insert("gloss-sc-" + element.tag);
    if (element.tag == "img")
    {
        element.classes.insert("gloss-image");
        element.classes.insert("gloss-image-container");
        element.classes.insert("gloss-image-link");
    }

    if (obj[KEY_DATA].isObject())
    {
        const QJsonObject data = obj[KEY_DATA].toObject();
        for (const QString &key : data.keys())
        {
            if (data[key].isString())
            {
                element.attributes[structuredDataAttributeName(key)] =
                    data[key].toString();
            }
        }
    }

    /* Standard attributes copied into the selector-node attribute map */
    constexpr std::array<const char *, 3> STANDARD_ATTRIBUTES = {
        KEY_HREF,
        KEY_LANG,
        KEY_TITLE
    };
    for (const char *key : STANDARD_ATTRIBUTES)
    {
        if (obj[key].isString())
        {
            element.attributes[key] = obj[key].toString();
        }
    }

    if (!ctx.siblings.isEmpty())
    {
        SiblingState &siblings = ctx.siblings.back();
        element.previousSibling = siblings.previousElement;
    }

    const qsizetype index = ctx.selectorElements.size();
    ctx.selectorElements.emplaceBack(std::move(element));
    if (!ctx.siblings.isEmpty())
    {
        ctx.siblings.back().previousElement = index;
    }
    return index;
}

QString StructuredRichText::structuredDataAttributeName(
    const QString &key) const
{
    QString out = "data-sc-";
    for (const QChar ch : key)
    {
        if (ch.isUpper())
        {
            out += '-';
            out += ch.toLower();
        }
        else
        {
            out += ch;
        }
    }
    return out;
}

bool StructuredRichText::isSupportedStructuredTag(const QString &tag) const
{
    return m_supportedTags.contains(tag);
}

double StructuredRichText::cssFontSizeToPixels(
    const QString &size,
    const QScreen *screen,
    const QFont &font) const
{
    const double fontSize = fontPixelSize(font, screen);
    return cssFontSizeToPixels(size, screen, fontSize, fontSize);
}


double StructuredRichText::cssFontSizeToPixels(
    const QString &size,
    const QScreen *screen,
    double parentFontPixelSize,
    double rootFontPixelSize) const
{
    const QString normalized = size.trimmed().toLower();
    if (normalized.isEmpty())
    {
        return -1.0;
    }

    if (normalized == "inherit" || normalized == "unset" ||
        normalized == "revert" || normalized == "revert-layer")
    {
        return parentFontPixelSize;
    }
    if (normalized == "initial" || normalized == "medium")
    {
        return rootFontPixelSize;
    }
    if (normalized == "xx-small")
    {
        return rootFontPixelSize * 3.0 / 5.0;
    }
    if (normalized == "x-small")
    {
        return rootFontPixelSize * 3.0 / 4.0;
    }
    if (normalized == "small")
    {
        return rootFontPixelSize * 8.0 / 9.0;
    }
    if (normalized == "large")
    {
        return rootFontPixelSize * 6.0 / 5.0;
    }
    if (normalized == "x-large")
    {
        return rootFontPixelSize * 3.0 / 2.0;
    }
    if (normalized == "xx-large")
    {
        return rootFontPixelSize * 2.0;
    }
    if (normalized == "xxx-large")
    {
        return rootFontPixelSize * 3.0;
    }
    if (normalized == "smaller")
    {
        return parentFontPixelSize / 1.2;
    }
    if (normalized == "larger")
    {
        return parentFontPixelSize * 1.2;
    }

    qsizetype index = 0;
    bool ok = false;
    normalized.toDouble(&ok);
    if (ok)
    {
        index = normalized.size();
    }
    else
    {
        while (index < normalized.size())
        {
            const QChar ch = normalized[index];
            if (!ch.isDigit() && ch != '-' && ch != '+' && ch != '.')
            {
                break;
            }
            ++index;
        }
        if (index == 0)
        {
            return -1.0;
        }
    }

    const QString number = normalized.first(index);
    ok = false;
    const double numericValue = number.toDouble(&ok);
    if (!ok || !std::isfinite(numericValue))
    {
        return -1.0;
    }

    const QString unit = normalized.sliced(index).trimmed();
    if (unit.isEmpty())
    {
        return numericValue == 0.0 ? 0.0 : -1.0;
    }
    if (unit == "px")
    {
        return numericValue;
    }
    if (unit == "%")
    {
        return parentFontPixelSize * numericValue / 100.0;
    }
    if (unit == "em")
    {
        return parentFontPixelSize * numericValue;
    }
    if (unit == "rem")
    {
        return rootFontPixelSize * numericValue;
    }
    if (unit == "ex")
    {
        return parentFontPixelSize * numericValue / 2.0;
    }
    if (unit == "rex")
    {
        return rootFontPixelSize * numericValue / 2.0;
    }
    if (unit == "ch")
    {
        return parentFontPixelSize * numericValue / 2.0;
    }
    if (unit == "rch")
    {
        return rootFontPixelSize * numericValue / 2.0;
    }
    if (unit == "cap")
    {
        return parentFontPixelSize * numericValue * 0.7;
    }
    if (unit == "rcap")
    {
        return rootFontPixelSize * numericValue * 0.7;
    }
    if (unit == "ic")
    {
        return parentFontPixelSize * numericValue;
    }
    if (unit == "ric")
    {
        return rootFontPixelSize * numericValue;
    }
    if (unit == "lh")
    {
        return parentFontPixelSize * numericValue * 1.2;
    }
    if (unit == "rlh")
    {
        return rootFontPixelSize * numericValue * 1.2;
    }

    if (screen != nullptr)
    {
        const QSize screenSize = screen->availableGeometry().size();
        if (unit == "vw" || unit == "svw" || unit == "lvw" || unit == "dvw" ||
            unit == "vi" || unit == "svi" || unit == "lvi" || unit == "dvi")
        {
            return screenSize.width() * numericValue / 100.0;
        }
        if (unit == "vh" || unit == "svh" || unit == "lvh" || unit == "dvh" ||
            unit == "vb" || unit == "svb" || unit == "lvb" || unit == "dvb")
        {
            return screenSize.height() * numericValue / 100.0;
        }
        if (unit == "vmin" || unit == "svmin" || unit == "lvmin" ||
            unit == "dvmin")
        {
            return std::min(screenSize.width(), screenSize.height()) *
                numericValue / 100.0;
        }
        if (unit == "vmax" || unit == "svmax" || unit == "lvmax" ||
            unit == "dvmax")
        {
            return std::max(screenSize.width(), screenSize.height()) *
                numericValue / 100.0;
        }
    }

    const double dpi = screenDpi(screen);
    if (unit == "pt")
    {
        return numericValue * dpi / 72.0;
    }
    if (unit == "pc")
    {
        return numericValue * dpi / 6.0;
    }
    if (unit == "in")
    {
        return numericValue * dpi;
    }
    if (unit == "cm")
    {
        return numericValue * dpi / 2.54;
    }
    if (unit == "mm")
    {
        return numericValue * dpi / 25.4;
    }
    if (unit == "q")
    {
        return numericValue * dpi / 101.6;
    }

    return -1.0;
}

double StructuredRichText::fontPixelSize(
    const QFont &font, const QScreen *screen) const
{
    /* Typographic points per physical inch */
    constexpr double POINTS_IN_INCH = 72.0;

    /* Browser-compatible font size used when the QFont has no valid size */
    constexpr double DEFAULT_PIXEL_SIZE = 16.0;

    if (font.pixelSize() > 0)
    {
        return font.pixelSize();
    }
    if (font.pointSizeF() > 0)
    {
        return font.pointSizeF() * screenDpi(screen) / POINTS_IN_INCH;
    }
    return DEFAULT_PIXEL_SIZE;
}

double StructuredRichText::screenDpi(const QScreen *screen) const
{
    /* CSS reference pixel density used when no screen DPI is available */
    constexpr double DEFAULT_SCREEN_DPI = 96.0;

    if (screen == nullptr)
    {
        screen = QGuiApplication::primaryScreen();
        if (screen == nullptr)
        {
            return DEFAULT_SCREEN_DPI;
        }
    }
    const double dpi = screen->logicalDotsPerInch();
    return dpi > 0.0 ? dpi : DEFAULT_SCREEN_DPI;
}

QString StructuredRichText::formatPixelSize(double size) const
{
    QString formatted = QString::number(size, 'f', 3);
    while (formatted.endsWith('0'))
    {
        formatted.chop(1);
    }
    if (formatted.endsWith('.'))
    {
        formatted.chop(1);
    }
    return formatted;
}

/* End Helpers */
