import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    property alias terms: definitionList.terms
    property alias kanji: definitionList.kanji

    property bool showToolbar: false

    signal closePressed()

    onTermsChanged: kanjiSearch.clearKanji()
    onKanjiChanged: kanjiSearch.clearKanji()

    Action {
        id: addAnkiAction
        enabled: AnkiConfig.enabled &&
                 stackView.currentItem &&
                 (definitionList.count > 0 || definitionList.kanji)
        shortcut: MementoSettings.keybinds.profile?.cardAdd
        onTriggered: stackView.currentItem.addNote()
    }

    DictionarySearch {
        id: kanjiSearch
        settings: MementoSettings
        onKanjiChanged: {
            if (kanji)
            {
                stackView.push(subKanjiEntry);
            }
            else
            {
                stackView.pop(null, StackView.Immediate);
            }
        }
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
            visible: Features.platform !== Features.Linux && toolbar.visible
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
            onKanjiClicked: (ch) => kanjiSearch.searchKanji(ch, "", 0)
        }
    }

    Component {
        id: subKanjiEntry
        ScrollView {
            id: kanjiScrollView

            /**
             * Add a note to Anki.
             */
            function addNote() {
                kanjiEntry.addKanji();
            }

            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            KanjiEntry {
                id: kanjiEntry
                width: kanjiScrollView.ScrollBar.vertical.visible ?
                           kanjiScrollView.width - kanjiScrollView.ScrollBar.vertical.width :
                           kanjiScrollView.width
                kanji: kanjiSearch.kanji
            }
        }
    }
}
