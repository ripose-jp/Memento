import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

ColumnLayout {
    id: root

    required property Term term
    required property TermDefinition definition
    required property color canvasColor

    readonly property string selectedText: glossaryText.ankiSelectedText

    signal searchRequested(query: string)

    CenteredFlow {
        Layout.fillWidth: true

        spacing: 5

        CheckBox {
            id: checkBox

            Component.onCompleted: {
                if (Features.isUnix)
                {
                    return;
                }

                checkBox.leftPadding = 0;
                checkBox.rightPadding = 0;
                checkBox.topPadding = 0;
                checkBox.bottomPadding = 0;
                checkBox.leftInset = 0;
                checkBox.rightInset = 0;
                checkBox.topInset = 0;
                checkBox.bottomInset = 0;
                checkBox.spacing = 0;
                checkBox.implicitWidth = Qt.binding(function() {
                    return checkBox.contentItem.implicitWidth +
                            checkBox.indicator.implicitWidth +
                            checkBox.spacing;
                });
            }

            visible: AnkiConfig.enabled &&
                     MementoSettings.showAnkiGlossaryCheckbox &&
                     root.term &&
                     root.term.ankiChecked &&
                     (root.term.addableExpression || root.term.addableReading)
            enabled: root.term && !root.term.ankiAdding
            checked: root.definition?.selected ?? false
            onClicked: root.definition.selected = checked
        }

        Repeater {
            model: root.definition?.tags ?? []
            delegate: TagLabel {
                required property int index
                tag: root.definition?.tags[index]
            }
        }

        DictionaryLabel {
            info: root.definition?.dictionaryInfo
        }
    }

    GlossaryText {
        id: glossaryText
        Layout.fillWidth: true
        persistentSelection: true
        definition: root.definition
        canvasColor: root.canvasColor

        onSearchRequested: (query) => root.searchRequested(query)
    }
}
