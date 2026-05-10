import QtQuick
import QtQuick.Shapes

Item {
    id: root

    property string text: ""
    property font font
    property color color: "white"
    property color background: "transparent"
    property color stroke: "black"
    property real strokeSize: 1.0
    property int lineSpacing: 0
    property int cursorShape: Qt.IBeamCursor

    property int hoverIndex: -1
    property int selectionStart: -1
    property int selectionEnd: -1
    readonly property string selectedText: root.text.substring(
                                               root.selectionStart, root.selectionEnd)

    readonly property real margin: root.strokeSize / 2

    signal clicked()
    signal doubleClicked()
    signal middleClicked()

    /**
     * Selects text and sets selectionStart, selectionEnd, and selectedText.
     * @param start The starting index of the text to select (inclusive).
     * @param end The ending index of the text to select (exclusive).
     */
    function select(start, end) {
        end = Math.min(end, root.text.length);

        if (start >= end)
        {
            clearSelection();
            return;
        }
        else if (start < 0 || end < 0)
        {
            clearSelection();
            return;
        }
        else if (start > root.text.length || end > root.text.length)
        {
            clearSelection();
            return;
        }

        root.selectionStart = start;
        root.selectionEnd = end;
    }

    /**
     * Clears the currently selected text.
     */
    function clearSelection() {
        root.selectionStart = -1;
        root.selectionEnd = -1;
    }

    /**
     * Creates a JSON model for a given string where each \n is it's own object.
     * The model contains a text property that has the text in the line.
     * The offset property contains the offset into the original text the line appears.
     * @param text The text string to turn into a model.
     * @return the JSON model.
     */
    function makeTextModel(text) {
        const lines = text.split("\n");
        let offset = 0;
        let model = [];
        for (let i = 0; i < lines.length; ++i)
        {
            model.push({ "text": lines[i], "offset": offset, });
            offset += lines[i].length + 1;
        }
        return model;
    }

    implicitWidth: content.width
    implicitHeight: content.height

    onTextChanged: {
        root.hoverIndex = -1;
        root.clearSelection();
    }

    Column {
        id: content

        anchors.centerIn: parent
        spacing: root.lineSpacing

        /* This is a hack to force redraws of text when one property changes */
        Connections {
            target: root
            function onTextChanged() {
                repeaterShape.model = root.makeTextModel(root.text);
            }
        }
        Connections {
            target: root
            function onFontChanged() {
                repeaterShape.model = [];
                repeaterShape.model = root.makeTextModel(root.text);
            }
        }
        Connections {
            target: root
            function onColorChanged() {
                repeaterShape.model = [];
                repeaterShape.model = root.makeTextModel(root.text);
            }
        }
        Connections {
            target: root
            function onBackgroundChanged() {
                repeaterShape.model = [];
                repeaterShape.model = root.makeTextModel(root.text);
            }
        }
        Connections {
            target: root
            function onStrokeChanged() {
                repeaterShape.model = [];
                repeaterShape.model = root.makeTextModel(root.text);
            }
        }
        Connections {
            target: root
            function onStrokeSizeChanged() {
                repeaterShape.model = [];
                repeaterShape.model = root.makeTextModel(root.text);
            }
        }

        Repeater {
            id: repeaterShape

            model: root.makeTextModel(root.text)
            delegate: Shape {
                id: shape
                anchors.horizontalCenter: parent.horizontalCenter
                layer.enabled: true
                layer.samples: 8

                // This draws the background
                ShapePath {
                    fillColor: root.background
                    strokeColor: "transparent"
                    strokeWidth: 0

                    PathRectangle {
                        x: 0
                        y: 0
                        width: shape.width
                        height: shape.height
                    }
                }

                // This draws the text stroke
                ShapePath {
                    strokeWidth: root.strokeSize
                    strokeColor: root.stroke
                    fillColor: "transparent"
                    fillRule: ShapePath.WindingFill
                    joinStyle: ShapePath.RoundJoin
                    capStyle: ShapePath.RoundCap

                    PathText {
                        id: pathTextStroke
                        x: root.margin
                        y: root.margin
                        font: root.font
                        text: modelData.text
                    }
                }

                // This draws the selection if there is one
                ShapePath {
                    id: selectionPath

                    strokeWidth: 0
                    fillColor: MementoPalette.highlight

                    readonly property rect area: selectionPath.getSelectionBounds(
                                                     root.selectionStart, root.selectionEnd)

                    /**
                     * Returns a rectangle of the area to draw.
                     * @param start The starting index in root.text (inclusive)
                     * @param end The ending index in root.text (exclusive)
                     * @return A rect representing the area to draw on this line
                     */
                    function getSelectionBounds(start, end) {
                        const minIndex = modelData.offset;
                        const maxIndex = minIndex + modelData.text.length;

                        const overlap = minIndex <= end && start <= maxIndex;
                        if (!overlap)
                        {
                            return Qt.rect(0, 0, 0, 0);
                        }

                        start = Math.max(start - minIndex, 0);
                        end = Math.min(end - minIndex, modelData.text.length);
                        if (start >= end)
                        {
                            return Qt.rect(0, 0, 0, 0);
                        }

                        const startRect = shadowText.positionToRectangle(start);
                        const endRect = shadowText.positionToRectangle(end);

                        const startX = root.margin + startRect.x;
                        const endX = root.margin + endRect.x + endRect.width;
                        const startY = 0
                        const endY = shape.height;
                        return Qt.rect(startX, startY, endX - startX, endY - startY);
                    }

                    PathRectangle {
                        x: selectionPath.area.x
                        y: selectionPath.area.y
                        width: selectionPath.area.width
                        height: selectionPath.area.height
                    }
                }

                // This fills in the text on top of the stroke layer
                ShapePath {
                    strokeWidth: 0
                    fillColor: root.color
                    fillRule: ShapePath.WindingFill

                    PathText {
                        id: pathTextFill
                        x: root.margin
                        y: root.margin
                        font: root.font
                        text: modelData.text
                    }
                }

                // This text edit is only used for hit testing, it is not shown
                TextEdit {
                    id: shadowText

                    x: root.margin // This centers the text horizontally perfectly
                    anchors.verticalCenter: shape.verticalCenter
                    font: root.font
                    color: "transparent"
                    readOnly: true
                    selectByMouse: false
                    selectByKeyboard: false
                    wrapMode: TextEdit.NoWrap
                    text: modelData.text

                    /**
                     * Returns the index into the root.text property for the
                     * given x-coordinate.
                     * @param x The x-coordinate in shadowText space.
                     * @return The index of the given coordinates in root.text.
                     */
                    function getCharacterIndexAt(x) {
                        const pos = shadowText.positionAt(x, 0);
                        const rect = shadowText.positionToRectangle(pos);
                        const actualIndex = (x < rect.x) ? pos - 1 : pos;
                        const lineIndex = Math.max(0, Math.min(shadowText.text.length - 1, actualIndex));
                        return lineIndex + modelData.offset;
                    }

                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        acceptedButtons: Qt.LeftButton | Qt.MiddleButton
                        cursorShape: root.cursorShape
                        onPositionChanged: function(mouse) {
                            let index = shadowText.getCharacterIndexAt(mouse.x);
                            root.hoverIndex = index;
                        }
                        onClicked: function(event) {
                            if (event.button === Qt.LeftButton)
                            {
                                root.clicked();
                            }
                            else if (event.button === Qt.MiddleButton)
                            {
                                root.middleClicked();
                            }
                        }
                        onDoubleClicked: root.doubleClicked()
                        onExited: root.hoverIndex = -1
                    }
                }
            }
        }
    }
}
