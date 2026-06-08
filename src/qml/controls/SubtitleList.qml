import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

ListView {
    id: root

    required property SubtitleListModel subtitleListModel
    readonly property ItemSelectionModel selectionModel: root.subtitleListModel?.selectionModel ?? null
    readonly property var regexFilter: Utils.safeRegex(MementoSettings.searchRemoveRegex, "g")

    property int delay: 0
    property color textColor: "white"
    property color hoverColor: "#1A1A1A"
    property color selectedColor: "#333333"
    property color selectedHoverColor: "#474747"
    property font textFont: ({
                                 family: "Noto Sans JP",
                                 pointSize: 14
                             })

    /**
     * Emitted when a player seek is requested.
     * @param position The time to seek to.
     */
    signal seekRequested(position: real)

    interactive: true
    acceptedButtons: Qt.NoButton
    boundsBehavior: Flickable.StopAtBounds
    flickDeceleration: 5000
    reuseItems: true

    ScrollBar.vertical: ScrollBar {}

    Clipboard {
        id: clipboard
    }

    Action {
        id: copySelectionAction
        enabled: root.visible &&
                 root.subtitleListModel &&
                 root.selectionModel &&
                 root.selectionModel.hasSelection
        shortcut: MementoSettings.keybinds.profile?.subtitleListCopySelected
        onTriggered: {
            let indices = root.selectionModel.selectedIndexes;

            let rows = [];
            for (let i = 0; i < indices.length; ++i)
            {
                rows.push(indices[i].row);
            }
            rows.sort();

            let lines = rows.map(function(i) {
                let index = root.subtitleListModel.index(i, 0);
                let text = root.subtitleListModel.data(index, SubtitleListModel.TextRole);
                return text.replace(root.regexFilter, "");
            });
            let text = lines.filter(str => str.length > 0).join("\n");
            if (text.length > 0)
            {
                clipboard.setText(text);
            }
        }
    }

    Connections {
        target: root.subtitleListModel
        function onPositionSelected(start, end) {
            root.positionViewAtIndex(end, ListView.Center);
        }
    }

    Label {
        id: timecodeMetricsLabel
        visible: false
        font.pointSize: root.textFont.pointSize
        text: "00:00:00"
    }

    model: root.subtitleListModel
    delegate: Rectangle {
        id: delegateItem

        width: ListView.view.width
        height: delegateItem.visible ? delegateLayout.implicitHeight : 0
        visible: delegateText.text.length > 0
        color: {
            if (root.selectionModel?.hasSelection && root.selectionModel?.isRowSelected(index))
            {
                return delegateHoverHandler.hovered ? root.selectedHoverColor : root.selectedColor;
            }
            return delegateHoverHandler.hovered ? root.hoverColor : "transparent";
        }

        RowLayout {
            id: delegateLayout
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 10

            Label {
                Layout.preferredWidth: timecodeMetricsLabel.implicitWidth
                Layout.alignment: Qt.AlignTop

                visible: MementoSettings.interfaceSubtitleListTimestamps
                color: root.textColor
                font.pointSize: root.textFont.pointSize
                text: {
                    const SECONDS_IN_HOUR = 3600;
                    const SECONDS_IN_MINUTE = 60;

                    let total = Math.floor(model.start + root.delay);
                    total = Math.max(total, 0);

                    const hours = Math.floor(total / SECONDS_IN_HOUR);
                    const minutes = Math.floor((total - hours * SECONDS_IN_HOUR) / SECONDS_IN_MINUTE);
                    const seconds = total % SECONDS_IN_MINUTE;

                    const hh = String(hours).padStart(2, '0');
                    const mm = String(minutes).padStart(2, '0');
                    const ss = String(seconds).padStart(2, '0');

                    return `${hh}:${mm}:${ss}`;
                }
            }

            SearchableText {
                id: delegateText
                Layout.fillWidth: true
                wrapMode: Text.Wrap
                color: root.textColor

                font.family: root.textFont.family
                font.italic: root.textFont.italic
                font.underline: root.textFont.underline
                font.pointSize: root.textFont.pointSize
                font.weight: root.textFont.weight
                font.overline: root.textFont.overline
                font.strikeout: root.textFont.strikeout
                font.letterSpacing: root.textFont.letterSpacing
                font.wordSpacing: root.textFont.wordSpacing
                font.kerning: root.textFont.kerning
                font.preferShaping: root.textFont.preferShaping
                font.hintingPreference: root.textFont.hintingPreference
                font.styleName: root.textFont.styleName

                text: model.text.replace(root.regexFilter, "")
            }
        }

        HoverHandler {
            id: delegateHoverHandler
        }
    }

    MouseArea {
        id: rootMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton

        property int anchorIndex: -1
        property bool dragActive: false

        /**
         * Get the index of the item at the mouse position.
         * @param event The mouse event.
         * @return The index of the item if it exists, -1 if not.
         */
        function getIndexAt(event) {
            return root.indexAt(event.x, event.y + root.contentY);
        }

        onDoubleClicked: function(event) {
            let index = getIndexAt(event);
            if (index === - 1)
            {
                return;
            }
            let modelIndex = root.subtitleListModel.index(index, 0);
            let position = root.subtitleListModel.data(modelIndex, SubtitleListModel.StartRole);
            position += root.delay;
            position = Math.max(position, 0);
            root.seekRequested(position);
        }

        onPressed: function(event) {
            let index = rootMouseArea.getIndexAt(event);
            if (index === -1)
            {
                return;
            }

            rootMouseArea.anchorIndex = index;
            let modelIndex = root.subtitleListModel.index(index, 0);
            if (event.modifiers & Qt.ControlModifier)
            {
                root.selectionModel.select(modelIndex, ItemSelectionModel.Toggle);
                root.selectionModel.setCurrentIndex(modelIndex, ItemSelectionModel.NoUpdate);
            }
            else if (event.modifiers & Qt.ShiftModifier && root.selectionModel.currentIndex.valid)
            {
                root.selectionModel.clearSelection();

                let start = Math.min(root.selectionModel.currentIndex.row, index);
                let end = Math.max(root.selectionModel.currentIndex.row, index);

                for (let i = start; i <= end; ++i)
                {
                    root.selectionModel.select(root.subtitleListModel.index(i, 0), ItemSelectionModel.Select);
                }
            }
            else
            {
                root.selectionModel.select(modelIndex, ItemSelectionModel.ClearAndSelect);
                root.selectionModel.setCurrentIndex(modelIndex, ItemSelectionModel.NoUpdate);
                rootMouseArea.dragActive = true;
            }
        }

        onPositionChanged: function(event) {
            if (rootMouseArea.anchorIndex === -1 || !rootMouseArea.dragActive)
            {
                return;
            }

            let index = rootMouseArea.getIndexAt(event);
            if (index === -1)
            {
                return;
            }

            root.selectionModel.clearSelection();

            let start = Math.min(rootMouseArea.anchorIndex, index);
            let end = Math.max(rootMouseArea.anchorIndex, index);

            for (let i = start; i <= end; ++i)
            {
                root.selectionModel.select(root.subtitleListModel.index(i, 0), ItemSelectionModel.Select);
            }
        }

        onReleased: {
            rootMouseArea.anchorIndex = -1;
            rootMouseArea.dragActive = false;
        }
    }
}
