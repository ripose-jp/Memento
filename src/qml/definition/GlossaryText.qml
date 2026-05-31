import QtQuick
import QtQuick.Controls
import Ripose.Memento

SearchableText {
    id: root

    property TermDefinition definition: null

    signal searchRequested(query: string)

    /* Information about <ruby> tooltips */
    property string rubyTooltip: ""
    property rect rubyTooltipRect: Qt.rect(0, 0, 0, 0)
    property string rubyTooltipText: ""

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
              root.color)

    onLinkActivated: function(link) {
        root.openLink(link);
    }

    onLinkHovered: function(link) {
        if (link.startsWith("memento://glossary-link?"))
        {
            const args = root.linkArgs(link);
            root.rubyTooltip = args.tooltip ? args.tooltip : "";
            root.rubyTooltipText = args.text ? args.text : "";
            if (root.rubyTooltip.length > 0)
            {
                root.updateRubyTooltipPosition(root.rubyTooltipText);
            }
        }
        else
        {
            root.rubyTooltip = "";
            root.rubyTooltipText = "";
        }
    }

    onHoverIndexChanged: {
        if (root.rubyTooltip.length > 0)
        {
            root.updateRubyTooltipPosition(root.rubyTooltipText);
        }
    }

    ToolTip {
        id: rubyToolTip

        readonly property real margin: 4

        visible: root.rubyTooltip.length > 0
        delay: 0
        text: root.rubyTooltip
        x: Math.max(0, Math.min(
            root.rubyTooltipRect.x +
                (root.rubyTooltipRect.width - implicitWidth) / 2,
            root.width - implicitWidth
        ))
        y: Math.max(0, root.rubyTooltipRect.y - implicitHeight - margin)
    }
}
