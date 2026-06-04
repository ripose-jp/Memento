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
            visible: Features.isUnix && !scrollView.atBottom
            color: MementoPalette.border
            height: 1
        }

        DialogButtonBox {
            id: buttonBoxFooter
            Layout.fillWidth: true
            standardButtons: DialogButtonBox.Apply |
                             DialogButtonBox.RestoreDefaults |
                             DialogButtonBox.Reset

            onApplied: MementoSettings.writeBehaviorSettings()
            onClicked: function(button) {
                if (button === standardButton(DialogButtonBox.Reset))
                {
                    MementoSettings.loadBehaviorSettings();
                }
                else if (button === standardButton(DialogButtonBox.RestoreDefaults))
                {
                    MementoSettings.defaultBehaviorSettings();
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
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Player")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Auto fit media on load")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.behaviorAutoFit
                            onClicked: MementoSettings.behaviorAutoFit = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Auto fit media percentage")
                        }
                        SpinBox {
                            Layout.alignment: Qt.AlignRight
                            editable: true
                            from: 1
                            to: 1000
                            value: MementoSettings.behaviorAutoFitPercent
                            onValueModified: MementoSettings.behaviorAutoFitPercent = value
                        }
                    }
                }
            }

            SettingsBox {
                id: oscBox
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("On Screen Controller")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Visibility duration milliseconds")
                        }
                        SpinBox {
                            Layout.alignment: Qt.AlignRight
                            editable: true
                            from: 50
                            to: 9999
                            value: MementoSettings.behaviorOscDuration
                            onValueModified: MementoSettings.behaviorOscDuration = value
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Fade duration milliseconds")
                        }
                        SpinBox {
                            Layout.alignment: Qt.AlignRight
                            editable: true
                            from: 0
                            to: 9999
                            value: MementoSettings.behaviorOscFadeDuration
                            onValueModified: MementoSettings.behaviorOscFadeDuration = value
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Minimum mouse pixel movement")
                        }
                        SpinBox {
                            Layout.alignment: Qt.AlignRight
                            editable: true
                            from: 0
                            to: 999
                            value: MementoSettings.behaviorOscMinMove
                            onValueModified: MementoSettings.behaviorOscMinMove = value
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Allow mpv to control cursor visibility")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.behaviorOscMpvCursor
                            onClicked: MementoSettings.behaviorOscMpvCursor = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Show preview thumbnails")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.behaviorOscPreviewThumbnails
                            onClicked: MementoSettings.behaviorOscPreviewThumbnails = checked
                        }
                    }
                }
            }

            SettingsBox {
                id: subtitlesBox
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Subtitles")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Automatically pause at the end of subtitles")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.behaviorSubtitlePause
                            onClicked: MementoSettings.behaviorSubtitlePause = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Show subtitles when the cursor is at the bottom of the player")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.behaviorSubtitleCursorShow
                            onClicked: MementoSettings.behaviorSubtitleCursorShow = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Show secondary subtitles when the cursor is at the top of the player")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.behaviorSecondarySubtitleCursorShow
                            onClicked: MementoSettings.behaviorSecondarySubtitleCursorShow = checked
                        }
                    }
                }
            }

            SettingsBox {
                id: openFileDirectoryBox
                Layout.preferredWidth: root.preferredWidth
                Layout.bottomMargin: root.groupSpacing
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Open File Directory")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Open file starting location")
                        }
                        ComboBox {
                            Layout.alignment: Qt.AlignRight
                            implicitContentWidthPolicy: ComboBox.WidestText
                            model: ListModel {
                                ListElement {
                                    text: qsTr("Current")
                                    value: MementoSetting.DirectoryCurrent
                                }
                                ListElement {
                                    text: qsTr("Custom")
                                    value: MementoSetting.DirectoryCustom
                                }
                                ListElement {
                                    text: qsTr("Documents")
                                    value: MementoSetting.DirectoryDocuments
                                }
                                ListElement {
                                    text: qsTr("Home")
                                    value: MementoSetting.DirectoryHome
                                }
                                ListElement {
                                    text: qsTr("Movies")
                                    value: MementoSetting.DirectoryMovies
                                }
                            }
                            textRole: "text"
                            valueRole: "value"
                            currentValue: MementoSettings.behaviorFileOpenDirectory
                            onActivated: MementoSettings.behaviorFileOpenDirectory = currentValue
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Custom file directory")
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight
                            text: qsTr("Browse")
                            onClicked: customFolderDialog.open()
                        }
                    }
                    Label {
                        Layout.alignment: Qt.AlignRight
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignRight
                        elide: Text.ElideRight
                        text: MementoSettings.behaviorFileOpenCustom
                    }
                }
            }
        }
    }
}
