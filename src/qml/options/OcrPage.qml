import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    property int preferredWidth: 600
    property int groupSpacing: 10

    FolderDialog {
        id: customFolderDialog
        title: qsTr("Select Custom Open Folder")
        currentFolder: MementoSettings.behaviorFileOpenCustom
        onAccepted: MementoSettings.behaviorFileOpenCustom = selectedFolder
    }

    footer: ColumnLayout {
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            visible: Features.unix && !scrollView.atBottom
            color: MementoPalette.border
            height: 1
        }

        DialogButtonBox {
            id: buttonBoxFooter
            Layout.fillWidth: true
            standardButtons: DialogButtonBox.Apply |
                             DialogButtonBox.RestoreDefaults |
                             DialogButtonBox.Reset

            onApplied: MementoSettings.writeOcrSettings()
            onClicked: function(button) {
                if (button === standardButton(DialogButtonBox.Reset))
                {
                    MementoSettings.loadOcrSettings();
                }
                else if (button === standardButton(DialogButtonBox.RestoreDefaults))
                {
                    MementoSettings.defaultOcrSettings();
                }
            }
        }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        contentWidth: scrollView.contentWidth
        leftPadding: root.groupSpacing
        rightPadding: root.groupSpacing
        clip: true

        readonly property bool atBottom:
            (ScrollBar.vertical.position + ScrollBar.vertical.size) >= 0.99

        ColumnLayout {
            id: scrollViewLayout
            width: parent.width
            spacing: root.groupSpacing

            SettingsBox {
                id: playerBox
                Layout.preferredWidth: root.preferredWidth
                Layout.topMargin: root.groupSpacing
                Layout.bottomMargin: root.groupSpacing
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("OCR")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Enabled")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.ocrEnabled
                            onClicked: MementoSettings.ocrEnabled = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Use GPU acceleration")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.ocrUseGpu
                            onClicked: MementoSettings.ocrUseGpu = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Use model")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 250
                            placeholderText: qsTr("Hugging Face Model")
                            text: MementoSettings.ocrModel
                            onEditingFinished: MementoSettings.ocrModel = text
                        }
                    }
                }
            }
        }
    }
}
