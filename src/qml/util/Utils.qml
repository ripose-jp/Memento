pragma Singleton

import QtCore
import QtQml
import Ripose.Memento

QtObject {
    /**
     * Converts a name and color to an svgicon provider.
     * @param name The name of the SVG icon in the images/ path.
     * @param color The color to set the text to.
     * @return The image provider path for that SVG and color.
     */
    function toImageProvider(name, color) {
        const componentToHex = function(c) {
            const hex = Math.round(c * 255).toString(16);
            return hex.length === 1 ? "0" + hex : hex;
        };
        const colorString = componentToHex(color.a) +
                          componentToHex(color.r) +
                          componentToHex(color.g) +
                          componentToHex(color.b);
        return `image://svgicon/${name}/${colorString}`;
    }

    /**
     * Creates a regex, or a regex that matches nothing if the pattern is
     * invalid.
     * @param pattern The regex pattern.
     * @param flags The regex flags.
     * @return A RegExp object.
     */
    function safeRegex(pattern, flags) {
        try
        {
            return new RegExp(pattern, flags);
        }
        catch (error)
        {
            return /$a/; // This matches nothing
        }
    }

    /**
     * Makes a time string from a duration.
     * @param time The duration in seconds.
     * @return The time as a string in the format [H:]MM:SS
     */
    function toTimeString(time) {
        const SECONDS_IN_HOUR = 3600;
        const SECONDS_IN_MINUTE = 60;

        time = Math.floor(time);
        const hours = Math.floor(time / SECONDS_IN_HOUR);
        time -= hours * SECONDS_IN_HOUR;
        const minutes = Math.floor(time / SECONDS_IN_MINUTE);
        time -= minutes * SECONDS_IN_MINUTE;
        const seconds = time;

        let result = "";
        if (hours)
        {
            result += `${hours}:`;
        }
        result += `${minutes.toString().padStart(2, "0")}:`;
        result += seconds.toString().padStart(2, "0");
        return result;
    }

    /**
     * Gets the path of the directory type requested.
     * @param dir The MementoSetting.Directory type.
     */
    function getFileOpenDirectory(dir) {
        switch (dir)
        {
        case MementoSetting.DirectoryCurrent:
            return ".";
        case MementoSetting.DirectoryMovies:
            return StandardPaths.standardLocations(StandardPaths.MoviesLocation)[0];
        case MementoSetting.DirectoryDocuments:
            return StandardPaths.standardLocations(StandardPaths.DocumentsLocation)[0];
        case MementoSetting.DirectoryCustom:
            return MementoSettings.behaviorFileOpenCustom;
        case MementoSetting.DirectoryHome:
        default:
            return StandardPaths.standardLocations(StandardPaths.HomeLocation)[0];
        }
    }

    /**
     * Fold frequencies into a model where entries from the same dictionary
     * are a comma separated list.
     * @param frequencies The list of Frequency objects.
     * @return An array of objects containing name and value properties.
     */
    function foldFrequencies(frequencies) {
        if (!frequencies)
        {
            return [];
        }

        let freqs = [];
        for (let i = 0; i < frequencies.length; ++i)
        {
            const freq = frequencies[i];
            if (freqs.length > 0 &&
                    freqs[freqs.length - 1].name === freq.dictionaryInfo.name)
            {
                freqs[freqs.length - 1].value += `, ${freq.frequency}`;
            }
            else
            {
                freqs.push({
                    "name": freq.dictionaryInfo.name,
                    "value": freq.frequency,
                });
            }

        }
        return freqs;
    }
}
