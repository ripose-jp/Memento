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
#include <cmath>

#include <QJsonObject>
#include <QQuickWindow>

#include "util/utils.h"

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
    const QFont &font) const

{
    if (info == nullptr)
    {
        return "";
    }

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
    ctx.rootFontPixelSize = fontPixelSize(font, ctx.screen);
    ctx.parentFontPixelSize = ctx.rootFontPixelSize;
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

    QString glossary = "<html><head></head><body>";

    if (shouldUseBullets)
    {
        glossary += "<ul>";
    }

    for (qsizetype i = 0; i < content.size(); ++i)
    {
        const QJsonValue &val = content[i];

        if (shouldUseBullets)
        {
            glossary += "<li>";
        }

        switch (val.type())
        {
            case QJsonValue::Type::String:
                glossary += escapeHtml(val.toString()).replace(
                    '\n',
                    "<br>"
                );
                break;

            case QJsonValue::Type::Object:
            {
                QJsonObject obj = val.toObject();
                if (obj[KEY_TYPE] == VALUE_TYPE_STRUCTURED_CONTENT)
                {
                    addStructuredContent(obj[KEY_CONTENT], ctx, glossary);
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
            glossary += "</li>";
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

    if (shouldUseBullets)
    {
        glossary += "</ul>";
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

double StructuredRichText::addStructuredStyle(
    const QJsonObject &obj,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    constexpr const char *KEY_FONT_STYLE = "fontStyle";
    constexpr const char *KEY_FONT_WEIGHT = "fontWeight";
    constexpr const char *KEY_FONT_SIZE = "fontSize";
    constexpr const char *KEY_COLOR = "color";
    constexpr const char *KEY_BACKGROUND = "background";
    constexpr const char *KEY_BACKGROUND_COLOR = "backgroundColor";
    constexpr const char *KEY_TEXT_DECORATION_LINE = "textDecorationLine";
    constexpr const char *KEY_TEXT_DECORATION_STYLE = "textDecorationStyle";
    constexpr const char *KEY_TEXT_DECORATION_COLOR = "textDecorationColor";
    constexpr const char *KEY_BORDER_COLOR = "borderColor";
    constexpr const char *KEY_BORDER_STYLE = "borderStyle";
    constexpr const char *KEY_BORDER_RADIUS = "borderRadius";
    constexpr const char *KEY_BORDER_WIDTH = "borderWidth";
    constexpr const char *KEY_CLIP_PATH = "clipPath";
    constexpr const char *KEY_VERTICAL_ALIGN = "verticalAlign";
    constexpr const char *KEY_TEXT_ALIGN = "textAlign";
    constexpr const char *KEY_TEXT_EMPHASIS = "textEmphasis";
    constexpr const char *KEY_TEXT_SHADOW = "textShadow";
    constexpr const char *KEY_MARGIN = "margin";
    constexpr const char *KEY_MARGIN_TOP = "marginTop";
    constexpr const char *KEY_MARGIN_LEFT = "marginLeft";
    constexpr const char *KEY_MARGIN_RIGHT = "marginRight";
    constexpr const char *KEY_MARGIN_BOTTOM = "marginBottom";
    constexpr const char *KEY_PADDING = "padding";
    constexpr const char *KEY_PADDING_TOP = "paddingTop";
    constexpr const char *KEY_PADDING_LEFT = "paddingLeft";
    constexpr const char *KEY_PADDING_RIGHT = "paddingRight";
    constexpr const char *KEY_PADDING_BOTTOM = "paddingBottom";
    constexpr const char *KEY_WORD_BREAK = "wordBreak";
    constexpr const char *KEY_WHITE_SPACE = "whiteSpace";
    constexpr const char *KEY_CURSOR = "cursor";
    constexpr const char *KEY_LIST_STYLE_TYPE = "listStyleType";

    constexpr const char QUOTE_SEARCH = '"';
    constexpr const char QUOTE_ESCAPE = '\'';

    double currentFontPixelSize = ctx.parentFontPixelSize;

    if (obj[KEY_FONT_STYLE].isString())
    {
        out += "font-style: ";
        out += obj[KEY_FONT_STYLE].toString("normal")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_FONT_WEIGHT].isString())
    {
        out += "font-weight: ";
        out += obj[KEY_FONT_WEIGHT].toString("normal")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_FONT_SIZE].isString())
    {
        const QString fontSize = obj[KEY_FONT_SIZE].toString("medium")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        const double fontPixelSize = cssFontSizeToPixels(
            fontSize,
            ctx.screen,
            ctx.parentFontPixelSize,
            ctx.rootFontPixelSize
        );

        out += "font-size: ";
        if (fontPixelSize >= 0)
        {
            currentFontPixelSize = fontPixelSize;
            out += formatPixelSize(fontPixelSize);
            out += "px";
        }
        else
        {
            out += fontSize;
        }
        out += ';';
    }

    if (obj[KEY_COLOR].isString())
    {
        out += "color: ";
        out += obj[KEY_COLOR].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_BACKGROUND].isString())
    {
        out += "background: ";
        out += obj[KEY_BACKGROUND].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_BACKGROUND_COLOR].isString())
    {
        out += "background-color: ";
        out += obj[KEY_BACKGROUND_COLOR].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_TEXT_DECORATION_LINE].isArray())
    {
        out += "text-decoration-line: ";
        for (const QJsonValue &val : obj[KEY_TEXT_DECORATION_LINE].toArray())
        {
            out += val.toString("none")
                .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
            out += ' ';
        }
        out += ';';
    }
    else if (obj[KEY_TEXT_DECORATION_LINE].isString())
    {
        out += "text-decoration-line: ";
        out += obj[KEY_TEXT_DECORATION_LINE].toString("none")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_TEXT_DECORATION_STYLE].isString())
    {
        out += "text-decoration-style: ";
        out += obj[KEY_TEXT_DECORATION_STYLE].toString("solid")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_TEXT_DECORATION_COLOR].isString())
    {
        out += "text-decoration-color: ";
        out += obj[KEY_TEXT_DECORATION_COLOR].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_BORDER_COLOR].isString())
    {
        out += "border-color: ";
        out += obj[KEY_BORDER_COLOR].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_BORDER_STYLE].isString())
    {
        out += "border-style: ";
        out += obj[KEY_BORDER_STYLE].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_BORDER_RADIUS].isString())
    {
        out += "border-radius: ";
        out += obj[KEY_BORDER_RADIUS].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_BORDER_WIDTH].isString())
    {
        out += "border-width: ";
        out += obj[KEY_BORDER_WIDTH].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_CLIP_PATH].isString())
    {
        out += "clip-path: ";
        out += obj[KEY_CLIP_PATH].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_VERTICAL_ALIGN].isString())
    {
        out += "vertical-align: ";
        out += obj[KEY_VERTICAL_ALIGN].toString("baseline")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_TEXT_ALIGN].isString())
    {
        out += "text-align: ";
        out += obj[KEY_TEXT_ALIGN].toString("start")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_TEXT_EMPHASIS].isString())
    {
        out += "text-emphasis: ";
        out += obj[KEY_TEXT_EMPHASIS].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_TEXT_SHADOW].isString())
    {
        out += "text-shadow: ";
        out += obj[KEY_TEXT_SHADOW].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_MARGIN].isString())
    {
        out += "margin: ";
        out += obj[KEY_MARGIN].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_MARGIN_TOP].isString())
    {
        out += "margin-top: ";
        out += obj[KEY_MARGIN_TOP].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }
    else if (obj[KEY_MARGIN_TOP].isDouble())
    {
        out += "margin-top: ";
        out += QString::number(obj[KEY_MARGIN_TOP].toDouble(0));
        out += "px;";
    }

    if (obj[KEY_MARGIN_LEFT].isString())
    {
        out += "margin-left: ";
        out += obj[KEY_MARGIN_LEFT].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }
    else if (obj[KEY_MARGIN_LEFT].isDouble())
    {
        out += "margin-left: ";
        out += QString::number(obj[KEY_MARGIN_LEFT].toDouble(0));
        out += "px;";
    }

    if (obj[KEY_MARGIN_RIGHT].isString())
    {
        out += "margin-right: ";
        out += obj[KEY_MARGIN_RIGHT].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }
    else if (obj[KEY_MARGIN_RIGHT].isDouble())
    {
        out += "margin-right: ";
        out += QString::number(obj[KEY_MARGIN_RIGHT].toDouble(0));
        out += "px;";
    }

    if (obj[KEY_MARGIN_BOTTOM].isString())
    {
        out += "margin-bottom: ";
        out += obj[KEY_MARGIN_BOTTOM].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }
    else if (obj[KEY_MARGIN_BOTTOM].isDouble())
    {
        out += "margin-bottom: ";
        out += QString::number(obj[KEY_MARGIN_BOTTOM].toDouble(0));
        out += "px;";
    }

    if (obj[KEY_PADDING].isString())
    {
        out += "padding: ";
        out += obj[KEY_PADDING].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_PADDING_TOP].isString())
    {
        out += "padding-top: ";
        out += obj[KEY_PADDING_TOP].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_PADDING_LEFT].isString())
    {
        out += "padding-left: ";
        out += obj[KEY_PADDING_LEFT].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_PADDING_RIGHT].isString())
    {
        out += "padding-right: ";
        out += obj[KEY_PADDING_RIGHT].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_PADDING_BOTTOM].isString())
    {
        out += "padding-bottom: ";
        out += obj[KEY_PADDING_BOTTOM].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_WORD_BREAK].isString())
    {
        out += "word-break: ";
        out += obj[KEY_WORD_BREAK].toString("normal")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_WHITE_SPACE].isString())
    {
        out += "white-space: ";
        out += obj[KEY_WHITE_SPACE].toString()
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_CURSOR].isString())
    {
        out += "cursor: ";
        out += obj[KEY_CURSOR].toString("auto")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    if (obj[KEY_LIST_STYLE_TYPE].isString())
    {
        out += "list-style-type: ";
        out += obj[KEY_LIST_STYLE_TYPE].toString("disc")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
        out += ';';
    }

    return currentFontPixelSize;
}

void StructuredRichText::addStructuredContentHelper(
    const QString &str, QString &out) const
{
    out += escapeHtml(str).replace('\n', "<br>");
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
    constexpr const char *KEY_ALT = "alt";
    constexpr const char *KEY_APPEARANCE = "appearance";
    constexpr const char *KEY_BACKGROUND = "background";
    constexpr const char *KEY_BORDER = "border";
    constexpr const char *KEY_BORDER_RADIUS = "borderRadius";
    constexpr const char *KEY_COLSPAN = "colSpan";
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *KEY_DATA = "data";
    constexpr const char *KEY_DESCRIPTION = "description";
    constexpr const char *KEY_HEIGHT = "height";
    constexpr const char *KEY_HREF = "href";
    constexpr const char *KEY_LANG = "lang";
    constexpr const char *KEY_OPEN = "open";
    constexpr const char *KEY_PATH = "path";
    constexpr const char *KEY_PIXELATED = "pixelated";
    constexpr const char *KEY_RENDERING = "imageRendering";
    constexpr const char *KEY_ROWSPAN = "rowSpan";
    constexpr const char *KEY_STYLE = "style";
    constexpr const char *KEY_TAG = "tag";
    constexpr const char *KEY_TITLE = "title";
    constexpr const char *KEY_UNITS = "sizeUnits";
    constexpr const char *KEY_VERT_ALIGN = "verticalAlign";
    constexpr const char *KEY_WIDTH = "width";

    constexpr const char QUOTE_SEARCH = '"';
    constexpr const char QUOTE_ESCAPE = '\'';

    constexpr const char *VALUE_RENDERING_MONOCHROME = "monochrome";

    QString tag = obj[KEY_TAG].toString();
    if (!isSupportedStructuredTag(tag))
    {
        return;
    }
    else if (tag == "br")
    {
        out += '<';
        out += tag;
        if (obj[KEY_DATA].isObject())
        {
            addStructuredData(obj[KEY_DATA].toObject(), out);
        }
        out += '>';
    }
    else if (tag == "img")
    {
        QString filename = escapeHtml(ctx.basepath + obj[KEY_PATH].toString());

        out += "<img src=\"";
        out += filename;
        out += '"';

        if (obj[KEY_DATA].isObject())
        {
            addStructuredData(obj[KEY_DATA].toObject(), out);
        }

        if (obj[KEY_TITLE].isString())
        {
            out += " title=\"";
            out += escapeHtml(obj[KEY_TITLE].toString());
            out += '"';
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

        out += " style=\"";

        if (obj[KEY_RENDERING].isString())
        {
            out += "image-rendering: ";
            out += obj[KEY_RENDERING].toString("auto")
                .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
            out += ';';
        }
        else if (obj[KEY_PIXELATED].toBool(false))
        {
            out += "image-rendering: pixelated;";
        }

        if (obj[KEY_APPEARANCE].toString("auto") == VALUE_RENDERING_MONOCHROME)
        {
            out += "object-fit: fill;";
            out += "object-position: -9999999px 9999999px;";
            out += "background-color: currentColor;";

            out += "-webkit-mask-image: url(" + filename + ");";
            out += "-webkit-mask-repeat: no-repeat;";
            out += "-webkit-mask-size: contain;";
            out += "-webkit-mask-position: center;";

            out += "mask-image: url(" + filename + ");";
            out += "mask-repeat: no-repeat;";
            out += "mask-size: contain;";
            out += "mask-position: center;";
        }
        else if (obj[KEY_BACKGROUND].toBool(true))
        {
            out += "background-color: currentColor;";
        }

        if (obj[KEY_VERT_ALIGN].isString())
        {
            out += "vertical-align: ";
            out += obj[KEY_VERT_ALIGN].toString()
                .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
            out += ';';
        }

        if (obj[KEY_BORDER].isString())
        {
            out += "border: ";
            out += obj[KEY_BORDER].toString()
                .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
            out += ';';
        }

        if (obj[KEY_BORDER_RADIUS].isString())
        {
            out += "border-radius: ";
            out += obj[KEY_BORDER_RADIUS].toString()
                .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
            out += ';';
        }

        out += "\">";
    }
    else
    {
        double currentFontPixelSize = ctx.parentFontPixelSize;

        out += '<';
        out += tag;

        if (obj[KEY_OPEN].toBool(false))
        {
            out += " open";
        }

        if (obj[KEY_HREF].isString())
        {
            out += " href=\"";
            out += escapeHtml(obj[KEY_HREF].toString());
            out += '"';
        }

        if (obj[KEY_DATA].isObject())
        {
            addStructuredData(obj[KEY_DATA].toObject(), out);
        }

        if (obj[KEY_COLSPAN].isDouble())
        {
            out += " colspan=\"";
            out += QString::number(
                static_cast<int>(obj[KEY_COLSPAN].toDouble())
            );
            out += '"';
        }

        if (obj[KEY_ROWSPAN].isDouble())
        {
            out += " rowspan=\"";
            out += QString::number(
                static_cast<int>(obj[KEY_ROWSPAN].toDouble())
            );
            out += '"';
        }

        if (obj[KEY_TITLE].isString())
        {
            out += " title=\"";
            out += escapeHtml(obj[KEY_TITLE].toString());
            out += '"';
        }

        if (obj[KEY_LANG].isString())
        {
            out += " lang=\"";
            out += escapeHtml(obj[KEY_LANG].toString());
            out += '"';
        }

        out += " style=\"";
        if (tag == "table")
        {
            out += "border: 1px solid;"
                "border-collapse: collapse;";
        }
        else if (tag == "th" || tag == "td")
        {
            out += "border: 1px solid;"
                "border-collapse: collapse;"
                "padding: 5px;";
        }

        if (obj[KEY_STYLE].isObject())
        {
            currentFontPixelSize = addStructuredStyle(
                obj[KEY_STYLE].toObject(),
                ctx,
                out
            );
        }
        out += '"';

        out += '>';

        std::swap(ctx.parentFontPixelSize, currentFontPixelSize);
        addStructuredContent(obj[KEY_CONTENT], ctx, out);
        std::swap(ctx.parentFontPixelSize, currentFontPixelSize);

        out += "</";
        out += tag;
        out += '>';
    }
}

void StructuredRichText::addStructuredContent(
    const QJsonValue &val,
    StructuredRichText::Context &ctx,
    QString &out) const
{
    switch (val.type())
    {
    case QJsonValue::Type::String:
        addStructuredContentHelper(val.toString(), out);
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
    constexpr const char *KEY_PATH = "path";
    constexpr const char *KEY_WIDTH = "width";
    constexpr const char *KEY_HEIGHT = "height";
    constexpr const char *KEY_TITLE = "title";
    constexpr const char *KEY_RENDERING = "imageRendering";
    constexpr const char *KEY_DESCRIPTION = "description";

    QString filename = escapeHtml(ctx.basepath + obj[KEY_PATH].toString());
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
    if (obj[KEY_TITLE].isString())
    {
        out += " title=\"";
        out += escapeHtml(obj[KEY_TITLE].toString());
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

    if (obj[KEY_DESCRIPTION].isString())
    {
        out += "<br>";
        out += escapeHtml(obj[KEY_DESCRIPTION].toString())
            .replace('\n', "<br>");
    }
}

void StructuredRichText::addText(const QJsonObject &obj, QString &out) const
{
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
            return value.type() == QJsonValue::Object;
        }
    );
}

QString StructuredRichText::escapeHtml(const QString &str) const
{
    return str.toHtmlEscaped();
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
    constexpr double POINTS_IN_INCH = 72.0;
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
    constexpr double DEFAULT_SCREEN_DPI = 96.0;

    if (screen == nullptr)
    {
        return DEFAULT_SCREEN_DPI;
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
