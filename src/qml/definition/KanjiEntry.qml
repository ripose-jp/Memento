import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Item {
    id: root

    required property Kanji kanji

    implicitHeight: rootLayout.implicitHeight +
                    rootLayout.anchors.topMargin +
                    rootLayout.anchors.bottomMargin

    /**
     * Check Anki to see if a kanji is addable.
     * Will not recheck kanji that have already been checked.
     */
    function checkKanjiAddable()
    {
        if (!AnkiConfig.enabled ||
                !root.kanji ||
                root.kanji.ankiChecked)
        {
            return;
        }

        AnkiClient.noteAddable(root.kanji).then(function(result) {
            if (!result.success || !root.kanji)
            {
                return;
            }
            root.kanji.addable = result.character;
            root.kanji.ankiChecked = true;
        });
    }

    /**
     * Add the kanji to Anki.
     */
    function addKanji()
    {
        if (!AnkiConfig.enabled ||
                !root.kanji ||
                !root.kanji.ankiChecked ||
                root.kanji.ankiAdding)
        {
            return;
        }

        root.kanji.ankiAdding = true;
        AnkiClient.addNote(root.kanji).then(function(result) {
            if (!result.success)
            {
                errorDialog.title = qsTr("Error Adding Anki Note");
                errorLabel.text = result.error;
                errorDialog.open();

                if (root.kanji)
                {
                    root.kanji.ankiAdding = false;
                }
                return;
            }

            if (!root.kanji)
            {
                return;
            }
            root.kanji.addable = false;
            root.kanji.ankiAdding = false;
        });
    }

    Connections {
        target: AnkiConfig
        function onEnabledChanged() {
            root.checkKanjiAddable();
        }
    }

    Connections {
        target: AnkiConfig
        function onProfileChanged() {
            if (root.kanji)
            {
                root.kanji.ankiChecked = false;
                root.checkKanjiAddable();
            }
        }
    }

    onKanjiChanged: root.checkKanjiAddable()

    Dialog {
        id: errorDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok

        Label {
            id: errorLabel
            width: parent.width
            wrapMode: Label.WordWrap
        }
    }

    ColumnLayout {
        id: rootLayout
        anchors.fill: parent
        anchors.margins: 10

        RowLayout {
            SelectableLabel {
                font.family: MementoSettings.interfaceSearchKanjiFont.family
                font.italic: MementoSettings.interfaceSearchKanjiFont.italic
                font.underline: MementoSettings.interfaceSearchKanjiFont.underline
                font.pointSize: MementoSettings.interfaceSearchKanjiFont.pointSize
                font.weight: MementoSettings.interfaceSearchKanjiFont.weight
                font.overline: MementoSettings.interfaceSearchKanjiFont.overline
                font.strikeout: MementoSettings.interfaceSearchKanjiFont.strikeout
                font.letterSpacing: MementoSettings.interfaceSearchKanjiFont.letterSpacing
                font.wordSpacing: MementoSettings.interfaceSearchKanjiFont.wordSpacing
                font.kerning: MementoSettings.interfaceSearchKanjiFont.kerning
                font.preferShaping: MementoSettings.interfaceSearchKanjiFont.preferShaping
                font.hintingPreference: MementoSettings.interfaceSearchKanjiFont.hintingPreference
                font.styleName: MementoSettings.interfaceSearchKanjiFont.styleName

                text: root.kanji?.character ?? ""
            }

            Item {
                Layout.fillWidth: true
            }

            ToolButton {
                id: openAnkiButton
                Layout.alignment: Qt.AlignTop
                focusPolicy: Qt.NoFocus
                visible: AnkiConfig.enabled &&
                         root.kanji &&
                         root.kanji.ankiChecked &&
                         !root.kanji.addable
                icon.source: Utils.toImageProvider("note-stack", MementoPalette.text)
                onClicked: AnkiClient.openDuplicates(root.kanji).then(function(result) {
                    if (result.success)
                    {
                        return;
                    }
                    errorDialog.title = qsTr("Could Not Open Anki");
                    errorLabel.text = result.error;
                    errorDialog.open();
                })
            }

            ToolButton {
                id: addAnkiButton
                Layout.alignment: Qt.AlignTop
                focusPolicy: Qt.NoFocus
                visible: AnkiConfig.enabled &&
                         root.kanji &&
                         root.kanji.ankiChecked &&
                         root.kanji.addable
                enabled: root.kanji && !root.kanji.ankiAdding
                icon.name: MementoSettings.interfaceSystemIcons ? "list-add" : null
                icon.source: Utils.toImageProvider("add", MementoPalette.text)
                onClicked: root.addKanji()
            }
        }

        Repeater {
            model: root.kanji?.definitions ?? []
            delegate: ColumnLayout {
                id: defLayout
                Layout.fillWidth: true

                required property int index
                readonly property KanjiDefinition definition: root.kanji?.definitions[index]

                CenteredFlow {
                    Layout.fillWidth: true
                    spacing: 5

                    Repeater {
                        model: defLayout?.definition.tags ?? []
                        delegate: TagLabel {
                            required property int index
                            tag: defLayout?.definition.tags[index]
                        }
                    }

                    DictionaryLabel {
                        info: defLayout?.definition.dictionaryInfo
                    }
                }

                CenteredFlow {
                    id: frequencyLayout
                    Layout.fillWidth: true
                    spacing: 5
                    visible: frequencyRepeater.count > 0

                    Repeater {
                        id: frequencyRepeater
                        model: Utils.foldFrequencies(root.kanji?.frequencies)
                        delegate: FrequencyLabel {
                            name: modelData.name
                            value: modelData.value
                        }
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 3
                    uniformCellWidths: true

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.columnSpan: parent.columns
                        implicitHeight: 1
                        color: MementoPalette.border
                    }

                    SelectableLabel {
                        font.family: MementoSettings.interfaceSearchGlossaryFont.family
                        font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                        font.bold: true
                        text: qsTr("Glossary")
                    }

                    SelectableLabel {
                        font.family: MementoSettings.interfaceSearchGlossaryFont.family
                        font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                        font.bold: true
                        text: qsTr("Reading")
                    }

                    SelectableLabel {
                        font.family: MementoSettings.interfaceSearchGlossaryFont.family
                        font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                        font.bold: true
                        text: qsTr("Statistics")
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.columnSpan: parent.columns
                        implicitHeight: 1
                        color: MementoPalette.border
                    }

                    SelectableLabel {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                        wrapMode: TextEdit.Wrap
                        font.family: MementoSettings.interfaceSearchGlossaryFont.family
                        font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                        text: {
                            if (!defLayout.definition)
                            {
                                return "";
                            }
                            return defLayout.definition.glossary.map(function(glossary, index) {
                                return `${index + 1}. ${glossary}`;
                            }).join('\n');
                        }
                    }

                    SelectableLabel {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                        wrapMode: TextEdit.Wrap
                        font.family: MementoSettings.interfaceSearchGlossaryFont.family
                        font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                        text: {
                            if (!defLayout.definition)
                            {
                                return "";
                            }
                            return [...defLayout.definition.onyomi, ...defLayout.definition.kunyomi].join('\n');
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop

                        Repeater {
                            model: defLayout?.definition.stats ?? []
                            delegate: KanjiValueTag {
                                Layout.fillWidth: true
                                required property int index
                                tag: defLayout?.definition.stats[index]
                            }
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 1
                    color: MementoPalette.border
                }

                SelectableLabel {
                    Layout.fillWidth: true
                    font.family: MementoSettings.interfaceSearchGlossaryFont.family
                    font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                    font.bold: true
                    text: qsTr("Classifications")
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 1
                    color: MementoPalette.border
                }

                Repeater {
                    model: defLayout?.definition.classes ?? []
                    delegate: KanjiValueTag {
                        Layout.fillWidth: true
                        required property int index
                        tag: defLayout?.definition.classes[index]
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 1
                    color: MementoPalette.border
                }

                SelectableLabel {
                    Layout.fillWidth: true
                    font.family: MementoSettings.interfaceSearchGlossaryFont.family
                    font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                    font.bold: true
                    text: qsTr("Codepoints")
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 1
                    color: MementoPalette.border
                }

                Repeater {
                    model: defLayout?.definition.codes ?? []
                    delegate: KanjiValueTag {
                        Layout.fillWidth: true
                        required property int index
                        tag: defLayout?.definition.codes[index]
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 1
                    color: MementoPalette.border
                }

                SelectableLabel {
                    Layout.fillWidth: true
                    font.family: MementoSettings.interfaceSearchGlossaryFont.family
                    font.pointSize: MementoSettings.interfaceSearchGlossaryFont.pointSize
                    font.bold: true
                    text: qsTr("Dictionary Indices")
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 1
                    color: MementoPalette.border
                }

                Repeater {
                    model: defLayout?.definition.indices ?? []
                    delegate: KanjiValueTag {
                        Layout.fillWidth: true
                        required property int index
                        tag: defLayout?.definition.indices[index]
                    }
                }
            }
        }
    }
}
