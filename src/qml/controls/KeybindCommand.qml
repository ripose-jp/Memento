import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

RowLayout {
    id: root

    property alias text: label.text
    property alias sequence: shortcut.sequence

    property bool isRecording: false

    signal sequenceSet(string seq)
    signal clearClicked()

    Shortcut {
        id: shortcut
        enabled: false
    }

    Item {
        id: keyHandler
        focus: root.isRecording
        onFocusChanged: root.isRecording = focus

        /**
         * Reset the key handler state.
         */
        function reset() {
            root.isRecording = false;
            keyHandler.focus = false;
        }

        Keys.onPressed: function(event) {
            if (!root.isRecording)
            {
                return;
            }

            if (event.key === Qt.Key_Escape)
            {
                keyHandler.reset();
                return;
            }

            switch (event.key)
            {
            case Qt.Key_Control:
            case Qt.Key_Alt:
            case Qt.Key_AltGr:
            case Qt.Key_Shift:
            case Qt.Key_Meta:
                return;
            }

            root.sequenceSet(KeyTracker.keyComboToString(event.key, event.modifiers));
            keyHandler.reset();
            event.accepted = true;
        }
    }

    Label {
        id: label
    }

    Item {
        Layout.fillWidth: true
    }

    Button {
        text: {
            if (root.isRecording)
            {
                return qsTr("...");
            }
            else if (shortcut.nativeText.length === 0)
            {
                return qsTr("Unbound");
            }
            return shortcut.nativeText;
        }
        onClicked: {
            root.isRecording = true;
            keyHandler.forceActiveFocus();
        }
    }

    Button {
        enabled: MementoSettings.keybinds.profile
        icon.name: MementoSettings.interfaceSystemIcons ? "edit-clear" : null
        icon.source: Utils.toImageProvider("clear", MementoPalette.text)
        text: qsTr("Clear")
        onClicked: {
            root.isRecording = false;
            root.clearClicked();
        }
    }
}
