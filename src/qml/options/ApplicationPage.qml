import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    property int preferredWidth: 600
    property int groupSpacing: 10

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

            onApplied: MementoSettings.writeApplicationSettings()
            onClicked: function(button) {
                if (button === standardButton(DialogButtonBox.Reset))
                {
                    MementoSettings.loadApplicationSettings();
                }
                else if (button === standardButton(DialogButtonBox.RestoreDefaults))
                {
                    MementoSettings.defaultApplicationSettings();
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
                id: languageBox
                Layout.preferredWidth: root.preferredWidth
                Layout.topMargin: root.groupSpacing
                Layout.bottomMargin: root.groupSpacing
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Language")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Interface language (Restart required)")
                        }
                        ComboBox {
                            Layout.alignment: Qt.AlignRight
                            implicitContentWidthPolicy: ComboBox.WidestText
                            model: ListModel {
                                ListElement {
                                    text: qsTr("System")
                                    value: MementoSetting.LanguageSystem
                                }
                                ListElement {
                                    text: "English"
                                    value: MementoSetting.LanguageEnglish
                                }
                                ListElement {
                                    text: "한국어 (MTL)"
                                    value: MementoSetting.LanguageKorean
                                }
                                ListElement {
                                    text: "简体中文 (MTL)"
                                    value: MementoSetting.LanguageChineseSimplified
                                }
                                ListElement {
                                    text: "繁體中文 (MTL)"
                                    value: MementoSetting.LanguageChineseTraditional
                                }
                            }
                            textRole: "text"
                            valueRole: "value"
                            currentValue: MementoSettings.applicationLanguage
                            onActivated: MementoSettings.applicationLanguage = currentValue
                        }
                    }
                }
            }

            SettingsBox {
                id: playerBox
                Layout.preferredWidth: root.preferredWidth
                Layout.topMargin: root.groupSpacing
                Layout.bottomMargin: root.groupSpacing
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Update")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Check for updates on launch")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.applicationAutoUpdateCheck
                            onClicked: MementoSettings.applicationAutoUpdateCheck = checked
                        }
                    }
                }
            }
        }
    }
}
