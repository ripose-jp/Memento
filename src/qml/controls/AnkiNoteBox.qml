import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

SettingsBox {
    id: root

    property int groupSpacing: 10
    property int radius: 10

    property alias decks: deckComboBox.model
    property alias currentDeck: deckComboBox.currentValue
    property alias currentDeckIndex: deckComboBox.currentIndex
    signal deckActivated(int index)

    property alias models: modelComboBox.model
    property alias currentModel: modelComboBox.currentValue
    property alias currentModelIndex: modelComboBox.currentIndex
    signal modelActivated(int index)

    property alias fields: fieldListView.model

    property var markers: []

    ColumnLayout {
        anchors.fill: parent
        spacing: root.groupSpacing

        RowLayout {
            Label {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                text: qsTr("Deck")
            }
            ComboBox {
                id: deckComboBox
                Layout.alignment: Qt.AlignRight
                Layout.preferredWidth: 250
                onActivated: (index) => root.deckActivated(index)
            }
        }

        SettingsBoxSeparator {
            Layout.fillWidth: true
        }

        RowLayout {
            Label {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                text: qsTr("Model")
            }
            ComboBox {
                id: modelComboBox
                Layout.alignment: Qt.AlignRight
                Layout.preferredWidth: 250
                onActivated: (index) => root.modelActivated(index)
            }
        }

        SettingsBoxSeparator {
            Layout.fillWidth: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 400
            color: "transparent"
            border.color: MementoPalette.border
            border.width: 1
            radius: root.radius

            ListView {
                id: fieldListView
                anchors.fill: parent
                anchors.margins: 5
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }

                delegate: Item {
                    implicitWidth: ListView.view.width
                    implicitHeight: delegateLayout.implicitHeight +
                                    delegateLayout.anchors.leftMargin +
                                    delegateLayout.anchors.rightMargin

                    RowLayout {
                        id: delegateLayout
                        anchors.fill: parent
                        anchors.margins: 5
                        spacing: 5

                        Label {
                            Layout.preferredWidth: 150
                            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                            elide: Text.ElideRight
                            text: model.name
                        }

                        AnkiFieldTextArea {
                            Layout.fillWidth: true
                            wrapMode: Text.WrapAnywhere
                            text: model.value
                            onEditingFinished: model.value = text
                            markers: root.markers
                        }
                    }
                }
            }
        }
    }
}
