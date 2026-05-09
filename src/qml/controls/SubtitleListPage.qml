import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    required property MpvPlayer player

    property color backgroundColor: MementoSettings.interfaceSubtitleListBackgroundColor
    property color textColor: MementoSettings.interfaceSubtitleListTextColor
    property color hoverColor: MementoSettings.interfaceSubtitleListHoverColor
    property color selectedColor: MementoSettings.interfaceSubtitleListSelectedColor
    property color selectedHoverColor: MementoSettings.interfaceSubtitleListSelectedHoverColor
    property font primaryTextFont: MementoSettings.interfaceSubtitleListPrimaryFont
    property font secondaryTextFont: MementoSettings.interfaceSubtitleListSecondaryFont

    Action {
        enabled: root.visible
        shortcut: MementoSettings.keybinds.profile?.subtitleListFind
        onTriggered: footerPane.visible = true
    }

    Action {
        id: prevResultAction
        enabled: root.visible && footerPane.matchResults.length > 0
        shortcut: MementoSettings.keybinds.profile?.subtitleListFindPrevious
        onTriggered: {
            footerPane.incrementMatch(-1);
            footerPane.jumpToCurrent();
        }
    }

    Action {
        id: nextResultAction
        enabled: root.visible && footerPane.matchResults.length > 0
        shortcut: MementoSettings.keybinds.profile?.subtitleListFindNext
        onTriggered: {
            footerPane.incrementMatch(1);
            footerPane.jumpToCurrent();
        }
    }

    background: Rectangle {
        color: root.backgroundColor
    }

    header: TabBar {
        id: tabBar
        width: parent.width
        visible: root.player.state.secondarySid !== 0
        onVisibleChanged: {
            if (!visible)
            {
                currentIndex = 0;
            }
        }

        background: Rectangle {
            color: root.backgroundColor
        }

        SubtitleListTabButton {
            text: qsTr("Primary")
            textColor: root.textColor
            hoverColor: root.hoverColor
            selectedColor: root.selectedColor
        }

        SubtitleListTabButton {
            text: qsTr("Secondary")
            textColor: root.textColor
            hoverColor: root.hoverColor
            selectedColor: root.selectedColor
        }
    }

    StackLayout {
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        clip: true

        SubtitleList {
            id: primarySubtitleList
            subtitleListModel: SubtitleLists.primary
            delay: root.player.state.subtitle.delay
            textColor: root.textColor
            hoverColor: root.hoverColor
            selectedColor: root.selectedColor
            selectedHoverColor: root.selectedHoverColor
            textFont: root.primaryTextFont
            onSeekRequested: (pos) => root.player.controller.seek(pos)
        }

        SubtitleList {
            id: secondarySubtitleList
            subtitleListModel: SubtitleLists.secondary
            delay: root.player.state.secondarySubtitle.delay
            textColor: root.textColor
            hoverColor: root.hoverColor
            selectedColor: root.selectedColor
            selectedHoverColor: root.selectedHoverColor
            textFont: root.secondaryTextFont
            onSeekRequested: (pos) => root.player.controller.seek(pos)
        }
    }

    footer: Pane {
        id: footerPane

        property var matchResults: []
        property int currentMatch: 0

        visible: false
        onVisibleChanged: {
            if (visible)
            {
                footerPane.executeSearch();
            }
        }

        /**
         * Clear the search results.
         */
        function clearResults() {
            footerPane.matchResults = [];
            footerPane.currentMatch = 0;
        }

        /**
         * Executes a search with the current settings.
         */
        function executeSearch() {
            if (!footerPane.visible)
            {
                footerPane.clearResults();
                return;
            }

            const model = tabBar.currentIndex === 0 ?
                            SubtitleLists.primary : SubtitleLists.secondary;
            if (!model)
            {
                footerPane.clearResults();
                return;
            }
            footerPane.matchResults = model.find(searchTextField.text, MementoSettings.subtitleListIgnoreWhitespace);
            footerPane.currentMatch = 0;
        }

        /**
         * Jump to the current match.
         */
        function jumpToCurrent() {
            if (footerPane.currentMatch < 0 ||
                    footerPane.currentMatch >= footerPane.matchResults.length)
            {
                return;
            }

            const subtitleList = tabBar.currentIndex === 0 ? primarySubtitleList : secondarySubtitleList;
            if (!subtitleList.subtitleListModel)
            {
                return;
            }
            let index = footerPane.matchResults[footerPane.currentMatch];
            let modelIndex = subtitleList.subtitleListModel.index(index, 0);
            subtitleList.subtitleListModel.selectionModel.select(modelIndex, ItemSelectionModel.ClearAndSelect);
            subtitleList.subtitleListModel.selectionModel.setCurrentIndex(modelIndex, ItemSelectionModel.NoUpdate);
            subtitleList.positionViewAtIndex(index, ListView.Center);

            if (MementoSettings.subtitleListAutoSeek)
            {
                let position = subtitleList.subtitleListModel.data(modelIndex, SubtitleListModel.StartRole);
                root.player.controller.seek(position + subtitleList.delay);
            }
        }

        /**
         * Increment that match index.
         * @param value The amount to increment by. Must be |value| < matchResults.length.
         */
        function incrementMatch(value) {
            let index = footerPane.currentMatch;
            let length = footerPane.matchResults.length;
            index += value;
            index = ((index % length) + length) % length;
            footerPane.currentMatch = index;
        }

        Connections {
            target: tabBar
            function onCurrentIndexChanged() {
                footerPane.executeSearch();
            }
        }

        Connections {
            target: SubtitleLists
            function onPrimaryChanged() {
                if (tabBar.currentIndex === 0)
                {
                    footerPane.executeSearch();
                }
            }
            function onSecondaryChanged() {
                if (tabBar.currentIndex === 1)
                {
                    footerPane.executeSearch();
                }
            }
        }

        Connections {
            target: tabBar.currentIndex === 0 ? SubtitleLists.primary : SubtitleLists.secondary
            function onDataChanged() {
                footerPane.executeSearch();
            }
            function onRowsInserted() {
                footerPane.executeSearch();
            }
            function onModelReset() {
                footerPane.executeSearch();
            }
        }

        ColumnLayout {
            anchors.fill: parent

            RowLayout {
                CheckBox {
                    text: qsTr("Ignore Whitespace")
                    checked: MementoSettings.subtitleListIgnoreWhitespace
                    onClicked: {
                        MementoSettings.subtitleListIgnoreWhitespace = checked;
                        footerPane.executeSearch();
                        footerPane.jumpToCurrent();
                    }
                }

                CheckBox {
                    text: qsTr("Auto Seek")
                    checked: MementoSettings.subtitleListAutoSeek
                    onClicked: {
                        MementoSettings.subtitleListAutoSeek = checked;
                        footerPane.jumpToCurrent();
                    }
                }
            }

            RowLayout {
                TextField {
                    id: searchTextField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Search")
                    onTextChanged: {
                        footerPane.executeSearch();
                        footerPane.jumpToCurrent();
                    }
                    onAccepted: {
                        footerPane.executeSearch();
                        footerPane.jumpToCurrent();
                    }
                }

                ToolButton {
                    id: prevResultButton
                    focusPolicy: Qt.NoFocus
                    icon.name: MementoSettings.interfaceSystemIcons ? "go-up" : null
                    icon.source: Utils.toImageProvider("arrow-up", MementoPalette.text)
                    action: prevResultAction
                }

                ToolButton {
                    id: nextResultButton
                    focusPolicy: Qt.NoFocus
                    icon.name: MementoSettings.interfaceSystemIcons ? "go-down" : null
                    icon.source: Utils.toImageProvider("arrow-down", MementoPalette.text)
                    action: nextResultAction
                }

                Label {
                    text: footerPane.matchResults.length === 0 ?
                              qsTr("No Matches") :
                              qsTr("%1 of %2")
                                .arg(footerPane.currentMatch + 1)
                                .arg(footerPane.matchResults.length)
                }

                ToolButton {
                    id: buttonClose
                    focusPolicy: Qt.NoFocus
                    icon.name: MementoSettings.interfaceSystemIcons ? "window-close" : null
                    icon.source: Utils.toImageProvider("close", MementoPalette.text)
                    onClicked: footerPane.visible = false
                }
            }
        }
    }
}
