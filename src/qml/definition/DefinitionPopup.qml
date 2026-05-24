import QtQuick
import QtQuick.Controls
import Ripose.Memento

Popup {
    id: root

    readonly property DictionarySearch search: dictionarySearch

    readonly property int matchLength: {
        if (dictionarySearch.terms.length > 0)
        {
            return dictionarySearch.terms[0].clozeBody.length;
        }
        else if (dictionarySearch.kanji)
        {
            return 1;
        }
        return 0;
    }

    readonly property bool shouldOpen: dictionarySearch.terms.length > 0 || dictionarySearch.kanji

    width: MementoSettings.interfacePopupWidth
    height: MementoSettings.interfacePopupHeight

    onShouldOpenChanged: root.shouldOpen ? root.open() : root.close()
    onClosed: dictionarySearch.clearResults()

    Rectangle {
        id: borderRectangle
        anchors.fill: parent
        color: "transparent"
        border.color: MementoPalette.border
        border.width: Features.platform === Features.Linux ? 1 : 0

        DefinitionPage {
            anchors.fill: parent
            anchors.margins: borderRectangle.border.width
            search: dictionarySearch
            showToolbar: true
            onClosePressed: root.close()
        }
    }

    DictionarySearch {
        id: dictionarySearch
        settings: MementoSettings
    }
}
