pragma Singleton

import QtQml

QtObject {
    id: root

    property int nextRequestId: 0

    signal resolved(requestId: int, audioSources: var)
    signal failed(requestId: int)

    /**
     * Request a remote audio source.
     * Emits a resolved or failed signal on completion.
     * @param url The URL of the JSON audio source.
     * @return A unique requestId that should be compared to the resolved and
     * failed signal.
     */
    function request(url) {
        const requestId = ++root.nextRequestId;
        const request = new XMLHttpRequest();

        request.onreadystatechange = function() {
            if (request.readyState === XMLHttpRequest.UNSENT)
            {
                return;
            }
            else if (request.readyState !== XMLHttpRequest.DONE)
            {
                return;
            }

            if (request.status === 0)
            {
                root.failed(requestId);
                return;
            }

            let response = null;
            try
            {
                response = JSON.parse(request.responseText);
            }
            catch (error)
            {
                root.failed(requestId);
                return;
            }

            if (response.type !== "audioSourceList" ||
                    !Array.isArray(response.audioSources))
            {
                root.failed(requestId);
                return;
            }

            root.resolved(requestId, response.audioSources);
        };

        request.open("GET", url);
        request.send();
        return requestId;
    }
}
