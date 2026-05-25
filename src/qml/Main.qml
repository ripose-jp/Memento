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

    /* Handle fullscreen on non-Windows platforms */
    Binding {
        root.visibility: player.state.fullscreen ? Window.FullScreen : Window.Windowed
        when: Features.platform !== Features.Windows
    }

    /* Handle borderless fullscreen on Windows */
    property int savedVisibility: 0
    property rect savedGeometry: Qt.rect(0, 0, 0, 0)
    Connections {
        target: player.state
        function onFullscreenChanged() {
            if (Features.platform !== Features.Windows)
            {
                return;
            }

            if (player.state.fullscreen)
            {
                root.savedVisibility = root.visibility;
                root.showNormal();
                root.savedGeometry = Qt.rect(root.x, root.y, root.width, root.height);

                /* It is important to set 1 dimension greater than the screen size.
                 * Windows has issue with OpenGL where fullscreen OpenGL whether
                 * windowed or true causes graphical glitches, hitches, etc.
                 * By adding just 1 pixel to a single dimension such that the window
                 * is greater than the screen size, this no longer happens. */
                root.flags = Qt.Window | Qt.FramelessWindowHint;
                root.x = 0;
                root.y = 0;
                root.width = root.screen.width;
                root.height = root.screen.height + 1;
                mainSplitView.anchors.bottomMargin = 1;
            }
            else
            {
                root.flags = Qt.Window;
                root.requestUpdate();

                Qt.callLater(function() {
                    mainSplitView.anchors.bottomMargin = 0;
                    root.x = root.savedGeometry.x;
                    root.y = root.savedGeometry.y;
                    root.width = root.savedGeometry.width;
                    root.height = root.savedGeometry.height;
                    root.visibility = root.savedVisibility;
                });
            }
        }
    }

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
        if (root.visibility !== Window.Windowed || player.state.fullscreen)
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

        /* Get the maximum width and height */
        const maxWidth = root.screen.desktopAvailableWidth;
        const maxHeight = root.screen.desktopAvailableHeight;

        /* Potentially scale if dimensions exceed the screen */
        const scale = Math.min(maxWidth / requestedWidth, maxHeight / requestedHeight, 1.0);
        const finalWidth = requestedWidth * scale;
        const finalHeight = requestedHeight * scale;

        /* Set Dimensions */
        root.width = finalWidth;
        root.height = finalHeight;

        /* Center - May not work on Wayland or macOS */
        root.x = (root.screen.width - finalWidth) / 2;
        root.y = (root.screen.height - finalHeight) / 2;
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

            onInitialized: player.controller.loadArgs(Qt.application.arguments)
            onShutdown: Qt.quit()
            onFileLoaded: function(w, h) {
                if (MementoSettings.behaviorAutoFit)
                {
                    root.updateWindowSize(w, h);
                }

                MementoSettings.recentFilesAdd(player.state.path);

                player.controller.play();
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
            visible: toolSplitView.hasVisibleChildren
            orientation: Qt.Vertical

            ManualSearchPage {
                id: inlineSearchPage
                SplitView.minimumHeight: 200
                SplitView.preferredHeight: 400
                visible: MementoSettings.windowSearch && !MementoSettings.interfaceSearchWindow
            }

            SubtitleListPage {
                id: inlineSubtitleLists
                SplitView.minimumHeight: 200
                visible: MementoSettings.windowSubtitleList && !MementoSettings.interfaceSubtitleListWindow
                player: player
            }
        }
    }

    Loader {
        active: MementoSettings.interfaceSearchWindow
        sourceComponent: Component {
            Window {
                id: searchPageWindow
                width: 500
                height: 500
                visible: MementoSettings.interfaceSearchWindow && MementoSettings.windowSearch
                title: qsTr("Search")
                onClosing: if (visible) MementoSettings.windowSearch = false

                ManualSearchPage {
                    anchors.fill: parent
                }
            }
        }
    }

    Loader {
        id: subtitleListWindowLoader

        /* Need to load the player into the SubtitleList */
        readonly property MpvPlayer mainPlayer: player

        active: MementoSettings.interfaceSubtitleListWindow
        sourceComponent: Component {
            Window {
                id: subtitleListWindow
                width: 500
                height: 720
                visible: MementoSettings.interfaceSubtitleListWindow && MementoSettings.windowSubtitleList
                title: qsTr("Subtitle List")
                onClosing: if (visible) MementoSettings.windowSubtitleList = false

                SubtitleListPage {
                    anchors.fill: parent
                    player: subtitleListWindowLoader.mainPlayer
                }
            }
        }
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
                    text: qsTr("<p>For searching to work, please install a dictionary.</p>" +
                               "<p>Dictionaries can be found <a href='https://yomitan.wiki/dictionaries/'>here</a>.</p>" +
                               "<p>To install a dictionary, go to %1.</p>").arg(Features.platform === Features.MacOS ?
                                                                                    qsTr("Memento → Preferences → Dictionary") :
                                                                                    qsTr("Settings → Options → Dictionary"))
                    onLinkActivated: (link) => Qt.openUrlExternally(link)
                }
            }
        }

        Component.onCompleted: active = DictionaryController.dictionaries.rowCount() === 0
        onLoaded: item.open()
    }
}
