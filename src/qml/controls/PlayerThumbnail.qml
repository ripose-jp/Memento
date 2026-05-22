import QtQuick
import QtQuick.Controls
import Ripose.Memento

Frame {
    id: root

    property int maxWidth: 16 * 13
    property int maxHeight: 9 * 13
    property string path: ""
    property real position: 0

    property bool active: false

    /**
     * Get if the given path is a remote URL.
     * @param path The path to the media.
     * @return true if the path is a remote file,
     * @return false if the path is a local file.
     */
    function isRemoteUrl(path) {
        return /^[a-zA-Z][a-zA-Z0-9+.-]*:\/\//.test(path)
            && !path.startsWith("file://")
    }

    onActiveChanged: {
        if (!root.active)
        {
            thumbnail.controller.stop();
        }
    }

    onPathChanged: {
        if (thumbnail.initialized && !root.isRemoteUrl(root.path))
        {
            root.active = thumbnail.controller.loadFile(root.path);
        }
        else
        {
            root.active = false;
        }
    }

    onPositionChanged: {
        if (root.visible && root.active)
        {
            thumbnail.controller.seek(root.position);
        }
    }

    onVisibleChanged: {
        if (root.visible && root.active)
        {
            thumbnail.controller.seek(root.position);
        }
    }

    MpvThumbnail {
        id: thumbnail

        property bool initialized: false

        implicitWidth: root.maxWidth
        implicitHeight: root.maxHeight

        onInitialized: {
            thumbnail.initialized = true;
            if (!root.isRemoteUrl(root.path))
            {
                root.active = thumbnail.controller.loadFile(root.path);
            }
        }

        onFileLoaded: function(width, height) {
            const scale = Math.min(root.maxWidth / width, root.maxHeight / height, 1.0);
            thumbnail.implicitWidth = width * scale;
            thumbnail.implicitHeight = height * scale;
        }
    }
}
