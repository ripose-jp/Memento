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

#include "glossarybuilder.h"

#include <QAbstractTextDocumentLayout>
#include <QJsonObject>
#include <QTextBlock>

#include "util/utils.h"

/* Begin Public Methods */

#define KEY_TYPE                        "type"
#define KEY_CONTENT                     "content"
#define VALUE_TYPE_IMAGE                "image"
#define VALUE_TYPE_STRUCTURED_CONTENT   "structured-content"
#define VALUE_TYPE_TEXT                 "text"

QStringList GlossaryBuilder::buildGlossary(
    const QJsonArray &definitions,
    QString basepath,
    QList<QPair<QString, QString>> &fileMap)
{
    basepath += SLASH;
    QStringList glossaries;

    for (int i = 0; i < definitions.size(); ++i)
    {
        const QJsonValue &val = definitions[i];
        QString glossary;
        switch (val.type())
        {
        case QJsonValue::Type::String:
            glossary += val.toString().trimmed().replace('\n', "<br>");
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
        glossaries << glossary;
    }

    return glossaries;
}

#undef KEY_TYPE
#undef KEY_CONTENT
#undef VALUE_TYPE_IMAGE
#undef VALUE_TYPE_STRUCTURED_CONTENT
#undef VALUE_TYPE_TEXT

/* End Public Methods */
/* Begin Structured Content Parsing */

void GlossaryBuilder::addStructuredData(const QJsonObject &obj, QString &out)
{
    for (const QString &key : obj.keys())
    {
        const QJsonValue &val = obj[key];
        if (!val.isString())
        {
            continue;
        }
        out += " data-";
        out += key;
        out += "=\"";
        out += val.toString();
        out += '"';
    }
}

#define KEY_FONT_STYLE      "fontStyle"
#define KEY_FONT_WEIGHT     "fontWeight"
#define KEY_FONT_SIZE       "fontSize"
#define KEY_TEXT_DECORATION "textDecorationLine"
#define KEY_VERTICAL_ALIGN  "verticalAlign"
#define KEY_MARGIN_TOP      "marginTop"
#define KEY_MARGIN_LEFT     "marginLeft"
#define KEY_MARGIN_RIGHT    "marginRight"
#define KEY_MARGIN_BOTTOM   "marginBottom"

void GlossaryBuilder::addStructuredStyle(
    const QJsonObject &obj, QString &out)
{
    if (obj[KEY_FONT_STYLE].isString())
    {
        out += "font-style: ";
        out += obj[KEY_FONT_STYLE].toString("normal");
        out += ';';
    }

    if (obj[KEY_FONT_WEIGHT].isString())
    {
        out += "font-weight: ";
        out += obj[KEY_FONT_WEIGHT].toString("normal");
        out += ';';
    }

    if (obj[KEY_FONT_SIZE].isString())
    {
        out += "font-size: ";
        out += obj[KEY_FONT_SIZE].toString("medium");
        out += ';';
    }

    if (obj[KEY_TEXT_DECORATION].isArray())
    {
        out += "text-decoration: ";
        for (const QJsonValue &val : obj[KEY_TEXT_DECORATION].toArray())
        {
            out += val.toString("none");
            out += ' ';
        }
        out += ';';
    }
    else if (obj[KEY_TEXT_DECORATION].isString())
    {
        out += "text-decoration: ";
        out += obj[KEY_TEXT_DECORATION].toString("none");
        out += ';';
    }

    if (obj[KEY_VERTICAL_ALIGN].isString())
    {
        out += "vertical-align: ";
        out += obj[KEY_VERTICAL_ALIGN].toString("baseline");
        out += ';';
    }

    if (obj[KEY_MARGIN_TOP].isDouble())
    {
        out += "margin-top: ";
        out += QString::number((int)obj[KEY_MARGIN_TOP].toDouble(0.0));
        out += "px;";
    }

    if (obj[KEY_MARGIN_LEFT].isDouble())
    {
        out += "margin-left: ";
        out += QString::number((int)obj[KEY_MARGIN_LEFT].toDouble(0.0));
        out += "px;";
    }

    if (obj[KEY_MARGIN_RIGHT].isDouble())
    {
        out += "margin-right: ";
        out += QString::number((int)obj[KEY_MARGIN_RIGHT].toDouble(0.0));
        out += "px;";
    }

    if (obj[KEY_MARGIN_BOTTOM].isDouble())
    {
        out += "margin-bottom: ";
        out += QString::number((int)obj[KEY_MARGIN_BOTTOM].toDouble(0.0));
        out += "px;";
    }
}

#undef KEY_FONT_STYLE
#undef KEY_FONT_WEIGHT
#undef KEY_FONT_SIZE
#undef KEY_TEXT_DECORATION
#undef KEY_VERTICAL_ALIGN
#undef KEY_MARGIN_TOP
#undef KEY_MARGIN_LEFT
#undef KEY_MARGIN_RIGHT
#undef KEY_MARGIN_BOTTOM

void GlossaryBuilder::addStructuredContentHelper(
    const QString &str, QString &out)
{
    out += QString(str).trimmed().replace('\n', "<br>");
}

void GlossaryBuilder::addStructuredContentHelper(
    const QJsonArray &arr,
    const QString &basepath,
    QString &out,
    QList<QPair<QString, QString>> &fileMap)
{
    for (const QJsonValue &val : arr)
    {
        addStructuredContent(val, basepath, out, fileMap);
    }
}

#define KEY_TAG                     "tag"
#define KEY_CONTENT                 "content"
#define KEY_DATA                    "data"
#define KEY_STYLE                   "style"
#define KEY_PATH                    "path"
#define KEY_TITLE                   "title"
#define KEY_WIDTH                   "width"
#define KEY_HEIGHT                  "height"
#define KEY_RENDERING               "imageRendering"
#define KEY_APPEARANCE              "appearance"
#define KEY_UNITS                   "sizeUnits"
#define KEY_VERT_ALIGN              "verticalAlign"
#define KEY_COLSPAN                 "colSpan"
#define KEY_ROWSPAN                 "rowSpan"

#define VALUE_RENDERING_MONOCHROME  "monochrome"

void GlossaryBuilder::addStructuredContentHelper(
    const QJsonObject &obj,
    const QString &basepath,
    QString &out,
    QList<QPair<QString, QString>> &fileMap)
{
    QString tag = obj[KEY_TAG].toString();
    if (tag.isEmpty())
    {
        return;
    }
    else if (tag == "br")
    {
        out += "<br";
        addStructuredData(obj[KEY_DATA].toObject(), out);
        out += '>';
    }
    else if (tag == "img")
    {
        QString filename = addFile(basepath, obj[KEY_PATH].toString(), fileMap);
        out += "<img src=\"";
        out += filename;
        out += '"';

        if (obj[KEY_TITLE].isString())
        {
            out += " title=\"";
            out += obj[KEY_TITLE].toString();
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
            out += obj[KEY_RENDERING].toString();
            out += ';';
        }
        out += "vertical-align: ";
        out += obj[KEY_VERT_ALIGN].toString("bottom");
        out += ';';
        if (obj[KEY_APPEARANCE].toString("auto") == VALUE_RENDERING_MONOCHROME)
        {
            out += "object-position: -9999999px 9999999px;";
            out += "background-color: currentColor;";
            out += "-webkit-mask-image: url(" + filename + ");";
            out += "-webkit-mask-repeat: no-repeat;";
            out += "-webkit-mask-size: contain;";
        }
        out += '"';

        addStructuredData(obj[KEY_DATA].toObject(), out);

        out += '>';
    }
    else if (tag == "span" || tag == "div")
    {
        out += '<';
        out += tag;
        if (obj[KEY_STYLE].isObject())
        {
            out += " style=\"";
            addStructuredStyle(obj[KEY_STYLE].toObject(), out);
            out += '"';
        }
        addStructuredData(obj[KEY_DATA].toObject(), out);
        out += '>';

        addStructuredContent(obj[KEY_CONTENT], basepath, out, fileMap);

        out += "</" + tag + '>';
    }
    else if (tag == "td" || tag == "th")
    {
        out += '<';
        out += tag;
        if (obj[KEY_COLSPAN].isDouble())
        {
            out += " colspan=\"";
            out += QString::number((int)obj[KEY_COLSPAN].toDouble());
            out += '"';
        }
        if (obj[KEY_ROWSPAN].isDouble())
        {
            out += " rowspan=\"";
            out += QString::number((int)obj[KEY_ROWSPAN].toDouble());
            out += '"';
        }
        if (obj[KEY_STYLE].isObject())
        {
            out += " style=\"";
            addStructuredStyle(obj[KEY_STYLE].toObject(), out);
            out += '"';
        }
        addStructuredData(obj[KEY_DATA].toObject(), out);
        out += '>';

        addStructuredContent(obj[KEY_CONTENT], basepath, out, fileMap);

        out += "</" + tag + '>';
    }
    else
    {
        out += '<';
        out += tag;
        addStructuredData(obj[KEY_DATA].toObject(), out);
        out += '>';

        addStructuredContent(obj[KEY_CONTENT], basepath, out, fileMap);

        out += "</" + tag + '>';
    }
}

#undef KEY_TAG
#undef KEY_CONTENT
#undef KEY_DATA
#undef KEY_STYLE
#undef KEY_PATH
#undef KEY_TITLE
#undef KEY_WIDTH
#undef KEY_HEIGHT
#undef KEY_RENDERING
#undef KEY_APPEARANCE
#undef KEY_UNITS
#undef KEY_VERT_ALIGN
#undef KEY_COLSPAN
#undef KEY_ROWSPAN

#undef VALUE_RENDERING_MONOCHROME

void GlossaryBuilder::addStructuredContent(
    const QJsonValue &val,
    const QString &basepath,
    QString &out,
    QList<QPair<QString, QString>> &fileMap)
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

#define KEY_IMAGE       "image"
#define KEY_PATH        "path"
#define KEY_WIDTH       "width"
#define KEY_HEIGHT      "height"
#define KEY_TITLE       "title"
#define KEY_RENDERING   "imageRendering"
#define KEY_DESCRIPTION "description"
#define KEY_COLLAPSED   "collapsed"
#define KEY_COLLAPSIBLE "collapsible"

void GlossaryBuilder::addImage(
    const QJsonObject &obj,
    const QString &basepath,
    QString &out,
    QList<QPair<QString, QString>> &fileMap)
{
    bool collapsed = obj[KEY_COLLAPSED].toBool(false);
    bool collapseable = obj[KEY_COLLAPSED].toBool(true);

    if (collapseable)
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
    out += addFile(basepath, obj[KEY_PATH].toString(), fileMap);
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
        out += obj[KEY_TITLE].toString();
        out += '"';
    }

    out += " style=\"display: inline-table;vertical-align: top;";
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
        out += obj[KEY_DESCRIPTION].toString().trimmed().replace('\n', "<br>");
    }

    if (collapseable)
    {
        out += "</details>";
    }
}

#undef KEY_IMAGE
#undef KEY_PATH
#undef KEY_WIDTH
#undef KEY_HEIGHT
#undef KEY_TITLE
#undef KEY_RENDERING
#undef KEY_DESCRIPTION
#undef KEY_COLLAPSED
#undef KEY_COLLAPSIBLE

#define KEY_TEXT "text"

void GlossaryBuilder::addText(const QJsonObject &obj, QString &out)
{
    out += obj[KEY_TEXT].toString().trimmed().replace('\n', "<br>");
}

#undef KEY_TEXT

/* End Other Object Parsers */
/* Begin Helpers */

QString GlossaryBuilder::addFile(
    QString basepath,
    const QString &path,
    QList<QPair<QString, QString>> &fileMap)
{
#if defined(Q_OS_WIN)
    basepath += QString(path).replace('/', SLASH);
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
    fileMap << QPair<QString, QString>(basepath, hash);
    return hash;
}

/* End Helpers */
