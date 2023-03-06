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

#ifndef STROKELABEL_H
#define STROKELABEL_H

#include <QWidget>

class QTextEdit;

/**
 * A label that displays text with a stroke.
 */
class StrokeLabel : public QWidget
{
    Q_OBJECT

public:
    StrokeLabel(QWidget *parent = nullptr);

    /**
     * Sets the color of the text.
     * @param color The color to set the text to.
     */
    void setTextColor(const QColor &color);

    /**
     * Sets the color of the stroke.
     * @param color The color of the stroke.
     */
    void setStrokeColor(const QColor &color);

    /**
     * Sets the size of the stroke.
     * @param size The size of the stroke.
     */
    void setStrokeSize(double size);

    /**
     * Sets the color of the background.
     * @param color The color of the background.
     */
    void setBackgroundColor(const QColor &color);

    /**
     * Gets the current text font.
     * @return The current font.
     */
    QFont textFont() const;

    /**
     * Sets the current text font.
     * @param f The font to make the current font.
     */
    void setTextFont(const QFont &f);

    /**
     * Sets the text of the label. Text is always centered.
     * @param text The text to set the label to.
     */
    void setText(const QString &text);

    /**
     * Gets the current text of the label in plain text.
     */
    QString getText() const;

    /**
     * Removes all text from the label.
     */
    void clearText();

    /**
     * Gets the index of the character at the current position.
     * @param pos The point (on this widget) to get the text index of.
     */
    int getPosition(const QPoint &pos) const;

public Q_SLOTS:
    /**
     * Selects the text starting at some index for some number of characters.
     * @param start  The starting index to begin the selection from.
     * @param length The length of the selection.
     */
    void selectText(int start, int length);

    /**
     * Deselects text if any is currently selected.
     */
    void deselectText();

    /**
     * Resizes the label to fit the context of the current text.
     */
    void fitToContents();

    /**
     * Sets the fixed size of the widget.
     * @param h The height of the widget.
     * @param w The width of the widget.
     */
    void setSize(int h, int w);

private:
    /**
     * Initializes a QTextEdit with all the expected common configuration.
     * @param te The QTextEdit to initialize.
     */
    void initTextEdit(QTextEdit *te);

    /**
     * Updates the colors of the QTextEdit.
     */
    void updateColors();

    /* The background QTextEdit that creates the stroke effect. */
    QTextEdit *m_backgroundText;

    /* The foreground QTextEdit that creates the text */
    QTextEdit *m_foregroundText;

    /* The current color of the text */
    QColor m_textColor;

    /* The current color of the stroke */
    QColor m_strokeColor;

    /* The current size of the stroke */
    double m_strokeSize;

    /* The current background color */
    QColor m_backgroundColor;

    /* Changed to true when font is changed for scaling purposes */
    bool m_fontChanged = false;

    /* The current saved font */
    QFont m_currentFont;
};

#endif // STROKELABEL_H
