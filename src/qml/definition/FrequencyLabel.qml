import QtQuick
import Ripose.Memento

MetaLabel {
    id: root

    required property Frequency frequency

    readonly property string name: root.frequency?.dictionaryInfo?.name ?? ""
    readonly property string value: root.frequency?.frequency ?? ""

    textFormat: TextEdit.RichText
    text: `${root.name} <font color='black'>${root.value}</font>`
    color: "#5cb85c"
}
