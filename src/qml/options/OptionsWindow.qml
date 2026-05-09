import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Window {
    id: root

    width: 1100
    height: 800
    color: MementoPalette.window
    title: qsTr("Options")

    RowLayout {
        anchors.fill: parent
        spacing: 0

        SideBarList {
            id: sidebar
            Layout.fillHeight: true
            Layout.preferredHeight: root.height

            model: ListModel {
                ListElement {
                    text: qsTr("Anki Integration")
                    icon: "anki"
                }
                ListElement{
                    text: qsTr("Audio Sources")
                    icon: "volume-on"
                }
                ListElement{
                    text: qsTr("Behavior")
                    icon: "gear"
                }
                ListElement {
                    text: qsTr("Dictionary")
                    icon: "dictionary"
                }
                ListElement {
                    text: qsTr("Interface")
                    icon: "window"
                }
                ListElement {
                    text: qsTr("Keybinds")
                    icon: "keyboard"
                }
                ListElement {
                    text: qsTr("OCR")
                    icon: "eye"
                }
                ListElement {
                    text: qsTr("Search")
                    icon: "search"
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            width: 1
            color: MementoPalette.border
        }

        StackLayout {
            id: stackLayoutPanels
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: sidebar.currentIndex

            AnkiIntegrationPage { }

            AudioSourcesPage { }

            BehaviorPage { }

            DictionaryPage { }

            InterfacePage { }

            KeybindsPage { }

            OcrPage {
                enabled: Features.ocr
            }

            SearchPage { }
        }
    }
}
