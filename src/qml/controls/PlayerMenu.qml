import QtCore
import QtQml.Models
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Ripose.Memento

MenuBar {
    id: root

    required property MpvPlayer player

    /* true if subtitles should be shown, false if they should be hidden */
    property alias showSubtitles: actionShowSubtitles.checked

    /* true if any children are under the cursor, false otherwise */
    readonly property bool anyHovered: {
        if (hovered)
        {
            return true;
        }
        for (let i = 0; i < root.contentChildren.length; ++i)
        {
            if (root.contentChildren[i].highlighted)
            {
                return true;
            }
        }
        return false;
    }

    /* The minimum width of each top level menu */
    property int minimumMenuWidth: 225

    signal ocrModeRequested()

    /**
     * Turns a track into a display string.
     * @param track The track to turn into a display string.
     * @return The display string of the track.
     */
    function makeTrackName(track) {
        let name = qsTr("Track %1").arg(track.id);
        if (track.language)
        {
            name += ` [${track.language}]`;
        }
        if (track.title)
        {
            name += ` - ${track.title}`;
        }
        return name;
    }

    Component.onCompleted: {
        if (Features.isWindows)
        {
            root.background = windowsBackground;
        }
    }

    resources: [
        Page {
            id: windowsBackground
        }
    ]

    Clipboard {
        id: clipboard
    }

    Menu {
        title: qsTr("&Media")
        width: Math.max(contentWidth + leftPadding + rightPadding, root.minimumMenuWidth)

        Action {
            text: qsTr("&Open File...")
            shortcut: MementoSettings.keybinds.profile?.openFile
            onShortcutChanged: {
                /* Hack to force a refresh */
                if (Features.isMacos)
                {
                    checkable = !checkable;
                    checkable = !checkable;
                }
            }
            onTriggered: loadFileDialog.open()
        }

        Action {
            text: qsTr("&Open URL...")
            shortcut: MementoSettings.keybinds.profile?.openUrl
            onShortcutChanged: {
                /* Hack to force a refresh */
                if (Features.isMacos)
                {
                    checkable = !checkable;
                    checkable = !checkable;
                }
            }
            onTriggered: loadUrlDialog.open()
        }

        Menu {
            id: recentMenu
            title: qsTr("&Recent")
            enabled: MementoSettings.recentFiles.length > 0

            Instantiator {
                model: MementoSettings.recentFiles
                delegate: Action {
                    text: modelData
                    onTriggered: root.player.controller.loadFile(modelData)
                }

                onObjectAdded: function(index, object) {
                    recentMenu.insertAction(index, object);

                    /* This is a hack to force a refresh after adding the item.
                     * For some reason the items are all added with blank text,
                     * until something causes a visual update. */
                    if (Features.isMacos)
                    {
                        object.checkable = !object.checkable;
                        object.checkable = !object.checkable;
                    }
                }
                onObjectRemoved: function(index, object) {
                    recentMenu.removeAction(object);
                }
            }

            MenuSeparator {}

            Action {
                text: qsTr("&Clear Recents")
                onTriggered: MementoSettings.recentFilesClear()
            }
        }
    }

    Menu {
        id: audioMenu
        title: qsTr("&Audio")
        width: Math.max(contentWidth + leftPadding + rightPadding, root.minimumMenuWidth)

        ActionGroup {
            id: audioTrackGroup
            exclusive: true
        }

        Action {
            checkable: true
            checked: root.player.state.aid === 0
            ActionGroup.group: audioTrackGroup
            text: qsTr("None")
            onTriggered: root.player.controller.setAid(0)
        }

        Instantiator {
            model: root.player.state.audioTracks
            delegate: Action {
                checkable: true
                checked: root.player.state.aid === modelData.id
                ActionGroup.group: audioTrackGroup
                text: root.makeTrackName(modelData)
                onTriggered: root.player.controller.setAid(modelData.id)
            }

            onObjectAdded: function(index, object) {
                audioMenu.insertAction(index + 1, object);

                /* This is a hack to force a refresh after adding the item.
                 * For some reason the items are all added with blank text,
                 * until something causes a visual update. */
                if (Features.isMacos)
                {
                    object.checkable = !object.checkable;
                    object.checkable = !object.checkable;
                }
            }
            onObjectRemoved: function(index, object) {
                audioMenu.removeAction(object);
            }
        }
    }

    Menu {
        id: subtitleMenu
        title: qsTr("&Subtitle")
        width: Math.max(contentWidth + leftPadding + rightPadding, root.minimumMenuWidth)

        Action {
            checkable: true
            checked: MementoSettings.behaviorSubtitlePause
            text: qsTr("&Auto Pause")
            shortcut: MementoSettings.keybinds.profile?.subtitleAutoPause
            onShortcutChanged: {
                /* Hack to force a refresh */
                if (Features.isMacos)
                {
                    checkable = !checkable;
                    checkable = !checkable;
                }
            }
            onTriggered: MementoSettings.behaviorSubtitlePause = checked
        }

        Menu {
            title: qsTr("&Size and Position")

            Action {
                enabled: MementoSettings.interfaceSubtitleScale < 1.0
                text: qsTr("&Increase Size")
                shortcut: MementoSettings.keybinds.profile?.subtitleIncreaseSize
                onShortcutChanged: {
                    /* Hack to force a refresh */
                    if (Features.isMacos)
                    {
                        checkable = !checkable;
                        checkable = !checkable;
                    }
                }
                onTriggered: MementoSettings.interfaceSubtitleScale =
                             Math.min(MementoSettings.interfaceSubtitleScale + 0.005, 1.0)
            }

            Action {
                enabled: MementoSettings.interfaceSubtitleScale > 0.001
                text: qsTr("&Decrease Size")
                shortcut: MementoSettings.keybinds.profile?.subtitleDecreaseSize
                onShortcutChanged: {
                    /* Hack to force a refresh */
                    if (Features.isMacos)
                    {
                        checkable = !checkable;
                        checkable = !checkable;
                    }
                }
                onTriggered: MementoSettings.interfaceSubtitleScale =
                             Math.max(MementoSettings.interfaceSubtitleScale - 0.005, 0.001)
            }

            Action {
                enabled: MementoSettings.interfaceSubtitleOffset < 1.0
                text: qsTr("&Move Up")
                shortcut: MementoSettings.keybinds.profile?.subtitleMoveUp
                onShortcutChanged: {
                    /* Hack to force a refresh */
                    if (Features.isMacos)
                    {
                        checkable = !checkable;
                        checkable = !checkable;
                    }
                }
                onTriggered: MementoSettings.interfaceSubtitleOffset =
                             Math.min(MementoSettings.interfaceSubtitleOffset + 0.005, 1.0)
            }

            Action {
                enabled: MementoSettings.interfaceSubtitleOffset > 0
                text: qsTr("&Move Down")
                shortcut: MementoSettings.keybinds.profile?.subtitleMoveDown
                onShortcutChanged: {
                    /* Hack to force a refresh */
                    if (Features.isMacos)
                    {
                        checkable = !checkable;
                        checkable = !checkable;
                    }
                }
                onTriggered: MementoSettings.interfaceSubtitleOffset =
                             Math.max(MementoSettings.interfaceSubtitleOffset - 0.005, 0.0)
            }
        }

        Action {
            id: actionShowSubtitles
            text: qsTr("&Show Subtitles")
            checkable: true
            checked: true
            shortcut: MementoSettings.keybinds.profile?.subtitleShow
            onShortcutChanged: {
                /* Hack to force a refresh */
                if (Features.isMacos)
                {
                    checkable = !checkable;
                    checkable = !checkable;
                }
            }
        }

        MenuSeparator {}

        Action {
            text: qsTr("&Add Subtitle...")
            onTriggered: loadSubtitleDialog.open()
        }

        Menu {
            id: secondarySubtitleMenu
            title: qsTr("&Second Track")

            ActionGroup {
                id: secondarySubtitleTrackGroup
                exclusive: true
            }

            Action {
                checkable: true
                checked: root.player.state.secondarySid === 0
                ActionGroup.group: secondarySubtitleTrackGroup
                text: qsTr("None")
                onTriggered: root.player.controller.setSecondarySid(0)
            }

            Instantiator {
                model: root.player.state.subtitleTracks
                delegate: Action {
                    checkable: true
                    checked: root.player.state.secondarySid === modelData.id
                    enabled: root.player.state.sid !== modelData.id
                    ActionGroup.group: secondarySubtitleTrackGroup
                    text: root.makeTrackName(modelData)
                    onTriggered: root.player.controller.setSecondarySid(modelData.id)
                }

                onObjectAdded: function(index, object) {
                    secondarySubtitleMenu.insertAction(index + 1, object);

                    /* This is a hack to force a refresh after adding the item.
                     * For some reason the items are all added with blank text,
                     * until something causes a visual update. */
                    if (Features.isMacos)
                    {
                        object.checkable = !object.checkable;
                        object.checkable = !object.checkable;
                    }
                }
                onObjectRemoved: function(index, object) {
                    secondarySubtitleMenu.removeAction(object);
                }
            }
        }

        MenuSeparator {}

        ActionGroup {
            id: subtitleTrackGroup
            exclusive: true
        }

        Action {
            id: subtitleNoneAction

            readonly property int index: {
                for (let i = 0; i < subtitleMenu.count; ++i)
                {
                    if (subtitleMenu.itemAt(i).action === subtitleNoneAction)
                    {
                        return i;
                    }
                }
                return -1;
            }

            checkable: true
            checked: root.player.state.sid === 0
            ActionGroup.group: subtitleTrackGroup
            text: qsTr("None")
            onTriggered: root.player.controller.setSid(0)
        }

        Instantiator {
            model: root.player.state.subtitleTracks
            delegate: Action {
                checkable: true
                checked: root.player.state.sid === modelData.id
                enabled: root.player.state.secondarySid !== modelData.id
                ActionGroup.group: subtitleTrackGroup
                text: root.makeTrackName(modelData)
                onTriggered: root.player.controller.setSid(modelData.id)
            }

            onObjectAdded: function(index, object) {
                subtitleMenu.insertAction(subtitleNoneAction.index + index + 1, object);

                /* This is a hack to force a refresh after adding the item.
                 * For some reason the items are all added with blank text,
                 * until something causes a visual update. */
                if (Features.isMacos)
                {
                    object.checkable = !object.checkable;
                    object.checkable = !object.checkable;
                }
            }
            onObjectRemoved: function(index, object) {
                subtitleMenu.removeAction(object);
            }
        }
    }

    Menu {
        id: toolsMenu
        title: qsTr("&Tools")
        width: Math.max(contentWidth + leftPadding + rightPadding, root.minimumMenuWidth)

        Action {
            text: qsTr("&Show Search")
            checkable: true
            checked: MementoSettings.windowSearch
            shortcut: MementoSettings.keybinds.profile?.showSearch
            onShortcutChanged: {
                /* Hack to force a refresh */
                if (Features.isMacos)
                {
                    checkable = !checkable;
                    checkable = !checkable;
                }
            }
            onTriggered: MementoSettings.windowSearch = checked
        }

        Action {
            text: qsTr("&Show Subtitle List")
            checkable: true
            checked: MementoSettings.windowSubtitleList
            shortcut: MementoSettings.keybinds.profile?.showSubtitleList
            onShortcutChanged: {
                /* Hack to force a refresh */
                if (Features.isMacos)
                {
                    checkable = !checkable;
                    checkable = !checkable;
                }
            }
            onTriggered: MementoSettings.windowSubtitleList = checked
        }

        Instantiator {
            /* Hide the action if OCR is disabled */
            model: Features.ocr && MementoSettings.ocrEnabled ? 1 : 0

            delegate: Action {
                text: qsTr("&Start OCR")
                shortcut: MementoSettings.keybinds.profile?.startOcr
                onShortcutChanged: {
                    /* Hack to force a refresh */
                    if (Features.isMacos)
                    {
                        checkable = !checkable;
                        checkable = !checkable;
                    }
                }
                onTriggered: root.ocrModeRequested()
            }

            onObjectAdded: function(index, object) {
                toolsMenu.insertAction(toolsMenu.count, object);

                /* Hack to force a refresh */
                if (Features.isMacos)
                {
                    object.checkable = !object.checkable;
                    object.checkable = !object.checkable;
                }
            }
            onObjectRemoved: function(index, object) {
                toolsMenu.removeAction(object);
            }
        }
    }

    Menu {
        id: settingsMenu
        title: qsTr("&Settings")
        width: Math.max(contentWidth + leftPadding + rightPadding, root.minimumMenuWidth)

        Menu {
            id: ankiProfileMenu
            title: qsTr("&Anki Profile")
            enabled: AnkiConfig.enabled

            ActionGroup {
                id: ankiProfileGroup
                exclusive: true
            }

            Instantiator {
                model: AnkiConfig.profiles
                delegate: Action {
                    checkable: true
                    checked: AnkiConfig.profile === modelData
                    ActionGroup.group: ankiProfileGroup
                    text: modelData.name
                    onTriggered: AnkiConfig.setProfile(modelData.name)
                }

                onObjectAdded: function(index, object) {
                    ankiProfileMenu.insertAction(index, object);

                    /* This is a hack to force a refresh after adding the item.
                     * For some reason the items are all added with blank text,
                     * until something causes a visual update. */
                    if (Features.isMacos)
                    {
                        object.checkable = !object.checkable;
                        object.checkable = !object.checkable;
                    }
                }
                onObjectRemoved: function(index, object) {
                    ankiProfileMenu.removeAction(object);
                }
            }
        }

        Action {
            text: qsTr("&Options")
            shortcut: StandardKey.Preferences
            onTriggered: optionsWindow.show()
        }

        Action {
            text: qsTr("&Open Config")
            onTriggered: Features.isWindows ?
                             Qt.openUrlExternally(`file:///${MementoPaths.config}`) :
                             Qt.openUrlExternally(`file://${MementoPaths.config}`)
        }

        Action {
            text: qsTr("&Check for Updates")
            onTriggered: updateDialog.check(false)
        }

        Action {
            text: qsTr("&About Memento")
            onTriggered: aboutWindow.show()
        }
    }

    FileDialog {
        id: loadFileDialog
        currentFolder: Utils.getFileOpenDirectory(MementoSettings.behaviorFileOpenDirectory)
        fileMode: FileDialog.OpenFiles
        nameFilters: [
            qsTr("Video Files (%1)").arg("*.webm *.mkv *.vob *.ogv *.ogg *.avi *.mov *.qt *.mp4 *.m4p *.m4v *.mpg *.mp2 *.mpeg *.mpe *.mpv)"),
            qsTr("All Files (*.*)")
        ]
        onAccepted: root.player.controller.loadFile(selectedFiles)
    }

    Dialog {
        id: loadUrlDialog
        parent: Overlay.overlay
        anchors.centerIn: parent
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel
        title: qsTr("Open Stream")
        onOpened: {
            urlTextField.text = clipboard.text();
            urlTextField.selectAll();
        }
        onClosed: urlTextField.text = ""
        onAccepted: root.player.controller.loadFile(urlTextField.text)

        TextField {
            id: urlTextField
            implicitWidth: 400
            focus: true
            placeholderText: qsTr("Enter URL")
            onAccepted: loadUrlDialog.accept()
        }
    }

    FileDialog {
        id: loadSubtitleDialog
        currentFolder: Utils.getFileOpenDirectory(MementoSettings.behaviorFileOpenDirectory)
        fileMode: FileDialog.OpenFile
        nameFilters: [
            qsTr("Subtitle Files (%1)").arg("*.ass *.idx *.lrc *.mks *.pgs *.rt *.scc *.smi *.srt *.ssa *.sub *.sup *.utf-8 *.utf *.utf8 *.vtt)"),
            qsTr("All Files (*.*)")
        ]
        title: qsTr("Select Subtitle File")
        onAccepted: root.player.controller.loadSubtitle(selectedFile)
    }

    UpdateDialog {
        id: updateDialog

        Component.onCompleted: {
            if (MementoSettings.applicationAutoUpdateCheck)
            {
                updateDialog.check(true);
            }
        }
    }

    OptionsWindow {
        id: optionsWindow
    }

    AboutWindow {
        id: aboutWindow
    }
}
