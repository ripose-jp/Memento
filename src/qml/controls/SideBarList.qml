import QtQuick
import QtQuick.Controls
import Ripose.Memento

Rectangle {
    id: root

    property alias model: listView.model
    property alias currentIndex: listView.currentIndex

    implicitWidth: 250
    color: "transparent"

    ListView {
        id: listView
        anchors.fill: parent

        ScrollBar.vertical: ScrollBar {
            id: scrollBarVertical
            policy: ScrollBar.AsNeeded
        }

        delegate: ItemDelegate {
            width: ListView.view.width
            spacing: 10
            text: model.text
            icon.source: Utils.toImageProvider(model.icon, MementoPalette.text)
            display: AbstractButton.TextBesideIcon
            highlighted: ListView.isCurrentItem
            onClicked: listView.currentIndex = index
        }
    }
}
