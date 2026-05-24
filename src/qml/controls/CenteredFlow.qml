import QtQuick

Flow {
    id: root

    function centerRows() {
        const rows = [];
        const children = root.visibleChildren;

        for (const child of children)
        {
            if (!child.visible)
            {
                continue;
            }

            let row = rows.find((candidate) => Math.abs(candidate.y - child.y) < 0.5);
            if (!row)
            {
                row = {
                    y: child.y,
                    height: child.height,
                    children: []
                };
                rows.push(row);
            }

            row.height = Math.max(row.height, child.height);
            row.children.push(child);
        }

        for (const row of rows)
        {
            for (const child of row.children)
            {
                child.y = row.y + (row.height - child.height) / 2;
            }
        }
    }

    onPositioningComplete: centerRows()
}
