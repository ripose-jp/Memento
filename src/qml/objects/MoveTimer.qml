import QtQml

Timer {
    id: root

    /* The minimum distance in pixels the cursor needs to move */
    property int minMovement: 0

    /* The current position of the mouse */
    required property point cursorPosition

    /* The position of the mouse at the last tick */
    property point lastTickPosition: Qt.point(0, 0)

    /* Emitted when the mouse is moved more than minMovement */
    signal mouseMoved()

    interval: 100
    running: true
    repeat: true
    onTriggered: {
        const p1 = root.lastTickPosition;
        const p2 = root.cursorPosition;
        if (Math.hypot(p1.x - p2.x, p1.y - p2.y) > root.minMovement)
        {
            root.mouseMoved();
        }
        root.lastTickPosition = p2;
    }
}
