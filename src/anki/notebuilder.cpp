////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2025 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#include "notebuilder.h"

#include "anki/ankiconnect.h"
#include "anki/glossarybuilder.h"
#include "anki/marker.h"
#include "anki/markertokenizer.h"
#include "player/playeradapter.h"
#include "util/globalmediator.h"
#include "util/utils.h"

/**
 * Holds all the fields that contain media.
 */
struct FieldContext
{
    /* Fields that contain the {audio} marker */
    QJsonArray fieldsWithAudio;

    /* Fields that contain {audio-media} marker*/
    QJsonArray fieldsWithAudioMedia;

    /* Fields that contain the {audio-context} marker */
    QJsonArray fieldsWithAudioContext;

    /* Fields that contain the {screenshot} marker */
    QJsonArray fieldsWithScreenshot;

    /* Fields that contain the {screenshot-video} marker */
    QJsonArray fieldsWithScreenshotVideo;

    /* Files to include in the note request */
    QSet<GlossaryBuilder::FileInfo> files;
};

/**
 * Holds are data related to glossary entries.
 */
struct GlossaryData
{
    /* Text for the {glossary} marker */
    QString glossary;

    /* Text for the {glossary-brief} marker */
    QString glossaryBrief;

    /* Text for the {glossary-compact} marker */
    QString glossaryCompact;

    /* Files required to render this glossary */
    QSet<GlossaryBuilder::FileInfo> files;
};

/* Begin Context Methods */

void Anki::Note::Context::setDeck(const QString &deck)
{
    ankiObject[AnkiConnect::Note::DECK] = deck;
}

void Anki::Note::Context::setModel(const QString &model)
{
    ankiObject[AnkiConnect::Note::MODEL] = model;
}

void Anki::Note::Context::setTags(const QJsonArray &tags)
{
    ankiObject[AnkiConnect::Note::TAGS] = tags;
}

void Anki::Note::Context::setDuplicatePolicy(AnkiConfig::DuplicatePolicy policy)
{
    switch (policy)
    {
    case AnkiConfig::DuplicatePolicy::None:
        ankiObject[AnkiConnect::Note::OPTIONS] = QJsonObject{
            {AnkiConnect::Note::Option::ALLOW_DUP, false},
        };
        break;
    case AnkiConfig::DuplicatePolicy::DifferentDeck:
        ankiObject[AnkiConnect::Note::OPTIONS] = QJsonObject{
            {
                AnkiConnect::Note::Option::SCOPE,
                AnkiConnect::Note::Option::SCOPE_CHECK_DECK
            }
        };
        break;
    case AnkiConfig::DuplicatePolicy::SameDeck:
        ankiObject[AnkiConnect::Note::OPTIONS] = QJsonObject{
            {AnkiConnect::Note::Option::ALLOW_DUP, true}
        };
        break;
    }
}

void Anki::Note::Context::setFields(const QJsonValue &fields)
{
    ankiObject[AnkiConnect::Note::FIELDS] = fields;
}

/* End Context Methods */
/* Begin Helper Functions */

/**
 * Replaces new line characters with the replacement value.
 * @param str    The string to replace new lines in.
 * @param config The Anki configuration to replace new lines with.
 * @return The string with the new lines replaced.
 */
[[nodiscard]]
static QString replaceNewLines(QString str, const AnkiConfig &config)
{
    return str.replace('\n', config.newlineReplacer);
}

/**
 * Extracts frequency numbers from a list of frequency tags.
 * @param frequencies A list of Frequency structs.
 * @return A vector of positive integers representing the frequency numbers.
 *         (Only selecting the first frequency displayed by a dictionary, to
 *         avoid picking secondary frequencies like kana frequencies)
 */
[[nodiscard]]
static std::vector<int> getFrequencyNumbers(const QList<Frequency> &frequencies)
{
    QString previousDictionary;
    std::vector<int> frequencyNumbers;

    for (const Frequency &frequencyEntry : frequencies)
    {
        if (frequencyEntry.dictionary == previousDictionary ||
            frequencyEntry.freq.isNull())
        {
            continue;
        }
        previousDictionary = frequencyEntry.dictionary;

        /* This regular expression only catches numbers in base 10 and
        * would not catch negative or decimal numbers because we make
        * the assumption that these special types of numbers will not
        * appear in frequency dictionaries. */
        QRegularExpression numberPattern("\\d+");
        QRegularExpressionMatch match =
            numberPattern.match(frequencyEntry.freq);

        if (match.hasMatch())
        {
            /* Only save the first number to avoid counting secondary frequency
            * information (e.g. frequency for the full kana orthography) in the
            * aggregate measures to align with Yomitan's behavior. */
            frequencyNumbers.push_back(match.captured(0).toInt());
            continue;
        }
    }

    return frequencyNumbers;
}

/**
 * Converts an integer to a string or a default value if negative.
 * @param value        The value to convert to a string.
 * @param defaultValue The value to default to if value is negative.
 * @return value as a string if value > 0, otherwise defaultValue.
 */
[[nodiscard]]
static QString positiveIntToQString(int value, int defaultValue)
{
    return (value < 0) ? QString::number(defaultValue) : QString::number(value);
}

/**
 * Helper method to determine if the kifuku pitch accent pattern is potentially
 * applicable to a term, by checking its part-of-speech.
 * @param defs The definitions of the term.
 * @return True if kifuku is applicable, false otherwise.
 */
[[nodiscard]]
static bool isKifukuApplicable(const QList<TermDefinition> &defs)
{
    bool canBeKifuku{false};

    for (const TermDefinition &def : defs)
    {
        for (const QString &rule : def.rules)
        {
            /* See http://www.edrdg.org/jmwsgi/edhelp.py?svc=jmdict&sid=#kw_pos
             * for more information on the meaning of JMDict part-of-speech tags
             * such as "v1", "adj-i", etc.
             */
            if (rule == "v1" ||  // Ichidan verb
                rule == "v5" ||  // Godan verb
                rule == "vk" ||  // Kuru verb - special class
                rule == "vz" ||  // Ichidan verb - zuru verb (alternative form of -jiru verb)
                rule == "adj-i") // I-adjective (keiyoushi)
            {
                canBeKifuku = true;
            }
            else if (rule == "vs") // Noun or participle which takes the aux. verb suru
            {
                return false;
            }
        }
    }

    return canBeKifuku;
}

/**
 * Get the image file extension from a FileType.
 * @param type The file type of the extension.
 * @return The extension of the image type.
 */
[[nodiscard]]
static QString getImageFileExtension(const AnkiConfig::FileType &type)
{
    switch (type)
    {
    case AnkiConfig::FileType::png:
        return ".png";
    case AnkiConfig::FileType::webp:
        return ".webp";
    case AnkiConfig::FileType::jpg:
    default:
        return ".jpg";
    }
}

/* End Helper Functions */
/* Begin Marker Functions */

/**
 * Gets the value of the {expression} marker.
 * @param term The term to get the expression from.
 * @return The expression if readAsExpression is false, reading otherwise.
 */
[[nodiscard]]
static QString getExpression(const Term &term)
{
    return term.readingAsExpression ? term.reading : term.expression;
}

/**
 * Gets the value of the {reading} marker.
 * @param term The term to get the reading from.
 * @return The value of the {reading} marker.
 */
[[nodiscard]]
static QString getReading(const Term &term)
{
    if (term.reading.isEmpty() || term.readingAsExpression)
    {
        return getExpression(term);
    }
    return term.reading;
}

/**
 * Gets the value of the {furigana} marker.
 * @param term The term to get the furigana from.
 * @return The value of the {furigana} marker.
 */
[[nodiscard]]
static QString getFurigana(const Term &term)
{
    if (term.reading.isEmpty() || term.readingAsExpression)
    {
        return getExpression(term);
    }
    return QString("<ruby>%1<rt>%2</rt></ruby>")
        .arg(getExpression(term))
        .arg(getReading(term));
}

/**
 * Gets the value of the {furigana-plain} marker.
 * @param term The term to get the furigana from.
 * @return The value of the {furigana-plain} marker.
 */
[[nodiscard]]
static QString getFuriganaPlain(const Term &term)
{
    if (term.reading.isEmpty() || term.readingAsExpression)
    {
        return getExpression(term);
    }
    return QString("%1[%2]")
        .arg(getExpression(term))
        .arg(getReading(term));
}

/**
 * Builds the HTML string for the frequencies marker.
 * @param frequencies The frequencies to use.
 * @return The string for the {frequency} marker.
 */
[[nodiscard]]
static QString buildFrequencies(const QList<Frequency> &frequencies)
{
    if (frequencies.isEmpty())
    {
        return "";
    }

    QString freqStr;
    freqStr += "<ul>";
    for (const Frequency &freq : frequencies)
    {
        freqStr += "<li>";
        freqStr += freq.dictionary;
        freqStr += freq.dictionary.endsWith(':') ? " " : ": ";
        freqStr += freq.freq;
        freqStr += "</li>";
    }
    freqStr += "</ul>";

    if (freqStr == "<ul></ul>")
    {
        freqStr.clear();
    }

    return freqStr;
}

/**
 * Function to calculate the harmonic mean of frequencies.
 * @param frequencies A list of Frequency structs.
 * @return The harmonic mean as an integer, or -1 if the list is empty.
 */
[[nodiscard]]
static int getFrequencyHarmonic(const QList<Frequency> &frequencies)
{
    const std::vector<int> frequencyNumbers = getFrequencyNumbers(frequencies);

    if (frequencyNumbers.empty())
    {
        return -1;
    }

    double total = 0.0;
    for (int frequencyNum : frequencyNumbers)
    {
        if (frequencyNum != 0)
        {
            total += 1.0 / frequencyNum;
        }
    }

    return std::floor(frequencyNumbers.size() / total);
}

/**
 * Function to calculate the arithmetic average of frequencies.
 * @param frequencies A list of Frequency structs.
 * @return The arithmetic average as an integer, or -1 if the list is empty.
 */
[[nodiscard]]
static int getFrequencyAverage(const QList<Frequency> &frequencies)
{
    const std::vector<int> frequencyNumbers = getFrequencyNumbers(frequencies);

    if (frequencyNumbers.empty())
    {
        return -1;
    }

    /* Sum the elements in the vector */
    double total = std::accumulate(frequencyNumbers.begin(),
            frequencyNumbers.end(), 0);

    return std::floor(total / frequencyNumbers.size());
}

/**
 * Build the text for {glossary}, {glossary-brief}, and {glossary-compact}.
 * @param definitions The definitions to build the glossary from.
 * @return The text and files for {glossary}, {glossary-brief}, and
 *         {glossary-compact}.
 */
[[nodiscard]]
static GlossaryData getGlossary(const QList<TermDefinition> &definitions)
{
    if (definitions.empty())
    {
        return {};
    }

    GlossaryData data;

    QString basepath = DirectoryUtils::getDictionaryResourceDir() + SLASH;

    data.glossary += "<div style=\"text-align: left;\"><ol>";
    data.glossaryBrief += "<div style=\"text-align: left;\"><ol>";
    data.glossaryCompact += "<div style=\"text-align: left;\"><ol>";

    for (const TermDefinition &def : definitions)
    {
        data.glossary += "<li data-dictionary=\"" + def.dictionary + "\">";
        data.glossaryCompact += "<li data-dictionary=\"" + def.dictionary + "\">";

        data.glossary += "<i>(";
        data.glossaryCompact += "<i>(";
        for (const Tag &tag : def.tags)
        {
            data.glossary += tag.name + ", ";
            data.glossaryCompact += tag.name + ", ";
        }
        data.glossary += def.dictionary;
        data.glossary += ")</i>";
        data.glossaryCompact += def.dictionary;
        data.glossaryCompact += ")</i>";

        data.glossary += "<ul>";
        data.glossaryCompact += ' ';

        QStringList items = GlossaryBuilder::buildGlossary(
            def.glossary, basepath + def.dictionary, data.files
        );
        for (const QString &item : items)
        {
            data.glossary += "<li>";
            data.glossary += item;
            data.glossary += "</li>";

            data.glossaryBrief += "<li>";
            data.glossaryBrief += item;
            data.glossaryBrief += "</li>";
        }
        data.glossaryCompact += items.join(" | ");

        data.glossary += "</ul></li>";
        data.glossaryCompact += "</li>";
    }

    data.glossary += "</ol></div>";
    data.glossaryBrief += "</ol></div>";
    data.glossaryCompact += "</ol></div>";
    return data;
}

/**
 * Build the text for {glossary}, {glossary-brief}, and {glossary-compact}.
 * @param definitions The definitions to build the glossary from.
 * @param args        The extender marker arguments.
 * @return The text and files for {glossary}, {glossary-brief}, and
 *         {glossary-compact}.
 */
[[nodiscard]]
static GlossaryData getGlossary(
    const QList<TermDefinition> &definitions,
    const QHash<QString, QString> &args)
{
    constexpr const char *ARG_DICTIONARY = "dict";

    const auto dicIdStr = args.constFind(ARG_DICTIONARY);
    if (dicIdStr == std::end(args))
    {
        return getGlossary(definitions);
    }

    bool success = false;
    const int dicId = dicIdStr->toInt(&success);
    if (!success)
    {
        GlossaryData data;
        data.glossary = data.glossaryBrief = data.glossaryCompact =
            "ERROR: Invalid dic-id value";
        return data;
    }

    QList<TermDefinition> filteredDefinitions;
    std::copy_if(
        std::begin(definitions),
        std::end(definitions),
        std::back_inserter(filteredDefinitions),
        [dicId] (const TermDefinition &td) -> bool
        {
            return td.dictionaryId == dicId;
        }
    );

    return getGlossary(filteredDefinitions);
}

/**
 * Build the {pitch} marker.
 * @param pitches The pitches to use.
 * @return The HTML of the {pitch} marker.
 */
[[nodiscard]]
static QString getPitch(const QList<Pitch> &pitches)
{
    if (pitches.isEmpty())
    {
        return {};
    }

    const QString PITCH_FORMAT = "<span style=\"%1\">%2</span>";
    const QString H_STYLE = "border-top: solid;";
    const QString HL_STYLE = "border-top: solid; border-right: solid;";

    QString pitch = "<span class=\"memento-pitch\">";

    const bool multipleDicts = pitches.size() > 1;

    if (multipleDicts)
    {
        pitch += "<ul>";
    }

    for (const Pitch &p : pitches)
    {
        const bool multiplePitches = p.position.size() > 1;

        /* Header */
        if (multipleDicts)
        {
            pitch += "<li><i>" + p.dictionary + "</i>";
        }
        if(multiplePitches)
        {
            pitch += "<ol>";
        }

        /* Body */
        for (const uint8_t pos : p.position)
        {
            if (p.mora.isEmpty())
            {
                continue;
            }

            if (multiplePitches)
            {
                pitch += "<li style=\"padding: 2px 0px 2px 0px;\">";
            }

            switch (pos)
            {
            case 0:
                pitch += p.mora.first();
                if (p.mora.size() > 1)
                {
                    pitch += PITCH_FORMAT
                        .arg(H_STYLE)
                        .arg(p.mora.join("")
                        .remove(0, p.mora.first().size()));
                }
                break;
            case 1:
                pitch += PITCH_FORMAT.arg(HL_STYLE).arg(p.mora.first());
                if (p.mora.size() > 1)
                {
                    pitch += p.mora.join("").remove(0, p.mora.first().size());
                }
                break;
            default:
            {
                QString text = p.mora.first();
                pitch += text;

                text.clear();
                for (size_t i = 1; i < pos; ++i)
                {
                    text += p.mora[i];
                }
                if (!text.isEmpty())
                {
                    pitch += PITCH_FORMAT.arg(HL_STYLE).arg(text);
                }

                text.clear();
                for (int i = pos; i < p.mora.size(); ++i)
                {
                    text += p.mora[i];
                }
                if (!text.isEmpty())
                {
                    pitch += text;
                }
            }
            }

            if (multiplePitches)
            {
                pitch += "</li>";
            }
        }

        /* Trailer */
        if(multiplePitches)
        {
            pitch += "</ol>";
        }
        if (multipleDicts)
        {
            pitch += "</li>";
        }
    }

    if (multipleDicts)
    {
        pitch += "</ul>";
    }

    pitch += "</span>";

    return pitch;
}

/**
 * Get the {pitch-graph} marker.
 * @param pitches The pitches to use to build the marker.
 * @return The SVGs for the {pitch-graph} marker.
 */
[[nodiscard]]
static QString getPitchGraph(const QList<Pitch> &pitches)
{
    if (pitches.isEmpty())
    {
        return {};
    }

    QString pitchGraph = "<span class=\"memento-pitch-graph\">";

    const bool multipleDicts = pitches.size() > 1;

    if (multipleDicts)
    {
        pitchGraph += "<ul>";
    }

    for (const Pitch &p : pitches)
    {
        const bool multiplePitches = p.position.size() > 1;

        /* Header */
        if (multipleDicts)
        {
            pitchGraph += "<li><i>" + p.dictionary + "</i>";
        }
        if(multiplePitches)
        {
            pitchGraph += "<ol>";
        }

        /* Body */
        for (const uint8_t pos : p.position)
        {
            if (p.mora.isEmpty())
            {
                continue;
            }

            if (multiplePitches)
            {
                pitchGraph += "<li>";
            }

            pitchGraph += GraphicUtils::generatePitchGraph(
                p.mora.size(), pos, "rgba(0,0,0,0)", "currentColor"
            );

            if (multiplePitches)
            {
                pitchGraph += "</li>";
            }
        }

        /* Trailer */
        if(multiplePitches)
        {
            pitchGraph    += "</ol>";
        }
        if (multipleDicts)
        {
            pitchGraph    += "</li>";
        }
    }

    if (multipleDicts)
    {
        pitchGraph += "</ul>";
    }

    pitchGraph += "</span>";

    return pitchGraph;
}

/**
 * Get the marker for {pitch-position}.
 * @param pitches The pitches to use.
 * @return The text for {pitch-position}.
 */
[[nodiscard]]
static QString getPitchPosition(const QList<Pitch> &pitches)
{
    if (pitches.isEmpty())
    {
        return {};
    }

    QString pitchPosition = "<span class=\"memento-pitch-position\">";

    const bool multipleDicts = pitches.size() > 1;

    if (multipleDicts)
    {
        pitchPosition += "<ul>";
    }

    for (const Pitch &p : pitches)
    {
        const bool multiplePitches = p.position.size() > 1;

        /* Header */
        if (multipleDicts)
        {
            pitchPosition += "<li><i>" + p.dictionary + "</i>";
        }
        if(multiplePitches)
        {
            pitchPosition += "<ol>";
        }

        /* Body */
        for (const uint8_t pos : p.position)
        {
            if (p.mora.isEmpty())
            {
                continue;
            }

            if (multiplePitches)
            {
                pitchPosition += "<li>";
            }

            pitchPosition += "[" + QString::number(pos) + "]";

            if (multiplePitches)
            {
                pitchPosition += "</li>";
            }
        }

        /* Trailer */
        if(multiplePitches)
        {
            pitchPosition += "</ol>";
        }
        if (multipleDicts)
        {
            pitchPosition += "</li>";
        }
    }

    if (multipleDicts)
    {
        pitchPosition += "</ul>";
    }

    pitchPosition += "</span>";

    return pitchPosition;
}

/**
 * Get the {pitch-categories} for a term.
 * @param term The term to get the categories from.
 * @return The text of the {pitch-categories} marker.
 */
[[nodiscard]]
static QString getPitchCategories(const Term &term)
{
    if (term.pitches.isEmpty())
    {
        return {};
    }

    const bool canBeKifuku = isKifukuApplicable(term.definitions);
    QSet<QString> pitchCategoriesSet{};
    for (const Pitch &p : term.pitches)
    {
        for (const uint8_t pos : p.position)
        {
            if (p.mora.isEmpty())
            {
                continue;
            }

            switch (pos)
            {
            case 0:
                pitchCategoriesSet.insert("heiban");
                break;

            case 1:
                pitchCategoriesSet.insert(canBeKifuku ? "kifuku" : "atamadaka");
                break;

            default:
                if (p.mora.size() == pos)
                {
                    pitchCategoriesSet.insert("odaka");
                }
                else if (canBeKifuku)
                {
                    pitchCategoriesSet.insert("kifuku");
                }
                else
                {
                    pitchCategoriesSet.insert("nakadaka");
                }
            }
        }
    }

    QString pitchCategories;
    for (const QString &pitchCategory : pitchCategoriesSet)
    {
        pitchCategories += pitchCategory + ',';
    }
    pitchCategories.chop(1);
    return pitchCategories;
}

/**
 * Helper method for building the {selection} list.
 * @param selections The list of text selections.
 * @return HTML string encoding the selected text.
 */
[[nodiscard]]
QString getSelection(const QStringList &selections)
{
    QString selection;

    if (selections.size() == 1)
    {
        selection = selections.front();
    }
    else if (selections.size() > 1)
    {
        selection = "<ul>";
        for (const QString &text : selections)
        {
            selection += "<li>";
            selection += text;
            selection += "</li>";
        }
        selection += "</ul>";
    }

    return selection;
}

/**
 * Builds the {tags} marker.
 * @param tags The tags to use.
 * @return The HTML for the {tags} marker.
 */
[[nodiscard]]
QString getTags(const QList<Tag> &tags)
{
    if (tags.isEmpty())
    {
        return {};
    }

    QString tagStr = "<ul>";
    for (const Tag &tag : tags)
    {
        tagStr += "<li>";
        tagStr += tag.name;
        if (!tag.notes.isEmpty())
        {
            tagStr += ": ";
            tagStr += tag.notes;
        }
        tagStr += "</li>";
    }
    tagStr += "</ul>";
    return tagStr;
}

/**
 * Builds the {tags-brief} marker.
 * @param tags The tags to use.
 * @return The HTML for the {tags-brief} marker.
 */
[[nodiscard]]
QString getTagsBrief(const QList<Tag> &tags)
{
    if (tags.isEmpty())
    {
        return {};
    }

    QString tagBriefStr = "<ul>";
    for (const Tag &tag : tags)
    {
        tagBriefStr += "<li>";
        tagBriefStr += tag.name;
        tagBriefStr += "</li>";
    }
    tagBriefStr += "</ul>";
    return tagBriefStr;
}

/**
 * Get the {kunyomi} marker of the kanji.
 * @param definitions The definitions to get the kunyomi from.
 * @return The HTML for the {kunyomi} marker.
 */
[[nodiscard]]
static QString getKunyomi(const QList<KanjiDefinition> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    QString kunyomi;
    for (const KanjiDefinition &def : definitions)
    {
        for (const QString &str : def.kunyomi)
        {
            kunyomi += str;
            kunyomi += ", ";
        }
    }
    kunyomi.chop(2);
    return kunyomi;
}

/**
 * Get the {onyomi} marker of the kanji.
 * @param definitions The definitions to get the onyomi from.
 * @return The HTML for the {onyomi} marker.
 */
[[nodiscard]]
static QString getOnyomi(const QList<KanjiDefinition> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    QString onyomi;
    for (const KanjiDefinition &def : definitions)
    {
        for (const QString &str : def.onyomi)
        {
            onyomi += str;
            onyomi += ", ";
        }
    }
    onyomi.chop(2);
    return onyomi;
}

/**
 * Get the {stroke-count} marker text.
 * @param definitions The definitions to use to get the marker.
 * @return The HTML of the {stroke-count} marker.
 */
[[nodiscard]]
static QString getStrokeCount(const QList<KanjiDefinition> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    for (const KanjiDefinition &def : definitions)
    {
        for (const QPair<Tag, QString> &pair : def.stats)
        {
            if (pair.first.notes == "Stroke count")
            {
                return pair.second;
            }
        }
    }
    return {};
}

/**
 * Get the kanji {glossary} marker.
 * @param definitions The definitions to build the glossary from.
 * @return The HTML for the {glossary} marker.
 */
[[nodiscard]]
static QString getGlossary(const QList<KanjiDefinition> &definitions)
{
    if (definitions.empty())
    {
        return {};
    }

    QString glossary = "<ol>";
    for (const KanjiDefinition &def : definitions)
    {
        if (def.glossary.isEmpty())
        {
            continue;
        }
        glossary += "<li><i>(";
        for (const Tag &tag : def.tags)
        {
            glossary += tag.name;
            glossary += ", ";
        }
        glossary += def.dictionary;
        glossary += ")</i></li>";
        glossary += "<ol>";
        for (const QString &glos : def.glossary)
        {
            glossary += "<li>";
            glossary += glos;
            glossary += "</li>";
        }
        glossary += "</ol>";
    }
    glossary += "</ol>";

    /* Clear out empty results */
    if (glossary == "<ol></ol>")
    {
        glossary.clear();
    }
    return glossary;
}

/**
 * Gets the kanji {tags} marker text.
 * @param definitions The definitions to get the tags from.
 * @return The HTML for the kanji {tags} marker.
 */
[[nodiscard]]
static QString getTags(const QList<KanjiDefinition> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    QString tags;
    for (const KanjiDefinition &def : definitions)
    {
        tags += getTags(def.tags);
    }
    return tags;
}

/**
 * Gets the kanji {tags-brief} marker
 * @param definitions The definitions to get the tags from.
 * @return The HTML for the kanji {tags-brief} marker.
 */
[[nodiscard]]
static QString getTagsBrief(const QList<KanjiDefinition> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    QString tagsBrief;
    for (const KanjiDefinition &def : definitions)
    {
        tagsBrief += getTagsBrief(def.tags);
    }
    return tagsBrief;
}

/**
 * Processes kanji specific tokens.
 * @param kanji         The kanji to build the token with.
 * @param token         The token to handle.
 * @param[out] value    The value to update.
 * @return true if the token was processed, false otherwise.
 */
static bool processToken(
    const Kanji &kanji,
    const Anki::Token &token,
    QString &value)
{
    if (token.marker == Anki::Marker::CHARACTER)
    {
        value.replace(token.raw, kanji.character);
        return true;
    }
    else if (token.marker == Anki::Marker::KUNYOMI)
    {
        value.replace(token.raw, getKunyomi(kanji.definitions));
        return true;
    }
    else if (token.marker == Anki::Marker::ONYOMI)
    {
        value.replace(token.raw, getOnyomi(kanji.definitions));
        return true;
    }
    else if (token.marker == Anki::Marker::STROKE_COUNT)
    {
        value.replace(token.raw, getStrokeCount(kanji.definitions));
        return true;
    }
    else if (token.marker == Anki::Marker::GLOSSARY)
    {
        value.replace(token.raw, getGlossary(kanji.definitions));
        return true;
    }
    else if (token.marker == Anki::Marker::TAGS)
    {
        value.replace(token.raw, getTags(kanji.definitions));
        return true;
    }
    else if (token.marker == Anki::Marker::TAGS_BRIEF)
    {
        value.replace(token.raw, getTagsBrief(kanji.definitions));
        return true;
    }
    return false;
}

/**
 * Processes term specific tokens.
 * @param config        The config
 * @param term          The term to build the token with.
 * @param token         The token to handle.
 * @param field         The field this token belongs to.
 * @param[out] value    The value to update.
 * @param[out] fieldCtx The field context to update.
 * @return true if the token was processed, false otherwise.
 */
static bool processToken(
    const AnkiConfig &config,
    const Term &term,
    const Anki::Token &token,
    const QString &field,
    QString &value,
    FieldContext &fieldCtx)
{
    if (token.marker == Anki::Marker::AUDIO)
    {
        fieldCtx.fieldsWithAudio.append(field);
        value.replace(token.raw, "");
        return true;
    }
    else if (token.marker == Anki::Marker::EXPRESSION)
    {
        value.replace(token.raw, getExpression(term));
        return true;
    }
    else if (token.marker == Anki::Marker::FURIGANA)
    {
        value.replace(token.raw, getFurigana(term));
        return true;
    }
    else if (token.marker == Anki::Marker::FURIGANA_PLAIN)
    {
        value.replace(token.raw, getFuriganaPlain(term));
        return true;
    }
    else if (token.marker == Anki::Marker::READING)
    {
        value.replace(token.raw, getReading(term));
        return true;
    }
    else if (token.marker == Anki::Marker::GLOSSARY)
    {
        GlossaryData data = getGlossary(term.definitions, token.args);
        value.replace(token.raw, data.glossary);
        fieldCtx.files.unite(data.files);
        return true;
    }
    else if (token.marker == Anki::Marker::GLOSSARY_BRIEF)
    {
        GlossaryData data = getGlossary(term.definitions, token.args);
        value.replace(token.raw, data.glossaryBrief);
        fieldCtx.files.unite(data.files);
        return true;
    }
    else if (token.marker == Anki::Marker::GLOSSARY_COMPACT)
    {
        GlossaryData data = getGlossary(term.definitions, token.args);
        value.replace(token.raw, data.glossaryCompact);
        fieldCtx.files.unite(data.files);
        return true;
    }
    else if (token.marker == Anki::Marker::PITCH)
    {
        value.replace(token.raw, getPitch(term.pitches));
        return true;
    }
    else if (token.marker == Anki::Marker::PITCH_GRAPHS)
    {
        value.replace(token.raw, getPitchGraph(term.pitches));
        return true;
    }
    else if (token.marker == Anki::Marker::PITCH_POSITION)
    {
        value.replace(token.raw, getPitchPosition(term.pitches));
        return true;
    }
    else if (token.marker == Anki::Marker::PITCH_CATEGORIES)
    {
        value.replace(token.raw, getPitchCategories(term));
        return true;
    }
    else if (token.marker == Anki::Marker::SELECTION)
    {
        value.replace(
            token.raw, replaceNewLines(getSelection(term.selection), config)
        );
        return true;
    }
    else if (token.marker == Anki::Marker::TAGS)
    {
        value.replace(token.raw, getTags(term.tags));
        return true;
    }
    else if (token.marker == Anki::Marker::TAGS_BRIEF)
    {
        value.replace(token.raw, getTagsBrief(term.tags));
        return true;
    }
    return false;
}

/**
 * Handles common token markers for a value.
 * @param      config   The current AnkiConfig to build to.
 * @param      exp      The details of the current expression.
 * @param      token    The Anki::Token to process in the field.
 * @param      field    The name of the field.
 * @param[out] value    The value of the field. This is modified by this
 *                      function.
 * @param[out] fieldCtx The context containing all the media fields.
 * @return True if the field was modified, false otherwise.
 */
static bool processTokenCommon(
    const AnkiConfig &config,
    const CommonExpFields &exp,
    const Anki::Token &token,
    const QString &field,
    QString &value,
    FieldContext &fieldCtx)
{
    /* If the term never occurs in the corpus of any loaded frequency
     * dictionary, assume it is a very rare word. (The higher the ranking, the
     * rarer the term) */
    constexpr int DEFAULT_FREQ_RANK = 9999999;
    constexpr int DEFAULT_FREQ_OCCURRENCE = 0;

    if (token.marker == Anki::Marker::AUDIO_MEDIA)
    {
        fieldCtx.fieldsWithAudioMedia.append(field);
        value.replace(token.raw, "");
        return true;
    }
    else if (token.marker == Anki::Marker::AUDIO_CONTEXT)
    {
        fieldCtx.fieldsWithAudioContext.append(field);
        value.replace(token.raw, "");
        return true;
    }
    else if (token.marker == Anki::Marker::SCREENSHOT)
    {
        fieldCtx.fieldsWithScreenshot.append(field),
        value.replace(token.raw, "");
        return true;
    }
    else if (token.marker == Anki::Marker::SCREENSHOT_VIDEO)
    {
        fieldCtx.fieldsWithScreenshotVideo.append(field);
        value.replace(token.raw, "");
        return true;
    }
    else if (token.marker == Anki::Marker::TITLE)
    {
        value.replace(token.raw, replaceNewLines(exp.title, config));
        return true;
    }
    else if (token.marker == Anki::Marker::CLIPBOARD)
    {
        value.replace(token.raw, replaceNewLines(exp.clipboard, config));
        return true;
    }
    else if (token.marker == Anki::Marker::CLOZE_BODY)
    {
        value.replace(token.raw, replaceNewLines(exp.clozeBody, config));
        return true;
    }
    else if (token.marker == Anki::Marker::CLOZE_PREFIX)
    {
        value.replace(token.raw, replaceNewLines(exp.clozePrefix, config));
        return true;
    }
    else if (token.marker == Anki::Marker::CLOZE_SUFFIX)
    {
        value.replace(token.raw, replaceNewLines(exp.clozeSuffix, config));
        return true;
    }
    else if (token.marker == Anki::Marker::SENTENCE)
    {
        value.replace(token.raw, replaceNewLines(exp.sentence, config));
        return true;
    }
    else if (token.marker == Anki::Marker::SENTENCE_SEC)
    {
        value.replace(token.raw, replaceNewLines(exp.sentence2, config));
        return true;
    }
    else if (token.marker == Anki::Marker::CONTEXT)
    {
        value.replace(token.raw, replaceNewLines(exp.context, config));
        return true;
    }
    else if (token.marker == Anki::Marker::CONTEXT_SEC)
    {
        value.replace(token.raw, replaceNewLines(exp.context2, config));
        return true;
    }
    else if (token.marker == Anki::Marker::FREQUENCIES)
    {
        value.replace(token.raw, buildFrequencies(exp.frequencies));
        return true;
    }
    else if (token.marker == Anki::Marker::FREQ_HARMONIC_RANK)
    {
        value.replace(
            token.raw,
            positiveIntToQString(
                getFrequencyHarmonic(exp.frequencies), DEFAULT_FREQ_RANK
            )
        );
        return true;
    }
    else if (token.marker == Anki::Marker::FREQ_HARMONIC_OCCU)
    {
        value.replace(
            token.raw,
            positiveIntToQString(
                getFrequencyHarmonic(exp.frequencies), DEFAULT_FREQ_OCCURRENCE
            )
        );
        return true;
    }
    else if (token.marker == Anki::Marker::FREQ_AVERAGE_RANK)
    {
        value.replace(
            token.raw,
            positiveIntToQString(
                getFrequencyAverage(exp.frequencies), DEFAULT_FREQ_RANK
            )
        );
        return true;
    }
    else if (token.marker == Anki::Marker::FREQ_AVERAGE_OCCU)
    {
        value.replace(
            token.raw,
            positiveIntToQString(
                getFrequencyAverage(exp.frequencies), DEFAULT_FREQ_OCCURRENCE
            )
        );
        return true;
    }
    return false;
}

/* End Marker Functions */
/* Begin Media Functions */

/**
 * Add the {audio} files to the context.
 * @param      term     The term to use to create audio parameters.
 * @param      fieldCtx The field context to get data from.
 * @param[out] ctx      The note context to add the audio to.
 */
static bool createAudio(
    const Term &term,
    const FieldContext &fieldCtx,
    Anki::Note::Context &ctx)
{
    if (fieldCtx.fieldsWithAudio.isEmpty())
    {
        return false;
    }

    constexpr const char *REPLACE_EXPRESSION = "{expression}";
    constexpr const char *REPLACE_READING = "{reading}";
    const QString AUDIO_FILENAME_FORMAT_STRING = "memento_%1_%2_%3.mp3";

    QJsonObject audObj;
    audObj[AnkiConnect::Note::URL] = QString(term.audioURL)
        .replace(REPLACE_EXPRESSION, getExpression(term))
        .replace(REPLACE_READING, getReading(term));
    audObj[AnkiConnect::Note::FILENAME] = AUDIO_FILENAME_FORMAT_STRING
        .arg(term.audioSrcName)
        .arg(term.reading)
        .arg(term.expression)
        .replace(' ', '_');
    audObj[AnkiConnect::Note::FIELDS] = fieldCtx.fieldsWithAudio;
    audObj[AnkiConnect::Note::SKIPHASH] = term.audioSkipHash;

    QJsonArray audio = ctx.ankiObject[AnkiConnect::Note::AUDIO].toArray();
    audio.append(audObj);
    ctx.ankiObject[AnkiConnect::Note::AUDIO] = audio;

    return true;
}

/**
 * Create the audio media file and add it to the context.
 * @param      config   The config to use when generating the media file.
 * @param      exp      The expression to use when build the {audio-media}.
 * @param      fieldCtx The field context containing fields that include media.
 * @param[out] ctx      The context to add the media to.
 * @return true if the media was added to the context, false otherwise.
 */
static bool createAudioMedia(
    const AnkiConfig &config,
    const CommonExpFields &exp,
    const FieldContext &fieldCtx,
    Anki::Note::Context &ctx)
{
    if (fieldCtx.fieldsWithAudio.isEmpty())
    {
        return false;
    }

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();

    double startTime = exp.startTime - config.audioPadStart;
    double endTime = exp.endTime + config.audioPadEnd;

    QString path;
    if (startTime >= 0 && endTime >= 0 && startTime < endTime)
    {
        path = player->tempAudioClip(
            startTime,
            endTime,
            config.audioNormalize,
            config.audioDb
        );
    }
    if (path.isEmpty())
    {
        return false;
    }

    QJsonObject audObj;
    audObj[AnkiConnect::Note::DATA] = FileUtils::toBase64(path);
    QString filename = FileUtils::calculateMd5(path) + ".aac";
    audObj[AnkiConnect::Note::FILENAME] = filename;
    audObj[AnkiConnect::Note::FIELDS] = fieldCtx.fieldsWithAudioMedia;

    QJsonArray audio = ctx.ankiObject[AnkiConnect::Note::AUDIO].toArray();
    audio.append(audObj);
    ctx.ankiObject[AnkiConnect::Note::AUDIO] = audio;

    QFile(path).remove();
    return true;
}

/**
 * Create the audio context file and add it to the context.
 * @param      config   The config to use when generating the context file.
 * @param      exp      The expression to use when build the {audio-context}.
 * @param      fieldCtx The field context containing fields that include media.
 * @param[out] ctx      The context to add the media to.
 * @return true if the media was added to the context, false otherwise.
 */
static bool createAudioContext(
    const AnkiConfig &config,
    const CommonExpFields &exp,
    const FieldContext &fieldCtx,
    Anki::Note::Context &ctx)
{
    if (fieldCtx.fieldsWithAudioContext.isEmpty())
    {
        return false;
    }

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();

    double startTime = exp.startTimeContext - config.audioPadStart;
    double endTime = exp.endTimeContext + config.audioPadEnd;

    QString path;
    if (startTime >= 0 && endTime >= 0 && startTime < endTime)
    {
        path = player->tempAudioClip(
            startTime,
            endTime,
            config.audioNormalize,
            config.audioDb
        );
    }
    if (path.isEmpty())
    {
        return false;
    }

    QJsonObject audObj;
    audObj[AnkiConnect::Note::DATA] = FileUtils::toBase64(path);
    QString filename = FileUtils::calculateMd5(path) + ".aac";
    audObj[AnkiConnect::Note::FILENAME] = filename;
    audObj[AnkiConnect::Note::FIELDS] = fieldCtx.fieldsWithAudioContext;

    QJsonArray audio = ctx.ankiObject[AnkiConnect::Note::AUDIO].toArray();
    audio.append(audObj);
    ctx.ankiObject[AnkiConnect::Note::AUDIO] = audio;

    QFile(path).remove();
    return true;
}

/**
 * Create the {screenshot} image and add it to the context.
 * @param      config   The config to use when generating the context file.
 * @param      fieldCtx The field context containing fields that include media.
 * @param[out] ctx      The context to add the media to.
 * @return true if the media was added to the context, false otherwise.
 */
static bool createScreenshot(
    const AnkiConfig &config,
    const FieldContext &fieldCtx,
    Anki::Note::Context &ctx)
{
    if (fieldCtx.fieldsWithScreenshot.isEmpty())
    {
        return false;
    }

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    const QString imageExt = getImageFileExtension(config.screenshotType);

    QJsonObject image;

    const bool visibility = player->getSubVisibility();
    player->setSubVisiblity(true);
    QString path = player->tempScreenshot(true, imageExt);
    image[AnkiConnect::Note::DATA] = FileUtils::toBase64(path);
    player->setSubVisiblity(visibility);

    QString filename = FileUtils::calculateMd5(path) + imageExt;
    image[AnkiConnect::Note::FILENAME] = filename;
    image[AnkiConnect::Note::FIELDS] = fieldCtx.fieldsWithScreenshot;

    if (filename == imageExt)
    {
        return false;
    }
    QJsonArray images =
        ctx.ankiObject[AnkiConnect::Note::PICTURE].toArray();
    images.append(image);
    ctx.ankiObject[AnkiConnect::Note::PICTURE] = images;

    QFile(path).remove();
    return true;
}

/**
 * Create the {screenshot-video} image and add it to the context.
 * @param      config   The config to use when generating the context file.
 * @param      fieldCtx The field context containing fields that include media.
 * @param[out] ctx      The context to add the media to.
 * @return true if the media was added to the context, false otherwise.
 */
static bool createScreenshotVideo(
    const AnkiConfig &config,
    const FieldContext &fieldCtx,
    Anki::Note::Context &ctx)
{
    if (fieldCtx.fieldsWithScreenshotVideo.isEmpty())
    {
        return false;
    }

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    const QString imageExt = getImageFileExtension(config.screenshotType);

    QJsonObject image;

    QString path = player->tempScreenshot(true, imageExt);
    image[AnkiConnect::Note::DATA] = FileUtils::toBase64(path);
    QString filename = FileUtils::calculateMd5(path) + imageExt;
    image[AnkiConnect::Note::FILENAME] = filename;
    image[AnkiConnect::Note::FIELDS] = fieldCtx.fieldsWithScreenshotVideo;

    if (filename == imageExt)
    {
        return false;
    }
    QJsonArray images =
        ctx.ankiObject[AnkiConnect::Note::PICTURE].toArray();
    images.append(image);
    ctx.ankiObject[AnkiConnect::Note::PICTURE] = images;

    QFile(path).remove();
    return true;
}

/* End Media Functions */
/* Begin Public Functions */

Anki::Note::Context Anki::Note::build(
    const AnkiConfig &config, const Term &term, bool media)
{
    Context ctx;
    ctx.setDeck(config.termDeck);
    ctx.setModel(config.termModel);
    ctx.setDuplicatePolicy(config.duplicatePolicy);
    ctx.setTags(config.tags);

    FieldContext fieldCtx;

    QJsonObject fieldsObj =
        ctx.ankiObject[AnkiConnect::Note::FIELDS].toObject();
    for (const QString &field : config.termFields.keys())
    {
        QString value = config.termFields[field].toString();
        QList<Anki::Token> tokens = Anki::tokenize(value);
        for (const Anki::Token &t : tokens)
        {
            bool tokenProcessed = processTokenCommon(
                config, term, t, field, value, fieldCtx
            );
            if (tokenProcessed)
            {
                continue;
            }
            tokenProcessed =
                processToken(config, term, t, field, value, fieldCtx);
        }
        fieldsObj[field] = value;
    }
    ctx.setFields(fieldsObj);

    if (media)
    {
        createAudio(term, fieldCtx, ctx);
        createAudioMedia(config, term, fieldCtx, ctx);
        createAudioContext(config, term, fieldCtx, ctx);
        createScreenshot(config, fieldCtx, ctx);
        createScreenshotVideo(config, fieldCtx, ctx);

        ctx.fileMap.reserve(fieldCtx.files.size());
        for (const GlossaryBuilder::FileInfo &fileInfo : fieldCtx.files)
        {
            ctx.fileMap.emplaceBack(fileInfo);
        }
    }

    return ctx;
}

Anki::Note::Context Anki::Note::build(
    const AnkiConfig &config, const Kanji &kanji, bool media)
{
    Context ctx;
    ctx.setDeck(config.kanjiDeck);
    ctx.setModel(config.kanjiModel);
    ctx.setDuplicatePolicy(config.duplicatePolicy);
    ctx.setTags(config.tags);

    FieldContext fieldCtx;

    QJsonObject fieldsObj =
        ctx.ankiObject[AnkiConnect::Note::FIELDS].toObject();
    for (const QString &field : config.kanjiFields.keys())
    {
        QString value = config.kanjiFields[field].toString();
        QList<Anki::Token> tokens = Anki::tokenize(value);
        for (const Anki::Token &t : tokens)
        {
            bool tokenProcessed = processTokenCommon(
                config, kanji, t, field, value, fieldCtx
            );
            if (tokenProcessed)
            {
                continue;
            }
            tokenProcessed = processToken(kanji, t, value);
        }
        fieldsObj[field] = value;
    }
    ctx.setFields(fieldsObj);

    if (media)
    {
        createAudioMedia(config, kanji, fieldCtx, ctx);
        createAudioContext(config, kanji, fieldCtx, ctx);
        createScreenshot(config, fieldCtx, ctx);
        createScreenshotVideo(config, fieldCtx, ctx);

        ctx.fileMap.reserve(fieldCtx.files.size());
        for (const GlossaryBuilder::FileInfo &fileInfo : fieldCtx.files)
        {
            ctx.fileMap.emplaceBack(fileInfo);
        }
    }

    return ctx;
}

/* End Public Functions */
