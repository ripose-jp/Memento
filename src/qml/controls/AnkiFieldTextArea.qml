import QtQuick
import QtQuick.Controls
import Ripose.Memento

TextArea {
    id: root

    property var markers: []
    property var filteredSuggestions: []
    property int suggestionIndex: 0

    /**
     * Autocomplete the current word with the given value.
     * @param value The value to autocomplete with.
     */
    function insertText(value) {
        let pos = root.cursorPosition;
        let markerStart = Math.max(
            root.text.lastIndexOf('{', pos - 1),
            root.text.lastIndexOf('|', pos - 1)
        );
        let argumentStart = Math.max(
            root.text.lastIndexOf(':', pos - 1),
            root.text.lastIndexOf(',', pos - 1)
        );
        let start = Math.max(markerStart, argumentStart) + 1;
        while (start < pos && /\s/.test(root.text.charAt(start)))
        {
            ++start;
        }
        root.remove(start, pos);
        root.insert(start, value);
        suggestionPopup.close();
    }

    /**
     * Filter markers using the starting index of the marker.
     * @param startIndex The index of the last { or | character.
     * @param pos The position of the cursor.
     */
    function filterMarkers(startIndex, pos) {
        ++startIndex;
        while (startIndex < pos && /\s/.test(root.text.charAt(startIndex)))
        {
            ++startIndex;
        }
        let searchKey = root.text.substring(startIndex, pos);
        root.filteredSuggestions = root.markers
            .filter(marker => marker.name.startsWith(searchKey))
            .map(marker => marker.name)
            .sort();
    }

    /**
     * Filter marker arguments using the starting index of the marker.
     * @param startIndex The index of the last { or | character.
     * @param lastColon The index of the last : character.
     * @param pos The position of the cursor.
     */
    function filterArguments(markerStart, lastColon, pos) {
        let lastComma = root.text.lastIndexOf(',', pos - 1);
        let startIndex = Math.max(lastColon, lastComma);
        let lastEquals = root.text.lastIndexOf('=', pos - 1);
        if (startIndex < lastEquals)
        {
            /* We're in the value section */
            root.filteredSuggestions = [];
            return;
        }
        ++startIndex;
        while (startIndex < pos && /\s/.test(root.text.charAt(startIndex)))
        {
            ++startIndex;
        }
        let searchKey = root.text.substring(startIndex, pos);

        let markerName = root.text.substring(markerStart + 1, lastColon).trim();
        let marker = root.markers.find(marker => marker.name === markerName);
        if (!marker)
        {
            root.filteredSuggestions = [];
            return;
        }
        root.filteredSuggestions = marker.arguments
            .filter(arg => arg.name.includes(searchKey))
            .map(arg => arg.name)
            .sort();
    }

    onTextChanged: {
        let pos = root.cursorPosition;
        let lastOpen = root.text.lastIndexOf('{', pos - 1);
        let lastPipe = root.text.lastIndexOf('|', pos - 1);
        let startIndex = Math.max(lastOpen, lastPipe);
        if (startIndex < 0)
        {
            /* Nothing has been typed */
            suggestionPopup.close();
            return;
        }
        let lastClose = root.text.lastIndexOf('}', pos - 1);
        if (startIndex < lastClose)
        {
            /* We're not inside a marker */
            suggestionPopup.close();
            return;
        }
        let lastColon = root.text.lastIndexOf(':', pos - 1);
        if (startIndex > lastColon)
        {
            root.filterMarkers(startIndex, pos);
        }
        else
        {
            root.filterArguments(startIndex, lastColon, pos);
        }

        root.suggestionIndex = 0;
        if (root.filteredSuggestions.length > 0)
        {
            suggestionPopup.open();
        }
        else
        {
            suggestionPopup.close();
        }
    }

    Keys.onPressed: function(event) {
        switch (event.key)
        {
        case Qt.Key_BraceLeft:
            if (root.selectionStart !== root.selectionEnd)
            {
                root.remove(root.selectionStart, root.selectionEnd);
            }

            let pos = root.cursorPosition;
            root.insert(pos, "{}");
            root.cursorPosition = pos + 1;
            root.textChanged();
            event.accepted = true;
            break;

        case Qt.Key_Up:
            if (suggestionPopup.visible)
            {
                root.suggestionIndex = Math.max(0, root.suggestionIndex - 1);
                event.accepted = true;
            }
            break;

        case Qt.Key_Down:
            if (suggestionPopup.visible)
            {
                root.suggestionIndex =
                        Math.min(root.filteredSuggestions.length - 1, root.suggestionIndex + 1);
                event.accepted = true;
            }
            break;

        case Qt.Key_Enter:
        case Qt.Key_Return:
            if (suggestionPopup.visible)
            {
                root.insertText(root.filteredSuggestions[root.suggestionIndex]);
                event.accepted = true;
            }
            break;

        case Qt.Key_Escape:
            if (suggestionPopup.visible)
            {
                suggestionPopup.close();
                event.acceptd = true;
            }
            break;
        }
    }

    Popup {
        id: suggestionPopup
        x: root.cursorRectangle.x
        y: root.cursorRectangle.y + root.cursorRectangle.height
        width: 250
        height: Math.min(150, suggestionList.contentHeight) +
                suggestionPopup.topPadding +
                suggestionPopup.bottomPadding +
                suggestionPopup.topInset +
                suggestionPopup.bottomInset

        contentItem: ListView {
            id: suggestionList
            clip: true
            model: root.filteredSuggestions
            currentIndex: root.suggestionIndex
            delegate: ItemDelegate {
                required property int index
                required property string modelData

                width: ListView.view.width
                text: modelData
                highlighted: suggestionList.currentIndex === index
                onClicked: root.insertText(modelData);
            }
        }
    }
}