import QtQuick
import QtQuick.Controls
import Ripose.Memento

GroupBox {
    id: root

    Component.onCompleted: {
        if (Features.platform === Features.Linux)
        {
            root.background = linuxBackground;
            root.label = linuxLabel;
        }
    }

    resources: [
        Rectangle {
            id: linuxBackground
            y: root.topPadding - root.bottomPadding
            width: root.width
            height: root.height - root.topPadding + root.bottomPadding
            color: Qt.styleHints.colorScheme === Qt.ColorScheme.Dark ?
                       MementoPalette.mid : MementoPalette.midlight
            border.color: MementoPalette.border
            radius: 10
        },
        Label {
            id: linuxLabel
            x: root.leftPadding
            width: root.availableWidth
            text: root.title
            font.bold: true
            elide: Text.ElideRight
        }
    ]
}
