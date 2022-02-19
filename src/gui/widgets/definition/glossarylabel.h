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

#ifndef GLOSSARYLABEL_H
#define GLOSSARYLABEL_H

#include <QTextEdit>

class GlossaryLabel : public QTextEdit
{
    Q_OBJECT

public:
    /**
     * Creates a new glossary label.
     * @param list   True to display content in bulleted list, false otherwise.
     * @param parent The parent of this label.
     */
    GlossaryLabel(bool list = true, QWidget *parent = nullptr);
    virtual ~GlossaryLabel();

    /**
     * Sets the contents of this label.
     * @param definitions The definitions to add to this label.
     * @param basepath    The path where all external resources begin.
     */
    void setContents(const QJsonArray &definitions, QString basepath);

protected:
    /**
     * Returns the ideal size for this widget.
     * @return The ideal size.
     */
    QSize sizeHint() const override;

    /**
     * Called when this widget is shown.
     * @param event The show event.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Called when this widget is resized.
     * @param event The resize event.
     */
    void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
    /**
     * Adjust the size of the label.
     */
    void adjustSize();

private:
    /**
     * @brief Adds structured style objects.
     * @param      obj The structured style object.
     * @param[out] out The string this style will be appended to.
     */
    void addStructuredStyle(const QJsonObject &obj, QString &out) const;

    /**
     * Adds string structured content.
     * @param      str The string to add.
     * @param[out] out The string this string will be appended to.
     */
    void addStructuredContentHelper(
        const QString &str, QString &out) const;

    /**
     * Adds an array of structured content.
     * @param      arr      The array of structured content.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this content will be appended to.
     */
    void addStructuredContentHelper(
        const QJsonArray &arr, const QString &basepath, QString &out) const;

    /**
     * Adds an object of structured content.
     * @param      obj      The object of structured content.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this content will be appended to.
     */
    void addStructuredContentHelper(
        const QJsonObject &obj, const QString &basepath, QString &out) const;

    /**
     * Parses and outputs structured content to HTML.
     * The root of the structured content add parser.
     * @param      val      The JSON value of the structured content.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this content will be appended to.
     */
    void addStructuredContent(
        const QJsonValue &val, const QString &basepath, QString &out) const;

    /**
     * Displays an image type object.
     * @param      obj      The image object.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this image will be appended to.
     */
    void addImage(
        const QJsonObject &obj,
        const QString &basepath,
        QString &out) const;

    /**
     * Adds a text object to the HTML document.
     * @param      obj The text object.
     * @param[out] out The string the formatted text will be appended to.
     */
    void addText(const QJsonObject &obj, QString &out) const;

    /* if the label should display contents as a list */
    bool m_list;
};

#endif // GLOSSARYLABEL_H
