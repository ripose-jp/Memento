pragma Singleton

import QtQml
import QtQuick
import Ripose.Memento

QtObject {
    id: root

    readonly property SystemPalette systemPalette: SystemPalette {}

    readonly property color accent: systemPalette.accent
    readonly property color alternateBase: systemPalette.alternateBase
    readonly property color base: systemPalette.base
    readonly property color button: systemPalette.button
    readonly property color buttonText: systemPalette.buttonText
    readonly property color colorGroup: systemPalette.colorGroup
    readonly property color dark: {
        if (Features.platform === Features.Linux)
        {
            return systemPalette.dark;
        }
        return Qt.darker(root.window, 2.0);
    }
    readonly property color highlight: systemPalette.highlight
    readonly property color highlightedText: systemPalette.highlightedText
    readonly property color light: systemPalette.light
    readonly property color mid: {
        if (Features.platform === Features.Linux)
        {
            return systemPalette.mid;
        }
        return Qt.darker(root.light, 1.5);
    }
    readonly property color midlight: {
        if (Features.platform === Features.Linux)
        {
            return systemPalette.midlight;
        }
        return Qt.darker(root.light, 1.1);
    }
    readonly property color placeholderText: systemPalette.placeholderText
    readonly property color shadow: systemPalette.shadow
    readonly property color text: systemPalette.text
    readonly property color window: systemPalette.window
    readonly property color windowText: systemPalette.windowText

    readonly property color border: {
        if (Features.platform === Features.Linux)
        {
            if (Qt.styleHints.colorScheme === Qt.ColorScheme.Dark)
            {
                return systemPalette.light;
            }
            else
            {
                return systemPalette.mid;
            }
        }
        else
        {
            if (Qt.styleHints.colorScheme === Qt.ColorScheme.Dark)
            {
                return "#323232";
            }
            else
            {
                return "#E5E5E5";
            }
        }
    }
}