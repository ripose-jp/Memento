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

#pragma once

#include <QObject>

#include <QFont>
#include <QJsonArray>
#include <QString>

#include "setting/keys.h"

class StructuredContent : public QObject
{
    Q_OBJECT

public:
    StructuredContent(QObject *parent = nullptr);
    virtual ~StructuredContent();

    [[nodiscard]]
    Q_INVOKABLE static QString parse(
        const QJsonArray &content,
        Setting::GlossaryStyle style,
        const QFont &font);

private:
    /**
     * @brief Adds structured style objects.
     * @param obj The structured style object.
     * @param[out] out The string this style will be appended to.
     */
    static void addStructuredStyle(const QJsonObject &obj, QString &out);

    /**
     * Adds string structured content.
     * @param str The string to add.
     * @param[out] out The string this string will be appended to.
     */
    static void addStructuredContentHelper(
        const QString &str, QString &out);

    /**
     * Adds an array of structured content.
     * @param arr The array of structured content.
     * @param basepath The base of the image path.
     * @param font The font being used.
     * @param[out] out The string this content will be appended to.
     */
    static void addStructuredContentHelper(
        const QJsonArray &arr,
        const QString &basepath,
        const QFont &font,
        QString &out);

    /**
     * Adds an object of structured content.
     * @param obj The object of structured content.
     * @param basepath The base of the image path.
     * @param font The font being used.
     * @param[out] out The string this content will be appended to.
     */
    static void addStructuredContentHelper(
        const QJsonObject &obj,
        const QString &basepath,
        const QFont &font,
        QString &out);

    /**
     * Parses and outputs structured content to HTML.
     * The root of the structured content add parser.
     * @param val The JSON value of the structured content.
     * @param basepath The base of the image path.
     * @param font The font being used.
     * @param[out] out The string this content will be appended to.
     */
    static void addStructuredContent(
        const QJsonValue &val,
        const QString &basepath,
        const QFont &font,
        QString &out);

    /**
     * Displays an image type object.
     * @param obj The image object.
     * @param basepath The base of the image path.
     * @param[out] out The string this image will be appended to.
     */
    static void addImage(
        const QJsonObject &obj, const QString &basepath, QString &out);

    /**
     * Adds a text object to the HTML document.
     * @param obj The text object.
     * @param style The style of glossaries.
     * @param[out] out The string the formatted text will be appended to.
     */
    static void addText(
        const QJsonObject &obj, Setting::GlossaryStyle style, QString &out);

    /**
     * Converts point size to pixel size.
     * @param pointSize The point size of the text.
     * @return The equivalent pixel size for the window.
     */
    [[nodiscard]]
    static int pointSizeToPixelSize(double pointSize);

    /**
     * Determines if an array contains structured content.
     * @param definitions The array of definitions.
     * @return True if the array contains structured content, false otherwise.
     */
    [[nodiscard]]
    static bool containsStructuredContent(const QJsonArray &definitions);
};
