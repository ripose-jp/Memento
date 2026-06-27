import QtQuick
import QtQuick.Controls
import Ripose.Memento

SearchableText {
    id: root

    enum TooltipType {
        None,
        Ruby,
        Title
    }

    required property TermDefinition definition
    required property color canvasColor

    readonly property string ankiSelectedText: root.cleanSelectedText(root.selectedText)

    signal searchRequested(query: string)

    /* Current type of tooltip */
    property int tooltipType: GlossaryText.TooltipType.None

    /* Raw link text of the tooltip */
    property string tooltipLink: ""

    /* Text of the current tooltip */
    property string tooltip: ""

    /* Text the current tooltip is attached to */
    property string tooltipText: ""

    /* Rectangle used for positioning ruby tool tips */
    property rect rubyTooltipRect: Qt.rect(0, 0, 0, 0)

    /* Position used for title tooltips */
    property point titleTooltipPosition: Qt.point(0, 0)

    /* true if the tooltip is visible, false otherwise */
    property bool tooltipVisible: false

    /**
     * Parse a query string from a link into a map.
     * @param link The link to parse.
     * @return The decoded link arguments.
     */
    function linkArgs(link) {
        const queryIndex = link.indexOf("?");
        if (queryIndex < 0)
        {
            return {};
        }

        let args = {};
        let pairs = link.substring(queryIndex + 1).split("&");
        for (let i = 0; i < pairs.length; ++i)
        {
            let pair = pairs[i].split("=");
            if (pair.length !== 2)
            {
                continue;
            }
            args[decodeURIComponent(pair[0])] =
                    decodeURIComponent(pair[1]);
        }
        return args;
    }

    /**
     * Convert a tooltip type name to the enum value.
     * @param tooltipType The tooltip type name.
     * @return The tooltip type enum value.
     */
    function toTooltipType(tooltipType) {
        switch (tooltipType)
        {
        case "ruby":
            return GlossaryText.TooltipType.Ruby;

        case "title":
            return GlossaryText.TooltipType.Title;

        default:
            return GlossaryText.TooltipType.None;
        }
    }

    /**
     * Open a link from the glossary.
     * @param link The link to open.
     */
    function openLink(link) {
        if (link.startsWith("memento://glossary-link?"))
        {
            const args = root.linkArgs(link);
            if (args.target)
            {
                root.openLink(args.target);
            }
            return;
        }

        if (link.startsWith("?"))
        {
            let query = "";

            const args = root.linkArgs(link);
            if (args.query)
            {
                query = args.query;
            }
            if (query.length > 0)
            {
                /* Hide the tooltip so it doesn't linger on transitions */
                root.tooltipVisible = false;

                root.searchRequested(query);
            }
        }
        else
        {
            Qt.openUrlExternally(link);
        }
    }

    /**
     * Update the ruby tooltip position to cover the full ruby base text.
     * @param text The ruby base text.
     */
    function updateRubyTooltipPosition(text) {
        if (root.hoverIndex < 0 || text.length <= 0)
        {
            root.rubyTooltipRect = Qt.rect(0, 0, 0, 0);
            return;
        }

        const start = root.plainText.lastIndexOf(
            text,
            Math.max(0, root.hoverIndex)
        );
        if (start < 0 || root.hoverIndex >= start + text.length)
        {
            root.rubyTooltipRect = root.positionToRectangle(root.hoverIndex);
            return;
        }

        const startRect = root.positionToRectangle(start);
        const endRect = root.positionToRectangle(start + text.length);
        if (Math.abs(startRect.y - endRect.y) > startRect.height)
        {
            root.rubyTooltipRect = startRect;
            return;
        }

        root.rubyTooltipRect = Qt.rect(
            startRect.x,
            startRect.y,
            Math.max(startRect.width, endRect.x - startRect.x),
            startRect.height
        );
    }

    /**
     * Return the x-coordinate for the current glossary tooltip.
     * @return The tooltip x-coordinate.
     */
    function tooltipX() {
        switch (root.tooltipType)
        {
        case GlossaryText.TooltipType.Ruby:
            return root.rubyTooltipRect.x +
                (root.rubyTooltipRect.width - glossaryToolTip.implicitWidth) / 2;

        case GlossaryText.TooltipType.Title:
        default:
            return root.titleTooltipPosition.x - glossaryToolTip.implicitWidth / 2;
        }
    }

    /**
     * Return the y-coordinate for the current glossary tooltip.
     * @return The tooltip y-coordinate.
     */
    function tooltipY() {
        switch (root.tooltipType)
        {
        case GlossaryText.TooltipType.Ruby:
            return root.rubyTooltipRect.y -
                glossaryToolTip.implicitHeight -
                glossaryToolTip.margin;

        case GlossaryText.TooltipType.Title:
        default:
        {
            const tooltipHeight = glossaryToolTip.implicitHeight > 0 ?
                glossaryToolTip.implicitHeight :
                glossaryToolTip.height;
            return root.titleTooltipPosition.y -
                tooltipHeight -
                glossaryToolTip.margin;
        }
        }
    }

    /**
     * Return true if a character is a Qt rich-text document sentinel.
     * @param ch The character to check.
     * @return true if the character should not be exported.
     */
    function isQtDocumentSentinel(ch) {
        const qtSentinelRangeStartCode = 0xFDD0;
        const qtSentinelRangeEndCode = 0xFDEF;
        const qtObjectReplacementCharacterCode = 0xFFFC;
        const code = ch.charCodeAt(0);
        return (code >= qtSentinelRangeStartCode &&
                code <= qtSentinelRangeEndCode) ||
                code === qtObjectReplacementCharacterCode;
    }

    /**
     * Remove generated glossary list markers and hidden rich-text sentinels.
     * @param text The raw selected text from TextEdit.
     * @return Text suitable for Anki marker output.
     */
    function cleanSelectedText(text) {
        const listItemSentinel = "\u2060";
        const listMarkerStartSentinel = "\u2061";
        const listMarkerEndSentinel = "\u2062";
        const paragraphSeparator = "\u2029";
        const noBreakSpace = "\u00A0";

        let out = [];
        let boundary = 0;
        let inListMarker = false;
        let inGeneratedListItem = false;
        let generatedListItemHasText = false;
        let discardSpacerAfterGeneratedListBoundary = false;

        for (let i = 0; i < text.length; ++i)
        {
            const ch = text.charAt(i);
            if (root.isQtDocumentSentinel(ch))
            {
                if (inGeneratedListItem && generatedListItemHasText)
                {
                    if (out.length > 0 && out[out.length - 1] !== "\n")
                    {
                        out.push("\n");
                    }
                    boundary = out.length;
                    inGeneratedListItem = false;
                    generatedListItemHasText = false;
                    discardSpacerAfterGeneratedListBoundary = true;
                }
                continue;
            }
            if (discardSpacerAfterGeneratedListBoundary)
            {
                if (ch === noBreakSpace || ch === " " || ch === "\t")
                {
                    continue;
                }
                discardSpacerAfterGeneratedListBoundary = false;
            }
            if (ch === listMarkerStartSentinel)
            {
                inListMarker = true;
                continue;
            }
            if (ch === listMarkerEndSentinel)
            {
                if (inListMarker)
                {
                    inListMarker = false;
                }
                else
                {
                    out.length = boundary;
                }
                continue;
            }
            if (inListMarker)
            {
                continue;
            }
            if (ch === listItemSentinel)
            {
                if (out.length > 0 && out[out.length - 1] !== "\n")
                {
                    out.push("\n");
                }
                boundary = out.length;
                inGeneratedListItem = true;
                generatedListItemHasText = false;
                continue;
            }
            out.push(ch === paragraphSeparator ? "\n" : ch);
            generatedListItemHasText = inGeneratedListItem;
        }

        return out.join("")
                  .replace(/[ \t]*\n[ \t]*/g, "\n")
                  .replace(/\n{2,}/g, "\n")
                  .trim();
    }

    /**
     * Copy the cleaned Anki selection text to the clipboard.
     */
    function copyAnkiSelection() {
        if (root.ankiSelectedText.length > 0)
        {
            clipboard.setText(root.ankiSelectedText);
        }
    }

    cursorShape: Qt.IBeamCursor
    textFormat: TextEdit.RichText
    wrapMode: TextEdit.Wrap
    selectByMouse: true
    selectByKeyboard: true

    font.family: MementoSettings.interfaceSearchGlossaryFont.family
    font.italic: MementoSettings.interfaceSearchGlossaryFont.italic
    font.underline: MementoSettings.interfaceSearchGlossaryFont.underline
    font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
    font.weight: MementoSettings.interfaceSearchGlossaryFont.weight
    font.overline: MementoSettings.interfaceSearchGlossaryFont.overline
    font.strikeout: MementoSettings.interfaceSearchGlossaryFont.strikeout
    font.letterSpacing: MementoSettings.interfaceSearchGlossaryFont.letterSpacing
    font.wordSpacing: MementoSettings.interfaceSearchGlossaryFont.wordSpacing
    font.kerning: MementoSettings.interfaceSearchGlossaryFont.kerning
    font.preferShaping: MementoSettings.interfaceSearchGlossaryFont.preferShaping
    font.hintingPreference: MementoSettings.interfaceSearchGlossaryFont.hintingPreference
    font.styleName: MementoSettings.interfaceSearchGlossaryFont.styleName

    text: StructuredRichText.parse(
              root.definition?.dictionaryInfo,
              root.definition?.glossary ?? [],
              MementoSettings.searchGlossaryStyle,
              root,
              root.font,
              root.color,
              root.canvasColor)

    onLinkActivated: function(link) {
        root.openLink(link);
    }

    onLinkHovered: function(link) {
        if (link.startsWith("memento://glossary-link?"))
        {
            const args = root.linkArgs(link);
            const linkChanged = root.tooltipLink !== link;

            root.tooltip = args.tooltip ? args.tooltip : "";
            root.tooltipText = args.tooltipText ? args.tooltipText : "";
            root.tooltipType = root.toTooltipType(args.tooltipType);

            if (root.tooltip.length > 0 &&
                root.tooltipType === GlossaryText.TooltipType.Title)
            {
                if (linkChanged)
                {
                    titleTooltipTimer.stop();
                    root.tooltipVisible = false;
                    titleTooltipTimer.start();
                }
                else if (!root.tooltipVisible &&
                         !titleTooltipTimer.running)
                {
                    titleTooltipTimer.start();
                }
            }
            else if (root.tooltip.length > 0 &&
                root.tooltipType === GlossaryText.TooltipType.Ruby)
            {
                titleTooltipTimer.stop();
                root.tooltipVisible = true;
                root.updateRubyTooltipPosition(root.tooltipText);
            }

            root.tooltipLink = link;
        }
        else
        {
            titleTooltipTimer.stop();
            root.tooltip = "";
            root.tooltipText = "";
            root.tooltipType = GlossaryText.TooltipType.None;
            root.tooltipVisible = false;
            root.tooltipLink = "";
        }
    }

    onHoverIndexChanged: {
        if (root.tooltip.length > 0 &&
            root.tooltipType === GlossaryText.TooltipType.Ruby)
        {
            root.updateRubyTooltipPosition(root.tooltipText);
        }
    }

    Keys.onPressed: function(event) {
        if (event.matches(StandardKey.Copy) && root.selectedText.length > 0)
        {
            root.copyAnkiSelection();
            event.accepted = true;
        }
    }

    Clipboard {
        id: clipboard
    }

    Timer {
        id: titleTooltipTimer

        interval: 500
        repeat: false
        onTriggered: {
            if (root.tooltip.length > 0 &&
                root.tooltipType === GlossaryText.TooltipType.Title)
            {
                root.titleTooltipPosition = root.mousePosition;
                root.tooltipVisible = true;
            }
        }
    }

    ToolTip {
        id: glossaryToolTip

        readonly property real margin: 4

        visible: root.visible &&
                 root.tooltip.length > 0 &&
                 root.tooltipVisible
        delay: 0
        text: root.tooltip
        x: root.tooltipX()
        y: root.tooltipY()
    }
}
