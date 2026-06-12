import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Rectangle {
    id: root

    required property string cardTitle
    property url thumbnailSource: ""
    property int cardIndex: -1

    signal clicked()
    signal removeRequested()

    implicitWidth: 220
    implicitHeight: 170
    radius: 8
    color: mouseArea.containsMouse ? "#2A2A2A" : "#1E1E1E"
    border.color: mouseArea.containsMouse ? "#555555" : "#333333"
    border.width: 1

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked: function(event) {
            if (event.button === Qt.LeftButton)
            {
                root.clicked()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 6

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: root.implicitWidth * 9 / 16
            radius: 4
            clip: true
            color: "#0D0D0D"

            Image {
                anchors.fill: parent
                source: root.thumbnailSource
                fillMode: Image.PreserveAspectCrop
                asynchronous: true
                cache: true
            }

            Label {
                anchors.centerIn: parent
                text: qsTr("No Thumbnail")
                color: "#555555"
                font.pixelSize: 11
                visible: root.thumbnailSource.toString().length === 0
            }
        }

        Label {
            Layout.fillWidth: true
            text: root.cardTitle
            color: MementoPalette.text
            font.pixelSize: 12
            elide: Text.ElideRight
            maximumLineCount: 2
            wrapMode: Text.Wrap
        }
    }

    Button {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 4
        implicitWidth: 20
        implicitHeight: 20
        flat: true
        visible: mouseArea.containsMouse

        contentItem: Label {
            text: qsTr("×")
            color: "#FF5555"
            font.pixelSize: 14
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        background: Rectangle {
            color: "#66000000"
            radius: 10
        }

        onClicked: root.removeRequested()
    }
}
