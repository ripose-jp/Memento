import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    required property DictionarySearch search

    property bool showToolbar: false

    signal closePressed()

    Action {
        id: addAnkiAction
        enabled: AnkiConfig.enabled &&
                 stackView.currentItem &&
                 (stackView.currentItem.count > 0 ||
                  stackView.currentItem.kanji)
        shortcut: MementoSettings.keybinds.profile?.cardAdd
        onTriggered: stackView.currentItem.addNote()
    }

    Connections {
        target: root.search

        function onKanjiChanged() {
            stackView.pop(null, StackView.Immediate);
        }

        function onTermsChanged() {
            stackView.pop(null, StackView.Immediate);
        }
    }

    /**
     * Add a recursive search result list to the stack.
     * @param search The search object that owns the recursive results.
     */
    function pushSearch(search) {
        stackView.push(definitionListComponent, {
            dictionarySearch: search
        });
    }

    header: ColumnLayout {
        spacing: 0

        ToolBar {
            id: toolbar
            Layout.fillWidth: true
            visible: root.showToolbar || stackView.depth > 1

            RowLayout {
                anchors.fill: parent

                ToolButton {
                    id: buttonBack
                    focusPolicy: Qt.NoFocus
                    visible: stackView.depth > 1
                    icon.name: MementoSettings.interfaceSystemIcons ? "go-previous" : null
                    icon.source: Utils.toImageProvider("arrow-back", MementoPalette.text)
                    onClicked: stackView.pop()
                }

                Item {
                    Layout.fillWidth: true
                }

                ToolButton {
                    id: buttonClose
                    focusPolicy: Qt.NoFocus
                    icon.name: MementoSettings.interfaceSystemIcons ? "window-close" : null
                    icon.source: Utils.toImageProvider("close", MementoPalette.text)
                    onClicked: root.closePressed()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            visible: !Features.isUnix && toolbar.visible
            implicitHeight: 1
            color: MementoPalette.border
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        clip: true
        initialItem: DefinitionList {
            id: definitionList
            dictionarySearch: root.search
            canvasColor: root.palette.window
            onRecursiveSearchRequested: function(search) {
                root.pushSearch(search);
            }
        }
    }

    Component {
        id: definitionListComponent

        DefinitionList {
            canvasColor: root.palette.window
            onRecursiveSearchRequested: function(search) {
                root.pushSearch(search);
            }
        }
    }
}
