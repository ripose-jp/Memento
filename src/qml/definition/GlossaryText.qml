import QtQuick
import Ripose.Memento

SearchableText {
    id: root

    property TermDefinition definition: null

    signal searchRequested(query: string)

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
              root.font)

    onLinkActivated: function(link) {
        if (link.startsWith("?"))
        {
            let query = "";
            let wildcards = false;

            let args = link.substring(1).split("&").map(v => v.split("="));
            for (let i = 0; i < args.length; ++i)
            {
                if (args[i].length !== 2)
                {
                    continue;
                }
                let argName = decodeURIComponent(args[i][0]);
                let argValue = decodeURIComponent(args[i][1]);

                if (argName === "query")
                {
                    query = argValue;
                }
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
}
