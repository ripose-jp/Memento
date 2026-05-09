import QtQuick
import Ripose.Memento

Rectangle {
    id: root

    color: Features.platform === Features.Linux ? MementoPalette.border : MementoPalette.mid
    height: 1
}
