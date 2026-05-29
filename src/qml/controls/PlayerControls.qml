import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Rectangle {
    id: root

    required property MpvPlayer player

    readonly property real position: durationSlider.position

    height: 50
    color: MementoPalette.window

    /* Make sure mouse events aren't sent to mpv */
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.AllButtons
    }

    RowLayout {
        id: layout

        readonly property int defaultMargin: 10

        anchors.fill: parent
        anchors.leftMargin: layout.defaultMargin
        anchors.rightMargin: layout.defaultMargin

        ToolButton {
            id: playButton
            focusPolicy: Qt.NoFocus
            visible: root.player.state.pause
            icon.name: MementoSettings.interfaceSystemIcons ? "media-playback-start" : null
            icon.source: Utils.toImageProvider("play", MementoPalette.text)
            onClicked: root.player.controller.play()
        }

        ToolButton {
            id: pauseButton
            focusPolicy: Qt.NoFocus
            visible: !root.player.state.pause
            icon.name: MementoSettings.interfaceSystemIcons ? "media-playback-pause" : null
            icon.source: Utils.toImageProvider("pause", MementoPalette.text)
            onClicked: root.player.controller.pause()
        }

        ToolButton {
            id: skipPrevButton
            focusPolicy: Qt.NoFocus
            icon.name: MementoSettings.interfaceSystemIcons ? "media-skip-backward" : null
            icon.source: Utils.toImageProvider("skip-previous", MementoPalette.text)
            onClicked: root.player.controller.playlistPrev()
        }

        ToolButton {
            id: seekPrevButton
            focusPolicy: Qt.NoFocus
            icon.name: MementoSettings.interfaceSystemIcons ? "media-seek-backward" : null
            icon.source: Utils.toImageProvider("fast-rewind", MementoPalette.text)
            onClicked: root.player.controller.subtitleSeek(-1, true)
        }

        ToolButton {
            id: seekNextButton
            focusPolicy: Qt.NoFocus
            icon.name: MementoSettings.interfaceSystemIcons ? "media-seek-forward" : null
            icon.source: Utils.toImageProvider("fast-forward", MementoPalette.text)
            onClicked: root.player.controller.subtitleSeek(1, true)
        }

        ToolButton {
            id: skipNextButton
            focusPolicy: Qt.NoFocus
            icon.name: MementoSettings.interfaceSystemIcons ? "media-skip-forward" : null
            icon.source: Utils.toImageProvider("skip-next", MementoPalette.text)
            onClicked: root.player.controller.playlistNext()
        }

        Label {
            id: positionLabel
            Layout.preferredWidth: durationLabel.width
            Layout.leftMargin: layout.defaultMargin
            Layout.rightMargin: layout.defaultMargin
            focusPolicy: Qt.NoFocus
            horizontalAlignment: Text.AlignHCenter
            text: Utils.toTimeString(durationSlider.value)
        }

        PlayerSlider {
            id: durationSlider
            Layout.fillWidth: true
            focusPolicy: Qt.NoFocus
            enabled: from !== to
            from: 0
            to: root.player.state.duration
            chapters: root.player.state.chapters
            onMoved: root.player.controller.seek(value)

            Timer {
                id: durationUpdateTimer
                interval: 250
                running: true
                repeat: true
                onTriggered: durationSlider.value = root.player.state.timePosition
            }
        }

        Label {
            id: durationLabel
            Layout.leftMargin: layout.defaultMargin
            Layout.rightMargin: layout.defaultMargin
            focusPolicy: Qt.NoFocus
            horizontalAlignment: Text.AlignHCenter
            text: Utils.toTimeString(root.player.state.duration);
        }

        Slider {
            id: volumeSlider
            Layout.preferredWidth: 150
            focusPolicy: Qt.NoFocus
            from: 0
            to: root.player.state.maxVolume
            value: root.player.state.volume
            onMoved: root.player.controller.setVolume(value)
        }

        Label {
            id: volumeLabel
            Layout.leftMargin: layout.defaultMargin
            Layout.rightMargin: layout.defaultMargin
            Layout.preferredWidth: 30
            focusPolicy: Qt.NoFocus
            horizontalAlignment: Text.AlignHCenter
            text: `${root.player.state.volume}%`
        }

        ToolButton {
            id: subtitleListButton
            focusPolicy: Qt.NoFocus
            icon.source: Utils.toImageProvider("list", MementoPalette.text)
            onClicked: MementoSettings.windowSubtitleList = !MementoSettings.windowSubtitleList
        }

        ToolButton {
            id: ocrButton
            focusPolicy: Qt.NoFocus
            visible: Features.ocr && MementoSettings.ocrEnabled
            icon.source: Utils.toImageProvider("eye", MementoPalette.text)
            onClicked: root.player.startOcrMode()
        }

        ToolButton {
            id: fullscreenButton
            focusPolicy: Qt.NoFocus
            visible: !root.player.state.fullscreen
            icon.name: MementoSettings.interfaceSystemIcons ? "view-fullscreen" : null
            icon.source: Utils.toImageProvider("fullscreen", MementoPalette.text)
            onClicked: root.player.controller.setFullscreen(true)
        }

        ToolButton {
            id: fullscreenExitButton
            focusPolicy: Qt.NoFocus
            visible: root.player.state.fullscreen
            icon.name: MementoSettings.interfaceSystemIcons ? "view-restore" : null
            icon.source: Utils.toImageProvider("fullscreen-exit", MementoPalette.text)
            onClicked: root.player.controller.setFullscreen(false)
        }
    }

    Loader {
        anchors.bottom: root.top
        anchors.bottomMargin: 5
        x: durationSlider.x + durationSlider.xPosition - width / 2

        active: MementoSettings.behaviorOscPreviewThumbnails
        sourceComponent: Component {
            PlayerThumbnail {
                visible: durationSlider.hovered && !durationSlider.pressed
                opacity: active ? 1 : 0
                path: root.player.state.path
                position: durationSlider.cursorValue
            }
        }
    }
}
