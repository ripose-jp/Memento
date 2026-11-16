import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Window
import Ripose.Memento

ApplicationWindow {
    id: root

    width: 1280
    height: 720
    visibility: Window.Windowed
    color: "black"
    title: player.state.title ? qsTr("%1 - Memento").arg(player.state.title) : qsTr("Memento")

    /* This is needed to return focus to the player after a dialog */
    onActiveFocusItemChanged: {
        let item = contentItem;
        while (item)
        {
            if (activeFocusItem === item)
            {
                player.forceActiveFocus();
                return;
            }
            item = item.parent;
        }
    }

    /**
     * Updates the screen to the requested width and height or a value close to it.
     * @param requestedWidth The requested width in pixels.
     * @param requestedHeight The requested height in pixels.
     */
    function updateWindowSize(requestedWidth, requestedHeight) {
        /* Don't trigger in fullscreen or maximized */
        if (root.visibility !== Window.Windowed)
        {
            return;
        }
        else if (fullscreenControllerLoader.item &&
                 fullscreenControllerLoader.item.restoringMaximized)
        {
            return;
        }

        /* Take account for other items */
        if (toolSplitView.visible)
        {
            requestedWidth += toolSplitView.width;
        }

        /* Apply sclaing factor */
        const scaleFactor = MementoSettings.behaviorAutoFitPercent / 100.0;
        requestedWidth *= scaleFactor;
        requestedHeight *= scaleFactor;

        /* Get the dimensions of the current screen */
        const maxWidth = Screen.width;
        const maxHeight = Screen.height;

        /* Potentially scale if dimensions exceed the screen */
        const scale = Math.min(maxWidth / requestedWidth, maxHeight / requestedHeight, 1.0);
        const finalWidth = requestedWidth * scale;
        const finalHeight = requestedHeight * scale;

        /* Set Dimensions */
        root.width = finalWidth;
        root.height = finalHeight;

        /* Center in the current screen's virtual-desktop geometry */
        root.x = Screen.virtualX + (maxWidth - finalWidth) / 2;
        root.y = Screen.virtualY + (maxHeight - finalHeight) / 2;
    }

    Binding {
        when: Features.isMacos
        root.visibility: player.state.fullscreen ? Window.FullScreen : Window.Windowed
    }

    Loader {
        active: Features.isMacos
        sourceComponent: Component {
            Connections {
                target: root
                function onVisibilityChanged() {
                    switch (root.visibility)
                    {
                    case Window.Windowed:
                        player.controller.setFullscreen(false);
                        break;
                    case Window.Maximized:
                    case Window.FullScreen:
                        player.controller.setFullscreen(true);
                        break;
                    }
                }
            }
        }
    }

    Loader {
        id: fullscreenControllerLoader
        active: !Features.isMacos
        sourceComponent: Component {
            FullscreenController {
                id: fullscreenController
                window: root
                fullscreen: player.state.fullscreen
            }
        }
    }

    SplitView {
        id: mainSplitView
        anchors.fill: parent

        handle: Rectangle {
            id: handleDelegate
            implicitWidth: 1
            color: {
                if (SplitHandle.pressed)
                {
                    return "#474747";
                }
                else if (SplitHandle.hovered)
                {
                    return "#333333";
                }
                return "#000000";
            }

            containmentMask: Item {
                x: (handleDelegate.width - width) / 2
                width: 10
                height: handleDelegate.height
            }

            MouseArea {
                x: handleDelegate.containmentMask.x
                width: handleDelegate.containmentMask.width
                height: handleDelegate.height
                cursorShape: Qt.SplitHCursor
                acceptedButtons: Qt.NoButton
                propagateComposedEvents: true
            }
        }

        Player {
            id: player

            SplitView.fillWidth: true
            SplitView.fillHeight: true
            SplitView.minimumWidth: 400

            objectName: "mainPlayer"
            focusPolicy: Qt.WheelFocus
            focus: true

            onInitialized: {
                const files = FileOpenHandler.takePendingFiles();
                if (files.length > 0)
                {
                    player.controller.loadFile(files);
                }
                else
                {
                    player.controller.loadArgs(Qt.application.arguments);
                }
            }
            onShutdown: Qt.quit()
            onFileLoaded: function(w, h) {
                if (MementoSettings.behaviorAutoFit)
                {
                    root.updateWindowSize(w, h);
                }

                if (player.state.path.length > 0)
                {
                    MementoSettings.recentFilesAdd(player.state.path);
                }

                player.controller.play();
            }
            onAuxiliarySearchRequested: function(text) {
                MementoSettings.windowSearch = true;
                searchPage.setQuery(text);
            }

            Connections {
                target: FileOpenHandler
                function onFilesOpened(files) {
                    player.controller.loadFile(files);
                }
            }

            DropArea {
                anchors.fill: parent
                onDropped: function(event) {
                    player.controller.loadFile(event.text);
                }
            }
        }

        SplitView {
            id: toolSplitView

            readonly property bool hasVisibleChildren:
                (MementoSettings.windowSearch && !MementoSettings.interfaceSearchWindow) ||
                (MementoSettings.windowSubtitleList && !MementoSettings.interfaceSubtitleListWindow)

            property real savedWidth: 400
            onWidthChanged: {
                /* On start, width will be minimumWidth, but we want preferredWidth to be used.
                 * Just ignore this if width is minimumWidth since the value just before will
                 * be really close. */
                if (width > 0 && width !== SplitView.minimumWidth)
                {
                    toolSplitView.savedWidth = width;
                }
            }

            SplitView.preferredWidth: toolSplitView.hasVisibleChildren ? toolSplitView.savedWidth : 0
            SplitView.minimumWidth: toolSplitView.hasVisibleChildren ? 300 : 0
            parent: toolSplitView.hasVisibleChildren ? mainSplitView : null
            visible: toolSplitView.hasVisibleChildren
            orientation: Qt.Vertical

            Item {
                id: inlineSearchHost
                SplitView.minimumHeight: 200
                SplitView.preferredHeight: 400
                visible: MementoSettings.windowSearch && !MementoSettings.interfaceSearchWindow
            }

            Item {
                id: inlineSubtitleListHost
                SplitView.minimumHeight: 200
                visible: MementoSettings.windowSubtitleList && !MementoSettings.interfaceSubtitleListWindow
            }
        }
    }

    Loader {
        id: searchWindowLoader

        active: MementoSettings.interfaceSearchWindow
        sourceComponent: Component {
            Window {
                id: searchPageWindow

                property alias searchHost: searchWindowHost

                width: 500
                height: 500
                visible: MementoSettings.interfaceSearchWindow && MementoSettings.windowSearch
                title: qsTr("Search")
                onClosing: if (visible) MementoSettings.windowSearch = false

                Item {
                    id: searchWindowHost
                    anchors.fill: parent
                }
            }
        }
    }

    Loader {
        id: subtitleListWindowLoader

        active: MementoSettings.interfaceSubtitleListWindow
        sourceComponent: Component {
            Window {
                id: subtitleListWindow

                property alias subtitleListHost: subtitleListWindowHost

                width: 500
                height: 720
                visible: MementoSettings.interfaceSubtitleListWindow && MementoSettings.windowSubtitleList
                title: qsTr("Subtitle List")
                onClosing: if (visible) MementoSettings.windowSubtitleList = false

                Item {
                    id: subtitleListWindowHost
                    anchors.fill: parent
                }
            }
        }
    }

    ManualSearchPage {
        id: searchPage
        parent: MementoSettings.interfaceSearchWindow ?
                    searchWindowLoader.item?.searchHost ?? null :
                    inlineSearchHost
        anchors.fill: parent
        visible: MementoSettings.windowSearch
    }

    SubtitleListPage {
        id: subtitleListPage
        parent: MementoSettings.interfaceSubtitleListWindow ?
                    subtitleListWindowLoader.item?.subtitleListHost ?? null :
                    inlineSubtitleListHost
        anchors.fill: parent
        visible: MementoSettings.windowSubtitleList
        player: player
    }

    Loader {
        id: dictionaryInstallLoader
        active: false
        sourceComponent: Component {
            Dialog {
                id: dictionaryInstallDialog
                parent: Overlay.overlay
                anchors.centerIn: parent
                modal: true
                standardButtons: Dialog.Ok
                title: qsTr("No Dictionaries Installed")

                onClosed: dictionaryInstallLoader.active = false

                Label {
                    textFormat: Text.RichText
                    text: qsTr(
                        "<p>For searching to work, please install a dictionary.</p>" +
                        "<p>Dictionaries can be found <a href='https://yomitan.wiki/dictionaries/'>here</a>.</p>" +
                        "<p>To install a dictionary, go to %1.</p>"
                    ).arg(Features.isMacos ?
                        qsTr("Memento → Preferences → Dictionary") :
                        qsTr("Settings → Options → Dictionary")
                    )
                    onLinkActivated: (link) => Qt.openUrlExternally(link)
                }
            }
        }

        Component.onCompleted: active = DictionaryController.dictionaries.rowCount() === 0
        onLoaded: item.open()
    }

    Loader {
        id: autoUpdateOptInLoader
        active: !MementoSettings.internalAutoUpdateOptInShown
        sourceComponent: Component {
            Dialog {
                id: autoUpdateOptInDialog
                parent: Overlay.overlay
                anchors.centerIn: parent
                modal: true
                standardButtons: Dialog.Yes | Dialog.No
                title: qsTr("Auto Update")

                Label {
                    text: qsTr(
                        "Do you want to check for updates on launch?\n\n" +
                        "This can be disabled at any time by going to %1.\n" +
                        "This will check GitHub's API when Memento is launched."
                    ).arg(Features.isMacos?
                              qsTr("Memento → Preferences → Application") :
                              qsTr("Settings → Options → Application")
                    )
                }

                onAccepted: MementoSettings.applicationAutoUpdateCheck = true
                onRejected: MementoSettings.applicationAutoUpdateCheck = false
                onClosed: MementoSettings.internalAutoUpdateOptInShown = true
            }
        }

        onLoaded: item.open()
    }
}
