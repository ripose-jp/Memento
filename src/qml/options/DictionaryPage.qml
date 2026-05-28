import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    enabled: !DictionaryController.modifyingDatabase

    FileDialog {
        id: addDictionaryDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.DownloadLocation)[0]
        fileMode: FileDialog.OpenFiles
        nameFilters: [qsTr("Dictionary Files (*.zip)")]
        title: qsTr("Select Dictionaries")
        onAccepted: {
            for (let i = 0; i < selectedFiles.length; ++i)
            {
                DictionaryController.addDictionary(addDictionaryDialog.selectedFiles[i]);
            }
        }
    }

    Dialog {
        id: errorDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok
        title: qsTr("Dictionary Error")

        Label {
            wrapMode: Text.WordWrap
            text: DictionaryController.lastError
        }

        Connections {
            target: DictionaryController
            function onLastErrorChanged() {
                errorDialog.open();
            }
        }
    }

    header: ColumnLayout {
        spacing: 0

        ToolBar {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                anchors.fill: parent

                ToolButton {
                    Layout.alignment: Qt.AlignRight
                    icon.name: MementoSettings.interfaceSystemIcons ? "list-add" : null
                    icon.source: Utils.toImageProvider("add", MementoPalette.text)
                    onClicked: addDictionaryDialog.open()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            visible: !Features.unix
            implicitHeight: 1
            color: MementoPalette.border
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ScrollBar.vertical: ScrollBar {
                id: scrollBarVertical
                policy: ScrollBar.AsNeeded
            }

            model: DictionaryController.dictionaries
            delegate: Rectangle {
                id: rootDelegate

                required property int index
                required property var model

                width: ListView.view.width
                height: delegateLayout.implicitHeight
                color: "transparent"

                ColumnLayout {
                    id: delegateLayout
                    anchors.fill: parent

                    RowLayout {
                        Layout.fillWidth: true
                        Layout.margins: 10

                        Switch {
                            checked: rootDelegate.model.enabled
                            onClicked: rootDelegate.model.enabled = checked
                        }

                        Label {
                            Layout.fillWidth: true
                            text: `${rootDelegate.model.name} [${rootDelegate.model.id}]`
                        }

                        ToolButton {
                            icon.name: MementoSettings.interfaceSystemIcons ? "go-up" : null
                            icon.source: Utils.toImageProvider("arrow-up", MementoPalette.text)
                            enabled: rootDelegate.index > 0
                            onClicked: DictionaryController.moveDictionary(rootDelegate.index, rootDelegate.index - 1)
                        }

                        ToolButton {
                            icon.name: MementoSettings.interfaceSystemIcons ? "go-down" : null
                            icon.source: Utils.toImageProvider("arrow-down", MementoPalette.text)
                            enabled: rootDelegate.index < listView.count - 1
                            onClicked: DictionaryController.moveDictionary(rootDelegate.index, rootDelegate.index + 1)
                        }

                        ToolButton {
                            icon.name: MementoSettings.interfaceSystemIcons ? "edit-delete" : null
                            icon.source: Utils.toImageProvider("delete", MementoPalette.text)
                            enabled: listView.count > 0
                            onClicked: DictionaryController.removeDictionary(rootDelegate.model.id)
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        implicitHeight: 1
                        color: MementoPalette.border
                    }
                }
            }
        }
    }
}
