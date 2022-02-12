////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#include "strokelabel.h"

#include <QAbstractTextDocumentLayout>
#include <QTextEdit>

/* Begin Constructor/Destructor */

#define TRANSPARENT_COLOR QColor("#00000000")

StrokeLabel::StrokeLabel(QWidget *parent)
    : QWidget(parent),
      m_backgroundText(new QTextEdit(this)),
      m_foregroundText(new QTextEdit(this)),
      m_textColor(palette().text().color()),
      m_strokeColor(palette().window().color()),
      m_strokeSize(4.0),
      m_backgroundColor(TRANSPARENT_COLOR),
      m_currentFont(m_backgroundText->font())
{
    initTextEdit(m_backgroundText);

    initTextEdit(m_foregroundText);
    m_foregroundText->raise();

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::FocusPolicy::NoFocus);
    fitToContents();
    updateColors();
}

void StrokeLabel::initTextEdit(QTextEdit *te)
{
    te->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    te->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    te->setAcceptDrops(false);
    te->setFrameShape(QFrame::Shape::NoFrame);
    te->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    te->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    te->setLineWrapMode(QTextEdit::NoWrap);
    te->setReadOnly(true);
    te->setAcceptRichText(false);
    te->setTextInteractionFlags(Qt::NoTextInteraction);
    te->setCursor(Qt::ArrowCursor);
}

void StrokeLabel::updateColors()
{
    /* Set the text and background color */
    QString stylesheetFormat =
        "QTextEdit {"
            "color: rgba(%1, %2, %3, %4);"
            "background: rgba(0, 0, 0, 0);"
        "}";
    m_foregroundText->setStyleSheet(
        stylesheetFormat.arg(QString::number(m_textColor.red()))
                        .arg(QString::number(m_textColor.green()))
                        .arg(QString::number(m_textColor.blue()))
                        .arg(QString::number(m_textColor.alpha()))
    );

    stylesheetFormat =
        "QTextEdit {"
            "color: rgba(%1, %2, %3, %4);"
            "background: rgba(%5, %6, %7, %8);"
        "}";
    m_backgroundText->setStyleSheet(
        stylesheetFormat.arg(QString::number(m_textColor.red()))
                        .arg(QString::number(m_textColor.green()))
                        .arg(QString::number(m_textColor.blue()))
                        .arg(QString::number(m_textColor.alpha()))
                        .arg(QString::number(m_backgroundColor.red()))
                        .arg(QString::number(m_backgroundColor.green()))
                        .arg(QString::number(m_backgroundColor.blue()))
                        .arg(QString::number(m_backgroundColor.alpha()))
    );

    setText(m_foregroundText->toPlainText());
}

void StrokeLabel::fitToContents()
{
    m_backgroundText->updateGeometry();
    QWidget *pw = parentWidget();
    int width = m_backgroundText->document()->idealWidth() + 4;
    if (pw && width > pw->width())
    {
        QFont f = textFont();
        int fontSizePx = f.pixelSize() * pw->width() / width;
        f.setPixelSize(fontSizePx);
        m_backgroundText->setFont(f);
        m_foregroundText->setFont(f);
        m_fontChanged = true;
        m_backgroundText->updateGeometry();

        width = pw->width();
    }
    int height = m_backgroundText->document()->size().toSize().height();
    setSize(width, height);
}

#undef TRANSPARENT_COLOR

/* End Constructor/Destructor */
/* Begin Color Setters */

void StrokeLabel::setTextColor(const QColor &color)
{
    m_textColor = color;
    updateColors();
}

void StrokeLabel::setStrokeColor(const QColor &color)
{
    m_strokeColor = color;
    updateColors();
}

void StrokeLabel::setStrokeSize(double size)
{
    m_strokeSize = size;
    updateColors();
}

void StrokeLabel::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    updateColors();
}

/* End Color Setters */
/* Begin Font Methods */

QFont StrokeLabel::textFont() const
{
    return m_currentFont;
}

void StrokeLabel::setTextFont(const QFont &f)
{
    m_currentFont = f;
    m_fontChanged = false;
    m_foregroundText->setFont(f);
    m_backgroundText->setFont(f);
    fitToContents();
}

/* End Font Methods */
/* Begin Text Methods */

void StrokeLabel::setText(const QString &text)
{
    clearText();
    if (m_fontChanged)
    {
        setTextFont(m_currentFont);
    }

    QStringList subList = text.split('\n');
    for (const QString &text : subList)
    {
        if (text.isEmpty())
            continue;

        m_backgroundText->append(text);
        m_backgroundText->setAlignment(Qt::AlignHCenter);

        m_foregroundText->append(text);
        m_foregroundText->setAlignment(Qt::AlignHCenter);
    }

    /* Add the stroke */
    QTextCharFormat format;
    format.setTextOutline(
        QPen(
            m_strokeColor,
            m_strokeSize,
            Qt::SolidLine,
            Qt::RoundCap,
            Qt::RoundJoin
        )
    );
    QTextCursor cursor(m_backgroundText->document());
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(format);

    format = QTextCharFormat();
    format.setTextOutline(QPen(Qt::transparent));
    cursor = QTextCursor(m_foregroundText->document());
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(format);

    fitToContents();
}


QString StrokeLabel::getText() const
{
    return m_foregroundText->toPlainText();
}

void StrokeLabel::clearText()
{
    m_backgroundText->clear();
    m_foregroundText->clear();
    setSize(0, 0);
}

void StrokeLabel::selectText(int start, int length)
{
    QTextCursor q = m_foregroundText->textCursor();
    q.setPosition(start);
    q.setPosition(start + length, QTextCursor::KeepAnchor);
    m_foregroundText->setTextCursor(q);
}

void StrokeLabel::deselectText()
{
    QTextCursor q = m_foregroundText->textCursor();
    q.clearSelection();
    m_foregroundText->setTextCursor(q);
}

/* End Text Methods */
/* Begin Helper Methods */

void StrokeLabel::setSize(int w, int h)
{
    m_backgroundText->setFixedSize(w, h);
    m_foregroundText->setFixedSize(w, h);
    setFixedSize(w, h);
}

int StrokeLabel::getPosition(const QPoint &pos) const
{
    return m_foregroundText->document()->documentLayout()->hitTest(
        pos, Qt::ExactHit
    );
}

/* End Helper Methods */
