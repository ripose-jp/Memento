pragma Singleton

import QtQml
import QtQml.Models

QtObject {
    id: root

    readonly property string audio: "audio"
    readonly property string audioContext: "audio-context"
    readonly property string audioMedia: "audio-media"
    readonly property string character: "character"
    readonly property string clipboard: "clipboard"
    readonly property string clozeBody: "cloze-body"
    readonly property string clozePrefix: "cloze-prefix"
    readonly property string clozeSuffix: "cloze-suffix"
    readonly property string context: "context"
    readonly property string context2: "context-2"
    readonly property string expression: "expression"
    readonly property string freqAverageOccu: "frequency-average-occurrence"
    readonly property string freqAverageRank: "frequency-average-rank"
    readonly property string freqHarmonicOccu: "frequency-harmonic-occurrence"
    readonly property string freqHarmonicRank: "frequency-harmonic-rank"
    readonly property string frequencies: "frequencies"
    readonly property string furigana: "furigana"
    readonly property string furiganaPlain: "furigana-plain"
    readonly property string glossary: "glossary"
    readonly property string glossaryBrief: "glossary-brief"
    readonly property string glossaryCompact: "glossary-compact"
    readonly property string kunyomi: "kunyomi"
    readonly property string onyomi: "onyomi"
    readonly property string pitch: "pitch"
    readonly property string pitchCategories: "pitch-categories"
    readonly property string pitchGraph: "pitch-graph"
    readonly property string pitchPosition: "pitch-position"
    readonly property string reading: "reading"
    readonly property string screenshot: "screenshot"
    readonly property string screenshotVideo: "screenshot-video"
    readonly property string selection: "selection"
    readonly property string strokeCount: "stroke-count"
    readonly property string subtitle: "subtitle"
    readonly property string subtitle2: "subtitle-2"
    readonly property string tags: "tags"
    readonly property string tagsBrief: "tags-brief"
    readonly property string title: "title"
    readonly property string video: "video"

    readonly property string audioContextDesc: qsTr(
        "<p>Audio of the subtitle list selection.</p>"
    )
    readonly property string audioDesc: qsTr(
        "<p>Audio of the word if found and the skip hash is not matched.</p>"
    )
    readonly property string audioMediaDesc: qsTr(
        "<p>Audio of the current subtitle.</p>"
    )
    readonly property string characterDesc: qsTr(
        "<p>The kanji character.</p>"
    )
    readonly property string clipboardDesc: qsTr(
        "<p>The current text in the clipboard.</p>"
    )
    readonly property string clozeBodyDesc: qsTr(
        "<p>The raw expression as recognized by Memento.</p>"
    )
    readonly property string clozePrefixDesc: qsTr(
        "<p>The text before <code>{%1}</code>.</p>"
    ).arg(root.clozeBody)
    readonly property string clozeSuffixDesc: qsTr(
        "<p>The text after <code>{%1}</code></p>"
    ).arg(root.clozeBody)
    readonly property string context2Desc: qsTr(
        "<p>Text of all selected lines in the secondary subtitle list.</p>"
    )
    readonly property string contextDesc: qsTr(
        "<p>Text of all selected lines in the primary subtitle list.</p>"
    )
    readonly property string expressionDesc: qsTr(
        "<p>The expression in kanji. Kana if there is no kanji or explicitly requested.</p>"
    )
    readonly property string freqAverageOccuDesc: qsTr(
        "<p>The average of frequency data for the expression. " +
        "Defaults to 0 occurrences when frequency data is not available.</p>"
    )
    readonly property string freqAverageRankDesc: qsTr(
        "<p>The average of frequency data for the expression. " +
        "Defaults to 9999999 when frequency data is not available.</p>"
    )
    readonly property string freqHarmonicOccuDesc: qsTr(
        "<p>The harmonic mean of frequency data for the expression. " +
        "Defaults to 0 for occurrences when frequency data is not available.</p>"
    )
    readonly property string freqHarmonicRankDesc: qsTr(
        "<p>The harmonic mean of frequency data for the expression. " +
        "Defaults to 9999999 for occurrences when frequency data is not available.</p>"
    )
    readonly property string frequenciesDesc: qsTr(
        "<p>Bulleted list of frequency tags.</p>"
    )
    readonly property string furiganaDesc: qsTr(
        "<p>The expression in kanji with furigana above. " +
        "Kana if there is no kanji.</p>"
    )
    readonly property string furiganaPlainDesc: qsTr(
        "<p>The expression in kanji with furigana proceeding in brackets. " +
        "Kana if there is no kanji.</p>"
    )
    readonly property string glossaryBriefDesc: qsTr(
        "<p>Dictionary definitions of the expression without dictionary information.</p>"
    )
    readonly property string glossaryCompactDesc: qsTr(
        "<p>Dictionary definitions of the expression in compact form.</p>"
    )
    readonly property string glossaryDesc: qsTr(
        "<p>Dictionary definitions of the expression.</p>"
    )
    readonly property string kunyomiDesc: qsTr(
        "<p>A comma separated list of kunyomi (Japanese) readings written in katakana.</p>"
    )
    readonly property string onyomiDesc: qsTr(
        "<p>A comma separated list of onyomi (Chinese) readings written in hiragana.</p>"
    )
    readonly property string pitchCategoriesDesc: qsTr(
        "<p>A comma separated list of pitch accent categories for the expression: " +
        "heiban, kifuku, atamadaka, nakadaka, odaka.</p>"
    )
    readonly property string pitchDesc: qsTr(
        "<p>Pitch accents for the reading of the expression.</p>"
    )
    readonly property string pitchGraphDesc: qsTr(
        "<p>Pitch graphs for the reading of the expression.</p>"
    )
    readonly property string pitchPositionDesc: qsTr(
        "<p>Pitch positions for the reading of the expression.</p>"
    )
    readonly property string readingDesc: qsTr(
        "<p>The reading of the expression in kana. " +
        "Empty if the expression has no kanji.</p>"
    )
    readonly property string screenshotDesc: qsTr(
        "<p>Screenshot of the current frame.</p>"
    )
    readonly property string screenshotVideoDesc: qsTr(
        "<p>Screenshot of the current frame without subtitles if visible.</p>"
    )
    readonly property string selectionDesc: qsTr(
        "<p>The currently selected glossary text. " +
        "If multiple glossaries are selected, they are ordered as a list.</p>"
    )
    readonly property string strokeCountDesc: qsTr(
        "<p>The number of strokes in the character.</p>"
    )
    readonly property string subtitle2Desc: qsTr(
        "<p>The text of the secondary subtitle.</p>"
    )
    readonly property string subtitleDesc: qsTr(
        "<p>The text of the primary subtitle.</p>"
    )
    readonly property string tagsBriefDesc: qsTr(
        "<p>Unordered list of expression tags without descriptions.</p>"
    )
    readonly property string tagsDesc: qsTr(
        "<p>Unordered list of expression tags.</p>"
    )
    readonly property string titleDesc: qsTr(
        "<p>Title of the current media. Filename if no title is set.</p>"
    )
    readonly property string videoDesc: qsTr(
        "<p>An MP4 file containing a clip of the current context.</p>"
    )

    readonly property string audioArg: "audio"
    readonly property string dbArg: "db"
    readonly property string dictArg: "dict"
    readonly property string keepRatioArg: "keep-ratio"
    readonly property string maxHeightArg: "max-height"
    readonly property string maxWidthArg: "max-width"
    readonly property string minValueArg: "min-value"
    readonly property string normArg: "norm"
    readonly property string sourceArg: "source"
    readonly property string subtitlesArg: "subtitles"
    readonly property string valueOnlyArg: "value-only"

    readonly property string audioArgDesc: qsTr(
        "<p>If true, audio is included in the clip.</p>"
    )
    readonly property string dbArgDesc: qsTr(
        "<p>The decibel value to normalize audio to. " +
        "Ignored if <code>%1 = false</code>.</p>"
    ).arg(normArg)
    readonly property string dictArgDesc: qsTr(
        "<p>Filter to only use entries from a particular dictionary. " +
        "Takes a dictionary ID. " +
        "Dictionary IDs can be found in <b>Options</b> under <b>Dictionaries</b> at the end of dictionary names.</p>"
    )
    readonly property string keepRatioArgDesc: qsTr(
        "<p>If true, maintains the aspect ratio of the image during resize.</p>"
    )
    readonly property string maxHeightArgDesc: qsTr(
        "<p>Used to determine the maximum height of a saved screenshot. " +
        "A value of -1 leaves this unbounded.</p>"
    )
    readonly property string maxWidthArgDesc: qsTr(
        "<p>Used to determine the maximum width of a saved screenshot. " +
        "A value of -1 leaves this unbounded.</p>"
    )
    readonly property string minValueArgDesc: qsTr(
        "<p>If true, only export the lowest frequency value.</p>"
    )
    readonly property string normArgDesc: qsTr(
        "<p>If true, normalize audio.</p>"
    )
    readonly property string sourceArgDesc: qsTr(
        "<p>" +
            "The source of clip timings. Possible values are:" +
            "<ul>" +
                "<li><b>subtitle</b>: Use timings from the current subtitle.</li>" +
                "<li><b>context</b>: Use timings from the currently selected context.</li>" +
            "</ul>" +
        "</p>"
    )
    readonly property string subtitlesArgDesc: qsTr(
        "<p>If true, subtitles are included in the clip.</p>"
    )
    readonly property string valueOnlyArgDesc: qsTr(
        "<p>If true, export frequency values without dictionary names.</p>"
    )

    readonly property string audioArgDefault: qsTr("<code>true</code>")
    readonly property string dbArgDefault: qsTr("The profile normalization dB setting")
    readonly property string dictArgDefault: qsTr("None")
    readonly property string keepRatioArgDefault: qsTr("<code>true</code>")
    readonly property string maxHeightArgDefault: qsTr("<code>-1</code>")
    readonly property string maxWidthArgDefault: qsTr("<code>-1</code>")
    readonly property string minValueArgDefault: qsTr("<code>false</code>")
    readonly property string normArgDefault: qsTr("The profile audio media normalization setting")
    readonly property string sourceArgDefault: qsTr("<code>subtitle</code>")
    readonly property string subtitlesArgDefault: qsTr("<code>true</code>")
    readonly property string valueOnlyArgDefault: qsTr("<code>false</code>")

    readonly property var termMarkers: [
        {
            "name": root.audio,
            "description": root.audioDesc,
            "arguments": [],
        },
        {
            "name": root.audioContext,
            "description": root.audioContextDesc,
            "arguments": [
                {
                    "name": root.normArg,
                    "description": root.normArgDesc,
                    "defaultValue": root.normArgDefault,
                },
                {
                    "name": root.dbArg,
                    "description": root.dbArgDesc,
                    "defaultValue": root.dbArgDefault,
                },
            ],
        },
        {
            "name": root.audioMedia,
            "description": root.audioMediaDesc,
            "arguments": [
                {
                    "name": root.sourceArg,
                    "description": root.sourceArgDesc,
                    "defaultValue": root.sourceArgDefault,
                },
                {
                    "name": root.normArg,
                    "description": root.normArgDesc,
                    "defaultValue": root.normArgDefault,
                },
                {
                    "name": root.dbArg,
                    "description": root.dbArgDesc,
                    "defaultValue": root.dbArgDefault,
                },
            ],
        },
        {
            "name": root.clipboard,
            "description": root.clipboardDesc,
            "arguments": [],
        },
        {
            "name": root.clozeBody,
            "description": root.clozeBodyDesc,
            "arguments": [],
        },
        {
            "name": root.clozePrefix,
            "description": root.clozePrefixDesc,
            "arguments": [],
        },
        {
            "name": root.clozeSuffix,
            "description": root.clozeSuffixDesc,
            "arguments": [],
        },
        {
            "name": root.context,
            "description": root.contextDesc,
            "arguments": [],
        },
        {
            "name": root.context2,
            "description": root.context2Desc,
            "arguments": [],
        },
        {
            "name": root.expression,
            "description": root.expressionDesc,
            "arguments": [],
        },
        {
            "name": root.frequencies,
            "description": root.frequenciesDesc,
            "arguments": [
                {
                    "name": root.minValueArg,
                    "description": root.minValueArgDesc,
                    "defaultValue": root.minValueArgDefault,
                },
                {
                    "name": root.valueOnlyArg,
                    "description": root.valueOnlyArgDesc,
                    "defaultValue": root.valueOnlyArgDefault,
                }
            ],
        },
        {
            "name": root.freqAverageOccu,
            "description": root.freqAverageOccuDesc,
            "arguments": [],
        },
        {
            "name": root.freqAverageRank,
            "description": root.freqAverageRankDesc,
            "arguments": [],
        },
        {
            "name": root.freqHarmonicOccu,
            "description": root.freqHarmonicOccuDesc,
            "arguments": [],
        },
        {
            "name": root.freqHarmonicRank,
            "description": root.freqHarmonicRankDesc,
            "arguments": [],
        },
        {
            "name": root.furigana,
            "description": root.furiganaDesc,
            "arguments": [],
        },
        {
            "name": root.furiganaPlain,
            "description": root.furiganaPlainDesc,
            "arguments": [],
        },
        {
            "name": root.glossary,
            "description": root.glossaryDesc,
            "arguments": [
                {
                    "name": root.dictArg,
                    "description": root.dictArgDesc,
                    "defaultValue": root.dictArgDefault,
                }
            ],
        },
        {
            "name": root.glossaryBrief,
            "description": root.glossaryBriefDesc,
            "arguments": [
                {
                    "name": root.dictArg,
                    "description": root.dictArgDesc,
                    "defaultValue": root.dictArgDefault,
                }
            ],
        },
        {
            "name": root.glossaryCompact,
            "description": root.glossaryCompactDesc,
            "arguments": [
                {
                    "name": root.dictArg,
                    "description": root.dictArgDesc,
                    "defaultValue": root.dictArgDefault,
                }
            ],
        },
        {
            "name": root.pitch,
            "description": root.pitchDesc,
            "arguments": [],
        },
        {
            "name": root.pitchCategories,
            "description": root.pitchCategoriesDesc,
            "arguments": [],
        },
        {
            "name": root.pitchGraph,
            "description": root.pitchGraphDesc,
            "arguments": [],
        },
        {
            "name": root.pitchPosition,
            "description": root.pitchPositionDesc,
            "arguments": [],
        },
        {
            "name": root.reading,
            "description": root.readingDesc,
            "arguments": [],
        },
        {
            "name": root.screenshot,
            "description": root.screenshotDesc,
            "arguments": [
                {
                    "name": root.maxWidthArg,
                    "description": root.maxWidthArgDesc,
                    "defaultValue": root.maxWidthArgDefault,
                },
                {
                    "name": root.maxHeightArg,
                    "description": root.maxHeightArgDesc,
                    "defaultValue": root.maxHeightArgDefault,
                },
                {
                    "name": root.keepRatioArg,
                    "description": root.keepRatioArgDesc,
                    "defaultValue": root.keepRatioArgDefault,
                },
            ],
        },
        {
            "name": root.screenshotVideo,
            "description": root.screenshotVideoDesc,
            "arguments": [
                {
                    "name": root.maxWidthArg,
                    "description": root.maxWidthArgDesc,
                    "defaultValue": root.maxWidthArgDefault,
                },
                {
                    "name": root.maxHeightArg,
                    "description": root.maxHeightArgDesc,
                    "defaultValue": root.maxHeightArgDefault,
                },
                {
                    "name": root.keepRatioArg,
                    "description": root.keepRatioArgDesc,
                    "defaultValue": root.keepRatioArgDefault,
                },
            ],
        },
        {
            "name": root.selection,
            "description": root.selectionDesc,
            "arguments": [],
        },
        {
            "name": root.subtitle,
            "description": root.subtitleDesc,
            "arguments": [],
        },
        {
            "name": root.subtitle2,
            "description": root.subtitle2Desc,
            "arguments": [],
        },
        {
            "name": root.tags,
            "description": root.tagsDesc,
            "arguments": [],
        },
        {
            "name": root.tagsBrief,
            "description": root.tagsBriefDesc,
            "arguments": [],
        },
        {
            "name": root.title,
            "description": root.titleDesc,
            "arguments": [],
        },
        {
            "name": root.video,
            "description": root.videoDesc,
            "arguments": [
                {
                    "name": root.sourceArg,
                    "description": root.sourceArgDesc,
                    "defaultValue": root.sourceArgDefault,
                },
                {
                    "name": root.audioArg,
                    "description": root.audioArgDesc,
                    "defaultValue": root.audioArgDefault,
                },
                {
                    "name": root.subtitlesArg,
                    "description": root.subtitlesArgDesc,
                    "defaultValue": root.subtitlesArgDefault,
                },
                {
                    "name": root.normArg,
                    "description": root.normArgDesc,
                    "defaultValue": root.normArgDefault,
                },
                {
                    "name": root.dbArg,
                    "description": root.dbArgDesc,
                    "defaultValue": root.dbArgDefault,
                },
            ],
        },
    ]

    readonly property var kanjiMarkers: [
        {
            "name": root.audioContext,
            "description": root.audioContextDesc,
            "arguments": [
                {
                    "name": root.normArg,
                    "description": root.normArgDesc,
                    "defaultValue": root.normArgDefault,
                },
                {
                    "name": root.dbArg,
                    "description": root.dbArgDesc,
                    "defaultValue": root.dbArgDefault,
                },
            ],
        },
        {
            "name": root.audioMedia,
            "description": root.audioMediaDesc,
            "arguments": [
                {
                    "name": root.sourceArg,
                    "description": root.sourceArgDesc,
                    "defaultValue": root.sourceArgDefault,
                },
                {
                    "name": root.normArg,
                    "description": root.normArgDesc,
                    "defaultValue": root.normArgDefault,
                },
                {
                    "name": root.dbArg,
                    "description": root.dbArgDesc,
                    "defaultValue": root.dbArgDefault,
                },
            ],
        },
        {
            "name": root.character,
            "description": root.characterDesc,
            "arguments": [],
        },
        {
            "name": root.clipboard,
            "description": root.clipboardDesc,
            "arguments": [],
        },
        {
            "name": root.clozeBody,
            "description": root.clozeBodyDesc,
            "arguments": [],
        },
        {
            "name": root.clozePrefix,
            "description": root.clozePrefixDesc,
            "arguments": [],
        },
        {
            "name": root.clozeSuffix,
            "description": root.clozeSuffixDesc,
            "arguments": [],
        },
        {
            "name": root.context,
            "description": root.contextDesc,
            "arguments": [],
        },
        {
            "name": root.context2,
            "description": root.context2Desc,
            "arguments": [],
        },
        {
            "name": root.frequencies,
            "description": root.frequenciesDesc,
            "arguments": [
                {
                    "name": root.minValueArg,
                    "description": root.minValueArgDesc,
                    "defaultValue": root.minValueArgDefault,
                },
                {
                    "name": root.valueOnlyArg,
                    "description": root.valueOnlyArgDesc,
                    "defaultValue": root.valueOnlyArgDefault,
                }
            ],
        },
        {
            "name": root.freqAverageOccu,
            "description": root.freqAverageOccuDesc,
            "arguments": [],
        },
        {
            "name": root.freqAverageRank,
            "description": root.freqAverageRankDesc,
            "arguments": [],
        },
        {
            "name": root.freqHarmonicOccu,
            "description": root.freqHarmonicOccuDesc,
            "arguments": [],
        },
        {
            "name": root.freqHarmonicRank,
            "description": root.freqHarmonicRankDesc,
            "arguments": [],
        },
        {
            "name": root.glossary,
            "description": root.glossaryDesc,
            "arguments": [],
        },
        {
            "name": root.kunyomi,
            "description": root.kunyomiDesc,
            "arguments": [],
        },
        {
            "name": root.onyomi,
            "description": root.onyomiDesc,
            "arguments": [],
        },
        {
            "name": root.screenshot,
            "description": root.screenshotDesc,
            "arguments": [
                {
                    "name": root.maxWidthArg,
                    "description": root.maxWidthArgDesc,
                    "defaultValue": root.maxWidthArgDefault,
                },
                {
                    "name": root.maxHeightArg,
                    "description": root.maxHeightArgDesc,
                    "defaultValue": root.maxHeightArgDefault,
                },
                {
                    "name": root.keepRatioArg,
                    "description": root.keepRatioArgDesc,
                    "defaultValue": root.keepRatioArgDefault,
                },
            ],
        },
        {
            "name": root.screenshotVideo,
            "description": root.screenshotVideoDesc,
            "arguments": [
                {
                    "name": root.maxWidthArg,
                    "description": root.maxWidthArgDesc,
                    "defaultValue": root.maxWidthArgDefault,
                },
                {
                    "name": root.maxHeightArg,
                    "description": root.maxHeightArgDesc,
                    "defaultValue": root.maxHeightArgDefault,
                },
                {
                    "name": root.keepRatioArg,
                    "description": root.keepRatioArgDesc,
                    "defaultValue": root.keepRatioArgDefault,
                },
            ],
        },
        {
            "name": root.strokeCount,
            "description": root.strokeCountDesc,
            "arguments": [],
        },
        {
            "name": root.subtitle,
            "description": root.subtitleDesc,
            "arguments": [],
        },
        {
            "name": root.subtitle2,
            "description": root.subtitle2Desc,
            "arguments": [],
        },
        {
            "name": root.tags,
            "description": root.tagsDesc,
            "arguments": [],
        },
        {
            "name": root.tagsBrief,
            "description": root.tagsBriefDesc,
            "arguments": [],
        },
        {
            "name": root.title,
            "description": root.titleDesc,
            "arguments": [],
        },
        {
            "name": root.video,
            "description": root.videoDesc,
            "arguments": [
                {
                    "name": root.sourceArg,
                    "description": root.sourceArgDesc,
                    "defaultValue": root.sourceArgDefault,
                },
                {
                    "name": root.audioArg,
                    "description": root.audioArgDesc,
                    "defaultValue": root.audioArgDefault,
                },
                {
                    "name": root.subtitlesArg,
                    "description": root.subtitlesArgDesc,
                    "defaultValue": root.subtitlesArgDefault,
                },
                {
                    "name": root.normArg,
                    "description": root.normArgDesc,
                    "defaultValue": root.normArgDefault,
                },
                {
                    "name": root.dbArg,
                    "description": root.dbArgDesc,
                    "defaultValue": root.dbArgDefault,
                },
            ],
        },
    ]
}
