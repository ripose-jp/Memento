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

        Repeater {
            id: lineRepeater

            model: root.makeTextModel(root.text)
            delegate: Rectangle {
                id: delegateRect

                required property string text
                required property int offset

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
            }
        }
    }
}
