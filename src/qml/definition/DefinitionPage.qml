import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Page {
    id: root

    required property DictionarySearch search

    /* true to show the toolbar in top-level searches */
    property bool showToolbar: false

    /* true if the root item needs to be discarded, false if it can be reused */
    property bool rootRefreshNeeded: false

    signal closePressed()

    Action {
        id: addAnkiAction
        enabled: AnkiConfig.enabled &&
                 stackView.currentItem &&
                 (stackView.currentItem.count > 0 ||
                  stackView.currentItem.kanji)
        shortcut: MementoSettings.keybinds.profile?.cardAdd
        onTriggered: stackView.currentItem.addNote()
    }

    Connections {
        target: root.search

        function onKanjiChanged() {
            root.resetStack();
        }

        function onTermsChanged() {
            root.resetStack();
        }
    }

    /**
     * Push the root result list onto an empty stack.
     */
    function pushRootSearch() {
        stackView.push(definitionListComponent, {
            dictionarySearch: root.search,
            canvasColor: root.palette.window
        }, StackView.Immediate);
    }

    /**
     * Discard all pages and create a fresh root result list.
     */
    function refreshRootSearch() {
        stackView.clear(StackView.Immediate);
        root.pushRootSearch();
        root.rootRefreshNeeded = false;
    }

    /**
     * Pop recursive result pages while keeping the root result list alive when
     * it is safe to reuse.
     */
    function resetStack() {
        if (stackView.depth <= 0)
        {
            root.refreshRootSearch();
            return;
        }

        const rootItem = stackView.get(0);
        if (!rootItem || root.rootRefreshNeeded)
        {
            root.refreshRootSearch();
            return;
        }

        if (stackView.depth > 1)
        {
            stackView.pop(rootItem, StackView.Immediate);
        }
    }

    /**
     * Create a search object for a recursive search result page.
     * @return A new search object, or null on failure.
     */
    function createRecursiveSearch() {
        const search = dictionarySearchComponent.createObject(stackView);
        if (!search)
        {
            console.error("Could not create recursive dictionary search");
        }
        return search;
    }

    /**
     * Add a recursive search result list to the stack and transfer ownership
     * of the search object to the result list.
     * @param search The search object that owns the recursive results.
     */
    function pushSearch(search) {
        root.rootRefreshNeeded = true;
        stackView.push(definitionListComponent, {
            dictionarySearch: search,
            ownsDictionarySearch: true
        });
    }

    /**
     * Search for a term and push its recursive result page.
     * @param query The term to search for.
     */
    function pushTermSearch(query) {
        const search = root.createRecursiveSearch();
        if (!search)
        {
            return;
        }

        search.searchTerms(query, query, 0);
        if (query.length > 0)
        {
            search.searchKanji(query[0], query[0], 0);
        }
        root.pushSearch(search);
    }

    /**
     * Search for a kanji and push its recursive result page.
     * @param expression The text containing the kanji.
     * @param index The index of the kanji to search.
     */
    function pushKanjiSearch(expression, index) {
        if (index < 0 || index >= expression.length)
        {
            return;
        }

        const search = root.createRecursiveSearch();
        if (!search)
        {
            return;
        }

        search.searchKanji(expression[index], expression, index);
        root.pushSearch(search);
    }

    header: ColumnLayout {
        spacing: 0

        ToolBar {
            id: toolbar
            Layout.fillWidth: true
            visible: root.showToolbar || stackView.depth > 1

            RowLayout {
                anchors.fill: parent

                ToolButton {
                    id: buttonBack
                    focusPolicy: Qt.NoFocus
                    visible: stackView.depth > 1
                    icon.name: MementoSettings.interfaceSystemIcons ? "go-previous" : null
                    icon.source: Utils.toImageProvider("arrow-back", MementoPalette.text)
                    onClicked: stackView.pop()
                }

                Item {
                    Layout.fillWidth: true
                }

                ToolButton {
                    id: buttonClose
                    focusPolicy: Qt.NoFocus
                    icon.name: MementoSettings.interfaceSystemIcons ? "window-close" : null
                    icon.source: Utils.toImageProvider("close", MementoPalette.text)
                    onClicked: root.closePressed()
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            visible: !Features.isUnix && toolbar.visible
            implicitHeight: 1
            color: MementoPalette.border
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent
        clip: true
        Component.onCompleted: root.pushRootSearch()
    }

    Component {
        id: definitionListComponent

        DefinitionList {
            canvasColor: root.palette.window
            onRecursiveTermSearchRequested: function(query) {
                root.pushTermSearch(query);
            }
            onRecursiveKanjiSearchRequested: function(expression, index) {
                root.pushKanjiSearch(expression, index);
            }
        }
    }

    Component {
        id: dictionarySearchComponent
        DictionarySearch { }
    }
}
