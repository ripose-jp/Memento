import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Ripose.Memento

Window {
    id: root
    title: qsTr("Anki Integration Help")
    color: MementoPalette.window
    width: 920
    height: 700

    readonly property int commonMargin: 10
    readonly property int sectionPointSize: 20

    ScrollView {
        id: scrollView
        anchors.fill: parent
        leftPadding: root.commonMargin
        rightPadding: root.commonMargin
        contentWidth: availableWidth
        clip: true

        ColumnLayout {
            id: layout
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(scrollView.availableWidth, 900)

            Label {
                Layout.fillWidth: true
                Layout.topMargin: root.commonMargin
                wrapMode: Label.WordWrap
                font.pointSize: root.sectionPointSize
                text: qsTr("Introduction")
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                text: qsTr("This document describes the syntax and the associated markers available for use in note templates.")
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                font.pointSize: root.sectionPointSize
                text: qsTr("Syntax")
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                textFormat: Text.RichText
                text: qsTr(
                    "<p>" +
                        "A marker is a section of text enclosed in curly braces like <code>{subtitle}</code>. " +
                        "When Memento's template processor sees a marker, it replaces it with its associated value. " +
                        "In the case of <code>{subtitle}</code> that is the currently displayed subtitle." +
                    "</p>" +
                    "<p>" +
                        "Some markers have arguments that allow their functionality to modified. " +
                        "For example, <code>{glossary : dict = 1}</code> makes it so the glossary marker only uses " +
                            "entries from the dictionary with the ID of 1. " +
                        "The <code>:</code> character indicates the beginning of marker arguments. " +
                        "Arguments are set in the form of <code>name = value</code>. " +
                        "If a marker has multiple arguments, arguments are separated by the <code>,</code> character. " +
                        "An example of a marker with multiple arguments is <code>{screenshot : max-width = 1280, max-height = 720}</code>." +
                    "</p>" +
                    "<p>" +
                        "A single marker can specify whether to use one or another value. " +
                        "For example, <code>{selection | clipboard}</code> specifies if <code>selection</code> is empty, use <code>clipboard</code> instead. " +
                        "This functionality is triggered by the <code>|</code> character. " +
                        "Multiple ors can be chained together like <code>{selection | clipboard | glossary : dict = 1}</code>." +
                    "</p>"
                )
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                font.pointSize: root.sectionPointSize
                text: qsTr("Term Markers")
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                text: qsTr("This section describes the markers available for use in term notes.")
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.topMargin: root.commonMargin
                Layout.bottomMargin: root.commonMargin
                spacing: root.commonMargin

                Repeater {
                    model: AnkiMarkers.termMarkers
                    delegate: AnkiMarkerDescription {
                        required property int index
                        required property var modelData
                        element: modelData
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                font.pointSize: root.sectionPointSize
                text: qsTr("Kanji Markers")
            }

            Label {
                Layout.fillWidth: true
                wrapMode: Label.WordWrap
                text: qsTr("This section describes the markers available for use in kanji notes.")
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.topMargin: root.commonMargin
                Layout.bottomMargin: root.commonMargin
                spacing: root.commonMargin

                Repeater {
                    model: AnkiMarkers.kanjiMarkers
                    delegate: AnkiMarkerDescription {
                        required property int index
                        required property var modelData
                        element: modelData
                    }
                }
            }
        }
    }
}
