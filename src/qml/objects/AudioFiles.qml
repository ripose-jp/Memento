import QtQml
import QtQml.Models
import Ripose.Memento

QtObject {
    id: root

    required property Term term

    enum LoadState {
        Unloaded = 0,
        Loading = 1,
        Loaded = 2
    }

    readonly property ListModel files: ListModel {}

    property int loadState: AudioFiles.LoadState.Unloaded

    /* Incremented whenever outstanding async loads should be ignored. */
    property int loadToken: 0

    /* Properties describing in flight requests */
    property int activeJsonRequestId: 0
    property int pendingIndex: -1
    property int pendingToken: 0
    property string pendingExpression
    property string pendingReading
    property string pendingSkipHash

    Component.onDestruction: {
        ++root.loadToken;
        root.activeJsonRequestId = 0;
    }

    onTermChanged: {
        root.clear();
        if (root.term)
        {
            root.load(root.term.expression, root.term.reading);
        }
    }

    /**
     * Clears all audio files.
     */
    function clear() {
        ++root.loadToken;
        root.activeJsonRequestId = 0;
        root.files.clear();
        root.loadState = AudioFiles.LoadState.Unloaded;
    }

    /**
     * Loads files into the files property.
     * @param expression The expression of the term.
     * @param reading The reading of the term.
     */
    function load(expression, reading) {
        if (root.loadState !== AudioFiles.LoadState.Unloaded)
        {
            root.clear();
        }
        root.loadState = AudioFiles.LoadState.Loading;
        root.loadHelper(expression, reading, 0, root.loadToken);
    }

    /**
     * Load the audio source at the index and add it to files.
     * @param expression The expression of the term.
     * @param reading The reading of the term.
     * @param index The index of the audio source to add.
     */
    function loadHelper(expression, reading, index, token) {
        if (token !== root.loadToken)
        {
            return;
        }
        if (index >= MementoSettings.audioSources.rowCount())
        {
            root.loadState = AudioFiles.LoadState.Loaded;
            return;
        }

        const modelIndex = MementoSettings.audioSources.index(index, 0);
        let name = MementoSettings.audioSources.data(modelIndex, AudioSourceModel.NameRole);
        let url = MementoSettings.audioSources.data(modelIndex, AudioSourceModel.UrlRole);
        let type = MementoSettings.audioSources.data(modelIndex, AudioSourceModel.TypeRole);
        let skipHash = MementoSettings.audioSources.data(modelIndex, AudioSourceModel.SkipHashRole);

        url = url.replace(/{expression}/g, expression)
            .replace(/{reading}/g, reading || expression)
            .replace(/{term}/g, expression);

        switch (type)
        {
        case MementoSetting.AudioSourceTypeFile:
            root.files.append({
                "name": name,
                "url": url,
                "skipHash": skipHash,
                "exists": true
            });
            root.loadHelper(expression, reading, index + 1, token);
            break;

        case MementoSetting.AudioSourceTypeJson:
            root.pendingExpression = expression;
            root.pendingReading = reading;
            root.pendingIndex = index;
            root.pendingSkipHash = skipHash;
            root.pendingToken = token;
            root.activeJsonRequestId = AudioSourceResolver.request(url);
            break;

        default:
            root.loadHelper(expression, reading, index + 1, token);
            return;
        }
    }

    /**
     * Handles a successful JSON source loaded and loads the next one if
     * necessary.
     * @param requestId The unique ID of the audio source request.
     * @param audioSources The JSON audio source object.
     */
    function continueJsonLoad(requestId, audioSources) {
        if (requestId !== root.activeJsonRequestId ||
                root.pendingToken !== root.loadToken)
        {
            return;
        }
        root.activeJsonRequestId = 0;

        for (let i = 0; i < audioSources.length; ++i)
        {
            if (!audioSources[i].name)
            {
                continue;
            }
            else if (!audioSources[i].url)
            {
                continue;
            }
            root.files.append({
                "name": audioSources[i].name,
                "url": audioSources[i].url,
                "skipHash": root.pendingSkipHash,
                "exists": true
            });
        }
        root.loadHelper(
            root.pendingExpression,
            root.pendingReading,
            root.pendingIndex + 1,
            root.pendingToken
        );
    }

    /**
     * Handles an error and continues loading JSON audio sources.
     * @param requestId The unique ID of the audio source request.
     */
    function continueJsonLoadAfterError(requestId) {
        if (requestId !== root.activeJsonRequestId ||
                root.pendingToken !== root.loadToken)
        {
            return;
        }
        root.activeJsonRequestId = 0;
        root.loadHelper(
            root.pendingExpression,
            root.pendingReading,
            root.pendingIndex + 1,
            root.pendingToken
        );
    }

    readonly property Connections resolverConnections: Connections {
        target: AudioSourceResolver

        function onResolved(requestId, audioSources) {
            root.continueJsonLoad(requestId, audioSources);
        }

        function onFailed(requestId) {
            root.continueJsonLoadAfterError(requestId);
        }
    }
}
