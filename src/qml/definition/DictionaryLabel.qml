import QtQuick
import Ripose.Memento

MetaLabel {
    id: root

    required property DictionaryInfo info

    text: root.info?.name ?? ""
    tip: ""
    color: "#aa66cc"
}
