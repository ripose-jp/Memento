import QtQuick
import Ripose.Memento

SearchableText {
    id: root

    property TermDefinition definition: null

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

    text: StructuredContent.parse(
              root.definition?.dictionaryInfo?.name ?? "",
              root.definition?.glossary ?? [],
              MementoSettings.searchGlossaryStyle,
              root.font)
}
