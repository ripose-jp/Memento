import QtQuick
import QtQuick.Layouts
import Ripose.Memento

Item {
    id: root

    required property var node
    required property var owner
    required property font baseFont

    property real availableWidth: width
    property bool selectable: true

    readonly property var style: root.node?.style ?? ({})
    readonly property real horizontalMargin:
        (root.style.marginLeft ?? 0) + (root.style.marginRight ?? 0)
    readonly property real verticalMargin:
        (root.style.marginTop ?? 0) + (root.style.marginBottom ?? 0)

    implicitWidth: rendererLoader.item?.implicitWidth ?? 0
    implicitHeight: rendererLoader.item?.implicitHeight ?? 0

    /**
     * Measure the natural horizontal extent of children in one inline run.
     * Flow.implicitWidth is not stable when the Flow may itself wrap.
     * @param item The item whose visible children should be measured.
     */
    function inlineChildrenWidth(item) {
        let width = 0;
        for (const child of item.children)
        {
            if (child.visible)
            {
                width += child.implicitWidth ?? child.width ?? 0;
            }
        }
        return width;
    }

    /**
     * Measure the widest visible child in one vertical stack.
     * @param item The item whose visible children should be measured.
     */
    function columnChildrenWidth(item) {
        let width = 0;
        for (const child of item.children)
        {
            if (child.visible)
            {
                width = Math.max(
                    width,
                    child.implicitWidth ?? child.width ?? 0);
            }
        }
        return width;
    }

    /**
     * Check whether one node or any descendant contains ruby content.
     * @param node The structured-content node to inspect.
     */
    function containsRuby(node) {
        if (!node)
        {
            return false;
        }
        if (node.kind === "ruby")
        {
            return true;
        }
        for (const child of node.children ?? [])
        {
            if (root.containsRuby(child))
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Get the clearance needed above one line flow that contains ruby.
     * @param children The child nodes rendered by the flow.
     */
    function rubyFlowInset(children) {
        for (const child of children ?? [])
        {
            if (root.containsRuby(child))
            {
                return Math.ceil(
                    root.baseFont.pointSize *
                    (root.style.fontScale ?? 1) *
                    0.65);
            }
        }
        return 0;
    }

    Loader {
        id: rendererLoader

        anchors.fill: parent
        sourceComponent: {
            switch (root.node.kind)
            {
            case "text":
            case "break":
                return textComponent;
            case "inline":
            case "ruby-text":
                return inlineComponent;
            case "link":
                return linkComponent;
            case "details":
                return detailsComponent;
            case "summary":
                return summaryComponent;
            case "list":
                return listComponent;
            case "list-item":
                return listItemComponent;
            case "table":
                return tableComponent;
            case "cell":
                return cellComponent;
            case "image":
                return imageComponent;
            case "ruby":
                return rubyComponent;
            default:
                return blockComponent;
            }
        }
    }

    Component {
        id: textComponent

        Item {
            implicitWidth: textItem.width
            implicitHeight: textItem.implicitHeight

            TextMetrics {
                id: textMetrics

                font: textItem.font
                text: root.node.text ?? ""
            }

            StructuredContentText {
                id: textItem

                width: Math.min(
                    Math.max(
                        Math.ceil(
                            Math.max(
                                textMetrics.advanceWidth,
                                textMetrics.boundingRect.width)) + 2,
                        1),
                    root.availableWidth)

                persistentSelection: root.owner.persistentSelection
                recursiveSearchOwner: root.owner
                selectionOwner: root.selectable ? root.owner : null
                selectionNodeId: root.node.id
                selectByKeyboard: true
                selectByMouse: false
                textFormat: TextEdit.PlainText
                wrapMode: TextEdit.Wrap

                color: root.style.exampleKeyword ?
                           Qt.styleHints.colorScheme ===
                           Qt.ColorScheme.Dark ?
                               "#54d26f" : "#007a2f" :
                           root.style.color || MementoPalette.text
                font.family: root.baseFont.family
                font.italic: root.style.italic ?? root.baseFont.italic
                font.underline:
                    root.style.underline ?? root.baseFont.underline
                font.overline:
                    root.style.overline ?? root.baseFont.overline
                font.strikeout:
                    root.style.strikeout ?? root.baseFont.strikeout
                font.pointSize:
                    root.baseFont.pointSize * (root.style.fontScale ?? 1)
                font.weight: root.style.bold ? Font.Bold : root.baseFont.weight
                font.letterSpacing: root.baseFont.letterSpacing
                font.wordSpacing: root.baseFont.wordSpacing
                font.kerning: root.baseFont.kerning
                font.preferShaping: root.baseFont.preferShaping
                font.hintingPreference: root.baseFont.hintingPreference
                font.styleName: root.baseFont.styleName
                text: root.node.text ?? ""

                onSelectedTextChanged:
                {
                    if (root.selectable)
                    {
                        root.owner.updateSelection(
                            root.node.id, selectedText);
                    }
                }

                Component.onCompleted:
                {
                    if (root.selectable)
                    {
                        root.owner.registerSelectableText(
                            root.node.id, textItem);
                    }
                }

                Component.onDestruction:
                {
                    if (root.selectable)
                    {
                        root.owner.unregisterSelectableText(
                            root.node.id, textItem);
                        root.owner.updateSelection(root.node.id, "");
                    }
                }
            }
        }
    }

    Component {
        id: inlineComponent

        Item {
            implicitWidth: inlineBox.implicitWidth + root.horizontalMargin
            implicitHeight: inlineBox.implicitHeight + root.verticalMargin

            Rectangle {
                id: inlineBox

                x: root.style.marginLeft ?? 0
                y: root.style.marginTop ?? 0
                implicitWidth: inlineFlow.naturalWidth +
                               (root.style.paddingLeft ?? 0) +
                               (root.style.paddingRight ?? 0)
                implicitHeight: inlineFlow.implicitHeight +
                                (root.style.paddingTop ?? 0) +
                                (root.style.paddingBottom ?? 0)
                width: implicitWidth
                height: implicitHeight

                color: root.style.backgroundColor || "transparent"
                radius: root.style.borderRadius ?? 0
                border.color: root.style.borderColor || "transparent"
                border.width: root.style.borderWidth ?? 0

                Flow {
                    id: inlineFlow

                    readonly property real naturalWidth:
                        root.inlineChildrenWidth(inlineFlow)

                    x: root.style.paddingLeft ?? 0
                    y: root.style.paddingTop ?? 0
                    width: Math.max(
                        0,
                        Math.min(
                            naturalWidth,
                            root.availableWidth -
                            root.horizontalMargin -
                            (root.style.paddingLeft ?? 0) -
                            (root.style.paddingRight ?? 0)))

                    Repeater {
                        model: root.node.children ?? []

                        delegate: StructuredContentChild {
                            required property var modelData

                            availableWidth: root.availableWidth
                            baseFont: root.baseFont
                            node: modelData
                            owner: root.owner
                            selectable: root.selectable
                        }
                    }
                }
            }
        }
    }

    Component {
        id: linkComponent

        Item {
            implicitWidth: linkBox.implicitWidth + root.horizontalMargin
            implicitHeight: linkBox.implicitHeight + root.verticalMargin

            Rectangle {
                id: linkBox

                x: root.style.marginLeft ?? 0
                y: root.style.marginTop ?? 0
                implicitWidth: linkFlow.naturalWidth +
                               (root.style.paddingLeft ?? 0) +
                               (root.style.paddingRight ?? 0)
                implicitHeight: linkFlow.implicitHeight +
                                (root.style.paddingTop ?? 0) +
                                (root.style.paddingBottom ?? 0)
                width: implicitWidth
                height: implicitHeight

                color: root.style.backgroundColor || "transparent"
                radius: root.style.borderRadius ?? 0
                border.color: root.style.borderColor || "transparent"
                border.width: root.style.borderWidth ?? 0

                Flow {
                    id: linkFlow

                    readonly property real naturalWidth:
                        root.inlineChildrenWidth(linkFlow)

                    x: root.style.paddingLeft ?? 0
                    y: root.style.paddingTop ?? 0
                    width: Math.max(
                        0,
                        Math.min(
                            naturalWidth,
                            root.availableWidth -
                            root.horizontalMargin -
                            (root.style.paddingLeft ?? 0) -
                            (root.style.paddingRight ?? 0)))

                    Repeater {
                        model: root.node.children ?? []

                        delegate: StructuredContentChild {
                            required property var modelData

                            availableWidth: root.availableWidth
                            baseFont: root.baseFont
                            node: modelData
                            owner: root.owner
                            selectable: root.selectable
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    z: 1
                    acceptedButtons: Qt.LeftButton
                    onClicked: function(event) {
                        root.owner.activateLink(
                            root.node.href ?? "",
                            linkBox,
                            event.x,
                            event.y);
                    }
                }
            }
        }
    }

    Component {
        id: detailsComponent

        Item {
            id: detailsBox

            property bool expanded: false
            readonly property var detailChildren: root.node.children ?? []
            readonly property var summaryNode:
                detailChildren.find(child => child.tag === "summary")
            readonly property var bodyNodes:
                detailChildren.filter(child => child.tag !== "summary")

            width: root.availableWidth
            implicitWidth: detailColumn.implicitWidth
            implicitHeight: detailColumn.implicitHeight

            Column {
                id: detailColumn

                width: parent.width

                Item {
                    id: summaryHeader

                    width: parent.width
                    implicitHeight: summaryRow.implicitHeight

                    Row {
                        id: summaryRow

                        spacing: 4

                        Text {
                            color: MementoPalette.text
                            font.family: root.baseFont.family
                            font.pointSize: root.baseFont.pointSize * 0.7
                            text: detailsBox.expanded ? "▾" : "▸"
                        }

                        StructuredContentChild {
                            visible: detailsBox.summaryNode !== undefined
                            availableWidth:
                                Math.max(0, summaryHeader.width - x)
                            baseFont: root.baseFont
                            node: detailsBox.summaryNode ?? ({
                                "id": -1,
                                "kind": "text",
                                "text": "",
                                "style": root.style
                            })
                            owner: root.owner
                            selectable: false
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked:
                            detailsBox.expanded = !detailsBox.expanded
                    }
                }

                Column {
                    width: parent.width

                    Repeater {
                        model: detailsBox.expanded ?
                            detailsBox.bodyNodes : []

                        delegate: StructuredContentChild {
                            required property var modelData

                            availableWidth: parent.width
                            baseFont: root.baseFont
                            fillWidth: true
                            node: modelData
                            owner: root.owner
                            selectable: root.selectable
                        }
                    }
                }
            }
        }
    }

    Component {
        id: summaryComponent

        Item {
            implicitWidth: summaryContent.item?.implicitWidth ?? 0
            implicitHeight: summaryContent.item?.implicitHeight ?? 0

            Loader {
                id: summaryContent

                sourceComponent:
                    root.node.layout === "flow" ?
                        naturalFlowContent : naturalColumnContent
            }
        }
    }

    Component {
        id: blockComponent

        Item {
            implicitWidth: blockBox.implicitWidth + root.horizontalMargin
            implicitHeight: blockBox.implicitHeight + root.verticalMargin

            Rectangle {
                id: blockBox

                x: root.style.marginLeft ?? 0
                y: root.style.marginTop ?? 0
                implicitWidth: blockContent.implicitWidth +
                               (root.style.paddingLeft ?? 0) +
                               (root.style.paddingRight ?? 0)
                implicitHeight: blockContent.implicitHeight +
                                (root.style.paddingTop ?? 0) +
                                (root.style.paddingBottom ?? 0)
                width: implicitWidth
                height: implicitHeight

                color: root.style.backgroundColor || "transparent"
                radius: root.style.borderRadius ?? 0
                border.color: root.style.borderColor || "transparent"
                border.width: root.style.leftBorderOnly ?
                                  0 : root.style.borderWidth ?? 0

                Rectangle {
                    visible: root.style.leftBorderOnly ?? false
                    width: root.style.borderWidth ?? 0
                    height: parent.height
                    color: root.style.borderColor || MementoPalette.text
                    radius: root.style.borderRadius ?? 0
                }

                Loader {
                    id: blockContent

                    x: root.style.paddingLeft ?? 0
                    y: root.style.paddingTop ?? 0
                    width: root.style.fitContent ?
                        Math.min(
                            implicitWidth,
                            Math.max(
                                0,
                                root.availableWidth -
                                root.horizontalMargin -
                                (root.style.paddingLeft ?? 0) -
                                (root.style.paddingRight ?? 0))) :
                        Math.max(
                            0,
                            root.availableWidth -
                            root.horizontalMargin -
                            (root.style.paddingLeft ?? 0) -
                            (root.style.paddingRight ?? 0))
                    sourceComponent: {
                        const useStyledFlow =
                            (root.node.styledText ?? "").length > 0;
                        if (root.style.fitContent)
                        {
                            return root.node.layout === "flow" ?
                                naturalFlowContent : naturalColumnContent;
                        }
                        if (root.node.layout === "flow")
                        {
                            return useStyledFlow ?
                                styledFlowContent : flowContent;
                        }
                        return columnContent;
                    }
                }
            }
        }
    }

    Component {
        id: flowContent

        Item {
            readonly property real rubyInset:
                root.rubyFlowInset(root.node.children ?? [])

            width: parent?.width ?? 0
            implicitWidth: contentFlow.implicitWidth
            implicitHeight: contentFlow.implicitHeight + rubyInset

            Flow {
                id: contentFlow

                y: parent.rubyInset
                width: parent.width

                Repeater {
                    model: root.node.children ?? []

                    delegate: StructuredContentChild {
                        required property var modelData

                        availableWidth: contentFlow.width
                        baseFont: root.baseFont
                        node: modelData
                        owner: root.owner
                        selectable: root.selectable
                    }
                }
            }
        }
    }

    Component {
        id: styledFlowContent

        Item {
            width: parent?.width ?? 0
            implicitWidth: styledText.width
            implicitHeight: styledText.implicitHeight

            StructuredContentText {
                id: styledText

                width: parent?.width ?? 0
                persistentSelection: root.owner.persistentSelection
                recursiveSearchOwner: root.owner
                selectionOwner: root.selectable ? root.owner : null
                selectionNodeId: root.node.id
                selectByKeyboard: true
                selectByMouse: false
                textFormat: TextEdit.RichText
                wrapMode: TextEdit.Wrap

                color: root.style.color || MementoPalette.text
                font.family: root.baseFont.family
                font.italic: root.style.italic ?? root.baseFont.italic
                font.underline:
                    root.style.underline ?? root.baseFont.underline
                font.overline:
                    root.style.overline ?? root.baseFont.overline
                font.strikeout:
                    root.style.strikeout ?? root.baseFont.strikeout
                font.pointSize:
                    root.baseFont.pointSize * (root.style.fontScale ?? 1)
                font.weight:
                    root.style.bold ? Font.Bold : root.baseFont.weight
                text: root.node.styledText ?? ""

                onSelectedTextChanged:
                {
                    if (root.selectable)
                    {
                        root.owner.updateSelection(
                            root.node.id, selectedText);
                    }
                }

                Component.onCompleted:
                {
                    if (root.selectable)
                    {
                        root.owner.registerSelectableText(
                            root.node.id, styledText);
                    }
                }

                Component.onDestruction:
                {
                    if (root.selectable)
                    {
                        root.owner.unregisterSelectableText(
                            root.node.id, styledText);
                        root.owner.updateSelection(root.node.id, "");
                    }
                }
            }
        }
    }

    Component {
        id: columnContent

        Column {
            width: parent?.width ?? 0

            Repeater {
                model: root.node.children ?? []

                delegate: StructuredContentChild {
                    required property var modelData

                    availableWidth: parent.width
                    baseFont: root.baseFont
                    fillWidth: true
                    node: modelData
                    owner: root.owner
                    selectable: root.selectable
                }
            }
        }
    }

    Component {
        id: listComponent

        Column {
            width: root.availableWidth

            Repeater {
                model: root.node.children ?? []

                delegate: StructuredContentChild {
                    required property var modelData

                    availableWidth: parent.width
                    baseFont: root.baseFont
                    fillWidth: true
                    node: modelData
                    owner: root.owner
                    selectable: root.selectable
                }
            }
        }
    }

    Component {
        id: listItemComponent

        Row {
            spacing: 6
            width: root.availableWidth

            Text {
                color: root.style.color || MementoPalette.text
                font.family: root.baseFont.family
                font.pointSize:
                    root.baseFont.pointSize * (root.style.fontScale ?? 1)
                text: root.node.marker ?? "•"
            }

            Loader {
                width: Math.max(0, parent.width - x)
                sourceComponent: {
                    const useStyledFlow =
                        (root.node.styledText ?? "").length > 0;
                    if (root.node.layout === "flow")
                    {
                        return useStyledFlow ?
                            styledFlowContent : flowContent;
                    }
                    return columnContent;
                }
            }
        }
    }

    Component {
        id: imageComponent

        Column {
            width: root.availableWidth

            Image {
                id: image

                readonly property real requestedWidth: {
                    if ((root.node.width ?? -1) <= 0)
                    {
                        return implicitWidth;
                    }
                    return root.node.width;
                }

                readonly property real requestedHeight: {
                    if ((root.node.height ?? -1) <= 0)
                    {
                        return implicitHeight;
                    }
                    return root.node.height;
                }

                source: root.node.source ?? ""
                width: Math.min(requestedWidth, 350, root.availableWidth)
                height: requestedWidth > 0 ?
                            requestedHeight * width / requestedWidth :
                            requestedHeight
                fillMode: Image.PreserveAspectFit
            }

            StructuredContentChild {
                visible: (root.node.description ?? "").length > 0
                availableWidth: parent.width
                baseFont: root.baseFont
                fillWidth: true
                node: ({
                    "id": -1,
                    "kind": "text",
                    "text": root.node.description ?? "",
                    "style": root.style
                })
                owner: root.owner
                selectable: root.selectable
            }
        }
    }

    Component {
        id: rubyComponent

        Item {
            readonly property real rubyInset:
                Math.max(
                    rubyText.implicitHeight,
                    root.rubyFlowInset([root.node]))
            readonly property real naturalWidth:
                Math.max(rubyText.implicitWidth, rubyBase.implicitWidth)

            implicitWidth: naturalWidth
            implicitHeight: rubyBase.implicitHeight + rubyInset

            Row {
                id: rubyText

                y: -height
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: (root.node.children ?? [])
                        .filter(child => child.tag === "rt")

                    delegate: StructuredContentChild {
                        required property var modelData

                        availableWidth: root.availableWidth
                        baseFont: root.baseFont
                        node: modelData
                        owner: root.owner
                        selectable: root.selectable
                    }
                }
            }

            Row {
                id: rubyBase

                y: 0
                anchors.horizontalCenter: parent.horizontalCenter

                Repeater {
                    model: (root.node.children ?? [])
                        .filter(child => child.tag !== "rt")

                    delegate: StructuredContentChild {
                        required property var modelData

                        availableWidth: root.availableWidth
                        baseFont: root.baseFont
                        node: modelData
                        owner: root.owner
                        selectable: root.selectable
                    }
                }
            }
        }
    }

    Component {
        id: tableComponent

        Item {
            id: table

            property var columnWidths: []
            property var rowHeights: []
            property real measuredWidth: 0
            property real measuredHeight: 0

            readonly property int columnCount:
                Math.max(1, root.node.columnCount ?? 1)
            readonly property int rowCount:
                Math.max(1, root.node.rowCount ?? 1)

            implicitWidth: measuredWidth
            implicitHeight: measuredHeight

            /**
             * Recompute shared table metrics from each cell's preferred size.
             */
            function updateMetrics() {
                const widths = new Array(table.columnCount).fill(0);
                const heights = new Array(table.rowCount).fill(0);
                for (let i = 0; i < cellRepeater.count; ++i)
                {
                    const cell = cellRepeater.itemAt(i);
                    if (!cell)
                    {
                        continue;
                    }
                    const columnSpan = cell.modelData.columnSpan ?? 1;
                    const rowSpan = cell.modelData.rowSpan ?? 1;
                    widths[cell.modelData.column] = Math.max(
                        widths[cell.modelData.column],
                        cell.preferredWidth / columnSpan);
                    heights[cell.modelData.row] = Math.max(
                        heights[cell.modelData.row],
                        cell.preferredHeight / rowSpan);
                }
                table.columnWidths = widths;
                table.rowHeights = heights;
                table.measuredWidth =
                    widths.reduce((sum, width) => sum + width, 0);
                table.measuredHeight =
                    heights.reduce((sum, height) => sum + height, 0);
            }

            /**
             * Get the shared width required by one table column.
             * @param column The zero-based table column.
             */
            function columnWidth(column) {
                return table.columnWidths[column] ?? 0;
            }

            /**
             * Get the shared height required by one table row.
             * @param row The zero-based table row.
             */
            function rowHeight(row) {
                return table.rowHeights[row] ?? 0;
            }

            /**
             * Get the x coordinate for one table column.
             * @param column The zero-based table column.
             */
            function columnX(column) {
                let x = 0;
                for (let i = 0; i < column; ++i)
                {
                    x += table.columnWidth(i);
                }
                return x;
            }

            /**
             * Get the y coordinate for one table row.
             * @param row The zero-based table row.
             */
            function rowY(row) {
                let y = 0;
                for (let i = 0; i < row; ++i)
                {
                    y += table.rowHeight(i);
                }
                return y;
            }

            /**
             * Get the width occupied by a cell spanning table columns.
             * @param column The first zero-based table column.
             * @param span The number of occupied columns.
             */
            function spanWidth(column, span) {
                let width = 0;
                for (let i = column; i < column + span; ++i)
                {
                    width += table.columnWidth(i);
                }
                return width;
            }

            /**
             * Get the height occupied by a cell spanning table rows.
             * @param row The first zero-based table row.
             * @param span The number of occupied rows.
             */
            function spanHeight(row, span) {
                let height = 0;
                for (let i = row; i < row + span; ++i)
                {
                    height += table.rowHeight(i);
                }
                return height;
            }

            Repeater {
                id: cellRepeater

                model: root.node.cells ?? []
                onItemAdded: Qt.callLater(table.updateMetrics)
                onItemRemoved: Qt.callLater(table.updateMetrics)

                delegate: Rectangle {
                    required property var modelData

                    readonly property real preferredWidth:
                        cellContent.implicitWidth + 10
                    readonly property real preferredHeight:
                        cellContent.implicitHeight + 10

                    x: table.columnX(modelData.column)
                    y: table.rowY(modelData.row)
                    width: table.spanWidth(
                        modelData.column,
                        modelData.columnSpan ?? 1)
                    height: table.spanHeight(
                        modelData.row,
                        modelData.rowSpan ?? 1)
                    implicitWidth: preferredWidth
                    implicitHeight: preferredHeight
                    color: modelData.style.backgroundColor || "transparent"
                    border.color:
                        modelData.style.borderColor || MementoPalette.text
                    border.width: Math.max(
                        1,
                        modelData.style.borderWidth ?? 0)

                    StructuredContentChild {
                        id: cellContent

                        readonly property string horizontalAlignment:
                            modelData.style.textAlign || "center"

                        anchors.verticalCenter: parent.verticalCenter
                        x: {
                            if (horizontalAlignment === "left")
                            {
                                return 5;
                            }
                            if (horizontalAlignment === "right")
                            {
                                return parent.width - width - 5;
                            }
                            return (parent.width - width) / 2;
                        }
                        availableWidth: root.availableWidth
                        baseFont: root.baseFont
                        node: modelData
                        owner: root.owner
                        selectable: root.selectable
                    }

                    onPreferredWidthChanged:
                        Qt.callLater(table.updateMetrics)
                    onPreferredHeightChanged:
                        Qt.callLater(table.updateMetrics)
                }
            }

            Component.onCompleted: Qt.callLater(table.updateMetrics)
        }
    }

    Component {
        id: cellComponent

        Item {
            implicitWidth: cellLoader.item?.implicitWidth ?? 0
            implicitHeight: cellLoader.item?.implicitHeight ?? 0

            Loader {
                id: cellLoader

                sourceComponent:
                    root.node.layout === "flow" ?
                        naturalFlowContent : naturalColumnContent
            }
        }
    }

    Component {
        id: naturalFlowContent

        Item {
            readonly property real naturalWidth:
                root.inlineChildrenWidth(naturalFlow)
            readonly property real rubyInset:
                root.rubyFlowInset(root.node.children ?? [])

            implicitWidth: naturalWidth
            implicitHeight: naturalFlow.implicitHeight + rubyInset

            Flow {
                id: naturalFlow

                y: parent.rubyInset
                width: Math.min(parent.naturalWidth, root.availableWidth)

                Repeater {
                    model: root.node.children ?? []

                    delegate: StructuredContentChild {
                        required property var modelData

                        availableWidth: root.availableWidth
                        baseFont: root.baseFont
                        node: modelData
                        owner: root.owner
                        selectable: root.selectable
                    }
                }
            }
        }
    }

    Component {
        id: naturalColumnContent

        Item {
            readonly property real naturalWidth:
                root.columnChildrenWidth(naturalColumn)

            implicitWidth: naturalWidth
            implicitHeight: naturalColumn.implicitHeight

            Column {
                id: naturalColumn

                width: parent.naturalWidth

                Repeater {
                    model: root.node.children ?? []

                    delegate: StructuredContentChild {
                        required property var modelData

                        availableWidth: root.availableWidth
                        baseFont: root.baseFont
                        node: modelData
                        owner: root.owner
                        selectable: root.selectable
                    }
                }
            }
        }
    }
}
