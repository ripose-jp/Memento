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
#include <QJsonObject>

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

void GlossaryBuilder::addStructuredStyle(
    const QJsonObject &obj, QString &out)
{
    constexpr const char *KEY_FONT_STYLE = "fontStyle";
    constexpr const char *KEY_FONT_WEIGHT = "fontWeight";
    constexpr const char *KEY_FONT_SIZE = "fontSize";
    constexpr const char *KEY_TEXT_DECORATION = "textDecorationLine";
    constexpr const char *KEY_VERTICAL_ALIGN = "verticalAlign";
    constexpr const char *KEY_MARGIN_TOP = "marginTop";
    constexpr const char *KEY_MARGIN_LEFT = "marginLeft";
    constexpr const char *KEY_MARGIN_RIGHT = "marginRight";
    constexpr const char *KEY_MARGIN_BOTTOM = "marginBottom";

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
        out += QString::number(
            static_cast<int>(obj[KEY_MARGIN_TOP].toDouble(0.0))
        );
        out += "px;";
    }

    if (obj[KEY_MARGIN_LEFT].isDouble())
    {
        out += "margin-left: ";
        out += QString::number(
            static_cast<int>(obj[KEY_MARGIN_LEFT].toDouble(0.0))
        );
        out += "px;";
    }

    if (obj[KEY_MARGIN_RIGHT].isDouble())
    {
        out += "margin-right: ";
        out += QString::number(
            static_cast<int>(obj[KEY_MARGIN_RIGHT].toDouble(0.0))
        );
        out += "px;";
    }

    if (obj[KEY_MARGIN_BOTTOM].isDouble())
    {
        out += "margin-bottom: ";
        out += QString::number(
            static_cast<int>(obj[KEY_MARGIN_BOTTOM].toDouble(0.0))
        );
        out += "px;";
    }
}

void GlossaryBuilder::addStructuredContentHelper(
    const QString &str, QString &out)
{
    out += QString(str).trimmed().replace('\n', "<br>");
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
    constexpr const char *KEY_TAG = "tag";
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *KEY_DATA = "data";
    constexpr const char *KEY_STYLE = "style";
    constexpr const char *KEY_PATH = "path";
    constexpr const char *KEY_TITLE = "title";
    constexpr const char *KEY_WIDTH = "width";
    constexpr const char *KEY_HEIGHT = "height";
    constexpr const char *KEY_RENDERING = "imageRendering";
    constexpr const char *KEY_APPEARANCE = "appearance";
    constexpr const char *KEY_UNITS = "sizeUnits";
    constexpr const char *KEY_VERT_ALIGN = "verticalAlign";
    constexpr const char *KEY_COLSPAN = "colSpan";
    constexpr const char *KEY_ROWSPAN = "rowSpan";

    constexpr const char *VALUE_RENDERING_MONOCHROME = "monochrome";

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

void GlossaryBuilder::addText(const QJsonObject &obj, QString &out)
{
    constexpr const char *KEY_TEXT = "text";
    out += obj[KEY_TEXT].toString().trimmed().replace('\n', "<br>");
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
