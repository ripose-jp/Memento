////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 Ripose
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

#include "anki/glossarybuilder.h"

#include <QDir>

#include "util/utils.h"

/* Begin Public Methods */

QStringList GlossaryBuilder::buildGlossary(
    const QJsonArray &definitions,
    QString basepath,
    QSet<FileInfo> &fileMap)
{
    constexpr const char *KEY_TYPE = "type";
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *VALUE_TYPE_IMAGE = "image";
    constexpr const char *VALUE_TYPE_STRUCTURED_CONTENT = "structured-content";
    constexpr const char *VALUE_TYPE_TEXT = "text";

    basepath += QDir::separator();
    QStringList glossaries;

    for (const QJsonValue &val : definitions)
    {
        QString glossary;
        switch (val.type())
        {
            case QJsonValue::Type::String:
                glossary += escapeHtml(val.toString().trimmed()).replace(
                    '\n',
                    "<br>"
                );
                break;

            case QJsonValue::Type::Object:
            {
                QJsonObject obj = val.toObject();
                if (obj[KEY_TYPE] == VALUE_TYPE_STRUCTURED_CONTENT)
                {
                    addStructuredContent(
                        obj[KEY_CONTENT], basepath, glossary, fileMap
                    );
                }
                else if (obj[KEY_TYPE] == VALUE_TYPE_IMAGE)
                {
                    addImage(obj, basepath, glossary, fileMap);
                }
                else if (obj[KEY_TYPE] == VALUE_TYPE_TEXT)
                {
                    addText(obj, glossary);
                }
                break;
            }

            default:
                continue;
        }
        glossaries.emplaceBack(std::move(glossary));
    }

    return glossaries;
}

/* End Public Methods */
/* Begin Structured Content Parsing */

QString GlossaryBuilder::escapeHtml(const QString &str)
{
    return str.toHtmlEscaped();
}

QString GlossaryBuilder::structuredDataAttributeName(const QString &key)
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

bool GlossaryBuilder::isSupportedStructuredTag(const QString &tag)
{
    static const QSet<QString> TAGS = {
        "br", "ruby", "rt", "rp", "table", "thead", "tbody", "tfoot",
        "tr", "td", "th", "span", "div", "ol", "ul", "li", "details",
        "summary", "img", "a"
    };
    return TAGS.contains(tag);
}

void GlossaryBuilder::addStructuredData(const QJsonObject &obj, QString &out)
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

void GlossaryBuilder::addStructuredStyle(
    const QJsonObject &obj, QString &out)
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
        out += "font-size: ";
        out += obj[KEY_FONT_SIZE].toString("medium")
            .replace(QUOTE_SEARCH, QUOTE_ESCAPE);
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
}

void GlossaryBuilder::addStructuredContentHelper(
    const QString &str, QString &out)
{
    out += escapeHtml(str).replace('\n', "<br>");
}

void GlossaryBuilder::addStructuredContentHelper(
    const QJsonArray &arr,
    const QString &basepath,
    QString &out,
    QSet<FileInfo> &fileMap)
{
    for (const QJsonValue &val : arr)
    {
        addStructuredContent(val, basepath, out, fileMap);
    }
}

void GlossaryBuilder::addStructuredContentHelper(
    const QJsonObject &obj,
    const QString &basepath,
    QString &out,
    QSet<FileInfo> &fileMap)
{
    constexpr const char *KEY_ALT = "alt";
    constexpr const char *KEY_APPEARANCE = "appearance";
    constexpr const char *KEY_BACKGROUND = "background";
    constexpr const char *KEY_BORDER = "border";
    constexpr const char *KEY_BORDER_RADIUS = "borderRadius";
    constexpr const char *KEY_COLLAPSED = "collapsed";
    constexpr const char *KEY_COLLAPSIBLE = "collapsible";
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
        if (obj[KEY_COLLAPSIBLE].toBool(false))
        {
            out += "<details";
            if (!obj[KEY_COLLAPSED].toBool(false))
            {
                out += " open";
            }
            out += ">";
        }

        QString filename = addFile(basepath, obj[KEY_PATH].toString(), fileMap);
        out += "<img src=\"";
        out += escapeHtml(filename);
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

        out += " style=\"";

        QString units = obj[KEY_UNITS].toString("px");
        if (obj[KEY_WIDTH].isDouble())
        {
            out += "width: ";
            out += QString::number(obj[KEY_WIDTH].toDouble());
            out += units;
            out += ';';
        }
        if (obj[KEY_HEIGHT].isDouble())
        {
            out += "height: ";
            out += QString::number(obj[KEY_HEIGHT].toDouble());
            out += units;
            out += ';';
        }

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

        if (obj[KEY_COLLAPSIBLE].toBool(false))
        {
            out += "</details>";
        }
    }
    else
    {
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

        if (obj[KEY_STYLE].isObject())
        {
            out += " style=\"";
            addStructuredStyle(obj[KEY_STYLE].toObject(), out);
            out += '"';
        }

        out += '>';

        addStructuredContent(obj[KEY_CONTENT], basepath, out, fileMap);

        out += "</";
        out += tag;
        out += '>';
    }
}

void GlossaryBuilder::addStructuredContent(
    const QJsonValue &val,
    const QString &basepath,
    QString &out,
    QSet<FileInfo> &fileMap)
{
    switch (val.type())
    {
    case QJsonValue::Type::String:
        addStructuredContentHelper(val.toString(), out);
        break;

    case QJsonValue::Type::Array:
        addStructuredContentHelper(val.toArray(), basepath, out, fileMap);
        break;

    case QJsonValue::Type::Object:
        addStructuredContentHelper(val.toObject(), basepath, out, fileMap);
        break;

    default:
        break;
    }
}

/* End Structured Content Parsing */
/* Begin Other Object Parsers */

void GlossaryBuilder::addImage(
    const QJsonObject &obj,
    const QString &basepath,
    QString &out,
    QSet<FileInfo> &fileMap)
{
    constexpr const char *KEY_PATH = "path";
    constexpr const char *KEY_WIDTH = "width";
    constexpr const char *KEY_HEIGHT = "height";
    constexpr const char *KEY_TITLE = "title";
    constexpr const char *KEY_RENDERING = "imageRendering";
    constexpr const char *KEY_DESCRIPTION = "description";
    constexpr const char *KEY_COLLAPSED = "collapsed";
    constexpr const char *KEY_COLLAPSIBLE = "collapsible";

    bool collapsed = obj[KEY_COLLAPSED].toBool(false);
    bool collapsible = obj[KEY_COLLAPSIBLE].toBool(true);

    if (collapsible)
    {
        out += "<details";
        if (!collapsed)
        {
            out += " open";
        }
        out += '>';
        out +=
            "<summary style=\"text-decoration: underline;cursor: pointer;\">"
                "[Image]"
            "</summary>";
    }

    out += "<img src=\"";
    out += escapeHtml(addFile(basepath, obj[KEY_PATH].toString(), fileMap));
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
        out += escapeHtml(obj[KEY_DESCRIPTION].toString().trimmed())
            .replace('\n', "<br>");
    }

    if (collapsible)
    {
        out += "</details>";
    }
}

void GlossaryBuilder::addText(const QJsonObject &obj, QString &out)
{
    constexpr const char *KEY_TEXT = "text";
    out += escapeHtml(obj[KEY_TEXT].toString()).replace('\n', "<br>");
}

/* End Other Object Parsers */
/* Begin Helpers */

QString GlossaryBuilder::addFile(
    QString basepath,
    const QString &path,
    QSet<FileInfo> &fileMap)
{
#if defined(Q_OS_WIN)
    basepath += QDir::toNativeSeparators(path);
#else
    basepath += path;
#endif
    QString hash = FileUtils::calculateMd5(basepath);
    if (hash.isEmpty())
    {
        return QString("File not found at: %1").arg(basepath);
    }
    int lastSlashIndex = path.lastIndexOf('/');
    if (lastSlashIndex == -1)
    {
        lastSlashIndex = 0;
    }
    int dotIndex = path.indexOf('.', lastSlashIndex);
    if (dotIndex != -1)
    {
        hash += path.right(path.length() - dotIndex);
    }
    fileMap.insert(FileInfo{basepath, hash});
    return hash;
}

/* End Helpers */
