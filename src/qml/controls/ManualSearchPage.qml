import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    property alias text: searchTextField.text

    /**
     * Set the search query and search from the beginning.
     * @param query The text to search.
     */
    function setQuery(query) {
        searchTextField.text = query;
        searchTextField.searchIndex(0);
    }

    background: Rectangle {
        color: MementoPalette.window
    }

    header: ToolBar {
        ColumnLayout {
            anchors.fill: parent

            TextField {
                id: searchTextField

                property int hoverIndex: -1

                /**
                 * Searches the text at the given index.
                 * @param index The index into the text to search.
                 */
                function searchIndex(index) {
                    dictionarySearch.clearResults();
                    if (index >= 0 && index < text.length)
                    {
                        dictionarySearch.searchTerms(text.substring(index), text, index);
                        dictionarySearch.searchKanji(text.charAt(index), text, index);
                    }
                }

                Layout.fillWidth: true
                Layout.margins: 5
                hoverEnabled: true
                placeholderText: qsTr("Search")
                onTextChanged: Qt.callLater(searchTextField.searchIndex, 0)
                onHoverIndexChanged: {
                    if (KeyTracker.modifierHeld(MementoSettings.searchModifier))
                    {
                        Qt.callLater(searchTextField.searchIndex, searchTextField.hoverIndex);
                    }
                }

                Connections {
                    target: KeyTracker
                    function onModifiersChanged() {
                        if (searchTextField.hovered &&
                                KeyTracker.modifierHeld(MementoSettings.searchModifier))
                        {
                            Qt.callLater(searchTextField.searchIndex, searchTextField.hoverIndex);
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.MiddleButton
                    cursorShape: Qt.IBeamCursor
                    hoverEnabled: true

                    onPositionChanged: function(event) {
                        let x = event.x - searchTextField.leftPadding;
                        let y = event.y - searchTextField.topPadding;
                        let cursorIndex = searchTextField.positionAt(x, y);
                        let rect = searchTextField.positionToRectangle(cursorIndex);
                        let charIndex = cursorIndex;
                        if (x < rect.x && cursorIndex > 0)
                        {
                            charIndex = cursorIndex - 1;
                        }
                        searchTextField.hoverIndex = charIndex;
                    }

                    onClicked: function(event) {
                        if (MementoSettings.searchMiddleMouseScan)
                        {
                            Qt.callLater(searchTextField.searchIndex, searchTextField.hoverIndex);
                        }
                    }

                    onExited: searchTextField.hoverIndex = -1
                }
            }
        }
    }

    DefinitionPage {
        anchors.fill: parent
        search: dictionarySearch
        onClosePressed: searchTextField.text = ""
    }

    DictionarySearch {
        id: dictionarySearch
    }
}
