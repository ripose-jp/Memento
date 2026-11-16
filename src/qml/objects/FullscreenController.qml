import QtQml
import QtQuick.Window

QtObject {
    id: root

    required property Window window
    required property bool fullscreen

    /* Keep the restore target through intermediate window-state changes. */
    property bool wasMaximized: false
    property bool restoringMaximized: false
    property bool initialized: false

    /**
     * Applies a fullscreen request, remembering the state before entry.
     */
    function updateFullscreen() {
        if (!root.initialized)
        {
            return;
        }

        if (root.fullscreen)
        {
            /* A rapid re-entry must not save the temporary normal state. */
            if (!root.restoringMaximized)
            {
                root.wasMaximized = root.window.visibility === Window.Maximized;
            }
            root.restoringMaximized = false;
            root.window.showFullScreen();
        }
        else
        {
            root.restoringMaximized = root.wasMaximized;

            /* Qt can leave incorrect geometry/decorations when going directly
             * from fullscreen to maximized. Leave fullscreen first, then
             * maximize on a later event-loop turn. */
            root.window.showNormal();
            Qt.callLater(root.restoreMaximized);
        }
    }

    /**
     * Finishes restoring after Qt has left fullscreen.
     */
    function restoreMaximized() {
        if (!root.restoringMaximized || root.fullscreen)
        {
            return;
        }

        /* Some platforms deliver the fullscreen exit asynchronously. */
        if (root.window.visibility === Window.FullScreen)
        {
            return;
        }

        root.restoringMaximized = false;
        if (root.window.visibility === Window.Windowed)
        {
            root.window.showMaximized();
        }
    }

    /* Retry after asynchronous exits, without restoring hidden/minimized
     * windows or re-entering a window-state change handler. */
    property Connections windowConnections: Connections {
        target: root.window
        function onVisibilityChanged() {
            if (root.restoringMaximized)
            {
                if (root.window.visibility === Window.Hidden ||
                    root.window.visibility === Window.Minimized)
                {
                    root.restoringMaximized = false;
                }
                else
                {
                    Qt.callLater(root.restoreMaximized);
                }
            }
        }
    }

    onFullscreenChanged: root.updateFullscreen()
    Component.onCompleted: {
        root.initialized = true;
        if (root.fullscreen)
        {
            root.updateFullscreen();
        }
    }
}
