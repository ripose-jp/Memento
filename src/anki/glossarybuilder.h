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

#ifndef GLOSSARYBUILDER_H
#define GLOSSARYBUILDER_H

#include <QString>
#include <QPair>
#include <QList>
#include <QJsonArray>

class GlossaryBuilder
{
public:
    /**
     * Sets the contents of this label.
     * @param      definitions The definitions to add to this label.
     * @param      basepath    The path where all external resources begin.
     * @param[out] fileMap     A mapping of files to file names.
     * @return A list of HTML formatted glossary entries.
     */
    static QStringList buildGlossary(
        const QJsonArray &definitions,
        QString basepath,
        QList<QPair<QString, QString>> &fileMap);

private:
    GlossaryBuilder() {}

    /**
     * Adds structured data attributes to the string.
     * @param      obj The structured data attributes to parse.
     * @param[out] out The string to append the data attributes to.
     */
    static void addStructuredData(const QJsonObject &obj, QString &out);

    /**
     * @brief Adds structured style objects.
     * @param      obj The structured style object.
     * @param[out] out The string this style will be appended to.
     */
    static void addStructuredStyle(const QJsonObject &obj, QString &out);

    /**
     * Adds string structured content.
     * @param      str The string to add.
     * @param[out] out The string this string will be appended to.
     */
    static void addStructuredContentHelper(const QString &str, QString &out);

    /**
     * Adds an array of structured content.
     * @param      arr      The array of structured content.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this content will be appended to.
     * @param[out] fileMap  A mapping of files to filenames.
     */
    static void addStructuredContentHelper(
        const QJsonArray &arr,
        const QString &basepath,
        QString &out,
        QList<QPair<QString, QString>> &fileMap);

    /**
     * Adds an object of structured content.
     * @param      obj      The object of structured content.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this content will be appended to.
     * @param[out] fileMap  A mapping of files to filenames.
     */
    static void addStructuredContentHelper(
        const QJsonObject &obj,
        const QString &basepath,
        QString &out,
        QList<QPair<QString, QString>> &fileMap);

    /**
     * Parses and outputs structured content to HTML.
     * The root of the structured content add parser.
     * @param      val      The JSON value of the structured content.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this content will be appended to.
     * @param[out] fileMap  A mapping of files to filenames.
     */
    static void addStructuredContent(
        const QJsonValue &val,
        const QString &basepath,
        QString &out,
        QList<QPair<QString, QString>> &fileMap);

    /**
     * Displays an image type object.
     * @param      obj      The image object.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this image will be appended to.
     * @param[out] fileMap  A mapping of files to filenames.
     */
    static void addImage(
        const QJsonObject &obj,
        const QString &basepath,
        QString &out,
        QList<QPair<QString, QString>> &fileMap);

    /**
     * Adds a text object to the HTML document.
     * @param      obj The text object.
     * @param[out] out The string the formatted text will be appended to.
     */
    static void addText(const QJsonObject &obj, QString &out);

    /**
     * Adds a file to the file map and returns its mapped filename.
     * @param      basepath The base path all files will be found at.
     * @param      path     The relative path of the file to add.
     * @param[out] fileMap  The map to add the file to.
     */
    static QString addFile(
        QString basepath,
        const QString &path,
        QList<QPair<QString, QString>> &fileMap);
};

#endif // GLOSSARYLABEL_H
