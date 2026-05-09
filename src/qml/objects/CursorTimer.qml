import QtQml

Timer {
    id: root

    required property MpvPlayer player

    property int cursorShape: Qt.ArrowCursor

    readonly property int autoHideType: root.player.state.cursorAutoHideType
    readonly property int autoHideTime: root.player.state.cursorAutoHideTime
    readonly property bool autoHideFullscreenOnly: root.player.state.cursorAutoHideFullscreenOnly
    readonly property bool fullscreen: root.player.state.fullscreen

    /**
     * Handles the timer being started.
     */
    function handleRunning() {
        if (MementoSettings.behaviorOscMpvCursor)
        {
            if (root.autoHideFullscreenOnly && !root.fullscreen)
            {
                root.cursorShape = Qt.ArrowCursor;
                return;
            }

            switch (root.autoHideType)
            {
            case MpvState.AutoHideNumber:
            case MpvState.AutoHideNever:
                root.cursorShape = Qt.ArrowCursor;
                break;

            case MpvState.AutoHideAlways:
                root.cursorShape = Qt.BlankCursor;
                break;
            }
        }
        else
        {
            root.cursorShape = Qt.ArrowCursor;
        }
    }

    /**
     * Handles timeouts by settings the cursor to blank if needed.
     */
    function handleTimeout() {
        if (MementoSettings.behaviorOscMpvCursor)
        {
            if (root.autoHideFullscreenOnly && !root.fullscreen)
            {
                root.cursorShape = Qt.ArrowCursor;
                return;
            }

            switch (root.autoHideType)
            {
            case MpvState.AutoHideNumber:
            case MpvState.AutoHideAlways:
                root.cursorShape = Qt.BlankCursor;
                break;

            case MpvState.AutoHideNever:
                root.cursorShape = Qt.ArrowCursor;
                break;
            }
        }
        else
        {
            root.cursorShape = Qt.BlankCursor;
        }
    }

    onAutoHideTypeChanged: {
        root.restart();
        root.handleRunning();
    }
    onAutoHideFullscreenOnlyChanged: {
        root.restart();
        root.handleRunning();
    }
    onFullscreenChanged: {
        root.handleTimeout();
    }

    repeat: false
    running: false
    interval: {
        if (MementoSettings.behaviorOscMpvCursor)
        {
            return root.autoHideTime;
        }
        return MementoSettings.behaviorOscDuration;
    }

    onRunningChanged: {
        if (root.running)
        {
            root.handleRunning();
        }
    }

    onTriggered: root.handleTimeout()
}
