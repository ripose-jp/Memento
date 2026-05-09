import QtQuick
import Ripose.Memento

SelectableLabel {
    id: root

    property string expression: ""

    signal kanjiClicked(kanji: string)
    onLinkActivated: (ch) => root.kanjiClicked(ch)

    HoverHandler {
        cursorShape: root.hoveredLink ? Qt.PointingHandCursor : Qt.IBeamCursor
    }

    /**
     * Get if the character is kanji.
     * @param ch The character to test.
     * @return true if the character is kanji,
     * @return false otherwise.
     */
    function isKanji(ch) {
        const code = ch.charCodeAt(0);
        return (code >= 0x4e00 && code <= 0x9faf) || // Common Kanji
                (code >= 0x3400 && code <= 0x4dbf);    // Rare/Extension A Kanji
    }

    textFormat: TextEdit.RichText
    text: {
        const body = [...root.expression].map(
            ch => root.isKanji(ch) ? `<a href='${ch}'>${ch}</a>` : ch).join("");
        return `<html><head><style>a { text-decoration: none; color: ${root.color}; }</style></head><body>${body}</body></html>`;
    }
}
