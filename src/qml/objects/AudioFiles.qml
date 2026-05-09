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

    /* The current active XHR request, null if nothing is active */
    property var activeRequest: null

    Component.onDestruction: {
        if (root.activeRequest)
        {
            root.activeRequest.abort();
        }
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
        if (root.activeRequest)
        {
            root.activeRequest.abort();
        }
        root.files.clear();
        root.loadState = AudioFiles.LoadState.Unloaded;
    }

    /**
     * Loads files into the files property.
     * @param expression The expression of the term.
     * @param reading The reading of the term.
     */
    function load(expression, reading) {
        if (root.loadState !== AudioFiles.Unloaded)
        {
            root.clear();
        }
        root.loadState = AudioFiles.LoadState.Loading;
        root.loadHelper(expression, reading, 0);
    }

    /**
     * Load the audio source at the index and add it to files.
     * @param expression The expression of the term.
     * @param reading The reading of the term.
     * @param index The index of the audio source to add.
     */
    function loadHelper(expression, reading, index) {
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
            root.loadHelper(expression, reading, index + 1);
            break;

        case MementoSetting.AudioSourceTypeJson:
            root.requestJsonSource(
                url,
                function(audioSources)
                {
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
                            "skipHash": skipHash,
                            "exists": true
                        });
                    }
                    root.loadHelper(expression, reading, index + 1);
                },
                function()
                {
                    root.loadHelper(expression, reading, index + 1);
                }
            );
            break;

        default:
            root.loadHelper(expression, reading, index + 1);
            return;
        }
    }

    /**
     * Requests a JSON audio source and gets the source list.
     * @param url The URL of the JSON source.
     * @param callback The function to callback with the audio source array.
     * @param errorCallback The function to callback on an error.
     */
    function requestJsonSource(url, callback, errorCallback) {
        let request = new XMLHttpRequest();
        root.activeRequest = request;

        request.onreadystatechange = function () {
            if (request.readyState === XMLHttpRequest.UNSENT)
            {
                /* This request has been aborted, don't continue. */
                return;
            }
            else if (request.readyState !== XMLHttpRequest.DONE)
            {
                return;
            }
            root.activeRequest = null;

            if (request.status === 0)
            {
                errorCallback();
                return;
            }
            const response = JSON.parse(request.responseText);
            if (response.type !== "audioSourceList")
            {
                errorCallback();
                return;
            }
            else if (!Array.isArray(response.audioSources))
            {
                errorCallback();
                return;
            }

            callback(response.audioSources);
        };

        request.open("GET", url);
        request.send();
    }
}
