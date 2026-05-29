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
                    pageIndex: 0
                }
                ListElement {
                    text: qsTr("Application")
                    icon: "settings"
                    pageIndex: 1
                }
                ListElement{
                    text: qsTr("Audio Sources")
                    icon: "volume-on"
                    pageIndex: 2
                }
                ListElement{
                    text: qsTr("Behavior")
                    icon: "gear"
                    pageIndex: 3
                }
                ListElement {
                    text: qsTr("Dictionary")
                    icon: "dictionary"
                    pageIndex: 4
                }
                ListElement {
                    text: qsTr("Interface")
                    icon: "window"
                    pageIndex: 5
                }
                ListElement {
                    text: qsTr("Keybinds")
                    icon: "keyboard"
                    pageIndex: 6
                }
                ListElement {
                    text: qsTr("Search")
                    icon: "search"
                    pageIndex: 8
                }
            }

            Component.onCompleted: {
                if (Features.ocr)
                {
                    model.insert(7, {
                        text: qsTr("OCR"),
                        icon: "eye",
                        pageIndex: 7,
                    });
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
            currentIndex: {
                if (sidebar.currentIndex < 0 || sidebar.currentIndex >= sidebar.model.count)
                {
                    return 0;
                }

                return sidebar.model.get(sidebar.currentIndex).pageIndex;
            }

            AnkiIntegrationPage { }

            ApplicationPage { }

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
