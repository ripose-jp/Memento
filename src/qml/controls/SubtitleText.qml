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

    readonly property int margin: Math.ceil(root.strokeSize / 2.0)

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

        root.clearSelection();
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

        Repeater {
            id: shapeRepeater

            model: root.makeTextModel(root.text)
            delegate: Rectangle {
                id: delegateRect

                required property string text
                required property int offset

                readonly property int selectionStart: {
                    if (root.selectionStart < 0)
                    {
                        return -1;
                    }
                    const start = root.selectionStart - delegateRect.offset;
                    return Math.max(start, 0);
                }

                readonly property int selectionEnd: {
                    if (root.selectionEnd < 0)
                    {
                        return -1;
                    }
                    const end = root.selectionEnd - delegateRect.offset;
                    return Math.min(end, delegateRect.text.length);
                }

                anchors.horizontalCenter: parent.horizontalCenter
                height: strokeShape.implicitHeight
                width: strokeShape.implicitWidth
                color: root.background

                Shape {
                    id: strokeShape
                    anchors.centerIn: parent
                    antialiasing: true
                    layer.enabled: true
                    layer.samples: 8
                    layer.smooth: true

                    ShapePath {
                        strokeWidth: root.strokeSize
                        strokeColor: root.stroke
                        fillColor: "transparent"
                        fillRule: ShapePath.WindingFill
                        joinStyle: ShapePath.RoundJoin
                        capStyle: ShapePath.RoundCap

                        PathText {
                            id: strokePathText
                            x: root.margin
                            y: root.margin
                            font: root.font
                            text: delegateRect.text
                        }
                    }
                }

                Rectangle {
                    id: selectionRect

                    readonly property int maxVerticalPadding: 10

                    anchors.verticalCenter: textShape.verticalCenter
                    x: {
                        if (delegateRect.selectionStart < 0 ||
                                delegateRect.selectionStart > textEdit.length)
                        {
                            return 0;
                        }
                        const startRect = textEdit.positionToRectangle(delegateRect.selectionStart);
                        return startRect.x + root.margin;
                    }
                    height: textShape.height + Math.min(selectionRect.maxVerticalPadding, root.margin) * 2
                    width: {
                        if (delegateRect.selectionStart < 0 ||
                                delegateRect.selectionStart > textEdit.length ||
                                delegateRect.selectionEnd < 0 ||
                                delegateRect.selectionEnd > textEdit.length ||
                                delegateRect.selectionStart >= delegateRect.selectionEnd)
                        {
                            return 0;
                        }
                        const startRect = textEdit.positionToRectangle(delegateRect.selectionStart);
                        const endRect = textEdit.positionToRectangle(delegateRect.selectionEnd);
                        return endRect.x - startRect.x;
                    }
                    color: MementoPalette.highlight
                    visible: delegateRect.selectionStart < delegateRect.selectionEnd
                }

                Shape {
                    id: textShape
                    anchors.centerIn: strokeShape
                    antialiasing: true
                    layer.enabled: true
                    layer.samples: 8
                    layer.smooth: true

                    ShapePath {
                        strokeWidth: 0
                        fillColor: root.color
                        fillRule: ShapePath.WindingFill
                        joinStyle: ShapePath.RoundJoin
                        capStyle: ShapePath.RoundCap

                        PathText {
                            id: textPathText
                            font: root.font
                            text: delegateRect.text
                        }
                    }
                }

                TextEdit {
                    id: textEdit

                    /**
                     * Returns the index into the root.text property for the
                     * given x-coordinate.
                     * @param x The x-coordinate in textEdit space.
                     * @return The index of the given coordinates in root.text.
                     */
                    function getCharacterIndexAt(x) {
                        const pos = textEdit.positionAt(x, 0);
                        const rect = textEdit.positionToRectangle(pos);
                        const actualIndex = (x < rect.x) ? pos - 1 : pos;
                        const lineIndex = Math.max(0, Math.min(textEdit.text.length - 1, actualIndex));
                        return lineIndex + delegateRect.offset;
                    }

                    x: root.margin
                    anchors.verticalCenter: textShape.verticalCenter
                    font: root.font
                    color: "transparent"
                    readOnly: true
                    selectByMouse: false
                    selectByKeyboard: false
                    selectionColor: MementoPalette.highlight
                    wrapMode: TextEdit.NoWrap
                    renderType: TextEdit.QtRendering
                    text: delegateRect.text
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton | Qt.MiddleButton
                    cursorShape: root.cursorShape
                    onPositionChanged: function(mouse) {
                        let x = mouse.x - root.margin;
                        if (x >= 0 && x < textEdit.width)
                        {
                            let index = textEdit.getCharacterIndexAt(x);
                            root.hoverIndex = index;
                        }
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
