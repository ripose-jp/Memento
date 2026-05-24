import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    header: ToolBar {
        ColumnLayout {
            anchors.fill: parent

            TextField {
                id: searchTextField

                property int hoverIndex: -1

                Layout.fillWidth: true
                Layout.margins: 5
                placeholderText: qsTr("Search")
                onTextChanged: searchIndex(0)
                onHoverIndexChanged: {
                    if (KeyTracker.modifierHeld(MementoSettings.searchModifier))
                    {
                        searchIndex(hoverIndex);
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
                            searchTextField.searchIndex(searchTextField.hoverIndex);
                        }
                    }

                    onExited: searchTextField.hoverIndex = -1
                }

                /**
                 * Searches the text at the given index.
                 * @param index The index into the text to search.
                 */
                function searchIndex(index) {
                    dictionarySearch.clearResults();
                    if (index < text.length)
                    {
                        dictionarySearch.searchTerms(text.substring(index), text, index);
                        dictionarySearch.searchKanji(text.charAt(index), text, index);
                    }
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
        settings: MementoSettings
    }
}
