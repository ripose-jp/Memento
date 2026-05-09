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

#include <QGuiApplication>
#include <QJsonObject>
#include <QScreen>

#include "util/utils.h"

static constexpr int MAX_WIDTH = 350;

StructuredContent::StructuredContent(QObject *parent) : QObject(parent)
{

}

StructuredContent::~StructuredContent()
{

}

QString StructuredContent::parse(
    const QJsonArray &content, Setting::GlossaryStyle style, const QFont &font)
{
    constexpr const char *KEY_TYPE = "type";
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *VALUE_TYPE_IMAGE = "image";
    constexpr const char *VALUE_TYPE_STRUCTURED_CONTENT = "structured-content";
    constexpr const char *VALUE_TYPE_TEXT = "text";

    QString basepath = DirectoryUtils::getDictionaryResourceDir();
#if defined(Q_OS_WIN)
    basepath.prepend('/');
    basepath.replace('\\', '/');
#endif
    basepath.prepend("file://");
    basepath += '/';

    bool shouldUseBullets =
        style == Setting::GlossaryStyleBullet &&
        !containsStructuredContent(content);

    QString text = "<html><head><style>"
            "img { vertical-align: bottom; }"
            "table, th, td {"
                "border: 1px solid;"
                "border-collapse: collapse;"
            "}"
            "th, td { padding: 5px; }"
        "</style></head><body>";
    if (shouldUseBullets)
    {
        text += "<ul>";
    }
    for (int i = 0; i < content.size(); ++i)
    {
        const QJsonValue &val = content[i];

        if (shouldUseBullets)
        {
            text += "<li>";
        }

        switch (val.type())
        {
        case QJsonValue::Type::String:
            text += val
                .toString()
                .replace(
                    '\n',
                    shouldUseBullets ? "</li><li>" : "<br>"
                );
            break;

        case QJsonValue::Type::Object:
        {
            QJsonObject obj = val.toObject();
            if (obj[KEY_TYPE] == VALUE_TYPE_STRUCTURED_CONTENT)
            {
                addStructuredContent(obj[KEY_CONTENT], basepath, font, text);
            }
            else if (obj[KEY_TYPE] == VALUE_TYPE_IMAGE)
            {
                addImage(obj, basepath, text);
            }
            else if (obj[KEY_TYPE] == VALUE_TYPE_TEXT)
            {
                addText(obj, style, text);
            }
            break;
        }

        default:
            break;
        }

        if (shouldUseBullets)
        {
            text += "</li>";
        }
        else if (i >= content.size() - 1)
        {
            /* Avoid putting <br> or | after the fine line */
        }
        else if (style == Setting::GlossaryStyleLineBreak)
        {
            text += "<br>";
        }
        else if (style == Setting::GlossaryStylePipe)
        {
            text += " | ";
        }
    }
    if (shouldUseBullets)
    {
        text += "</ul>";
    }

    text += "</body></html>";

    return text;
}

void StructuredContent::addStructuredStyle(const QJsonObject &obj, QString &out)
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

void StructuredContent::addStructuredContentHelper(
    const QString &str, QString &out)
{
    out += QString(str).replace('\n', "<br>");
}

void StructuredContent::addStructuredContentHelper(
    const QJsonArray &arr,
    const QString &basepath,
    const QFont &font,
    QString &out)
{
    for (const QJsonValue &val : arr)
    {
        addStructuredContent(val, basepath, font, out);
    }
}

void StructuredContent::addStructuredContentHelper(
    const QJsonObject &obj,
    const QString &basepath,
    const QFont &font,
    QString &out)
{
    constexpr const char *KEY_TAG = "tag";
    constexpr const char *KEY_CONTENT = "content";
    constexpr const char *KEY_PATH = "path";
    constexpr const char *KEY_WIDTH = "width";
    constexpr const char *KEY_HEIGHT = "height";
    constexpr const char *KEY_UNITS = "sizeUnits";
    constexpr const char *KEY_VERT_ALIGN = "verticalAlign";
    constexpr const char *KEY_COLSPAN = "colSpan";
    constexpr const char *KEY_ROWSPAN = "rowSpan";
    constexpr const char *KEY_STYLE = "style";

    QString tag = obj[KEY_TAG].toString();
    if (tag.isEmpty())
    {
        return;
    }
    else if (tag == "br")
    {
        out += "<br>";
    }
    else if (tag == "img")
    {
        out += "<img src=\"";
        out += basepath;
        out += '/';
        out += obj[KEY_PATH].toString();
        out += '"';

        if (obj[KEY_UNITS].isNull() ||
            obj[KEY_UNITS].toString() == "px" ||
            obj[KEY_UNITS].toString() == "em")
        {
            double width = -1;
            double height = -1;
            if (obj[KEY_WIDTH].isDouble())
            {
                width = obj[KEY_WIDTH].toDouble();
            }
            if (obj[KEY_HEIGHT].isDouble())
            {
                height = obj[KEY_HEIGHT].toDouble();
            }
            if (obj[KEY_UNITS].toString() == "em")
            {
                int size = font.pixelSize();
                if (size < 0)
                {
                    size = pointSizeToPixelSize(font.pointSizeF());
                }
                width *= size;
                height *= size;
            }
            if (width > MAX_WIDTH)
            {
                height = MAX_WIDTH * height / width;
                width = MAX_WIDTH;
            }
            if (width > 0)
            {
                out += " width=\"";
                out += QString::number(static_cast<int>(width));
                out += '"';
            }
            if (height > 0)
            {
                out += " height=\"";
                out += QString::number(static_cast<int>(height));
                out += '"';
            }
        }
        if (obj[KEY_VERT_ALIGN].isString())
        {
            out += " style=\"vertical-align: ";
            out += obj[KEY_VERT_ALIGN].toString();
            out += ";\"";
        }

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
        out += '>';

        addStructuredContent(obj[KEY_CONTENT], basepath, font, out);

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
        out += '>';

        addStructuredContent(obj[KEY_CONTENT], basepath, font, out);

        out += "</" + tag + '>';
    }
    else
    {
        out += '<' + tag + '>';
        addStructuredContent(obj[KEY_CONTENT], basepath, font, out);
        out += "</" + tag + '>';
    }
}

void StructuredContent::addStructuredContent(
    const QJsonValue &val,
    const QString &basepath,
    const QFont &font,
    QString &out)
{
    switch (val.type())
    {
    case QJsonValue::Type::String:
        addStructuredContentHelper(val.toString(), out);
        break;
    case QJsonValue::Type::Array:
        addStructuredContentHelper(val.toArray(), basepath, font, out);
        break;
    case QJsonValue::Type::Object:
        addStructuredContentHelper(val.toObject(), basepath, font, out);
        break;
    default:
        break;
    }
}

void StructuredContent::addImage(
    const QJsonObject &obj, const QString &basepath, QString &out)
{
    constexpr const char *KEY_PATH = "path";
    constexpr const char *KEY_WIDTH = "width";
    constexpr const char *KEY_HEIGHT = "height";
    constexpr const char *KEY_DESCRIPTION = "description";

    out += "<img src=\"";
    out += basepath;
    out += '/';
    out += obj[KEY_PATH].toString();
    out += '"';

    int width = MAX_WIDTH;
    int height = -1;
    if (obj[KEY_WIDTH].isDouble())
    {
        width = static_cast<int>(obj[KEY_WIDTH].toDouble());
    }
    if (obj[KEY_HEIGHT].isDouble())
    {
        height = static_cast<int>(obj[KEY_HEIGHT].toDouble());
    }

    if (width < 0 || width > MAX_WIDTH)
    {
        height = static_cast<int>(
            (height * MAX_WIDTH) / static_cast<double>(width)
        );
        width = MAX_WIDTH;
    }

    out += " width=\"";
    out += QString::number(width);
    out += '"';
    if (height > 0)
    {
        out += " height=\"";
        out += QString::number(height);
        out += '"';
    }

    out += '>';

    if (obj[KEY_DESCRIPTION].isString())
    {
        out += "<br>";
        out += obj[KEY_DESCRIPTION].toString().replace('\n', "<br>");
    }
}

void StructuredContent::addText(
    const QJsonObject &obj, Setting::GlossaryStyle style, QString &out)
{
    constexpr const char *KEY_TEXT = "text";

    out += obj[KEY_TEXT]
        .toString()
        .replace(
            '\n',
            style == Setting::GlossaryStyleBullet ?
                "</li><li>" : "<br>"
        );
}

int StructuredContent::pointSizeToPixelSize(double pointSize)
{
    constexpr double POINTS_IN_INCH = 72.0;
    return pointSize / POINTS_IN_INCH *
        QGuiApplication::primaryScreen()->physicalDotsPerInch();
}

bool StructuredContent::containsStructuredContent(const QJsonArray &definitions)
{
    return std::any_of(
        std::begin(definitions), std::end(definitions),
        [] (const QJsonValue &value) -> bool
        {
            return value.type() == QJsonValue::Object;
        }
    );
}
