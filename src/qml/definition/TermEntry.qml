import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Item {
    id: root

    required property Term term

    signal searchRequested(query: string)
    signal kanjiClicked(expression: string, index: int)

    implicitHeight: rootLayout.implicitHeight

    /**
     * Check Anki to see if a term is addable.
     * Will not recheck terms that have already been checked.
     */
    function checkTermAddable()
    {
        if (!AnkiConfig.enabled ||
                !root.term ||
                root.term.ankiChecked)
        {
            return;
        }

        AnkiClient.noteAddable(root.term).then(function(result) {
            if (!result.success || !root.term)
            {
                return;
            }
            root.term.addableExpression = result.expression;
            root.term.addableReading = result.reading;
            if (!root.term.addableExpression && root.term.addableReading)
            {
                root.term.readingAsExpression = true;
            }

            let id = -1;
            let selected = false;
            for (let i = 0; i < root.term.definitions.length; ++i)
            {
                if (root.term.definitions[i].dictionaryInfo.id !== id)
                {
                    id = root.term.definitions[i].dictionaryInfo.id;
                    selected = !AnkiConfig.profile.excludeGlossaries.includes(
                                root.term.definitions[i].dictionaryInfo.name);
                }
                root.term.definitions[i].selected = selected;
            }

            root.term.ankiChecked = true;
        });
    }

    /**
     * Add the term to Anki.
     */
    function addTerm()
    {
        if (!AnkiConfig.enabled ||
                !root.term ||
                !root.term.ankiChecked ||
                root.term.ankiAdding)
        {
            return;
        }

        let selection = [];
        for (let i = 0; i < glossaryRepeater.count; ++i)
        {
            let item = glossaryRepeater.itemAt(i);
            if (item && item.selectedText.length > 0)
            {
                selection.push(item.selectedText);
            }
        }
        root.term.selection = selection;

        root.term.ankiAdding = true;
        AnkiClient.addNote(root.term).then(function(result) {
            if (!result.success)
            {
                errorDialog.title = qsTr("Error Adding Anki Note");
                errorLabel.text = result.error;
                errorDialog.open();

                if (root.term)
                {
                    root.term.ankiAdding = false;
                }
                return;
            }

            if (!root.term)
            {
                return;
            }

            if (root.term.readingAsExpression)
            {
                root.term.addableReading = false;
            }
            else
            {
                root.term.addableExpression = false;
            }
            root.term.readingAsExpression = !root.term.readingAsExpression;

            root.term.ankiAdding = false;
        });
    }

    AudioFiles {
        id: audioFiles
        term: root.term
        onLoadStateChanged: {
            if (!root.term?.autoPlay ||
                    audioFiles.loadState !== AudioFiles.LoadState.Loaded ||
                    audioFiles.files.count === 0)
            {
                return;
            }
            audioButton.clicked();
            root.term.autoPlay = false;
        }
    }

    Connections {
        target: AnkiConfig
        function onEnabledChanged() {
            root.checkTermAddable();
        }
    }

    Connections {
        target: AnkiConfig
        function onProfileChanged() {
            if (root.term)
            {
                root.term.ankiChecked = false;
                root.checkTermAddable();
            }
        }
    }

    onTermChanged: root.checkTermAddable()

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

        RowLayout {
            id: topLayout
            Layout.fillWidth: true

            ColumnLayout {
                Layout.alignment: Qt.AlignLeft
                spacing: 0

                SelectableLabel {
                    Layout.alignment: Qt.AlignHCenter
                    visible: text.length > 0

                    font.family: MementoSettings.interfaceSearchReadingFont.family
                    font.italic: MementoSettings.interfaceSearchReadingFont.italic
                    font.underline: MementoSettings.interfaceSearchReadingFont.underline
                    font.pointSize: MementoSettings.interfaceSearchReadingFont.pointSize
                    font.weight: MementoSettings.interfaceSearchReadingFont.weight
                    font.overline: MementoSettings.interfaceSearchReadingFont.overline
                    font.strikeout: MementoSettings.interfaceSearchReadingFont.strikeout
                    font.letterSpacing: MementoSettings.interfaceSearchReadingFont.letterSpacing
                    font.wordSpacing: MementoSettings.interfaceSearchReadingFont.wordSpacing
                    font.kerning: MementoSettings.interfaceSearchReadingFont.kerning
                    font.preferShaping: MementoSettings.interfaceSearchReadingFont.preferShaping
                    font.hintingPreference: MementoSettings.interfaceSearchReadingFont.hintingPreference
                    font.styleName: MementoSettings.interfaceSearchReadingFont.styleName

                    text: root.term?.reading ?? ""
                }

                ExpressionLabel {
                    Layout.alignment: Qt.AlignHCenter
                    visible: text.length > 0

                    font.family: MementoSettings.interfaceSearchExpressionFont.family
                    font.italic: MementoSettings.interfaceSearchExpressionFont.italic
                    font.underline: MementoSettings.interfaceSearchExpressionFont.underline
                    font.pointSize: MementoSettings.interfaceSearchExpressionFont.pointSize
                    font.weight: MementoSettings.interfaceSearchExpressionFont.weight
                    font.overline: MementoSettings.interfaceSearchExpressionFont.overline
                    font.strikeout: MementoSettings.interfaceSearchExpressionFont.strikeout
                    font.letterSpacing: MementoSettings.interfaceSearchExpressionFont.letterSpacing
                    font.wordSpacing: MementoSettings.interfaceSearchExpressionFont.wordSpacing
                    font.kerning: MementoSettings.interfaceSearchExpressionFont.kerning
                    font.preferShaping: MementoSettings.interfaceSearchExpressionFont.preferShaping
                    font.hintingPreference: MementoSettings.interfaceSearchExpressionFont.hintingPreference
                    font.styleName: MementoSettings.interfaceSearchExpressionFont.styleName

                    expression: root.term?.expression ?? ""
                    onKanjiClicked: (expression, index) => root.kanjiClicked(expression, index)
                }
            }

            ColumnLayout {
                Layout.fillWidth: true

                RowLayout {
                    Layout.fillWidth: true

                    Item {
                        Layout.fillWidth: true
                    }

                    ToolButton {
                        id: openAnkiButton
                        focusPolicy: Qt.NoFocus
                        visible: {
                            if (!AnkiConfig.enabled || !root.term || !root.term.ankiChecked)
                            {
                                return false;
                            }
                            else if (root.term.reading.length === 0)
                            {
                                return !root.term.addableExpression;
                            }
                            return !root.term.addableExpression || !root.term.addableReading;
                        }
                        icon.source: Utils.toImageProvider("note-stack", MementoPalette.text)
                        onClicked: AnkiClient.openDuplicates(root.term).then(function(result) {
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
                        id: kanjiKanaButton
                        focusPolicy: Qt.NoFocus
                        visible: AnkiConfig.enabled &&
                                 root.term &&
                                 root.term.ankiChecked &&
                                 root.term.reading.length > 0 &&
                                 (root.term.addableExpression || root.term.addableReading)
                        enabled: root.term &&
                                 !root.term.ankiAdding &&
                                 root.term.addableExpression &&
                                 root.term.addableReading
                        icon.source: Utils.toImageProvider(root.term?.readingAsExpression ? "kana" : "kanji", MementoPalette.text)
                        onClicked: {
                            if (root.term)
                            {
                                root.term.readingAsExpression = !root.term.readingAsExpression
                            }
                        }
                    }

                    ToolButton {
                        id: addAnkiButton
                        focusPolicy: Qt.NoFocus
                        visible: AnkiConfig.enabled &&
                                 root.term &&
                                 root.term.ankiChecked &&
                                 (root.term.addableExpression || root.term.addableReading)
                        enabled: audioFiles.loadState === AudioFiles.LoadState.Loaded &&
                                 root.term &&
                                 !root.term.ankiAdding
                        icon.name: MementoSettings.interfaceSystemIcons ? "list-add" : null
                        icon.source: Utils.toImageProvider("add", MementoPalette.text)

                        onClicked: {
                            const audioSource = audioFiles.files.get(0);
                            if (audioSource.exists)
                            {
                                root.term.audioSourceName = audioSource.name;
                                root.term.audioUrl = audioSource.url;
                                root.term.audioSkipHash = audioSource.skipHash;
                            }
                            root.addTerm();
                        }

                        ContextMenu.menu: Menu {
                            id: addAnkiMenu

                            Instantiator {
                                model: audioFiles.files
                                delegate: MenuItem {
                                    text: model.name
                                    enabled: model.exists
                                    icon.source: Utils.toImageProvider(model.exists ? "volume-on" : "volume-off", MementoPalette.text)
                                    onTriggered: {
                                        root.term.audioSourceName = model.name;
                                        root.term.audioUrl = model.url;
                                        root.term.audioSkipHash = model.skipHash;
                                        root.addTerm();
                                    }
                                }
                                onObjectAdded: (index, object) => addAnkiMenu.insertItem(index, object)
                                onObjectRemoved: (index, object) => addAnkiMenu.removeItem(object)
                            }
                        }
                    }

                    ToolButton {
                        id: audioButton

                        /* true when audio has been requested but not yet played */
                        property bool loading: false

                        focusPolicy: Qt.NoFocus

                        enabled: !audioButton.loading &&
                                 audioFiles.loadState === AudioFiles.LoadState.Loaded &&
                                 audioFiles.files.count > 0

                        icon.source: {
                            if (audioFiles.loadState !== AudioFiles.LoadState.Loaded ||
                                    (audioFiles.files.count > 0 && audioFiles.files.get(0).exists))
                            {
                                return Utils.toImageProvider("volume-on", MementoPalette.text);
                            }
                            return Utils.toImageProvider("volume-off", MementoPalette.text);
                        }

                        onClicked: {
                            if (!audioFiles.files.get(0).exists)
                            {
                                return;
                            }
                            audioButton.loading = true;
                            AudioPlayer.play(audioFiles.files.get(0).url, audioFiles.files.get(0).skipHash)
                                .then(
                                    function(result)
                                    {
                                        if (audioFiles.files.get(0).exists !== result)
                                        {
                                            audioFiles.files.setProperty(0, "exists", result);
                                        }
                                        audioButton.loading = false;
                                    }
                                );
                        }

                        ContextMenu.menu: Menu {
                            id: audioButtonMenu

                            Instantiator {
                                model: audioFiles.files

                                delegate: MenuItem {
                                    text: model.name
                                    enabled: model.exists
                                    icon.source: Utils.toImageProvider(model.exists ? "volume-on" : "volume-off", MementoPalette.text)
                                    onTriggered: {
                                        audioButton.loading = true;
                                        AudioPlayer.play(model.url, model.skipHash)
                                            .then(
                                                function(result)
                                                {
                                                    if (model.exists !== result)
                                                    {
                                                        audioFiles.files.setProperty(index, "exists", result);
                                                    }
                                                    audioButton.loading = false;
                                                }
                                            );
                                    }
                                }

                                onObjectAdded: (index, object) => audioButtonMenu.insertItem(index, object)
                                onObjectRemoved: (index, object) => audioButtonMenu.removeItem(object)
                            }
                        }
                    }

                    ToolButton {
                        id: collapseButton
                        focusPolicy: Qt.NoFocus
                        visible: !root.term?.collapsed ?? false
                        icon.name: MementoSettings.interfaceSystemIcons ? "go-down" : null
                        icon.source: Utils.toImageProvider("arrow-drop-down", MementoPalette.text)
                        onClicked: if (root.term) root.term.collapsed = true
                    }

                    ToolButton {
                        id: uncollapseButton
                        focusPolicy: Qt.NoFocus
                        visible: root.term?.collapsed ?? false
                        icon.name: MementoSettings.interfaceSystemIcons ? "go-up" : null
                        icon.source: Utils.toImageProvider("arrow-drop-up", MementoPalette.text)
                        onClicked: if (root.term) root.term.collapsed = false
                    }
                }

                SelectableLabel {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    wrapMode: TextEdit.WordWrap
                    visible: text.length > 0

                    font.family: MementoSettings.interfaceSearchConjFont.family
                    font.italic: MementoSettings.interfaceSearchConjFont.italic
                    font.underline: MementoSettings.interfaceSearchConjFont.underline
                    font.pointSize: MementoSettings.interfaceSearchConjFont.pointSize
                    font.weight: MementoSettings.interfaceSearchConjFont.weight
                    font.overline: MementoSettings.interfaceSearchConjFont.overline
                    font.strikeout: MementoSettings.interfaceSearchConjFont.strikeout
                    font.letterSpacing: MementoSettings.interfaceSearchConjFont.letterSpacing
                    font.wordSpacing: MementoSettings.interfaceSearchConjFont.wordSpacing
                    font.kerning: MementoSettings.interfaceSearchConjFont.kerning
                    font.preferShaping: MementoSettings.interfaceSearchConjFont.preferShaping
                    font.hintingPreference: MementoSettings.interfaceSearchConjFont.hintingPreference
                    font.styleName: MementoSettings.interfaceSearchConjFont.styleName

                    text: root.term?.conjugationExplanation ?? ""
                }
            }
        }

        CenteredFlow {
            id: tagsLayout
            Layout.fillWidth: true
            spacing: 5
            visible: !root.term?.collapsed && (root.term?.tags.length ?? false)

            Repeater {
                model: root.term?.tags ?? []
                delegate: TagLabel {
                    required property int index
                    tag: root.term?.tags[index]
                }
            }
        }

        CenteredFlow {
            id: frequencyLayout
            Layout.fillWidth: true
            spacing: 5
            visible: !root.term?.collapsed && frequencyRepeater.count > 0

            Repeater {
                id: frequencyRepeater
                model: Utils.foldFrequencies(root.term?.frequencies)
                delegate: FrequencyLabel {
                    name: modelData.name
                    value: modelData.value
                }
            }
        }

        CenteredFlow {
            id: pitchLayout
            Layout.fillWidth: true
            spacing: 5
            visible: !root.term?.collapsed && (root.term?.pitches.length ?? false)

            Repeater {
                model: root.term?.pitches ?? []
                delegate: TermPitch {
                    required property int index
                    pitch: root.term?.pitches[index]
                }
            }
        }

        Repeater {
            id: glossaryRepeater
            Layout.fillWidth: true

            model: root.term?.definitions ?? []
            delegate: TermGlossary {
                required property int index
                term: root.term
                definition: root.term?.definitions[index]
                visible: !root.term?.collapsed

                onSearchRequested: (query) => root.searchRequested(query)
            }
        }
    }
}
