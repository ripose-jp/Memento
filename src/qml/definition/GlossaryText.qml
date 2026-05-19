import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Item {
    id: root

    property TermDefinition definition: null
    property bool persistentSelection: false
    property font baseFont: MementoSettings.interfaceSearchGlossaryFont

    readonly property var document: StructuredContent.document(
        root.definition?.dictionaryInfo ?? null,
        root.definition?.glossary ?? [],
        MementoSettings.searchGlossaryStyle,
        root.baseFont)

    property string selectedText: ""
    property var selectableTextEntries: []
    property var selectionAnchor: null
    property var selectionCursor: null
    property bool glossarySelectionActive: false
    property bool selectionBatchActive: false
    property var activeRecursiveText: null
    property int pendingRecursiveIndex: -1
    property var pendingLinkSource: null

    readonly property var recursiveSearchPopup: popupLoader.item

    implicitHeight: documentColumn.implicitHeight

    /**
     * Update the aggregate selection after one selectable node changes.
     */
    function updateSelection() {
        if (!root.selectionBatchActive)
        {
            root.rebuildSelectedText();
        }
    }

    /**
     * Begin a batch of text selection updates.
     */
    function beginSelectionBatch() {
        root.selectionBatchActive = true;
    }

    /**
     * Finish a batch of text selection updates and rebuild selected text.
     */
    function endSelectionBatch() {
        root.selectionBatchActive = false;
        root.rebuildSelectedText();
    }

    /**
     * Get selected text leaves in document order.
     * @return The ordered selected text leaves.
     */
    function selectedTexts() {
        return root.selectableTexts()
            .filter(entry => entry.item.selectedText.length > 0);
    }

    /**
     * Get the separator to place between two selected leaves.
     * @param previous The previous rendered text item.
     * @param current The current rendered text item.
     * @return An empty string for one visual line, otherwise a newline.
     */
    function selectionSeparator(previous, current) {
        const previousTop = previous.mapToItem(root, 0, 0).y;
        const currentTop = current.mapToItem(root, 0, 0).y;
        const previousBottom = previousTop + previous.height;
        const currentBottom = currentTop + current.height;
        const overlapsVertically =
            previousTop < currentBottom && currentTop < previousBottom;
        return overlapsVertically ? "" : "\n";
    }

    /**
     * Rebuild the aggregate selected text exposed by this glossary.
     */
    function rebuildSelectedText() {
        const selected = root.selectedTexts();
        let text = "";
        for (let i = 0; i < selected.length; ++i)
        {
            if (i > 0)
            {
                text += root.selectionSeparator(
                    selected[i - 1].item,
                    selected[i].item);
            }
            text += selected[i].item.selectedText;
        }
        root.selectedText = text;
    }

    /**
     * Get if this glossary currently owns selected text.
     * @return true if selected text exists, false otherwise.
     */
    function hasGlossarySelection() {
        return root.selectedText.length > 0;
    }

    /**
     * Copy the full glossary-owned selection to the system clipboard.
     */
    function copyGlossarySelection() {
        if (root.selectedText.length > 0)
        {
            clipboard.setText(root.selectedText);
        }
    }

    /**
     * Register one selectable text leaf with this glossary.
     * @param nodeId The stable document node ID.
     * @param item The rendered text item.
     */
    function registerSelectableText(nodeId, item) {
        const numericId = Number(nodeId);
        let entries = root.selectableTextEntries.filter(
            entry => entry.nodeId !== numericId && entry.item !== item);
        entries.push({
            "nodeId": numericId,
            "item": item
        });
        entries.sort((a, b) => a.nodeId - b.nodeId);
        root.selectableTextEntries = entries;
    }

    /**
     * Remove one selectable text leaf from this glossary.
     * @param nodeId The stable document node ID.
     * @param item The rendered text item.
     */
    function unregisterSelectableText(nodeId, item) {
        const numericId = Number(nodeId);
        root.selectableTextEntries = root.selectableTextEntries.filter(
            entry => entry.nodeId !== numericId || entry.item !== item);
    }

    /**
     * Get selectable text leaves in document order.
     * @return The ordered selectable text leaves.
     */
    function selectableTexts() {
        return root.selectableTextEntries
            .filter(entry => entry.item && entry.item.visible);
    }

    /**
     * Convert a local text-item position into a document selection point.
     * @param item The text item receiving the pointer event.
     * @param x The local x position.
     * @param y The local y position.
     * @return A document-order selection point.
     */
    function selectionPoint(item, x, y) {
        const index = Math.max(
            0,
            Math.min(item.length, item.getIndexAt(x, y)));
        return {
            "nodeId": Number(item.selectionNodeId),
            "index": index
        };
    }

    /**
     * Get a selection point nearest to one glossary-local coordinate.
     * @param x The glossary-local x position.
     * @param y The glossary-local y position.
     * @return The nearest document-order selection point.
     */
    function nearestSelectionPoint(x, y) {
        let nearest = null;
        for (const entry of root.selectableTexts())
        {
            const item = entry.item;
            const topLeft = item.mapToItem(root, 0, 0);
            const right = topLeft.x + item.width;
            const bottom = topLeft.y + item.height;
            const dx = x < topLeft.x ? topLeft.x - x :
                x > right ? x - right : 0;
            const dy = y < topLeft.y ? topLeft.y - y :
                y > bottom ? y - bottom : 0;
            const distance = dx * dx + dy * dy;
            if (!nearest || distance < nearest.distance)
            {
                const local = item.mapFromItem(root, x, y);
                nearest = {
                    "distance": distance,
                    "nodeId": entry.nodeId,
                    "index": Math.max(
                        0,
                        Math.min(
                            item.length,
                            item.getIndexAt(local.x, local.y)))
                };
            }
        }
        return nearest;
    }

    /**
     * Compare two document-order selection points.
     * @param left The first selection point.
     * @param right The second selection point.
     * @return A negative number, zero, or a positive number.
     */
    function compareSelectionPoints(left, right) {
        if (left.nodeId !== right.nodeId)
        {
            return left.nodeId - right.nodeId;
        }
        return left.index - right.index;
    }

    /**
     * Begin a glossary-wide selection.
     * @param item The text item receiving the pointer press.
     * @param x The item-local x position.
     * @param y The item-local y position.
     */
    function beginGlossarySelection(item, x, y) {
        root.clearGlossarySelection();
        root.glossarySelectionActive = true;
        root.selectionAnchor = root.selectionPoint(item, x, y);
        root.selectionCursor = root.selectionAnchor;
        root.applyGlossarySelection();
    }

    /**
     * Extend the active glossary-wide selection.
     * @param item The text item that owns the drag grab.
     * @param x The item-local x position.
     * @param y The item-local y position.
     */
    function extendGlossarySelection(item, x, y) {
        if (!root.glossarySelectionActive)
        {
            return;
        }
        const local = item.mapToItem(root, x, y);
        const point = root.nearestSelectionPoint(local.x, local.y);
        if (!point)
        {
            return;
        }
        root.selectionCursor = point;
        root.applyGlossarySelection();
    }

    /**
     * Finish the active glossary-wide drag selection.
     */
    function endGlossarySelection() {
        root.glossarySelectionActive = false;
    }

    /**
     * Clear every text leaf selected in this glossary.
     */
    function clearGlossarySelection() {
        root.beginSelectionBatch();
        for (const entry of root.selectableTexts())
        {
            entry.item.deselect();
        }
        root.selectionAnchor = null;
        root.selectionCursor = null;
        root.endSelectionBatch();
    }

    /**
     * Apply the active document-order selection range to every text leaf.
     */
    function applyGlossarySelection() {
        if (!root.selectionAnchor || !root.selectionCursor)
        {
            return;
        }

        let start = root.selectionAnchor;
        let end = root.selectionCursor;
        if (root.compareSelectionPoints(start, end) > 0)
        {
            const swap = start;
            start = end;
            end = swap;
        }

        root.beginSelectionBatch();
        for (const entry of root.selectableTexts())
        {
            const item = entry.item;
            if (entry.nodeId < start.nodeId || entry.nodeId > end.nodeId)
            {
                item.deselect();
                continue;
            }
            if (start.nodeId === end.nodeId)
            {
                item.select(start.index, end.index);
            }
            else if (entry.nodeId === start.nodeId)
            {
                item.select(start.index, item.length);
            }
            else if (entry.nodeId === end.nodeId)
            {
                item.select(0, end.index);
            }
            else
            {
                item.selectAll();
            }
        }
        root.endSelectionBatch();
    }

    /**
     * Position a popup relative to one source item.
     * @param popup The popup to position.
     * @param sourceItem The item that initiated the popup.
     * @param x The source-item-local x coordinate.
     * @param y The source-item-local y coordinate.
     * @param height The source marker height.
     */
    function positionPopup(popup, sourceItem, x, y, height) {
        popup.parent = Overlay.overlay;
        const position = sourceItem.mapToItem(null, x, y);
        let popupX = position.x - (popup.width / 2);
        popupX = Math.max(popupX, 0);
        popupX = Math.min(popupX, Overlay.overlay.width - popup.width);
        let popupY = position.y - popup.height;
        if (popupY < 0)
        {
            popupY = position.y + height;
        }
        popup.x = popupX;
        popup.y = popupY;
    }

    /**
     * Activate a glossary link.
     * @param href The link target.
     * @param sourceItem The item that initiated the link.
     * @param x The source-item-local x coordinate.
     * @param y The source-item-local y coordinate.
     */
    function activateLink(href, sourceItem, x, y) {
        if (href.startsWith("http://") || href.startsWith("https://"))
        {
            Qt.openUrlExternally(href);
            return;
        }
        if (!href.startsWith("?"))
        {
            return;
        }

        const match = href.match(/[?&]query=([^&]+)/);
        if (!match)
        {
            return;
        }
        const query = decodeURIComponent(match[1].replace(/\+/g, " "));
        if (root.activeRecursiveText)
        {
            root.activeRecursiveText.deselect();
        }
        root.activeRecursiveText = null;
        root.pendingRecursiveIndex = -1;
        root.pendingLinkSource = {
            "href": href,
            "item": sourceItem,
            "x": x,
            "y": y
        };
        if (popupLoader.status === Loader.Null)
        {
            popupLoader.active = true;
            return;
        }
        if (popupLoader.status !== Loader.Ready)
        {
            return;
        }

        popupLoader.item.search.searchTerms(query, query, 0);
        popupLoader.item.search.searchKanji(query.charAt(0), query, 0);
        root.positionPopup(popupLoader.item, sourceItem, x, y, 1);
        root.pendingLinkSource = null;
    }

    /**
     * Search glossary text with one popup shared across every text leaf.
     * @param textItem The text item that initiated the recursive search.
     * @param index The plain-text index to search.
     */
    function activateRecursiveSearch(textItem, index) {
        if (root.activeRecursiveText &&
            root.activeRecursiveText !== textItem)
        {
            root.activeRecursiveText.deselect();
        }
        root.activeRecursiveText = textItem;
        root.pendingRecursiveIndex = index;
        root.pendingLinkSource = null;
        if (popupLoader.status === Loader.Null)
        {
            popupLoader.active = true;
            return;
        }
        if (popupLoader.status !== Loader.Ready)
        {
            return;
        }

        textItem.searchWithPopup(popupLoader.item, index);
    }

    /**
     * Execute the pending popup search after the popup finishes initialization.
     */
    function runPendingPopupSearch() {
        if (root.pendingLinkSource)
        {
            const source = root.pendingLinkSource;
            root.activateLink(
                source.href, source.item, source.x, source.y);
        }
        else if (root.activeRecursiveText)
        {
            root.activeRecursiveText.searchWithPopup(
                popupLoader.item,
                root.pendingRecursiveIndex);
        }
    }

    Column {
        id: documentColumn

        width: root.width

        Repeater {
            model: root.document.children ?? []

            delegate: StructuredContentNode {
                required property var modelData

                width: documentColumn.width
                availableWidth: documentColumn.width
                baseFont: root.baseFont
                node: modelData
                owner: root
            }
        }
    }

    Loader {
        id: popupLoader

        active: false
        source: "qrc:/qt/qml/Ripose/Memento/qml/definition/DefinitionPopup.qml"
        onLoaded: Qt.callLater(root.runPendingPopupSearch)
    }

    Clipboard {
        id: clipboard
    }
}
