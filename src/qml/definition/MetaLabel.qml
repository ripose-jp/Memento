import QtQuick
import QtQuick.Controls
import Ripose.Memento

Rectangle {
    id: root

    property string text: ""
    property string tip: ""
    property alias textFormat: tagLabel.textFormat

    height: tagLabel.contentHeight + tagLabel.anchors.topMargin + tagLabel.anchors.bottomMargin
    width: tagLabel.contentWidth + tagLabel.anchors.leftMargin + tagLabel.anchors.rightMargin

    radius: 3

    HoverHandler {
        id: hoverHandler
    }

    SelectableLabel {
        id: tagLabel
        anchors {
            fill: parent
            leftMargin: 5
            rightMargin: 5
            topMargin: 2
            bottomMargin: 2
        }

        font.family: MementoSettings.interfaceSearchTagFont.family
        font.italic: MementoSettings.interfaceSearchTagFont.italic
        font.underline: MementoSettings.interfaceSearchTagFont.underline
        font.pointSize: MementoSettings.interfaceSearchTagFont.pointSize
        font.weight: MementoSettings.interfaceSearchTagFont.weight
        font.overline: MementoSettings.interfaceSearchTagFont.overline
        font.strikeout: MementoSettings.interfaceSearchTagFont.strikeout
        font.letterSpacing: MementoSettings.interfaceSearchTagFont.letterSpacing
        font.wordSpacing: MementoSettings.interfaceSearchTagFont.wordSpacing
        font.kerning: MementoSettings.interfaceSearchTagFont.kerning
        font.preferShaping: MementoSettings.interfaceSearchTagFont.preferShaping
        font.hintingPreference: MementoSettings.interfaceSearchTagFont.hintingPreference
        font.styleName: MementoSettings.interfaceSearchTagFont.styleName

        color: "white"
        text: root.text
    }

    ToolTip {
        id: toolTip
        visible: text.length > 0 && hoverHandler.hovered
        delay: 500
        text: root.tip
    }
}
