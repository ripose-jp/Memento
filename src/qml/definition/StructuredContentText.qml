import QtQuick
import QtQuick.Controls
import Ripose.Memento

TextEdit {
    id: root

    /* The optional owner used to share one recursive-search popup. */
    property var recursiveSearchOwner: null

    /* The optional owner used to coordinate a glossary-wide selection. */
    property var selectionOwner: null

    /* The stable source node ID used by glossary-wide selection. */
    property int selectionNodeId: -1

    /* The index into plainText that is currently hovered */
    property int hoverIndex: -1

    /* The unformatted text string */
    readonly property string plainText: root.getText(0, root.length)

    /* The popup currently serving this text item. */
    readonly property var activePopup:
        root.recursiveSearchOwner ?
            root.recursiveSearchOwner.recursiveSearchPopup :
            popupLoader.item

    /**
     * Returns the index into plain text of the coordinates.
     * @param x The x-coordinate of the TextEdit.
     * @param y The y-coordinate of the TextEdit.
     * @return The index of the character.
     */
    function getIndexAt(x, y) {
        const index = root.positionAt(x, y);
        const rect = root.positionToRectangle(index);
        return (x < rect.x) ? index - 1 : index;
    }

    /**
     * Executes a search at a given plain text index.
     * @param index The index into plain text to search.
     */
    function search(index) {
        if (!root.canSearchAt(index))
        {
            root.clearActivePopup();
            return;
        }

        if (root.recursiveSearchOwner)
        {
            root.recursiveSearchOwner.activateRecursiveSearch(root, index);
            return;
        }

        switch (popupLoader.status)
        {
        case Loader.Null:
            popupLoader.active = true;
            return;

        case Loader.Ready:
            break;

        default:
        case Loader.Loading:
            return;
        }

        root.searchWithPopup(popupLoader.item, index);
    }

    /**
     * Execute a search using a ready popup.
     * @param popup The popup to populate and position.
     * @param index The index into plain text to search.
     */
    function searchWithPopup(popup, index) {
        if (!popup || !root.canSearchAt(index))
        {
            root.clearPopup(popup);
            return;
        }

        const MAX_SEARCH_LENGTH = 40;

        popup.search.searchTerms(
                    root.plainText.substring(index, index + MAX_SEARCH_LENGTH),
                    root.plainText,
                    index);
        popup.search.searchKanji(
                    root.plainText.charAt(index),
                    root.plainText,
                    index);

        const rect = root.positionToRectangle(index);
        if (root.recursiveSearchOwner)
        {
            root.recursiveSearchOwner.positionPopup(
                popup, root, rect.x, rect.y, rect.height);
            return;
        }

        popup.parent = Overlay.overlay;
        const globalRect = root.mapToItem(null, rect);
        let x = globalRect.x - (popup.width / 2);
        x = Math.max(x, 0);
        x = Math.min(x, Overlay.overlay.width - popup.width);
        let y = globalRect.y - popup.height;
        if (y < 0)
        {
            y = globalRect.y + globalRect.height;
        }
        popup.x = x;
        popup.y = y;
    }

    /**
     * Get if a recursive search should begin at one text offset.
     * @param index The plain-text index to inspect.
     * @return true if the offset starts on non-whitespace text.
     */
    function canSearchAt(index) {
        if (index < 0 || index >= root.length)
        {
            return false;
        }
        return !/\s/.test(root.plainText.charAt(index));
    }

    /**
     * Clear the popup currently associated with this text leaf.
     */
    function clearActivePopup() {
        root.clearPopup(root.activePopup);
    }

    /**
     * Clear results from one popup, if it exists.
     * @param popup The popup to clear.
     */
    function clearPopup(popup) {
        if (popup)
        {
            popup.search.clearResults();
        }
    }

    /**
     * Return whether this text item owns the active recursive popup.
     */
    function ownsActivePopup() {
        return !root.recursiveSearchOwner ||
            root.recursiveSearchOwner.activeRecursiveText === root;
    }

    Connections {
        target: root.activePopup
        function onClosed() {
            if (root.ownsActivePopup())
            {
                root.deselect();
            }
        }
    }

    Connections {
        target: root.activePopup?.search ?? null
        function onTermsChanged() {
            if (root.ownsActivePopup())
            {
                root.select(root.hoverIndex,
                            root.hoverIndex + root.activePopup.matchLength);
            }
        }
        function onKanjiChanged() {
            if (root.ownsActivePopup())
            {
                root.select(root.hoverIndex,
                            root.hoverIndex + root.activePopup.matchLength);
            }
        }
    }

    readOnly: true
    selectByMouse: false
    selectByKeyboard: false
    textFormat: TextEdit.PlainText
    color: MementoPalette.text
    selectionColor: MementoPalette.highlight

    Keys.priority: Keys.BeforeItem
    Keys.onShortcutOverride: function(event) {
        if (root.selectionOwner &&
            root.selectionOwner.hasGlossarySelection() &&
            event.matches(StandardKey.Copy))
        {
            event.accepted = true;
        }
    }
    Keys.onPressed: function(event) {
        if (root.selectionOwner &&
            root.selectionOwner.hasGlossarySelection() &&
            event.matches(StandardKey.Copy))
        {
            root.selectionOwner.copyGlossarySelection();
            event.accepted = true;
        }
    }

    onHoverIndexChanged: {
        if (!KeyTracker.modifierHeld(MementoSettings.searchModifier))
        {
            return;
        }
        root.search(root.hoverIndex);
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        propagateComposedEvents: true
        preventStealing: root.selectionOwner !== null
        acceptedButtons:
            Qt.MiddleButton |
            (root.selectionOwner ? Qt.LeftButton : Qt.NoButton)

        onPressed: function(event) {
            if (event.button === Qt.LeftButton && root.selectionOwner)
            {
                root.forceActiveFocus();
                root.selectionOwner.beginGlossarySelection(
                    root, event.x, event.y);
                event.accepted = true;
            }
        }

        onPositionChanged: function(event) {
            root.hoverIndex = root.getIndexAt(event.x, event.y);
            if ((pressedButtons & Qt.LeftButton) && root.selectionOwner)
            {
                root.selectionOwner.extendGlossarySelection(
                    root, event.x, event.y);
                event.accepted = true;
                return;
            }
        }

        onClicked: function(event) {
            if (event.button !== Qt.MiddleButton)
            {
                return;
            }
            if (!MementoSettings.searchMiddleMouseScan)
            {
                event.accepted = false;
                return;
            }
            root.search(root.hoverIndex);
        }

        onReleased: function(event) {
            if (event.button === Qt.LeftButton && root.selectionOwner)
            {
                root.selectionOwner.endGlossarySelection();
            }
        }

        onExited: root.hoverIndex = -1
    }

    Loader {
        id: popupLoader
        active: false
        source: "qrc:/qt/qml/Ripose/Memento/qml/definition/DefinitionPopup.qml"
        onLoaded: Qt.callLater(root.search, root.hoverIndex)
    }
}
