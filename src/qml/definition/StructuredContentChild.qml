import QtQuick
import Ripose.Memento

Item {
    id: root

    required property var node
    required property var owner
    required property font baseFont

    property real availableWidth: width
    property bool fillWidth: false
    property bool selectable: true

    readonly property bool shouldFillWidth:
        root.fillWidth &&
        root.node.kind !== "table" &&
        !(root.node.style?.fitContent ?? false)

    implicitWidth: child.item?.implicitWidth ?? 0
    implicitHeight: child.item?.implicitHeight ?? 0

    width: root.shouldFillWidth ? root.availableWidth : root.implicitWidth

    Loader {
        id: child

        /**
         * Load one recursive child renderer without creating a static QML type
         * cycle between the node and child wrapper components.
         */
        Component.onCompleted: child.setSource("StructuredContentNode.qml", {
            "node": root.node,
            "owner": root.owner,
            "baseFont": root.baseFont,
            "availableWidth": root.availableWidth,
            "selectable": root.selectable
        })
    }

    Binding {
        target: child.item
        property: "width"
        value: root.shouldFillWidth ? root.width :
                   child.item?.implicitWidth ?? 0
        when: child.item
    }

    Binding {
        target: child.item
        property: "availableWidth"
        value: root.availableWidth
        when: child.item
    }
}
