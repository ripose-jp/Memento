import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import Ripose.Memento

Dialog {
    id: root
    parent: Overlay.overlay
    anchors.centerIn: parent
    modal: true
    standardButtons: Dialog.Ok

    Label {
        id: messageLabel
        textFormat: Text.RichText
        onLinkActivated: (link) => Qt.openUrlExternally(link)
    }

    /**
     * Check for updates and open the dialog if there is one available.
     * @param silentFailure true to show error states or up to date, false
     * otherwise.
     */
    function check(silentFailure) {
        const MEMENTO_GITHUB_API_URL = "https://api.github.com/repos/ripose-jp/memento/releases/latest";
        const MEMENTO_GITHUB_URL = "https://github.com/ripose-jp/Memento/releases/latest";
        const FAILURE_TITLE = qsTr("Update Check Failed");
        const FAILURE_MESSAGE = qsTr(
            "<p>The GitHub API did not return a valid reply.</p>" +
            "<p>Check for updates manually <a href=\"%1\">here</a>."
        ).arg(MEMENTO_GITHUB_URL);

        let xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function () {
            if (xhr.readyState !== XMLHttpRequest.DONE)
            {
                return;
            }

            if (xhr.status !== 200)
            {
                root.title = FAILURE_TITLE;
                messageLabel.text = FAILURE_MESSAGE;
                if (!silentFailure)
                {
                    root.open();
                }
                return;
            }

            let response = JSON.parse(xhr.responseText);
            if (!response || !response.hasOwnProperty("tag_name"))
            {
                root.title = FAILURE_TITLE;
                messageLabel.text = FAILURE_MESSAGE;
                if (!silentFailure)
                {
                    root.open();
                }
                return;
            }

            if (response.tag_name === `v${Features.version}`)
            {
                root.title = qsTr("Update to Date");
                messageLabel.text = qsTr("<p>You're on the latest version.</p>");
                if (!silentFailure)
                {
                    root.open();
                }
                return;
            }

            root.title = qsTr("New Version Available")
            messageLabel.text = qsTr(
                "<p>Version <a href=\"%1\">%2</a> is available.</p>"
            ).arg(MEMENTO_GITHUB_URL).arg(response.tag_name);
            root.open();
        };

        xhr.open("GET", MEMENTO_GITHUB_API_URL);
        xhr.send();
    }
}
