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

#include "glossarylabel.h"

#include <QAbstractTextDocumentLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextBlock>

/* The maximum allowed width of images. This is chosen with consideration of
 * DefinitionWidget's default 500px width. */
#define MAX_WIDTH 350

/* Begin Constructor/Destructor */

GlossaryLabel::GlossaryLabel(bool list, QWidget *parent)
    : QTextEdit(parent),
      m_list(list)
{
    setFrameStyle(QFrame::NoFrame);
    setStyleSheet("QTextEdit { background: rgba(0, 0, 0, 0); }");
    setReadOnly(true);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWordWrapMode(QTextOption::WrapMode::WordWrap);
    setAcceptRichText(true);

    document()->setDefaultStyleSheet(
        "img { vertical-align: bottom; }"
        "table, th, td {"
            "border: 1px solid;"
            "border-collapse: collapse;"
        "}"
        "th, td { padding: 5px; }"
    );

    connect(
        this, &GlossaryLabel::textChanged,
        this, &GlossaryLabel::adjustSize
    );
}

GlossaryLabel::~GlossaryLabel()
{

}

/* End Constructor/Destructor */
/* Begin Public Methods */

#define KEY_TYPE                        "type"
#define KEY_CONTENT                     "content"
#define VALUE_TYPE_IMAGE                "image"
#define VALUE_TYPE_STRUCTURED_CONTENT   "structured-content"
#define VALUE_TYPE_TEXT                 "text"

void GlossaryLabel::setContents(const QJsonArray &definitions, QString basepath)
{
#if defined(Q_OS_WIN)
    basepath.prepend('/');
    basepath.replace('\\', '/');
#endif
    basepath.prepend("file://");
    basepath += '/';

    QString content = "<html><head/><body>";
    if (m_list)
    {
        content += "<ul>";
    }
    for (int i = 0; i < definitions.size(); ++i)
    {
        const QJsonValue &val = definitions[i];

        if (m_list)
        {
            content += "<li>";
        }

        switch (val.type())
        {
        case QJsonValue::Type::String:
            content += val.toString()
                          .replace('\n', m_list ? "</li><li>" : "<br>");
            break;
        case QJsonValue::Type::Object:
        {
            QJsonObject obj = val.toObject();
            if (obj[KEY_TYPE] == VALUE_TYPE_STRUCTURED_CONTENT)
            {
                addStructuredContent(obj[KEY_CONTENT], basepath, content);
            }
            else if (obj[KEY_TYPE] == VALUE_TYPE_IMAGE)
            {
                addImage(obj, basepath, content);
            }
            else if (obj[KEY_TYPE] == VALUE_TYPE_TEXT)
            {
                addText(obj, content);
            }
            break;
        }
        default:
            break;
        }

        if (m_list)
        {
            content += "</li>";
        }
        else if (i < definitions.size() - 1)
        {
            content += "<br>";
        }
    }
    if (m_list)
    {
        content += "</ul>";
    }

    content += "</body></html>";
    setHtml(content);
}

#undef KEY_TYPE
#undef KEY_CONTENT
#undef VALUE_TYPE_IMAGE
#undef VALUE_TYPE_STRUCTURED_CONTENT
#undef VALUE_TYPE_TEXT

/* End Public Methods */
/* Begin Structured Content Parsing */

#define KEY_FONT_STYLE      "fontStyle"
#define KEY_FONT_WEIGHT     "fontWeight"
#define KEY_FONT_SIZE       "fontSize"
#define KEY_TEXT_DECORATION "textDecorationLine"
#define KEY_VERTICAL_ALIGN  "verticalAlign"
#define KEY_MARGIN_TOP      "marginTop"
#define KEY_MARGIN_LEFT     "marginLeft"
#define KEY_MARGIN_RIGHT    "marginRight"
#define KEY_MARGIN_BOTTOM   "marginBottom"

void GlossaryLabel::addStructuredStyle(
    const QJsonObject &obj, QString &out) const
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

void GlossaryLabel::addStructuredContentHelper(
    const QString &str, QString &out) const
{
    out += QString(str).replace('\n', "<br>");
}

void GlossaryLabel::addStructuredContentHelper(
    const QJsonArray &arr, const QString &basepath, QString &out) const
{
    for (const QJsonValue &val : arr)
    {
        addStructuredContent(val, basepath, out);
    }
}

#define KEY_TAG         "tag"
#define KEY_CONTENT     "content"
#define KEY_PATH        "path"
#define KEY_WIDTH       "width"
#define KEY_HEIGHT      "height"
#define KEY_UNITS       "sizeUnits"
#define KEY_VERT_ALIGN  "verticalAlign"
#define KEY_COLSPAN     "colSpan"
#define KEY_ROWSPAN     "rowSpan"
#define KEY_STYLE       "style"

void GlossaryLabel::addStructuredContentHelper(
    const QJsonObject &obj, const QString &basepath, QString &out) const
{
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

        if (obj[KEY_UNITS].isNull() || obj[KEY_UNITS].toString() == "px")
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
            /*
             * As of right now, this code creates mustard gas because while
             * QTextEdit does inherit its font from stylesheets set in a parent
             * widget, there is no good way to get the font size. That means
             * there is no good way to scale the text according to ems.
             */
            /*
            if (obj[KEY_UNITS].toString() == "em")
            {
                int size = font().pixelSize();
                if (size < 0)
                {
                    size = (int)(font().pointSize() / 0.75);
                }
                width *= size;
                height *= size;
            }
            */
            if (width > MAX_WIDTH)
            {
                height = MAX_WIDTH * height / width;
                width = MAX_WIDTH;
            }
            if (width > 0)
            {
                out += " width=\"";
                out += QString::number((int)width);
                out += '"';
            }
            if (height > 0)
            {
                out += " height=\"";
                out += QString::number((int)height);
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

        addStructuredContent(obj[KEY_CONTENT], basepath, out);

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
        out += '>';

        addStructuredContent(obj[KEY_CONTENT], basepath, out);

        out += "</" + tag + '>';
    }
    else
    {
        out += '<' + tag + '>';
        addStructuredContent(obj[KEY_CONTENT], basepath, out);
        out += "</" + tag + '>';
    }
}

#undef KEY_TAG
#undef KEY_CONTENT
#undef KEY_PATH
#undef KEY_WIDTH
#undef KEY_HEIGHT
#undef KEY_UNITS
#undef KEY_COLSPAN
#undef KEY_ROWSPAN
#undef KEY_STYLE

void GlossaryLabel::addStructuredContent(
    const QJsonValue &val, const QString &basepath, QString &out) const
{
    switch (val.type())
    {
    case QJsonValue::Type::String:
        addStructuredContentHelper(val.toString(), out);
        break;
    case QJsonValue::Type::Array:
        addStructuredContentHelper(val.toArray(), basepath, out);
        break;
    case QJsonValue::Type::Object:
        addStructuredContentHelper(val.toObject(), basepath, out);
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
#define KEY_DESCRIPTION "description"

void GlossaryLabel::addImage(
    const QJsonObject &obj, const QString &basepath, QString &out) const
{
    out += "<img src=\"";
    out += basepath;
    out += '/';
    out += obj[KEY_PATH].toString();
    out += '"';

    int width = MAX_WIDTH;
    int height = -1;
    if (obj[KEY_WIDTH].isDouble())
    {
        width = (int)obj[KEY_WIDTH].toDouble();
    }
    if (obj[KEY_HEIGHT].isDouble())
    {
        height = (int)obj[KEY_HEIGHT].toDouble();
    }

    if (width < 0 || width > MAX_WIDTH)
    {
        height = (int)((height * MAX_WIDTH) / ((double)width));
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

#undef KEY_IMAGE
#undef KEY_PATH
#undef KEY_WIDTH
#undef KEY_HEIGHT
#undef KEY_DESCRIPTION

#define KEY_TEXT "text"

void GlossaryLabel::addText(const QJsonObject &obj, QString &out) const
{
    out += obj[KEY_TEXT].toString()
                        .replace('\n', m_list ? "</li><li>" : "<br>");
}

#undef KEY_TEXT

/* End Other Object Parsers */
/* Begin Event Handlers */

void GlossaryLabel::adjustSize()
{
    setFixedHeight(document()->size().toSize().height());
}

QSize GlossaryLabel::sizeHint() const
{
    QSize s = QTextEdit::sizeHint();
    s.setHeight(document()->size().toSize().height());
    return s;
}

void GlossaryLabel::showEvent(QShowEvent *event)
{
    QTextEdit::showEvent(event);
    adjustSize();
}

void GlossaryLabel::resizeEvent(QResizeEvent *event)
{
    QTextEdit::resizeEvent(event);
    adjustSize();
}

/* End Event Handlers */
