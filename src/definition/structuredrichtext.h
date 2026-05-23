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
#include <QQuickItem>
#include <QScreen>
#include <QSet>
#include <QString>
#include <QWindow>

#include "dict/data/dictionaryinfo.h"
#include "setting/keys.h"

/**
 * @brief Parses structred content into Qt friendly rich text.
 */
class StructuredRichText : public QObject
{
    Q_OBJECT

public:
    StructuredRichText(QObject *parent = nullptr);
    virtual ~StructuredRichText();

    /**
     * @brief Parse structred content into Qt rich text.
     *
     * @param name The name of the dictionary.
     * @param content The structured content to parse.
     * @param style The glossary style to display.
     * @param item The item this rich text will be displayed in.
     * @param font The font in use.
     * @return A string containing the structured content as rich text.
     */
    [[nodiscard]]
    Q_INVOKABLE QString parse(
        const DictionaryInfo *info,
        const QJsonArray &content,
        Setting::GlossaryStyle style,
        const QQuickItem *item,
        const QFont &font) const;

private:
    /**
     * @brief Current context of the StructuredRichText parser.
     */
    struct Context
    {
        /* The current DictionaryInfo */
        const DictionaryInfo *info{nullptr};

        /* The style to render v1 style terms in */
        Setting::GlossaryStyle style{
            Setting::GlossaryStyle::GlossaryStyleBullet
        };

        /* The current screen */
        const QScreen *screen{nullptr};

        /* The current font */
        QFont font;

        /* Font size in pixels of the root element */
        double rootFontPixelSize{12.0};

        /* Font size in pixels of the parent element */
        double parentFontPixelSize{12.0};

        /* Base path of resources for this dictionary */
        QString basepath;
    };

    /**
     * @brief Add structured data attributes to the string.
     *
     * @param obj The structured data attributes to parse.
     * @param[out] out The string to append the data attributes to.
     */
    void addStructuredData(const QJsonObject &obj, QString &out) const;

    /**
     * @brief Add structured style objects.
     *
     * @param obj The structured style object.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this style will be appended to.
     * @return The font size in pixels of the top level element.
     */
    [[nodiscard]]
    double addStructuredStyle(
        const QJsonObject &obj,
        StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Add string structured content.
     *
     * @param str The string to add.
     * @param[out] out The string this string will be appended to.
     */
    void addStructuredContentHelper(const QString &str, QString &out) const;

    /**
     * @brief Add an array of structured content.
     *
     * @param arr The array of structured content.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this content will be appended to.
     */
    void addStructuredContentHelper(
        const QJsonArray &arr,
        StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Add an object of structured content.
     *
     * @param obj The object of structured content.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this content will be appended to.
     */
    void addStructuredContentHelper(
        const QJsonObject &obj,
        StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Parses and outputs structured content to HTML.
     *
     * @param val The JSON value of the structured content.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this content will be appended to.
     */
    void addStructuredContent(
        const QJsonValue &val,
        StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Add an image type object.
     *
     * @param obj The image object.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this image will be appended to.
     */
    void addImage(
        const QJsonObject &obj,
        StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Add a text object to the HTML document.
     *
     * @param obj The text object.
     * @param[out] out The string the formatted text will be appended to.
     */
    void addText(const QJsonObject &obj, QString &out) const;

    /**
     * Determines if an array contains structured content.
     * @param content The array of content.
     * @return True if the array contains structured content, false otherwise.
     */
    [[nodiscard]]
    bool containsStructuredContent(const QJsonArray &content) const;

    /**
     * @brief Escape a string for use in HTML.
     *
     * @param str The string to escape.
     * @return An HTML escaped string.
     */
    [[nodiscard]]
    QString escapeHtml(const QString &str) const;

    /**
     * @brief Convert a structured data key to an HTML data attribute name.
     *
     * @param key The structured data key.
     * @return The HTML data attribute name.
     */
    [[nodiscard]]
    QString structuredDataAttributeName(const QString &key) const;

    /**
     * @brief Check if a structured content tag is supported.
     *
     * @param tag The structured content tag to check.
     * @return true if the tag is supported, false otherwise.
     */
    [[nodiscard]]
    bool isSupportedStructuredTag(const QString &tag) const;

    /**
     * @brief Convert a CSS font-size value to pixels.
     *
     * @param size The CSS font-size value.
     * @param screen The current screen.
     * @param font The font that relative CSS values are based on.
     * @return The converted size in pixels, or a negative value if unsupported.
     */
    [[nodiscard]]
    double cssFontSizeToPixels(
        const QString &size, const QScreen *screen, const QFont &font) const;

    /**
     * @brief Convert a CSS font-size value to pixels.
     *
     * @param size The CSS font-size value.
     * @param screen The current screen.
     * @param parentFontPixelSize The inherited font size in pixels.
     * @param rootFontPixelSize The root font size in pixels.
     * @return The converted size in pixels, or a negative value if unsupported.
     */
    [[nodiscard]]
    double cssFontSizeToPixels(
        const QString &size,
        const QScreen *screen,
        double parentFontPixelSize,
        double rootFontPixelSize) const;

    /**
     * @brief Get the pixel size for a QFont.
     *
     * @param font The font to measure.
     * @param screen The current screen.
     * @return The font size in pixels.
     */
    [[nodiscard]]
    double fontPixelSize(const QFont &font, const QScreen *screen) const;

    /**
     * @brief Get the current screen DPI used for font conversion.
     *
     * @return The current logical screen DPI, or 96 if unavailable.
     */
    [[nodiscard]]
    double screenDpi(const QScreen *screen) const;

    /**
     * @brief Format a pixel size for CSS output.
     *
     * @param size The pixel size.
     * @return The formatted size.
     */
    [[nodiscard]]
    QString formatPixelSize(double size) const;

    /* Set of supported structured content tags */
    const QSet<QString> m_supportedTags = {
        "br", "ruby", "rt", "rp", "table", "thead", "tbody", "tfoot",
        "tr", "td", "th", "span", "div", "ol", "ul", "li", "details",
        "summary", "img", "a"
    };
};
