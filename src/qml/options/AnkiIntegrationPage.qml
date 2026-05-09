import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    property int preferredWidth: 600
    property int groupSpacing: 10

    property var decks: []
    property bool decksValid: false

    property var models: []
    property bool modelsValid: false

    /**
     * Update the decks and models properties with AnkiConnect values.
     */
    function updateDecksAndModels() {
        AnkiClient.getDeckNames().then(function(result) {
            root.decksValid = result.success;
            if (!result.success)
            {
                return;
            }
            root.decks = result.decks;
        });
        AnkiClient.getModelNames().then(function(result) {
            root.modelsValid = result.success;
            if (!result.success)
            {
                return;
            }
            root.models = result.models;
        });
    }

    Component.onCompleted: root.updateDecksAndModels()
    onVisibleChanged: {
        if (root.visible)
        {
            root.updateDecksAndModels();
        }
    }

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
                    icon.name: MementoSettings.interfaceSystemIcons ? "edit-delete" : null
                    icon.source: Utils.toImageProvider("delete", MementoPalette.text)
                    onClicked: AnkiConfig.removeProfile(AnkiConfig.profile.name)
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
            visible: Features.platform !== Features.Linux
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
                return text.length > 0 && !AnkiConfig.profileExists(text);
            });
        }

        onClosed: {
            addProfileNameTextField.text = "";
            addProfileCopyCheckBox.checked = true;
        }

        onAccepted: {
            const name = addProfileNameTextField.text.trim();
            const added = AnkiConfig.addProfile(
                name,
                addProfileCopyCheckBox.checked ? AnkiConfig.profile : null
            );
            if (added)
            {
                AnkiConfig.setProfile(name);
            }
        }
    }

    Dialog {
        id: messageDialog
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok

        Label {
            id: messageDialogLabel
        }
    }

    footer: ColumnLayout {
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            visible: Features.platform === Features.Linux && !scrollView.atBottom
            color: MementoPalette.border
            height: 1
        }

        DialogButtonBox {
            id: buttonBoxFooter
            Layout.fillWidth: true
            standardButtons: DialogButtonBox.Apply |
                                DialogButtonBox.RestoreDefaults |
                                DialogButtonBox.Reset |
                                DialogButtonBox.Help

            onApplied: AnkiConfig.write()
            onClicked: function(button) {
                if (button === standardButton(DialogButtonBox.Reset))
                {
                    AnkiConfig.load();
                }
                else if (button === standardButton(DialogButtonBox.RestoreDefaults))
                {
                    AnkiConfig.profile.defaults();
                }
            }
            onHelpRequested: ankiIntegrationHelpWindow.show()
        }
    }

    AnkiIntegrationHelpWindow {
        id: ankiIntegrationHelpWindow
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
                id: generalBox
                Layout.preferredWidth: root.preferredWidth
                Layout.topMargin: root.groupSpacing
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("General")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Enable Anki integration")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: AnkiConfig.enabled
                            onClicked: AnkiConfig.enabled = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Show advanced settings")
                        }
                        Switch {
                            id: advancedSwitch
                            Layout.alignment: Qt.AlignRight
                        }
                    }
                }
            }

            SettingsBox {
                id: profileBox
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Profile")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Current profile")
                        }
                        ComboBox {
                            id: profileComboBox
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 200
                            editable: true
                            model: AnkiConfig.profiles
                            textRole: "name"
                            currentIndex: AnkiConfig.profileIndex(AnkiConfig.profile.name)

                            onActivated: function(index) {
                                AnkiConfig.setProfile(AnkiConfig.profiles[index].name);
                            }

                            onAccepted: profileComboBox.updateName()

                            onActiveFocusChanged: {
                                if (!activeFocus)
                                {
                                    profileComboBox.updateName()
                                }
                            }

                            function updateName() {
                                let newName = editText.trim();
                                if (newName && !AnkiConfig.profileExists(editText))
                                {
                                    AnkiConfig.profile.name = editText;
                                }
                                else
                                {
                                    editText = currentText;
                                }
                            }
                        }
                    }

                    Label {
                        Layout.alignment: Qt.AlignRight
                        wrapMode: Text.Wrap
                        font.bold: true
                        visible: {
                            let newName = profileComboBox.editText.trim();
                            return newName !== profileComboBox.currentText &&
                                    AnkiConfig.profileExists(newName);
                        }
                        text: qsTr("A profile with this name already exists")
                    }
                }
            }

            SettingsBox {
                id: ankiConnectBox
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("AnkiConnect")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Hostname")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            placeholderText: qsTr("Hostname")
                            text: AnkiConfig.profile.hostname
                            onEditingFinished: AnkiConfig.profile.hostname = text
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Port")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            placeholderText: qsTr("Port")
                            validator: IntValidator {
                                bottom: 0
                                top: 65535
                            }
                            text: AnkiConfig.profile.port
                            onEditingFinished: AnkiConfig.profile.port = text
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Test connection")
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight
                            text: qsTr("Connect")
                            onClicked: {
                                enabled = false;
                                AnkiClient.testConnection().then(function(result) {
                                    if (result.success)
                                    {
                                        root.updateDecksAndModels();
                                        messageDialog.title = qsTr("Anki Connection");
                                        messageDialogLabel.text = qsTr("Connection successful.");
                                        messageDialog.open();
                                    }
                                    else
                                    {
                                        messageDialog.title = qsTr("Anki Connection Error");
                                        messageDialogLabel.text = qsTr("Failed to connect to Anki: %1.").arg(result.error);
                                        messageDialog.open();
                                    }
                                    enabled = true;
                                })
                            }
                        }
                    }
                }
            }

            SettingsBox {
                id: advancedBox
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                visible: advancedSwitch.checked
                title: qsTr("Advanced")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Duplicate policy")
                        }
                        ComboBox {
                            Layout.alignment: Qt.AlignRight
                            implicitContentWidthPolicy: ComboBox.WidestText
                            model: ListModel {
                                ListElement {
                                    text: qsTr("None")
                                    value: AnkiSetting.DuplicatePolicyNone
                                }
                                ListElement {
                                    text: qsTr("Allowed in different decks")
                                    value: AnkiSetting.DuplicatePolicyDifferentDeck
                                }
                                ListElement {
                                    text: qsTr("Allowed in same deck")
                                    value: AnkiSetting.DuplicatePolicySameDeck
                                }
                            }
                            textRole: "text"
                            valueRole: "value"
                            currentValue: AnkiConfig.profile.duplicatePolicy
                            onActivated: AnkiConfig.profile.duplicatePolicy = currentValue
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Replace newlines with")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            placeholderText: qsTr("Replacement string")
                            text: AnkiConfig.profile.newlineReplacer
                            onEditingFinished: AnkiConfig.profile.newlineReplacer = text
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Screenshot file type")
                        }
                        ComboBox {
                            Layout.alignment: Qt.AlignRight
                            implicitContentWidthPolicy: ComboBox.WidestText
                            model: ListModel {
                                ListElement {
                                    text: qsTr("JPEG")
                                    value: AnkiSetting.FileTypeJpeg
                                }
                                ListElement {
                                    text: qsTr("PNG")
                                    value: AnkiSetting.FileTypePng
                                }
                                ListElement {
                                    text: qsTr("WebP")
                                    value: AnkiSetting.FileTypeWebp
                                }
                            }
                            textRole: "text"
                            valueRole: "value"
                            currentValue: AnkiConfig.profile.screenshotType
                            onActivated: AnkiConfig.profile.screenshotType = currentValue
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Audio padding start duration")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 75
                            placeholderText: qsTr("Seconds")
                            validator: DoubleValidator {}
                            text: AnkiConfig.profile.audioPadStart
                            onEditingFinished: AnkiConfig.profile.audioPadStart = text
                            onActiveFocusChanged: {
                                if (!acceptableInput)
                                {
                                    text = Qt.binding(function () {
                                        return AnkiConfig.profile.audioPadStart;
                                    });
                                }
                            }
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Audio padding end duration")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 75
                            placeholderText: qsTr("Seconds")
                            validator: DoubleValidator {}
                            text: AnkiConfig.profile.audioPadEnd
                            onEditingFinished: AnkiConfig.profile.audioPadEnd = text
                            onActiveFocusChanged: {
                                if (!acceptableInput)
                                {
                                    text = Qt.binding(function () {
                                        return AnkiConfig.profile.audioPadEnd;
                                    });
                                }
                            }
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Audio media normalization")
                        }
                        Switch {
                            Layout.alignment: Qt.AlignRight
                            checked: AnkiConfig.profile.audioNormalize
                            onClicked: AnkiConfig.profile.audioNormalize = checked
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Audio normalization dB")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 75
                            placeholderText: qsTr("dB")
                            validator: DoubleValidator {}
                            text: AnkiConfig.profile.audioDb
                            onEditingFinished: AnkiConfig.profile.audioDb = text
                            onActiveFocusChanged: {
                                if (!acceptableInput)
                                {
                                    text = Qt.binding(function () {
                                        return AnkiConfig.profile.audioDb;
                                    });
                                }
                            }
                        }
                    }

                    SettingsBoxSeparator {
                        Layout.fillWidth: true
                    }

                    Label {
                        Layout.alignment: Qt.AlignLeft
                        text: qsTr("Include in glossary")
                    }
                    Repeater {
                        model: DictionaryController.dictionaries
                        delegate: CheckBox {
                            text: model.name
                            checked: !AnkiConfig.profile.excludeGlossaries.includes(model.name)
                            onToggled: {
                                if (checked)
                                {
                                    AnkiConfig.profile.excludeGlossaries =
                                            AnkiConfig.profile.excludeGlossaries.filter(name => name !== model.name);
                                }
                                else
                                {
                                    AnkiConfig.profile.excludeGlossaries.push(model.name);
                                }
                            }
                        }
                    }
                }
            }

            SettingsBox {
                id: cardsBox
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                title: qsTr("Cards")

                ColumnLayout {
                    anchors.fill: parent
                    spacing: root.groupSpacing

                    RowLayout {
                        Label {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignLeft
                            text: qsTr("Tags")
                        }
                        TextField {
                            Layout.alignment: Qt.AlignRight
                            Layout.preferredWidth: 250
                            placeholderText: qsTr("Tags")
                            text: AnkiConfig.profile.tags.join(" ")
                            onEditingFinished: AnkiConfig.profile.tags = text.trim().split(/[ ,]+/).filter(Boolean)
                        }
                    }
                }
            }

            AnkiNoteBox {
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                groupSpacing: root.groupSpacing
                title: qsTr("Term Notes")

                enabled: root.decksValid && root.modelsValid

                decks: root.decks
                currentDeck: AnkiConfig.profile.termDeck
                onDeckActivated: function(index) {
                    AnkiConfig.profile.termDeck = decks[index];
                }

                models: root.models
                currentModel: AnkiConfig.profile.termModel
                onModelActivated: function(index) {
                    let model = models[index];
                    AnkiConfig.profile.termModel = model;
                    AnkiClient.getFieldNames(model).then(function(result) {
                        if (!result.success)
                        {
                            messageDialog.title = qsTr("Anki Connection Error");
                            messageDialogLabel.text = qsTr("Could not get model fields: %1.").arg(result.error);
                            messageDialog.open();
                            AnkiConfig.profile.termFields.clear();
                            return;
                        }
                        AnkiConfig.profile.termFields.setFields(result.fields);
                    });
                }

                fields: AnkiConfig.profile.termFields
                markers: AnkiMarkers.termMarkers
            }

            AnkiNoteBox {
                Layout.preferredWidth: root.preferredWidth
                Layout.alignment: Qt.AlignHCenter
                Layout.bottomMargin: root.groupSpacing
                groupSpacing: root.groupSpacing
                title: qsTr("Kanji Notes")

                enabled: root.decksValid && root.modelsValid

                decks: root.decks
                currentDeck: AnkiConfig.profile.kanjiDeck
                onDeckActivated: function(index) {
                    AnkiConfig.profile.kanjiDeck = decks[index];
                }

                models: root.models
                currentModel: AnkiConfig.profile.kanjiModel
                onModelActivated: function(index) {
                    let model = models[index];
                    AnkiConfig.profile.kanjiModel = model;
                    AnkiClient.getFieldNames(model).then(function(result) {
                        if (!result.success)
                        {
                            messageDialog.title = qsTr("Anki Connection Error");
                            messageDialogLabel.text = qsTr("Could not get model fields: %1.").arg(result.error);
                            messageDialog.open();
                            AnkiConfig.profile.kanjiFields.clear();
                            return;
                        }
                        AnkiConfig.profile.kanjiFields.setFields(result.fields);
                    });
                }

                fields: AnkiConfig.profile.kanjiFields
                markers: AnkiMarkers.kanjiMarkers
            }
        }
    }
}
