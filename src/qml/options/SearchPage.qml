import QtQuick
import QtQuick.Controls
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

            onApplied: MementoSettings.writeSearchSettings()
            onClicked: function(button) {
                if (button === standardButton(DialogButtonBox.Reset))
                {
                    MementoSettings.loadSearchSettings();
                }
                else if (button === standardButton(DialogButtonBox.RestoreDefaults))
                {
                    MementoSettings.defaultSearchSettings();
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
                id: matcherBox
                Layout.preferredWidth: root.preferredWidth
                Layout.topMargin: root.groupSpacing
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Matcher")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Exact")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.searchMatcherExact
                            onClicked: MementoSettings.searchMatcherExact = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Deconjugation")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.searchMatcherDeconj
                            onClicked: MementoSettings.searchMatcherDeconj = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                        visible: Features.mecab
                    }

                    RowLayout {
                        visible: Features.mecab
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("MeCab IPAdic")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.searchMatcherMecabIpadic
                            onClicked: MementoSettings.searchMatcherMecabIpadic = checked
                        }
                    }
                }
            }

            SettingsBox {
                id: popupBox
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Popup")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Search method")
                        }
                        ComboBox {
                            Layout.alignment: Qt.AlignRight
                            implicitContentWidthPolicy: ComboBox.WidestText
                            model: ListModel {
                                ListElement {
                                    text: qsTr("Hover")
                                    value: MementoSetting.SearchMethodHover
                                }
                                ListElement {
                                    text: qsTr("Modifier")
                                    value: MementoSetting.SearchMethodModifier
                                }
                            }
                            textRole: "text"
                            valueRole: "value"
                            currentValue: MementoSettings.searchMethod
                            onActivated: MementoSettings.searchMethod = currentValue
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Search hover millisecond delay")
                        }
                        SpinBox {
                            Layout.alignment: Qt.AlignRight
                            editable: true
                            from: 0
                            to: 99999
                            value: MementoSettings.searchDelay
                            onValueModified: MementoSettings.searchDelay = value
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Search modifier key")
                        }
                        ComboBox {
                            Layout.alignment: Qt.AlignRight
                            implicitContentWidthPolicy: ComboBox.WidestText
                            model: ListModel {
                                ListElement {
                                    text: qsTr("Alt")
                                    value: MementoSetting.ModifierAlt
                                }
                                ListElement {
                                    text: qsTr("Control")
                                    value: MementoSetting.ModifierControl
                                }
                                ListElement {
                                    text: qsTr("Shift")
                                    value: MementoSetting.ModifierShift
                                }
                                ListElement {
                                    text: qsTr("Super")
                                    value: MementoSetting.ModifierSuper
                                }
                            }
                            textRole: "text"
                            valueRole: "value"
                            currentValue: MementoSettings.searchModifier
                            onActivated: MementoSettings.searchModifier = currentValue
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Search using middle mouse button")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.searchMiddleMouseScan
                            onClicked: MementoSettings.searchMiddleMouseScan = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Glossary style")
                        }
                        ComboBox {
                            Layout.alignment: Qt.AlignRight
                            implicitContentWidthPolicy: ComboBox.WidestText
                            model: ListModel {
                                ListElement {
                                    text: qsTr("Bullet Points")
                                    value: MementoSetting.GlossaryStyleBullet
                                }
                                ListElement {
                                    text: qsTr("Line Breaks")
                                    value: MementoSetting.GlossaryStyleLineBreak
                                }
                                ListElement {
                                    text: qsTr("Pipes")
                                    value: MementoSetting.GlossaryStylePipe
                                }
                            }
                            textRole: "text"
                            valueRole: "value"
                            currentValue: MementoSettings.searchGlossaryStyle
                            onActivated: MementoSettings.searchGlossaryStyle = currentValue
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Auto play first result audio")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.searchAutoPlayAudio
                            onClicked: MementoSettings.searchAutoPlayAudio = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Show add to Anki glossary checkbox")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.showAnkiGlossaryCheckbox
                            onClicked: MementoSettings.showAnkiGlossaryCheckbox = checked
                        }
                    }
                }
            }

            SettingsBox {
                id: subtitleBox
                Layout.preferredWidth: root.preferredWidth
                Layout.bottomMargin: root.groupSpacing
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Subtitles")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Hide mpv subtitles while Memento subtitles are visible")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.searchHideMpvSubs
                            onClicked: MementoSettings.searchHideMpvSubs = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Hide Memento subtitles while playing media")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.searchHideSubs
                            onClicked: MementoSettings.searchHideSubs = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Pause when hovering on subtitles")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.searchPauseOnHover
                            onClicked: MementoSettings.searchPauseOnHover = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Replace new lines in subtitles")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: MementoSettings.searchReplaceNewlines
                            onClicked: MementoSettings.searchReplaceNewlines = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("New line replacement text")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            text: MementoSettings.searchReplaceNewlinesWith
                            onEditingFinished: MementoSettings.searchReplaceNewlinesWith = text
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Remove subtitle text matching regex")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            placeholderText: qsTr("Regex")
                            text: MementoSettings.searchRemoveRegex
                            onEditingFinished: MementoSettings.searchRemoveRegex = text
                        }
                    }
                }
            }
        }
    }
}
