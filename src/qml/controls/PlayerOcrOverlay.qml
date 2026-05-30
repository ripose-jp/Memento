import QtQuick
import QtQuick.Controls
import Ripose.Memento

Item {
    id: root

    required property MpvPlayer player

    property color highlight: MementoPalette.highlight
    property real highlightOpacity: 0.2

    property point startPoint: Qt.point(0, 0)
    property point endPoint: Qt.point(0, 0)
    property bool active: false
    property bool selecting: false
    property bool running: false
    property int requestId: 0

    signal hideOscRequested()
    signal modeChanged(bool enabled)
    signal restoreOscRequested()
    signal showTextRequested(string text)
    signal textRecognized(string text)

    anchors.fill: parent
    visible: root.active
    z: 100
    focus: root.active

    /**
     * Reset the current selection to the null selection.
     */
    function resetSelection() {
        selecting = false;
        startPoint = Qt.point(0, 0);
        endPoint = Qt.point(0, 0);
    }

    /**
     * Ignore the result of any in-flight OCR request.
     */
    function invalidateRequest() {
        requestId++;
    }

    /**
     * Start OCR selection mode.
     */
    function start() {
        if (!Features.ocr || !MementoSettings.ocrEnabled || running)
        {
            return;
        }

        player.forceActiveFocus();
        root.hideOscRequested();
        root.active = true;
        root.modeChanged(true);
        resetSelection();
    }

    /**
     * Cancel OCR selection mode and ignore any in-flight OCR result.
     */
    function cancel() {
        root.active = false;
        root.modeChanged(false);
        invalidateRequest();
        resetSelection();
        root.restoreOscRequested();
    }

    /**
     * Get a rect from the given selection area.
     * @return A rect from the given selection area.
     */
    function selectionRect() {
        const x = Math.min(startPoint.x, endPoint.x);
        const y = Math.min(startPoint.y, endPoint.y);
        const w = Math.abs(startPoint.x - endPoint.x);
        const h = Math.abs(startPoint.y - endPoint.y);
        return Qt.rect(x, y, w, h);
    }

    /**
     * Attempt to get text from an OCR selection and emit the results on
     * success, and an error on failure.
     */
    function submitSelection() {
        const rect = selectionRect();
        resetSelection();
        if (rect.width < 2 || rect.height < 2)
        {
            root.cancel();
            return;
        }

        const currentRequestId = ++root.requestId;
        root.active = false;
        root.modeChanged(false);
        root.running = true;
        root.restoreOscRequested();
        OcrController.readRegion(player.controller, rect, player.width, player.height)
            .then(function(result) {
                if (currentRequestId !== root.requestId)
                {
                    return;
                }
                root.running = false;
                if (result.text)
                {
                    root.textRecognized(result.text);
                }
                else if (result.error)
                {
                    root.showTextRequested(result.error);
                }
                root.restoreOscRequested();
            });
    }

    Rectangle {
        x: root.selectionRect().x
        y: root.selectionRect().y
        width: root.selectionRect().width
        height: root.selectionRect().height
        visible: root.selecting
        color: Qt.rgba(root.highlight.r, root.highlight.g, root.highlight.b, root.highlightOpacity)
        border.color: root.highlight
        border.width: 1
    }

    MouseArea {
        anchors.fill: parent
        enabled: root.active
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        cursorShape: Qt.CrossCursor

        onPressed: function(event) {
            if (event.button === Qt.RightButton)
            {
                root.cancel();
                event.accepted = true;
                return;
            }
            root.startPoint = Qt.point(event.x, event.y);
            root.endPoint = Qt.point(event.x, event.y);
            root.selecting = true;
        }

        onPositionChanged: function(event) {
            if (root.selecting)
            {
                root.endPoint = Qt.point(event.x, event.y);
            }
        }

        onReleased: function(event) {
            if (event.button !== Qt.LeftButton || !root.selecting)
            {
                return;
            }
            root.endPoint = Qt.point(event.x, event.y);
            root.submitSelection();
        }
    }
}
