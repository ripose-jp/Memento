import QtQuick
import QtQuick.Controls
import Ripose.Memento

TabButton {
    id: root

    property color textColor: "white"
    property color hoverColor: "#1A1A1A"
    property color selectedColor: "#333333"

    HoverHandler {
        id: hoverHandler
    }

    background: Rectangle {
        color: {
            if (root.checked)
            {
                return root.selectedColor;
            }
            else if (hoverHandler.hovered)
            {
                return root.hoverColor;
            }
            return "transparent";
        }
    }

    contentItem: Label {
        text: root.text
        color: root.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
