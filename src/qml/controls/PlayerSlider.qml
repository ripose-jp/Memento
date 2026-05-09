import QtQuick
import QtQuick.Controls

Slider {
    id: root

    property var chapters: []

    readonly property real xPosition: {
        if (root.pressed)
        {
            return Math.max(0,
                Math.min(
                    root.background.width,
                    pointHandler.point.position.x
                )
            );
        }
        else if (root.hovered)
        {
            return hoverHandler.point.position.x;
        }
        return -1;
    }

    readonly property real cursorValue: {
        if (root.xPosition !== -1)
        {
            const pos = root.xPosition / root.background.width;
            return root.valueAt(pos);
        }
        return -1;
    }

    implicitHeight: 30
    implicitWidth: 200
    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0
    leftInset: 0
    topInset: 0
    rightInset: 0
    bottomInset: 0

    HoverHandler {
        id: hoverHandler
        grabPermissions: PointerHandler.TakeOverForbidden
    }

    PointHandler {
        id: pointHandler
        grabPermissions: PointerHandler.TakeOverForbidden
    }

    handle: Item {
        implicitWidth: 1
        implicitHeight: root.implicitHeight
    }

    background: Rectangle {
        implicitWidth: root.implicitWidth
        implicitHeight: root.implicitHeight
        width: root.availableWidth
        color: MementoPalette.dark

        Rectangle {
            width: root.visualPosition * parent.width
            height: parent.height
            color: MementoPalette.accent
        }

        Repeater {
            model: root.chapters
            Rectangle {
                x: parent.width * (modelData / root.to)
                y: 0
                width: 1
                height: parent.height
                color: MementoPalette.window
            }
        }
    }

    StrokeLabel {
        anchors.verticalCenter: parent.verticalCenter
        x: {
            const idealPosition = root.xPosition - width - 10;
            if (idealPosition < 10)
            {
                return root.xPosition + 15;
            }
            return idealPosition;
        }
        visible: root.enabled && (root.hovered || root.pressed)
        text: Utils.toTimeString(root.cursorValue)
        font.pixelSize: root.height
        color: MementoPalette.text
        stroke: MementoPalette.window
        strokeSize: 5
    }
}
