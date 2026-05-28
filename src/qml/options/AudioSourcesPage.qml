import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

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
                    onClicked: MementoSettings.audioSources.appendItem(
                                   '', '', MementoSetting.AudioSourceTypeFile, '')
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: !Features.unix
            implicitHeight: 1
            color: MementoPalette.border
        }
    }

    footer: DialogButtonBox {
        id: buttonBoxFooter
        standardButtons: DialogButtonBox.Apply |
                         DialogButtonBox.RestoreDefaults |
                         DialogButtonBox.Reset |
                         DialogButtonBox.Help

        onApplied: MementoSettings.writeAudioSourceSettings()
        onClicked: function(button) {
            if (button === standardButton(DialogButtonBox.Reset))
            {
                MementoSettings.loadAudioSourceSettings();
            }
            else if (button === standardButton(DialogButtonBox.RestoreDefaults))
            {
                MementoSettings.defaultAudioSourceSettings();
            }
        }
        onHelpRequested: helpDialog.open()
    }

    Dialog {
        id: helpDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok
        title: qsTr("Audio Sources Help")

        Label {
            wrapMode: Text.WordWrap

            text: qsTr("<p><b>Source Name</b>: The name of the audio source as it will appear in Memento.</p>
                        <p><b>URL</b>: The URL of the audio source. Supports inserting <b>{expression}</b> and
                            <b>{reading}</b> markers into the URL.</p>
                        <p><b>MD5 Skip Hash</b>: Audio that matches this MD5 hash will be ignored.</p>")
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

            model: MementoSettings.audioSources
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

                        TextField {
                            placeholderText: qsTr("Name")
                            text: rootDelegate.model.name
                            onEditingFinished: rootDelegate.model.name = text
                        }

                        TextField {
                            Layout.fillWidth: true
                            placeholderText: qsTr("URL")
                            text: rootDelegate.model.url
                            onEditingFinished: rootDelegate.model.url = text
                        }

                        ComboBox {
                            Layout.preferredWidth: 100
                            model: ListModel {
                                ListElement {
                                    text: qsTr("File")
                                    value: MementoSetting.AudioSourceTypeFile
                                }
                                ListElement {
                                    text: qsTr("JSON")
                                    value: MementoSetting.AudioSourceTypeJson
                                }
                            }
                            textRole: "text"
                            valueRole: "value"
                            currentValue: rootDelegate.model.type
                            onActivated: rootDelegate.model.type = currentValue
                        }

                        TextField {
                            placeholderText: qsTr("MD5 Skip Hash")
                            text: rootDelegate.model.skipHash
                            onEditingFinished: rootDelegate.model.skipHash = text
                        }

                        ToolButton {
                            icon.name: MementoSettings.interfaceSystemIcons ? "go-up" : null
                            icon.source: Utils.toImageProvider("arrow-up", MementoPalette.text)
                            enabled: rootDelegate.index > 0
                            onClicked: MementoSettings.audioSources.move(rootDelegate.index, rootDelegate.index - 1)
                        }

                        ToolButton {
                            icon.name: MementoSettings.interfaceSystemIcons ? "go-down" : null
                            icon.source: Utils.toImageProvider("arrow-down", MementoPalette.text)
                            enabled: rootDelegate.index < listView.count - 1
                            onClicked: MementoSettings.audioSources.move(rootDelegate.index, rootDelegate.index + 1)
                        }

                        ToolButton {
                            icon.name: MementoSettings.interfaceSystemIcons ? "edit-delete" : null
                            icon.source: Utils.toImageProvider("delete", MementoPalette.text)
                            enabled: listView.count > 0
                            onClicked: MementoSettings.audioSources.remove(rootDelegate.index)
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

        Rectangle {
            Layout.fillWidth: true
            visible: Features.unix
            implicitHeight: 1
            color: MementoPalette.border
        }
    }
}
