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

#include <array>
#include <memory>

#include <QObject>

#include <QColor>
#include <QFont>
#include <QHash>
#include <QJsonArray>
#include <QList>
#include <QMap>
#include <QQuickItem>
#include <QRegularExpression>
#include <QScreen>
#include <QSet>
#include <QString>
#include <QStringList>
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
     * @param info Information about the dictionary being rendered.
     * @param content The structured content to parse.
     * @param style The glossary style to display.
     * @param item The item this rich text will be displayed in.
     * @param font The font in use.
     * @param color The glossary text color.
     * @param backgroundColor The glossary canvas color.
     * @return A string containing the structured content as rich text.
     */
    [[nodiscard]]
    Q_INVOKABLE QString parse(
        const DictionaryInfo *info,
        const QJsonArray &content,
        Setting::GlossaryStyle style,
        const QQuickItem *item,
        const QFont &font,
        const QColor &color,
        const QColor &backgroundColor) const;

private:
    /**
     * @brief A structured content element used for CSS selector matching.
     */
    struct StructuredElement
    {
        /* The element tag */
        QString tag;

        /* Attributes used by stylesheet selectors */
        QHash<QString, QString> attributes;

        /* Generated classes used by stylesheet selectors */
        QSet<QString> classes;

        /* Index of the previous element sibling in selectorElements, or -1 */
        qsizetype previousSibling{-1};

        /* One-based element-child index among siblings */
        qsizetype childIndex{0};

        /* Total element-child count in this element's parent */
        qsizetype childCount{0};
    };

    /**
     * @brief Element sibling state for the current content container.
     */
    struct SiblingState
    {
        /* Index of the most recently rendered element child in
         * selectorElements */
        qsizetype previousElement{-1};

        /* Number of element children encountered in this sibling group */
        qsizetype visitedElementCount{0};

        /* Total number of element children in this sibling group */
        qsizetype elementCount{0};

        /* Largest positive margin waiting to be emitted */
        double pendingPositiveMarginPixels{0.0};

        /* Smallest negative margin waiting to be emitted */
        double pendingNegativeMarginPixels{0.0};
    };

    /**
     * @brief List rendering state.
     */
    struct StructuredList
    {
        /* Parent list tag */
        QString tag;

        /* Parent marker type */
        QString marker;

        /* Current item index */
        qsizetype item{0};
    };

    using CssRule = DictionaryStyles::CssRule;
    using CssDeclaration = DictionaryStyles::CssDeclaration;
    using CssSelectorPart = DictionaryStyles::CssSelectorPart;
    using ParsedStylesheet = DictionaryStyles::ParsedStylesheet;
    using CssDeclarations = QMap<QString, QString>;

    /**
     * @brief Physical side of a CSS box in clockwise order.
     */
    enum class BoxSide : std::size_t
    {
        Top,
        Right,
        Bottom,
        Left,
        Count /* Only used to determine the number of sides */
    };

    /**
     * @brief Qt-compatible layout selected for a structured element.
     */
    enum class ElementLayout
    {
        Inline,
        Block,
        List,
        MarkedListItem,
        MarkerlessListItem,
        Box
    };

    /**
     * @brief Width behavior used by an emitted compatibility box.
     */
    enum class WidthPolicy
    {
        Natural,
        Fill,
        FitContent,
        Explicit
    };

    /**
     * @brief Vertical margin behavior of a rendered element.
     */
    enum class MarginFlow
    {
        Inline,
        Collapsible,
        Contained,
        PropagateLastChild
    };

    /**
     * @brief Resolved border data for one side of a CSS box.
     */
    struct BorderSide
    {
        /* Resolved border color */
        QString color;

        /* Resolved border style */
        QString style;

        /* Border width converted to pixels */
        double widthPixels{0.0};

        /* True when a compatibility frame paints this border */
        bool painted{false};
    };

    /**
     * @brief Normalized CSS box used by the Qt compatibility renderer.
     */
    struct BoxStyle
    {
        /* Margins in top, right, bottom, left order */
        std::array<QString, static_cast<std::size_t>(BoxSide::Count)> margins{};

        /* Padding widths in top, right, bottom, left order, in pixels */
        std::array<double, static_cast<std::size_t>(BoxSide::Count)> padding{};

        /* Border definitions in top, right, bottom, left order */
        std::array<
            BorderSide,
            static_cast<std::size_t>(BoxSide::Count)
        > borders{};

        /* Opaque background color painted by the box frame */
        QString backgroundColor;

        /* True when the element needs a table-backed compatibility box */
        bool enabled{false};

        /* True when all painted borders can share one compact frame */
        bool compactBorderFrame{false};
    };

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

        /* Text color of the parent element */
        QString textColor;

        /* Global glossary text color used by palette CSS variables */
        QString glossaryTextColor;

        /* Background color of the glossary canvas */
        QString backgroundColor;

        /* Global glossary background color used by palette CSS variables */
        QString glossaryBackgroundColor;

        /* Nearest opaque background painted around the current content */
        QString paintedBackgroundColor;

        /* Base path of resources for this dictionary */
        QString basepath;

        /* CSS rules parsed from the dictionary stylesheet */
        std::shared_ptr<const DictionaryStyles> dictionaryStyles;

        /* True when selectors need total element-child counts */
        bool needsElementChildCount{false};

        /* Storage containing selector elements for the current render */
        QList<StructuredElement> selectorElements;

        /* Stack of indexes into selectorElements */
        QList<qsizetype> elements;

        /* Stack of sibling state for structured content containers */
        QList<SiblingState> siblings;

        /* Stack of lists being rendered */
        QList<StructuredList> lists;

        /* CSS values resolved during this render, keyed by source value */
        QHash<QString, QString> resolvedCssValues;

        /* Inherited text layout direction */
        Qt::LayoutDirection textDirection{Qt::LeftToRight};

        /* The href inherited from a parent anchor */
        QString linkHref;

        /* The title tooltip inherited from a parent element */
        QString titleTooltip;
    };

    /**
     * @brief Resolved state used to render a structured element.
     */
    struct ElementRenderState
    {
        /* Original structured content tag used for selector matching */
        QString tag;

        /* Qt-compatible HTML tag emitted for the element */
        QString outputTag;

        /* HTML attributes emitted on the outer element */
        QString attributes;

        /* CSS declarations emitted on the outer element */
        CssDeclarations declarations;

        /* CSS declarations moved to a table-backed content cell */
        CssDeclarations cellDeclarations;

        /* Normalized CSS box used by the compatibility emitter */
        BoxStyle box;

        /* Qt-compatible layout selected for the element */
        ElementLayout layout{ElementLayout::Inline};

        /* Width behavior of the element or compatibility box */
        WidthPolicy widthPolicy{WidthPolicy::Natural};

        /* Vertical margin behavior selected for the element */
        MarginFlow marginFlow{MarginFlow::Inline};

        /* Resolved font size inherited by child content */
        double fontPixelSize{0.0};

        /* Resolved text color inherited by child content */
        QString textColor;

        /* Resolved title tooltip inherited by child content */
        QString titleTooltip;

        /* Background color inherited by child content */
        QString paintedBackgroundColor;

        /* Generated ::before content resolved with the element styles */
        QString beforeContent;

        /* Generated ::after content resolved with the element styles */
        QString afterContent;

        /* Visible marker emitted for the current list item */
        QString listMarker;

        /* Marker type inherited by child list items */
        QString listMarkerType;

        /* Resolved layout direction inherited by child content */
        Qt::LayoutDirection textDirection{Qt::LeftToRight};

        /* True when a known label span needs non-breaking side padding */
        bool paddedSpan{false};

        /* True when the element contains only a text string */
        bool textOnlyContent{false};

        /* Inline spacing before text-only block content */
        QString contentInlineSpacingBefore;

        /* Inline spacing after text-only block content */
        QString contentInlineSpacingAfter;

        /* Inline spacing emitted before the element */
        QString inlineSpacingBefore;

        /* Inline spacing emitted after the element */
        QString inlineSpacingAfter;
    };

    /**
     * @brief Add structured data attributes to the string.
     *
     * @param obj The structured data attributes to parse.
     * @param[out] out The string to append the data attributes to.
     */
    void addStructuredData(const QJsonObject &obj, QString &out) const;

    /**
     * @brief Add structured style objects to an element render state.
     *
     * @param obj The structured style object.
     * @param ctx The StructuredRichText context.
     * @param[out] state The element render state to update.
     */
    void addStructuredStyle(
        const QJsonObject &obj,
        StructuredRichText::Context &ctx,
        ElementRenderState &state) const;

    /**
     * @brief Add string structured content.
     *
     * @param str The string to add.
     * @param[out] out The string this string will be appended to.
     */
    void addStructuredContentHelper(const QString &str, QString &out) const;

    /**
     * @brief Add string structured content, inheriting a parent link if
     * present.
     *
     * @param str The string to add.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this string will be appended to.
     */
    void addStructuredContentHelper(
        const QString &str,
        StructuredRichText::Context &ctx,
        QString &out) const;

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
     * @brief Add children with a new element-sibling context.
     *
     * @param val The child structured content.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this content will be appended to.
     */
    void addStructuredChildren(
        const QJsonValue &val,
        StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Count direct element children used by CSS sibling selectors.
     *
     * @param val The structured content value to inspect.
     * @return Number of supported structured elements in this child group.
     */
    [[nodiscard]]
    qsizetype structuredElementChildCount(const QJsonValue &val) const;

    /**
     * @brief Render a normal structured element.
     *
     * @param obj The structured content object.
     * @param tag The original structured content tag.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this content will be appended to.
     */
    void addStructuredElement(
        const QJsonObject &obj,
        const QString &tag,
        StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Resolve the attributes, styles, and layout for an element.
     *
     * @param obj The structured content object.
     * @param tag The original structured content tag.
     * @param ctx The StructuredRichText context.
     * @return The resolved element render state.
     */
    [[nodiscard]]
    ElementRenderState elementRenderState(
        const QJsonObject &obj,
        const QString &tag,
        StructuredRichText::Context &ctx) const;

    /**
     * @brief Add HTML attributes from a structured content object.
     *
     * @param obj The structured content object.
     * @param[out] state The element render state to update.
     */
    void addElementAttributes(
        const QJsonObject &obj,
        ElementRenderState &state) const;

    /**
     * @brief Resolve matched and inline styles for an element.
     *
     * @param obj The structured content object.
     * @param ctx The StructuredRichText context.
     * @param[out] state The element render state to update.
     */
    void resolveElementStyles(
        const QJsonObject &obj,
        StructuredRichText::Context &ctx,
        ElementRenderState &state) const;

    /**
     * @brief Resolve list state and the Qt-compatible output tag.
     *
     * @param ctx The StructuredRichText context.
     * @param[out] state The element render state to update.
     */
    void resolveElementLayout(
        StructuredRichText::Context &ctx,
        ElementRenderState &state) const;

    /**
     * @brief Apply Qt rich text compatibility rewrites to an element.
     *
     * @param ctx The StructuredRichText context.
     * @param[out] state The element render state to update.
     */
    void applyElementCompatibility(
        const StructuredRichText::Context &ctx,
        ElementRenderState &state) const;

    /**
     * @brief Move CSS box declarations into normalized render state.
     *
     * @param ctx The StructuredRichText context.
     * @param[out] state The element render state to update.
     */
    void resolveBoxStyle(
        const StructuredRichText::Context &ctx,
        ElementRenderState &state) const;

    /**
     * @brief Select vertical margin behavior after box normalization.
     *
     * @param state The resolved element render state.
     * @return The margin flow for the element.
     */
    [[nodiscard]]
    MarginFlow marginFlow(const ElementRenderState &state) const;

    /**
     * @brief Add the opening HTML for a structured element.
     *
     * @param state The element render state.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this HTML will be appended to.
     */
    void addStructuredElementStart(
        const ElementRenderState &state,
        const StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Add the closing HTML for a structured element.
     *
     * @param state The element render state.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this HTML will be appended to.
     */
    void addStructuredElementEnd(
        const ElementRenderState &state,
        const StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Add a zero-width selection sentinel to generated rich text.
     *
     * @param sentinel The sentinel character to add.
     * @param[out] out The string this HTML will be appended to.
     */
    void addSelectionSentinel(char16_t sentinel, QString &out) const;

    /**
     * @brief Add the opening table frame for a normalized CSS box.
     *
     * @param state The resolved element render state.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this HTML will be appended to.
     */
    void addBoxStart(
        const ElementRenderState &state,
        const StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Close the table frame for a normalized CSS box.
     *
     * @param state The resolved element render state.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this HTML will be appended to.
     */
    void addBoxEnd(
        const ElementRenderState &state,
        const StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Add an inline spacer supported by Qt rich text.
     *
     * @param spacing The CSS spacing value.
     * @param fontPixelSize The element font size used for relative spacing.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this spacer will be appended to.
     */
    void addInlineSpacer(
        const QString &spacing,
        double fontPixelSize,
        const StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Add a vertical spacer supported by Qt rich text.
     *
     * @param spacing The CSS spacing value.
     * @param fontPixelSize The element font size used for relative spacing.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this spacer will be appended to.
     */
    void addVerticalSpacer(
        const QString &spacing,
        double fontPixelSize,
        const StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Emit a vertical spacer with a deterministic rendered height.
     *
     * @param pixelSize The signed spacer height in pixels.
     * @param[out] out The string this spacer will be appended to.
     */
    void addVerticalPixelSpacer(double pixelSize, QString &out) const;

    /**
     * @brief Add a margin to a pending CSS margin-collapse group.
     *
     * @param spacing The CSS margin value.
     * @param fontPixelSize The element font size used for relative spacing.
     * @param ctx The StructuredRichText context.
     * @param[out] siblings The sibling state receiving the margin.
     */
    void addPendingVerticalMargin(
        const QString &spacing,
        double fontPixelSize,
        const StructuredRichText::Context &ctx,
        SiblingState &siblings) const;

    /**
     * @brief Merge one pending CSS margin-collapse group into another.
     *
     * @param source The pending margins to merge.
     * @param[out] destination The sibling state receiving the margins.
     */
    void mergePendingVerticalMargins(
        const SiblingState &source,
        SiblingState &destination) const;

    /**
     * @brief Emit and clear the current pending collapsed margin.
     *
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this spacer will be appended to.
     */
    void flushPendingVerticalMargin(
        StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Clear pending collapsed margins from a sibling state.
     *
     * @param[out] siblings The sibling state to clear.
     */
    void clearPendingVerticalMargins(SiblingState &siblings) const;

    /**
     * @brief Get one side from a one-to-four-value CSS box shorthand.
     *
     * @param value The CSS shorthand value.
     * @param side The side index in top, right, bottom, left order.
     * @return The value for the requested side, or an empty string.
     */
    [[nodiscard]]
    QString cssBoxSideValue(const QString &value, qsizetype side) const;

    /**
     * @brief Check whether a CSS spacing value is zero.
     *
     * @param spacing The CSS spacing value.
     * @return true if the value represents zero spacing.
     */
    [[nodiscard]]
    bool isZeroSpacing(const QString &spacing) const;

    /**
     * @brief Add a ruby structured content object.
     *
     * @param obj The ruby object to add.
     * @param ctx The StructuredRichText context.
     * @param[out] out The string this content will be appended to.
     */
    void addRuby(
        const QJsonObject &obj,
        StructuredRichText::Context &ctx,
        QString &out) const;

    /**
     * @brief Split ruby content into visible base text and hidden reading.
     *
     * @param val The ruby content value.
     * @param[out] base The visible ruby base content.
     * @param[out] reading The hidden ruby reading.
     */
    void splitRubyContent(
        const QJsonValue &val,
        QJsonArray &base,
        QString &reading) const;

    /**
     * @brief Convert structured content to plain text.
     *
     * @param val The structured content value.
     * @return The plain text content.
     */
    [[nodiscard]]
    QString structuredContentText(const QJsonValue &val) const;

    /**
     * @brief Check if structured content contains a ruby tag.
     *
     * @param val The structured content value.
     * @return true if ruby content exists.
     */
    [[nodiscard]]
    bool containsRuby(const QJsonValue &val) const;

    /**
     * @brief Check if an anchor needs custom tooltip rendering.
     *
     * @param obj The structured content anchor object.
     * @param ctx The StructuredRichText context.
     * @return true if the anchor should be rendered through tooltip-aware code.
     */
    [[nodiscard]]
    bool anchorNeedsTooltipHandling(
        const QJsonObject &obj,
        const StructuredRichText::Context &ctx) const;

    /**
     * @brief Build an internal href carrying optional target and tooltip data.
     *
     * @param target The normal link target.
     * @param tooltip The tooltip text.
     * @param tooltipText The visible text covered by the tooltip.
     * @param tooltipType The type of tooltip to display.
     * @return The internal href.
     */
    [[nodiscard]]
    QString internalLinkHref(
        const QString &target,
        const QString &tooltip,
        const QString &tooltipText,
        const QString &tooltipType) const;

    /**
     * @brief Add an anchor tag.
     *
     * @param href The link href.
     * @param color The concrete text color used to suppress default link
     * styling.
     * @param[out] out The string this anchor will be appended to.
     */
    void addAnchorStart(
        const QString &href,
        const QString &color,
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
     * @brief Add a CSS declaration to a declaration map.
     *
     * @param property The CSS property.
     * @param value The CSS value.
     * @param[out] declarations The declaration map to append to.
     */
    void addCssDeclaration(
        const QString &property,
        const QString &value,
        CssDeclarations &declarations) const;

    /**
     * @brief Resolve a CSS value before adding it to a declaration map.
     *
     * @param property The CSS property.
     * @param value The unresolved CSS value.
     * @param ctx The StructuredRichText context.
     * @param[out] declarations The declaration map to append to.
     */
    void addResolvedCssDeclaration(
        const QString &property,
        const QString &value,
        StructuredRichText::Context &ctx,
        CssDeclarations &declarations) const;

    /**
     * @brief Add declarations to a CSS style attribute.
     *
     * @param declarations The declarations to output.
     * @param[out] out The string to append CSS to.
     */
    void addCssDeclarations(
        const CssDeclarations &declarations, QString &out) const;

    /**
     * @brief Resolve browser CSS values to values supported by Qt rich text.
     *
     * @param value The browser CSS value.
     * @param ctx The StructuredRichText context.
     * @return A Qt-compatible value, or an empty string if unsupported.
     */
    [[nodiscard]]
    QString resolveCssValue(
        const QString &value,
        StructuredRichText::Context &ctx) const;

    /**
     * @brief Resolve CSS variable references in a declaration value.
     *
     * @param value The declaration value.
     * @param ctx The StructuredRichText context.
     * @return The value with supported variables replaced.
     */
    [[nodiscard]]
    QString resolveCssVariables(
        QString value,
        const StructuredRichText::Context &ctx) const;

    /**
     * @brief Resolve supported calc functions embedded in a CSS value.
     *
     * @param value The declaration value.
     * @return The value with calculations replaced, or an empty string if
     * unsupported.
     */
    [[nodiscard]]
    QString resolveCssCalculations(QString value) const;

    /**
     * @brief Resolve a color-mix function to a concrete color.
     *
     * @param value The color-mix value.
     * @return The mixed color, or an empty string if invalid.
     */
    [[nodiscard]]
    QString resolveColorMix(const QString &value) const;

    /**
     * @brief Resolve a simple calc division to a concrete length.
     *
     * @param value The calc value.
     * @return The calculated length, or an empty string if unsupported.
     */
    [[nodiscard]]
    QString resolveCssCalc(const QString &value) const;

    /**
     * @brief Select a solid-color fallback from a CSS gradient.
     *
     * @param value The gradient value.
     * @return The fallback color, or an empty string if unsupported.
     */
    [[nodiscard]]
    QString cssGradientFallback(const QString &value) const;

    /**
     * @brief Split a CSS function argument list at top-level commas.
     *
     * @param arguments The function argument text.
     * @return The trimmed function arguments.
     */
    [[nodiscard]]
    QStringList splitCssArguments(const QString &arguments) const;

    /**
     * @brief Normalize a list marker value.
     *
     * @param marker The CSS list marker value.
     * @return The normalized marker.
     */
    [[nodiscard]]
    QString normalizeListMarker(QString marker) const;

    /**
     * @brief Get the default marker for a list tag.
     *
     * @param tag The list tag.
     * @return The default marker type.
     */
    [[nodiscard]]
    QString defaultListMarker(const QString &tag) const;

    /**
     * @brief Get a rendered marker for a list item.
     *
     * @param list The list state.
     * @param marker The normalized item marker override.
     * @return The marker text.
     */
    [[nodiscard]]
    QString listMarker(const StructuredList &list, const QString &marker) const;

    /**
     * @brief Check if the list marker is a built-in marker.
     *
     * @param marker The list marker type.
     * @return true if the marker should be formatted from the item index.
     */
    [[nodiscard]]
    bool isBuiltInListMarker(const QString &marker) const;

    /**
     * @brief Apply matching stylesheet rules to a declaration map.
     *
     * @param ctx The StructuredRichText context.
     * @param[out] declarations The declaration map to update.
     * @param[out] beforeContent Generated ::before content, when requested.
     */
    void addMatchingCssRules(
        StructuredRichText::Context &ctx,
        CssDeclarations &declarations,
        QString *beforeContent = nullptr,
        QString *afterContent = nullptr) const;

    /**
     * @brief Check if a rule matches the current element stack.
     *
     * @param rule The CSS rule to check.
     * @param ctx The current render context and selector storage.
     * @return true if the rule matches, false otherwise.
     */
    [[nodiscard]]
    bool cssRuleMatches(
        const CssRule &rule,
        const Context &ctx) const;

    /**
     * @brief Match a selector recursively from a candidate element.
     *
     * @param rule The CSS rule being matched.
     * @param ctx The current render context and selector storage.
     * @param partIndex The selector part being matched.
     * @param stackIndex The candidate element's depth in the stack.
     * @param selectorElementIndex The candidate element's index in
     * selectorElements.
     * @return true if this part and all preceding parts match.
     */
    [[nodiscard]]
    bool cssRuleMatchesAt(
        const CssRule &rule,
        const Context &ctx,
        qsizetype partIndex,
        qsizetype stackIndex,
        qsizetype selectorElementIndex) const;

    /**
     * @brief Check if a selector part matches an element.
     *
     * @param part The selector part to check.
     * @param element The element to check.
     * @return true if the selector part matches, false otherwise.
     */
    [[nodiscard]]
    bool cssSelectorPartMatches(
        const CssSelectorPart &part,
        const StructuredElement &element) const;

    /**
     * @brief Convert structured data to element attributes.
     *
     * @param obj The structured content object.
     * @param ctx The StructuredRichText context.
     * @return The index of the structured element in selectorElements.
     */
    [[nodiscard]]
    qsizetype structuredElement(
        const QJsonObject &obj,
        StructuredRichText::Context &ctx) const;

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
        "br", "ruby", "rp", "rt", "table", "thead", "tbody", "tfoot", "tr",
        "td", "th", "span", "div", "ol", "ul", "li", "details", "summary",
        "img", "a"
    };

    /* Set of supported CSS properties */
    const QSet<QString> m_supportedCssProperties = {
        "background-color", "border", "border-bottom",
        "border-bottom-color", "border-bottom-style",
        "border-bottom-width", "border-collapse", "border-color",
        "border-left", "border-left-color", "border-left-style",
        "border-left-width", "border-radius", "border-right",
        "border-right-color", "border-right-style",
        "border-right-width", "border-style", "border-top", "display",
        "border-top-color", "border-top-style", "border-top-width",
        "border-width", "color", "cursor", "float", "font",
        "font-family", "font-kerning", "font-size", "font-style",
        "font-variant", "font-weight", "image-rendering",
        "line-height", "list-style-type", "margin-bottom",
        "margin-left", "margin-right", "margin-top", "padding",
        "padding-bottom", "padding-left", "padding-right",
        "padding-top", "text-align", "text-decoration",
        "text-indent", "text-transform", "vertical-align",
        "white-space", "width", "word-spacing"
    };

    /* Matches CSS function names */
    const QRegularExpression m_cssFunctionRegex{
        "\\b([A-Za-z-]+)\\s*\\("
    };

    /* CSS functions that are supported by Qt rich text */
    const QSet<QString> m_supportedCssFunctions = {
        "hsl", "hsla", "rgb", "rgba"
    };
};
