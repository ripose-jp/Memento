import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

ColumnLayout {
    id: root

    required property var element

    SelectableLabel {
        textFormat: Text.RichText
        text: `<p><b><code>{${root.element.name}}</code></b></p>`
    }

    Label {
        Layout.fillWidth: true
        wrapMode: Label.WordWrap
        textFormat: Text.RichText
        text: root.element.description
    }

    GridLayout {
        Layout.leftMargin: 20
        columns: 2

        Repeater {
            model: root.element.arguments
            delegate: SelectableLabel {
                required property int index
                required property var modelData

                Layout.column: 0
                Layout.row: 2 * index
                Layout.rightMargin: 10
                textFormat: Text.RichText
                text: `<p><b><code>${modelData.name}</code></b></p>`
            }
        }

        Repeater {
            model: root.element.arguments
            delegate: Label {
                required property int index
                required property var modelData

                Layout.column: 1
                Layout.row: 2 * index
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                textFormat: Text.RichText
                text: qsTr("<p><b>Default Value:</b> %1</p>").arg(modelData.defaultValue)
            }
        }

        Repeater {
            model: root.element.arguments
            delegate: Label {
                required property int index
                required property var modelData

                Layout.column: 1
                Layout.row: 1 + 2 * index
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                textFormat: Text.RichText
                text: modelData.description
            }
        }
    }
}
