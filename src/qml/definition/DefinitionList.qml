import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

ListView {
    id: root

    required property DictionarySearch dictionarySearch
    property color canvasColor: MementoPalette.window

    readonly property var terms: dictionarySearch.terms
    readonly property Kanji kanji: dictionarySearch.kanji
    property bool ownsDictionarySearch: false

    signal recursiveTermSearchRequested(query: string)
    signal recursiveKanjiSearchRequested(expression: string, index: int)

    /**
     * Add the current entry to Anki.
     */
    function addNote() {
        let index = root.indexAt(0, root.contentY);
        if (index === -1)
        {
            if (root.footerItem)
            {
                root.footerItem.addKanji();
            }
        }
        else
        {
            let termEntry = root.itemAt(0, root.contentY);
            if (termEntry)
            {
                termEntry.addTerm();
            }
        }
    }

    /**
     * Scroll to the next result.
     */
    function scrollToNext() {
        let index = root.indexAt(0, root.contentY);
        if (index >= 0)
        {
            root.scrollTo(index + 1);
        }
    }

    /**
     * Scroll to the previous result.
     */
    function scrollToPrevious() {
        let currentIndex = root.indexAt(0, root.contentY);
        let previousIndex = root.indexAt(0, root.contentY - 1);

        if (currentIndex === 0)
        {
            previousIndex = 0;
        }
        else if (currentIndex === -1)
        {
            currentIndex = root.count;
        }

        if (previousIndex === -1)
        {
            previousIndex = root.count;
        }

        if (currentIndex === previousIndex)
        {
            root.scrollTo(currentIndex);
        }
        else
        {
            root.scrollTo(currentIndex - 1);
        }
    }

    /**
     * Scroll to the given index.
     * @param index The index of the item to scroll to.
     * Indices less than 0 scroll to the beginning.
     * Indices greater than count scroll to the footer.
     */
    function scrollTo(index) {
        if (index < 0)
        {
            root.positionViewAtBeginning();
        }
        else if (index >= root.count)
        {
            if (root.kanji)
            {
                root.positionViewAtEnd();
                if (root.footerItem && root.footerItem.height > root.height)
                {
                    root.contentY -= root.footerItem.height - root.height;
                }
            }
        }
        else
        {
            root.positionViewAtIndex(index, ListView.Beginning);
        }
    }

    /**
     * Searches for a term and requests a recursive result page.
     * @param query The term to search for.
     */
    function searchTermRecursively(query) {
        if (query.length <= 0)
        {
            return;
        }
        root.recursiveTermSearchRequested(query);
    }

    /**
     * Searches for a kanji and requests a recursive result page.
     * @param expression The text containing the kanji.
     * @param index The index of the kanji to search.
     */
    function searchKanjiRecursively(expression, index) {
        if (index < 0 || index >= expression.length)
        {
            return;
        }
        root.recursiveKanjiSearchRequested(expression, index);
    }

    Action {
        id: nextAction
        enabled: root.visible && (root.count > 0 || root.kanji)
        shortcut: MementoSettings.keybinds.profile?.cardNext
        onTriggered: root.scrollToNext()
    }

    Action {
        id: previousAction
        enabled: root.visible && (root.count > 0 || root.kanji)
        shortcut: MementoSettings.keybinds.profile?.cardPrevious
        onTriggered: root.scrollToPrevious()
    }

    cacheBuffer: 1000
    reuseItems: true
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: ScrollBar {
        id: scrollBarVertical
        policy: ScrollBar.AlwaysOn
    }

    model: root.terms
    delegate: ColumnLayout {
        id: termLayout

        required property int index

        /**
         * Add this term to Anki.
         */
        function addTerm() {
            termEntry.addTerm();
        }

        width: scrollBarVertical.visible ?
                   ListView.view.width - scrollBarVertical.width :
                   ListView.view.width

        TermEntry {
            id: termEntry
            Layout.fillWidth: true
            Layout.margins: 10
            term: termLayout.index < root.terms.length ? root.terms[termLayout.index] : null
            canvasColor: root.canvasColor

            onSearchRequested: function(query) {
                root.searchTermRecursively(query);
            }

            onKanjiClicked: function(expression, index) {
                root.searchKanjiRecursively(expression, index);
            }
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 1
            color: MementoPalette.border
            visible: termLayout.index < root.count - 1 || root.kanji
        }
    }

    footer: KanjiEntry {
        id: kanjiEntry
        width: scrollBarVertical.visible ?
                   ListView.view.width - scrollBarVertical.width :
                   ListView.view.width
        height: kanjiEntry.visible ? kanjiEntry.implicitHeight : 0
        visible: root.kanji
        kanji: root.kanji
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.ForwardButton | Qt.BackButton
        onClicked: function(event) {
            if (event.button === Qt.ForwardButton)
            {
                root.scrollToPrevious();
                event.accepted = true;
            }
            else if (event.button === Qt.BackButton)
            {
                root.scrollToNext();
                event.accepted = true;
            }
            else
            {
                event.accepted = false;
            }
        }
    }

    StackView.onRemoved: {
        if (root.ownsDictionarySearch && root.dictionarySearch)
        {
            root.dictionarySearch.clearResults();
            root.dictionarySearch.destroy();
        }
    }
}
