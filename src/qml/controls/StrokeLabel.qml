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
    property real lineSpacing: 0

    readonly property int margin: Math.ceil(root.strokeSize / 2.0)

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

                // This fills in the text on top of the stroke layer
                ShapePath {
                    strokeWidth: 0
                    fillColor: root.color
                    fillRule: ShapePath.WindingFill

                    PathText {
                        x: root.margin
                        y: root.margin
                        font: root.font
                        text: modelData.text
                    }
                }
            }
        }
    }
}
