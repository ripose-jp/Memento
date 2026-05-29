import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    property int commonMargin: 5

    header: ColumnLayout {
        spacing: 0

        ToolBar {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                anchors.fill: parent

                Item {
                    Layout.fillWidth: true
                }

                ToolButton {
                    Layout.alignment: Qt.AlignRight
                    enabled: MementoSettings.keybinds.profile
                    icon.name: MementoSettings.interfaceSystemIcons ? "edit-delete" : null
                    icon.source: Utils.toImageProvider("delete", MementoPalette.text)
                    onClicked: MementoSettings.keybinds.removeProfile(MementoSettings.keybinds.profile.name)
                }

                ToolButton {
                    Layout.alignment: Qt.AlignRight
                    icon.name: MementoSettings.interfaceSystemIcons ? "list-add" : null
                    icon.source: Utils.toImageProvider("add", MementoPalette.text)
                    onClicked: addProfileDialog.open()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            visible: !Features.unix
            implicitHeight: 1
            color: MementoPalette.border
        }
    }

    Dialog {
        id: addProfileDialog
        anchors.centerIn: parent
        title: qsTr("Add Profile")
        modal : true
        standardButtons: Dialog.Ok | Dialog.Cancel

        ColumnLayout {
            Label {
                text: qsTr("Profile name")
            }

            TextField {
                id: addProfileNameTextField
                Layout.preferredWidth: 300
                placeholderText: qsTr("Name")
            }

            CheckBox {
                id: addProfileCopyCheckBox
                text: qsTr("Copy current profile")
                checked: true
            }
        }

        Component.onCompleted: {
            const okButton = addProfileDialog.standardButton(Dialog.Ok);
            okButton.enabled = Qt.binding(function() {
                let text = addProfileNameTextField.text.trim();
                return text.length > 0 && !MementoSettings.keybinds.profileExists(text);
            });
        }

        onClosed: {
            addProfileNameTextField.text = "";
            addProfileCopyCheckBox.checked = true;
        }

        onAccepted: {
            const name = addProfileNameTextField.text.trim();
            const added = MementoSettings.keybinds.addProfile(
                name,
                addProfileCopyCheckBox.checked ? MementoSettings.keybinds.profile : null
            );
            if (added)
            {
                MementoSettings.keybinds.setProfileByName(name);
            }
        }
    }

    footer: ColumnLayout {
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            visible: Features.unix
            implicitHeight: 1
            color: MementoPalette.border
        }

        DialogButtonBox {
            id: buttonBoxFooter
            Layout.fillWidth: true
            standardButtons: DialogButtonBox.Apply |
                             DialogButtonBox.RestoreDefaults |
                             DialogButtonBox.Reset

            onApplied: MementoSettings.writeKeybindSettings()
            onClicked: function(button) {
                if (button === standardButton(DialogButtonBox.Reset))
                {
                    MementoSettings.loadKeybindSettings();
                }
                else if (button === standardButton(DialogButtonBox.RestoreDefaults))
                {
                    MementoSettings.defaultKeybindSettings();
                }
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        ColumnLayout {
            Layout.maximumWidth: 200
            Layout.fillHeight: true

            ListView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                }
                clip: true
                model: MementoSettings.keybinds.profiles
                delegate: ItemDelegate {
                    width: ListView.view.width
                    text: modelData.name
                    highlighted: MementoSettings.keybinds.profile === modelData
                    onClicked: MementoSettings.keybinds.profile = modelData
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            implicitWidth: 1
            color: MementoPalette.border
        }

        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: scrollView.contentWidth
            clip: true

            ColumnLayout {
                width: parent.width

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Add current expression to Anki")
                    sequence: MementoSettings.keybinds?.profile.cardAdd
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.cardAdd = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.cardAdd = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Scroll to previous search result")
                    sequence: MementoSettings.keybinds?.profile.cardPrevious
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.cardPrevious = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.cardPrevious = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Scroll to next search result")
                    sequence: MementoSettings.keybinds?.profile.cardNext
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.cardNext = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.cardNext = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Open file")
                    sequence: MementoSettings.keybinds?.profile.openFile
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.openFile = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.openFile = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Open URL")
                    sequence: MementoSettings.keybinds?.profile.openUrl
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.openUrl = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.openUrl = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Cycle OSC visibility")
                    sequence: MementoSettings.keybinds?.profile.oscVisibility
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.oscVisibility = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.oscVisibility = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Show auxiliary search")
                    sequence: MementoSettings.keybinds?.profile.showSearch
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.showSearch = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.showSearch = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Show subtitle list")
                    sequence: MementoSettings.keybinds?.profile.showSubtitleList
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.showSubtitleList = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.showSubtitleList = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    visible: Features.ocr

                    text: qsTr("Start OCR")
                    sequence: MementoSettings.keybinds?.profile.startOcr
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.startOcr = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.startOcr = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Toggle subtitle auto pause")
                    sequence: MementoSettings.keybinds?.profile.subtitleAutoPause
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleAutoPause = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleAutoPause = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Toggle subtitle visibility")
                    sequence: MementoSettings.keybinds?.profile.subtitleShow
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleShow = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleShow = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Decrease subtitle size")
                    sequence: MementoSettings.keybinds?.profile.subtitleDecreaseSize
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleDecreaseSize = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleDecreaseSize = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Increase subtitle size")
                    sequence: MementoSettings.keybinds?.profile.subtitleIncreaseSize
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleIncreaseSize = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleIncreaseSize = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Move subtitle up")
                    sequence: MementoSettings.keybinds?.profile.subtitleMoveUp
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleMoveUp = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleMoveUp = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Move subtitle down")
                    sequence: MementoSettings.keybinds?.profile.subtitleMoveDown
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleMoveDown = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleMoveDown = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Copy subtitle list selection")
                    sequence: MementoSettings.keybinds?.profile.subtitleListCopySelected
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleListCopySelected = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleListCopySelected = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Subtitle list find")
                    sequence: MementoSettings.keybinds?.profile.subtitleListFind
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleListFind = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleListFind = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Subtitle list find next")
                    sequence: MementoSettings.keybinds?.profile.subtitleListFindNext
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleListFindNext = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleListFindNext = ""
                }

                KeybindCommand {
                    Layout.fillWidth: true
                    Layout.margins: root.commonMargin

                    text: qsTr("Subtitle list find previous")
                    sequence: MementoSettings.keybinds?.profile.subtitleListFindPrevious
                    onSequenceSet: function(seq) {
                        if (MementoSettings.keybinds.profile)
                        {
                            MementoSettings.keybinds.profile.subtitleListFindPrevious = seq;
                        }
                    }
                    onClearClicked: MementoSettings.keybinds.profile.subtitleListFindPrevious = ""
                }
            }
        }
    }
}
