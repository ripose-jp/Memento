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
            delegate: Item {
                id: delegateItem
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.max(shape.width, textMetrics.tightBoundingRect.width)
                height: Math.max(shape.height, textMetrics.tightBoundingRect.height)
                clip: true

                TextMetrics {
                    id: textMetrics
                    font: textEdit.font
                    text: textEdit.text
                    renderType: textEdit.renderType
                }

                Shape {
                    id: shape
                    antialiasing: true
                    layer.enabled: true
                    layer.samples: 4
                    layer.smooth: true

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
                }

                TextEdit {
                    id: textEdit

                    x: root.margin
                    y: root.margin - (textMetrics.tightBoundingRect.y - textMetrics.boundingRect.y)
                    font: root.font
                    color: root.color
                    readOnly: true
                    selectByMouse: false
                    selectByKeyboard: false
                    selectionColor: MementoPalette.highlight
                    wrapMode: TextEdit.NoWrap
                    renderType: TextEdit.QtRendering
                    text: modelData.text

                    Connections {
                        target: root
                        function onSelectionStartChanged() {
                            textEdit.updateSelection();
                        }
                    }
                    Connections {
                        target: root
                        function onSelectionEndChanged() {
                            textEdit.updateSelection();
                        }
                    }

                    /**
                     * Update the selection with the new selectionStart and selectionEnd values.
                     */
                    function updateSelection() {
                        if (root.selectionStart < 0 || root.selectionEnd < 0)
                        {
                            textEdit.deselect();
                            return;
                        }

                        let start = root.selectionStart - modelData.offset;
                        start = Math.max(start, 0);
                        let end = root.selectionEnd - modelData.offset;
                        end = Math.min(end, textEdit.text.length);
                        if (start >= end)
                        {
                            textEdit.deselect();
                            return;
                        }
                        textEdit.select(start, end);
                    }

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
                        return lineIndex + modelData.offset;
                    }
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
