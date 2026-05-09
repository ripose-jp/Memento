import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

ColumnLayout {
    id: root

    required property Term term
    required property TermDefinition definition

    readonly property alias selectedText: glossaryText.selectedText

    Flow {
        Layout.fillWidth: true

        spacing: 5

        CheckBox {
            id: checkBox

            Component.onCompleted: {
                if (Features.platform === Features.Linux)
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
                     root.term &&
                     root.term.ankiChecked &&
                     (root.term.addableExpression || root.term.addableReading)
            enabled: root.term && !root.term.ankiAdding
            checked: root.definition?.selected ?? false
            onClicked: root.definition.selected = checked
        }

        Repeater {
            model: root.definition?.tags ?? []
            delegate: Item {
                id: delegateItem

                required property int index

                implicitHeight: Math.max(checkBox.implicitHeight, delegateTag.height)
                implicitWidth: delegateTag.width

                TagLabel {
                    id: delegateTag
                    anchors.centerIn: parent
                    tag: root.definition?.tags[delegateItem.index]
                }
            }
        }

        Item {
            implicitHeight: Math.max(checkBox.implicitHeight, dictionaryTag.height)
            implicitWidth: dictionaryTag.width

            DictionaryLabel {
                id: dictionaryTag
                anchors.centerIn: parent
                info: root.definition?.dictionaryInfo
            }
        }
    }

    GlossaryText {
        id: glossaryText
        Layout.fillWidth: true
        persistentSelection: true
        content: root.definition?.glossary
    }
}
