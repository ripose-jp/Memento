import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Ripose.Memento

Window {
    id: root

    property Item playerItem: null

    width: 900
    height: 600
    minimumWidth: 400
    minimumHeight: 300
    visible: MementoSettings.windowHistory
    title: qsTr("History")

    onClosing: {
        MementoSettings.windowHistory = false
    }

    Connections {
        target: HistoryManager
        function onFileError(index, message) {
            errorDialog.text = message
            errorDialog.open()
        }
    }

    Dialog {
        id: errorDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok
        title: qsTr("File Error")
    }

    Page {
        anchors.fill: parent
        focus: true

        background: Rectangle {
            color: MementoPalette.window
        }

        header: ToolBar {
            background: Rectangle {
                color: MementoPalette.window
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10

                Label {
                    text: qsTr("History")
                    font.pixelSize: 18
                    font.bold: true
                    color: MementoPalette.text
                    Layout.fillWidth: true
                }

                Button {
                    text: qsTr("Clear All")
                    visible: HistoryManager.entryCount > 0
                    flat: true
                    font.pixelSize: 12
                    onClicked: {
                        HistoryManager.clearAll()
                    }
                }
            }
        }

        GridView {
            id: gridView
            anchors.fill: parent
            anchors.topMargin: 5
            cellWidth: 230
            cellHeight: 180
            clip: true

            model: HistoryManager

            ScrollBar.vertical: ScrollBar {
                parent: gridView.parent
                anchors.top: gridView.top
                anchors.bottom: gridView.bottom
                anchors.right: parent.right
            }

            delegate: Item {
                required property string title
                required property string videoPath
                required property int primarySubtitleTrackId
                required property int secondarySubtitleTrackId
                required property var externalSubtitles
                required property real playbackPosition
                required property string thumbnail

                width: 230
                height: 180

                HistoryCard {
                    width: 220
                    height: 170
                    anchors.centerIn: parent

                    cardTitle: parent.title
                    thumbnailSource: parent.thumbnail.length > 0
                        ? "file:///" + parent.thumbnail.replace(/\\/g, "/")
                        : ""
                    cardIndex: index

                    onClicked: {
                        if (root.playerItem)
                        {
                            HistoryManager.checkFiles(cardIndex)
                            root.playerItem.restoreFromHistory(
                                parent.videoPath,
                                parent.primarySubtitleTrackId,
                                parent.secondarySubtitleTrackId,
                                parent.externalSubtitles,
                                parent.playbackPosition)
                        }
                    }

                    onRemoveRequested: {
                        HistoryManager.removeEntry(cardIndex)
                    }
                }
            }
        }

        Label {
            anchors.centerIn: parent
            text: qsTr("No history yet")
            color: "#666666"
            font.pixelSize: 16
            visible: HistoryManager.entryCount === 0
        }
    }
}
