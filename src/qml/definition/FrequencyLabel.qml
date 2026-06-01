import QtQuick
import Ripose.Memento

MetaLabel {
    id: root

    property string name: root.frequency?.dictionaryInfo?.name ?? ""
    property string value: root.frequency?.frequency ?? ""

    textFormat: TextEdit.RichText
    text: root.name.length > 0 ?
              `${root.name} <font color='black'>${root.value}</font>` :
              `<font color='black'>${root.value}</font>`
    color: "#5cb85c"
}
