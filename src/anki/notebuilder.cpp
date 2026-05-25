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

#include "anki/notebuilder.h"

#include <optional>

#include <QDir>
#include <QHash>
#include <QSet>

#include "anki/ankiconnect.h"
#include "anki/marker.h"
#include "anki/markertokenizer.h"
#include "state/context.h"
#include "util/utils.h"

/**
 * @brief Enum representing possible timing sources.
 */
enum class TimingSource
{
    subtitle,
    context,
};

/**
 * @brief Holds the parameters for an audio media clip.
 */
struct AudioMediaParams
{
    /* The source of the timings */
    TimingSource source = TimingSource::subtitle;

    /* True to normalize audio, false to leave as is */
    bool normalize = false;

    /* The decibels to normalize the audio to */
    double db = -20.0;
};

/**
 * @brief Holds parameters for a screenshot.
 */
struct ScreenshotParams
{
    /* The maximum width an image can be */
    int maxWidth = -1;

    /* The maximum height an image can be */
    int maxHeight = -1;

    /* true to keep the aspect ratio during resize, false otherwise */
    bool keepAspectRatio = true;
};

/**
 * @brief Holds parameters for a video.
 */
struct VideoParams
{
    /* The source of the timings */
    TimingSource source = TimingSource::subtitle;

    /* If the video should include audio */
    bool audio = true;

    /* True if subtitles should be burned in, false otherwise */
    bool subtitles = true;

    /* True to normalize audio, false to leave as is */
    bool normalize = false;

    /* The decibels to normalize the audio to */
    double db = -20.0;
};

/**
 * @brief Holds all the fields that contain media.
 */
struct FieldContext
{
    /* Fields that contain the {audio} marker */
    QJsonArray fieldsWithAudio;

    /* Fields that contain {audio-media} marker */
    QHash<AudioMediaParams, QJsonArray> fieldsWithAudioMedia;

    /* Fields that contain the {screenshot} marker */
    QHash<ScreenshotParams, QJsonArray> fieldsWithScreenshot;

    /* Fields that contain the {screenshot-video} marker */
    QHash<ScreenshotParams, QJsonArray> fieldsWithScreenshotVideo;

    /* Fields that contain the {video} marker */
    QHash<VideoParams, QJsonArray> fieldsWithVideo;

    /* Files to include in the note request */
    QSet<GlossaryBuilder::FileInfo> files;
};

/**
 * @brief Holds are data related to glossary entries.
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

/**
 * @brief Holds information about Anki marker data.
 */
struct MarkerResult
{
    /* true if the marker was handled, false otherwise. */
    bool handled;

    /* true if the marker was a media marker, false otherwise */
    bool media;

    /* Text result of the marker */
    QString text;

    /**
     * Returns if the result is empty.
     * @return true if this result contains empty text, false otherwise.
     */
    [[nodiscard]]
    bool isEmpty() const
    {
        return !media && text.isEmpty();
    }
};

/* Begin AudioMediaParams Operators */

/**
 * @brief Check for equality between AudioMediaParams structs.
 *
 * @param rhs The left-hand side of the operator.
 * @param lhs The right-hand side of the operator.
 * @return true if AudioMediaParams are equal,
 * @return false otherwise.
 */
[[nodiscard]]
inline bool operator==(const AudioMediaParams &lhs, const AudioMediaParams &rhs)
{
    return lhs.source == rhs.source &&
        lhs.normalize == rhs.normalize &&
        lhs.db == rhs.db;
}

/**
 * @brief Calculates a hash for a AudioMediaParams.
 *
 * @param key The AudioMediaParams struct to hash.
 * @param seed The seed to use in the hash function calculation.
 * @return The value of the hash.
 */
[[nodiscard]]
inline size_t qHash(const AudioMediaParams &key, size_t seed)
{
    return qHash(static_cast<int>(key.source), seed) *
        qHash(key.normalize, seed) *
        qHash(key.db, seed);
}

/* End AudioMediaParams Operators */
/* Begin ScreenshotParams Operators */

/**
 * @brief Check for equality between ScreenshotParams structs.
 *
 * @param rhs The left-hand side of the operator.
 * @param lhs The right-hand side of the operator.
 * @return true if ScreenshotParams are equal,
 * @return false otherwise.
 */
[[nodiscard]]
inline bool operator==(const ScreenshotParams &lhs, const ScreenshotParams &rhs)
{
    return lhs.maxWidth == rhs.maxWidth &&
        lhs.maxHeight == rhs.maxHeight &&
        lhs.keepAspectRatio == rhs.keepAspectRatio;
}

/**
 * @brief Calculates a hash for a ScreenshotParams.
 *
 * @param key  The ScreenshotParams struct to hash.
 * @param seed The seed to use in the hash function calculation.
 * @return The value of the hash.
 */
[[nodiscard]]
inline size_t qHash(const ScreenshotParams &key, size_t seed)
{
    return qHash(key.maxWidth, seed) *
        qHash(key.maxHeight, seed) *
        qHash(key.keepAspectRatio, seed);
}

/* End ScreenshotParams Operators */
/* Begin VideoParams Operators */

/**
 * @brief Check for equality between VideoParams structs.
 *
 * @param rhs The left-hand side of the operator.
 * @param lhs The right-hand side of the operator.
 * @return true if VideoParams are equal,
 * @return false otherwise.
 */
[[nodiscard]]
inline bool operator==(const VideoParams &lhs, const VideoParams &rhs)
{
    return lhs.source == rhs.source &&
        lhs.audio == rhs.audio &&
        lhs.subtitles == rhs.subtitles &&
        lhs.normalize == rhs.normalize &&
        lhs.db == rhs.db;
}

/**
 * @brief Calculates a hash for a VideoParams.
 *
 * @param key The VideoParams struct to hash.
 * @param seed The seed to use in the hash function calculation.
 * @return The value of the hash.
 */
[[nodiscard]]
inline size_t qHash(const VideoParams &key, size_t seed)
{
    return qHash(static_cast<int>(key.source), seed) *
        qHash(key.audio, seed) *
        qHash(key.subtitles, seed) *
        qHash(key.normalize, seed) *
        qHash(key.db, seed);
}

/* End ScreenshotParams Operators */
/* Begin Context Methods */

void Anki::Note::Context::setDeck(const QString &deck)
{
    ankiObject[AnkiConnect::Note::DECK] = deck;
}

void Anki::Note::Context::setModel(const QString &model)
{
    ankiObject[AnkiConnect::Note::MODEL] = model;
}

void Anki::Note::Context::setTags(const QStringList &tags)
{
    QJsonArray tagsJson;
    for (const QString &tag : tags)
    {
        tagsJson.append(tag);
    }
    ankiObject[AnkiConnect::Note::TAGS] = tagsJson;
}

void Anki::Note::Context::setDuplicatePolicy(Anki::DuplicatePolicy policy)
{
    switch (policy)
    {
    case Anki::DuplicatePolicyNone:
        ankiObject[AnkiConnect::Note::OPTIONS] = QJsonObject{
            {AnkiConnect::Note::Option::ALLOW_DUP, false},
        };
        break;
    case Anki::DuplicatePolicyDifferentDeck:
        ankiObject[AnkiConnect::Note::OPTIONS] = QJsonObject{
            {
                AnkiConnect::Note::Option::SCOPE,
                AnkiConnect::Note::Option::SCOPE_CHECK_DECK
            }
        };
        break;
    case Anki::DuplicatePolicySameDeck:
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
 * @brief Replaces new line characters with the replacement value.
 *
 * @param str The string to replace new lines in.
 * @param config The Anki configuration to replace new lines with.
 * @return The string with the new lines replaced.
 */
[[nodiscard]]
static QString replaceNewLines(QString str, const AnkiProfile &profile)
{
    return str.replace('\n', profile.newlineReplacer());
}

/**
 * @brief Parses the frequency string into a double.
 *
 * @param frequency The string representation of the current frequency.
 * @return Float value of the frequency, nullopt if it could not be parsed.
 */
[[nodiscard]]
static std::optional<double> parseFrequencyValue(const QString &frequency)
{
    const QRegularExpression FLOAT_ONLY_REGEX(
        "^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)"
    );

    QRegularExpressionMatch numberMatch = FLOAT_ONLY_REGEX.match(frequency);
    if (!numberMatch.hasMatch())
    {
        if (frequency == "★")
        {
            return 20.0;
        }
        else if (frequency == "★★")
        {
            return 40.0;
        }
        else if (frequency == "★★★")
        {
            return 60.0;
        }
        else if (frequency == "★★★★")
        {
            return 80.0;
        }
        else if (frequency == "★★★★★")
        {
            return 100.0;
        }
        return {};
    }

    bool ok = false;
    double frequencyValue = numberMatch.captured().toDouble(&ok);
    if (!ok)
    {
        return {};
    }
    return frequencyValue;
}

/**
 * @brief Extracts frequency numbers from a list of frequency tags.
 *
 * @param frequencies A list of Frequency structs.
 * @return A vector of positive integers representing the frequency numbers.
 * (Only selecting the first frequency displayed by a dictionary, to avoid
 * picking secondary frequencies like kana frequencies)
 */
[[nodiscard]]
static std::vector<double> getFrequencyNumbers(
    const QList<Frequency *> &frequencies)
{
    QString previousDictionary;
    std::vector<double> frequencyNumbers;

    for (const Frequency *frequencyEntry : frequencies)
    {
        if (frequencyEntry->dictionaryInfo()->name() == previousDictionary ||
            frequencyEntry->frequency().isEmpty())
        {
            continue;
        }
        previousDictionary = frequencyEntry->dictionaryInfo()->name();

        std::optional<double> value =
            parseFrequencyValue(frequencyEntry->frequency());
        if (value)
        {
            /* Only save the first number to avoid counting secondary frequency
             * information (e.g. frequency for the full kana orthography) in the
             * aggregate measures to align with Yomitan's behavior. */
            frequencyNumbers.push_back(*value);
        }
    }

    return frequencyNumbers;
}

/**
 * @brief Converts an integer to a string or a default value if negative.
 *
 * @param value The value to convert to a string.
 * @param defaultValue The value to default to if value is negative.
 * @return Value as a string if value > 0, otherwise defaultValue.
 */
[[nodiscard]]
static QString positiveIntToQString(int value, int defaultValue)
{
    return (value < 0) ? QString::number(defaultValue) : QString::number(value);
}

/**
 * @brief Helper method to determine if the kifuku pitch accent pattern is
 * potentially applicable to a term, by checking its part-of-speech.
 *
 * @param defs The definitions of the term.
 * @return true if kifuku is applicable,
 * @return false otherwise.
 */
[[nodiscard]]
static bool isKifukuApplicable(const QList<TermDefinition *> &defs)
{
    bool canBeKifuku{false};

    for (const TermDefinition *def : defs)
    {
        for (const QString &rule : def->rules())
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
 * @brief Get the image file extension from a FileType.
 *
 * @param type The file type of the extension.
 * @return The extension of the image type.
 */
[[nodiscard]]
static QString getImageFileExtension(const Anki::FileType type)
{
    switch (type)
    {
    case Anki::FileTypePng:
        return ".png";
    case Anki::FileTypeWebp:
        return ".webp";
    case Anki::FileTypeJpeg:
    default:
        return ".jpg";
    }
}

/**
 * @brief Get the timing source from a string.
 *
 * @param source The string representation of the source.
 * @return A timing source on success, nullopt on failure.
 */
[[nodiscard]]
static std::optional<TimingSource> getTimingSource(const QString &source)
{
    constexpr const char *SOURCE_VALUE_SUBTITLE = "subtitle";
    constexpr const char *SOURCE_VALUE_CONTEXT = "context";

    if (source == SOURCE_VALUE_SUBTITLE)
    {
        return TimingSource::subtitle;
    }
    else if (source == SOURCE_VALUE_CONTEXT)
    {
        return TimingSource::context;
    }
    return {};
}

/**
 * @brief Create a AudioMediaParams from the given marker.
 *
 * @param marker The marker to create the params from.
 * @param profile The current profile to grab values from.
 * @return A populated AudioMediaParams if all arguments are valid, empty
 * otherwise.
 */
[[nodiscard]]
static std::optional<AudioMediaParams> getAudioMediaParams(
    const Anki::Tokenizer::Marker &marker,
    const AnkiProfile &profile)
{
    constexpr const char *SOURCE_KEY = "source";
    constexpr const char *NORMALIZE_KEY = "norm";
    constexpr const char *DB_KEY = "db";

    AudioMediaParams params{};
    if (marker.args.contains(SOURCE_KEY))
    {
        std::optional<TimingSource> source =
            getTimingSource(marker.args[SOURCE_KEY]);
        if (!source)
        {
            return {};
        }
        params.source = *source;
    }

    if (marker.args.contains(NORMALIZE_KEY))
    {
        params.normalize = QVariant(marker.args[NORMALIZE_KEY]).toBool();
    }
    else
    {
        params.normalize = profile.audioNormalize();
    }

    if (marker.args.contains(DB_KEY))
    {
        bool ok = false;
        params.db = marker.args[DB_KEY].toDouble(&ok);
        if (!ok)
        {
            return {};
        }
    }
    else
    {
        params.db = profile.audioDb();
    }

    return params;
}

/**
 * @brief Create a ScreenshotParams from the given marker.
 *
 * @param marker The marker to create the params from.
 * @return A populated ScreenshotParams if all arguments are valid, empty
 * otherwise.
 */
[[nodiscard]]
static std::optional<ScreenshotParams> getScreenshotParams(
    const Anki::Tokenizer::Marker &marker)
{
    constexpr const char *MAX_WIDTH_KEY = "max-width";
    constexpr const char *MAX_HEIGHT_KEY = "max-height";
    constexpr const char *KEEP_RATIO_KEY = "keep-ratio";

    ScreenshotParams params{};
    if (marker.args.contains(MAX_WIDTH_KEY))
    {
        bool ok = false;
        params.maxWidth = marker.args[MAX_WIDTH_KEY].toInt(&ok);
        if (!ok)
        {
            return {};
        }
    }
    if (marker.args.contains(MAX_HEIGHT_KEY))
    {
        bool ok = false;
        params.maxHeight = marker.args[MAX_HEIGHT_KEY].toInt(&ok);
        if (!ok)
        {
            return {};
        }
    }
    if (marker.args.contains(KEEP_RATIO_KEY))
    {
        params.keepAspectRatio = QVariant(marker.args[KEEP_RATIO_KEY]).toBool();
    }
    return params;
}

/**
 * @brief Create a VideoParams from the given marker.
 *
 * @param marker The marker to create the params from.
 * @param profile The current profile to grab values from.
 * @return A populated VideoParams if all arguments are valid, empty otherwise.
 */
[[nodiscard]]
static std::optional<VideoParams> getVideoParams(
    const Anki::Tokenizer::Marker &marker,
    const AnkiProfile &profile)
{
    constexpr const char *SOURCE_KEY = "source";
    constexpr const char *AUDIO_KEY = "audio";
    constexpr const char *SUBTITLES_KEY = "subtitles";
    constexpr const char *NORMALIZE_KEY = "norm";
    constexpr const char *DB_KEY = "db";

    VideoParams params{};
    if (marker.args.contains(SOURCE_KEY))
    {
        std::optional<TimingSource> source =
            getTimingSource(marker.args[SOURCE_KEY]);
        if (!source)
        {
            return {};
        }
        params.source = *source;
    }

    if (marker.args.contains(AUDIO_KEY))
    {
        params.audio = QVariant(marker.args[AUDIO_KEY]).toBool();
    }

    if (marker.args.contains(SUBTITLES_KEY))
    {
        params.subtitles = QVariant(marker.args[SUBTITLES_KEY]).toBool();
    }

    if (marker.args.contains(NORMALIZE_KEY))
    {
        params.normalize = QVariant(marker.args[NORMALIZE_KEY]).toBool();
    }
    else
    {
        params.normalize = profile.audioNormalize();
    }

    if (marker.args.contains(DB_KEY))
    {
        bool ok = false;
        params.db = marker.args[DB_KEY].toDouble(&ok);
        if (!ok)
        {
            return {};
        }
    }
    else
    {
        params.db = profile.audioDb();
    }

    return params;
}

/**
 * @brief Create an audio clip according to the given parameters.
 *
 * @param appCtx The application context.
 * @param profile The current Anki profile.
 * @param exp The current expression object.
 * @param params The {audio-media} parameters.
 * @param fields The fields this set of parameters appears in.
 * @param ctx The note context to add to.
 * @return true if the audio clip was added,
 * @return false otherwise.
 */
static bool createAudioMediaHelper(
    const ::Context &appCtx,
    const AnkiProfile &profile,
    const Expression &exp,
    const AudioMediaParams &params,
    const QJsonArray &fields,
    Anki::Note::Context &ctx)
{
    MpvAudioClipArgs args{};
    switch (params.source)
    {
        case TimingSource::subtitle:
            args.start = exp.startTime() - profile.audioPadStart();
            args.end = exp.endTime() + profile.audioPadEnd();
            break;

        case TimingSource::context:
            args.start = exp.contextStartTime() - profile.audioPadStart();
            args.end = exp.contextEndTime() + profile.audioPadEnd();
            break;

        default:
            return false;
    }
    args.start = std::max(args.start, 0.0);
    args.end = std::max(args.end, 0.0);
    args.normalize = params.normalize;
    args.db = params.db;

    if (args.start >= args.end)
    {
        return false;
    }
    QString path = appCtx.player()->controller()->tempAudioClip(args);
    if (path.isEmpty())
    {
        return false;
    }

    QJsonObject audObj;
    audObj[AnkiConnect::Note::DATA] = FileUtils::toBase64(path);
    QString filename = FileUtils::calculateMd5(path) + args.extension;
    audObj[AnkiConnect::Note::FILENAME] = filename;
    audObj[AnkiConnect::Note::FIELDS] = fields;

    QJsonArray audio = ctx.ankiObject[AnkiConnect::Note::AUDIO].toArray();
    audio.append(audObj);
    ctx.ankiObject[AnkiConnect::Note::AUDIO] = audio;

    QFile(path).remove();
    return true;
}

/**
 * @brief Add a screenshot with the given params to the context object.
 *
 * @param path The path of the of the image file to use.
 * @param ext The extension of the image file.
 * @param params The parameters to use to manipulate the file.
 * @param fields The fields this image appears in.
 * @param[out] ctx The context to add the image to.
 * @return true if the image was added to the context,
 * @return false otherwise.
 */
static bool createScreenshotHelper(
    QString path,
    const QString &ext,
    const ScreenshotParams &params,
    const QJsonArray &fields,
    Anki::Note::Context &ctx)
{
    bool resizeSuccessful = false;
    if (params.maxHeight != -1 || params.maxWidth != -1)
    {
        QString newPath = ImageUtils::resizeImage(
            path,
            ext,
            params.maxWidth,
            params.maxHeight,
            params.keepAspectRatio
        );
        if (!newPath.isEmpty())
        {
            path = std::move(newPath);
            resizeSuccessful = true;
        }
        else
        {
            qWarning("Could not resize screenshot");
        }
    }

    QJsonObject image;
    image[AnkiConnect::Note::DATA] = FileUtils::toBase64(path);
    QString filename = FileUtils::calculateMd5(path);
    if (filename.isEmpty())
    {
        if (resizeSuccessful)
        {
            QFile(path).remove();
        }
        return false;
    }
    image[AnkiConnect::Note::FILENAME] = filename + ext;
    image[AnkiConnect::Note::FIELDS] = fields;

    QJsonArray images = ctx.ankiObject[AnkiConnect::Note::PICTURE].toArray();
    images.append(image);
    ctx.ankiObject[AnkiConnect::Note::PICTURE] = images;

    if (resizeSuccessful)
    {
        QFile(path).remove();
    }
    return true;
}

/**
 * @brief Create a video based on the given parameters.
 *
 * @param appCtx The application context.
 * @param profile The profile to use when generating the file.
 * @param exp The current expression object.
 * @param params The parameters to create the video with.
 * @param fields The fields the video belongs to.
 * @param ctx The context to add the video to.
 * @return true on success,
 * @return false on failure.
 */
static bool createVideoHelper(
    const ::Context &appCtx,
    const AnkiProfile &profile,
    const Expression &exp,
    const VideoParams &params,
    const QJsonArray &fields,
    Anki::Note::Context &ctx)
{
    MpvVideoClipArgs args{};
    switch (params.source)
    {
        case TimingSource::subtitle:
            args.start = exp.startTime() - profile.audioPadStart();
            args.end = exp.endTime() + profile.audioPadEnd();
            break;

        case TimingSource::context:
            args.start = exp.contextStartTime() - profile.audioPadStart();
            args.end = exp.contextEndTime() + profile.audioPadEnd();
            break;

        default:
            return false;
    }
    args.start = std::max(args.start, 0.0);
    args.end = std::max(args.end, 0.0);
    args.audio = params.audio;
    args.normalize = params.normalize;
    args.db = params.db;
    args.subtitles = params.subtitles;

    if (args.start >= args.end)
    {
        return false;
    }
    QString path = appCtx.player()->controller()->tempVideoClip(args);
    if (path.isEmpty())
    {
        return false;
    }

    QJsonObject vidObj;
    vidObj[AnkiConnect::Note::DATA] = FileUtils::toBase64(path);
    QString filename = FileUtils::calculateMd5(path) + ".mp4";
    vidObj[AnkiConnect::Note::FILENAME] = filename;
    vidObj[AnkiConnect::Note::FIELDS] = fields;

    QJsonArray video = ctx.ankiObject[AnkiConnect::Note::VIDEO].toArray();
    video.append(vidObj);
    ctx.ankiObject[AnkiConnect::Note::VIDEO] = video;

    QFile(path).remove();
    return true;
}

/* End Helper Functions */
/* Begin Marker Functions */

/**
 * @brief Get the value of the {expression} marker.
 *
 * @param term The term to get the expression from.
 * @return The expression if readAsExpression is false, reading otherwise.
 */
[[nodiscard]]
static QString getExpression(const Term &term)
{
    return term.readingAsExpression() ? term.reading() : term.expression();
}

/**
 * @brief Get the value of the {reading} marker.
 *
 * @param term The term to get the reading from.
 * @return The value of the {reading} marker.
 */
[[nodiscard]]
static QString getReading(const Term &term)
{
    if (term.reading().isEmpty() || term.readingAsExpression())
    {
        return getExpression(term);
    }
    return term.reading();
}

/**
 * @brief Get the value of the {furigana} marker.
 *
 * @param term The term to get the furigana from.
 * @return The value of the {furigana} marker.
 */
[[nodiscard]]
static QString getFurigana(const Term &term)
{
    if (term.reading().isEmpty() || term.readingAsExpression())
    {
        return getExpression(term);
    }
    return QString("<ruby>%1<rt>%2</rt></ruby>")
        .arg(getExpression(term))
        .arg(getReading(term));
}

/**
 * @brief Get the value of the {furigana-plain} marker.
 *
 * @param term The term to get the furigana from.
 * @return The value of the {furigana-plain} marker.
 */
[[nodiscard]]
static QString getFuriganaPlain(const Term &term)
{
    if (term.reading().isEmpty() || term.readingAsExpression())
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
 * @param args Extended marker syntax args.
 * @return The string for the {frequency} marker.
 */
[[nodiscard]]
static QString buildFrequencies(
    const QList<Frequency *> &frequencies,
    const QHash<QString, QString> &args)
{
    if (frequencies.isEmpty())
    {
        return {};
    }

    constexpr const char *VALUE_ONLY_KEY = "value-only";
    constexpr const char *MIN_VALUE_KEY = "min-value";
    constexpr const double MIN_VALUE_DEFAULT = 9999999.0;

    bool valueOnly = false;
    bool minValueOnly = false;

    QString minFreqDictionary;
    double minFreq = MIN_VALUE_DEFAULT;
    QString minFreqStr;

    if (args.contains(VALUE_ONLY_KEY))
    {
        valueOnly = QVariant(args[VALUE_ONLY_KEY]).toBool();
    }
    if (args.contains(MIN_VALUE_KEY))
    {
        minValueOnly = QVariant(args[MIN_VALUE_KEY]).toBool();
    }

    QString freqStr;
    if (!valueOnly)
    {
        freqStr += "<ul>";
    }
    for (const Frequency *freq : frequencies)
    {
        if (!valueOnly)
        {
            freqStr += "<li>";
            freqStr += freq->dictionaryInfo()->name();
            freqStr +=
                freq->dictionaryInfo()->name().endsWith(':') ? " " : ": ";
        }
        freqStr += freq->frequency();
        if (valueOnly)
        {
            freqStr += "<br>";
        }
        else
        {
            freqStr += "</li>";
        }

        std::optional<double> freqValue =
            parseFrequencyValue(freq->frequency());
        if (minValueOnly && freqValue && *freqValue < minFreq)
        {
            minFreq = *freqValue;
            minFreqStr = freq->frequency();
            minFreqDictionary = freq->dictionaryInfo()->name();
            minFreqDictionary +=
                freq->dictionaryInfo()->name().endsWith(':') ? " " : ": ";
        }
    }
    if (!valueOnly)
    {
        freqStr += "</ul>";
    }

    if (minValueOnly)
    {
        if (!valueOnly)
        {
            freqStr = "<ul>";
            freqStr += "<li>";
            freqStr += minFreqDictionary;
            freqStr += minFreqStr;
            freqStr += "</li>";
            freqStr += "</ul>";
        }
        else
        {
            freqStr = std::move(minFreqStr);
        }
    }

    if (freqStr == "<ul></ul>")
    {
        freqStr.clear();
    }

    return freqStr;
}

/**
 * @brief Function to calculate the harmonic mean of frequencies.
 *
 * @param frequencies A list of Frequency structs.
 * @return The harmonic mean as an integer, or -1 if the list is empty.
 */
[[nodiscard]]
static int getFrequencyHarmonic(const QList<Frequency *> &frequencies)
{
    const std::vector<double> frequencyNumbers =
        getFrequencyNumbers(frequencies);

    if (frequencyNumbers.empty())
    {
        return -1;
    }

    double total = 0.0;
    for (double frequencyNum : frequencyNumbers)
    {
        if (frequencyNum != 0)
        {
            total += 1.0 / frequencyNum;
        }
    }

    return std::floor(frequencyNumbers.size() / total);
}

/**
 * @brief Function to calculate the arithmetic average of frequencies.
 *
 * @param frequencies A list of Frequency structs.
 * @return The arithmetic average as an integer, or -1 if the list is empty.
 */
[[nodiscard]]
static int getFrequencyAverage(const QList<Frequency *> &frequencies)
{
    const std::vector<double> frequencyNumbers =
        getFrequencyNumbers(frequencies);

    if (frequencyNumbers.empty())
    {
        return -1;
    }

    /* Sum the elements in the vector */
    double total = std::accumulate(
        std::begin(frequencyNumbers),
        std::end(frequencyNumbers),
        0.0
    );

    return std::floor(total / frequencyNumbers.size());
}

/**
 * @brief Build the text for {glossary}, {glossary-brief}, and
 * {glossary-compact}.
 *
 * @param definitions The definitions to build the glossary from.
 * @return The text and files for {glossary}, {glossary-brief}, and
 * {glossary-compact}.
 */
[[nodiscard]]
static GlossaryData getGlossary(const QList<TermDefinition *> &definitions)
{
    constexpr const char *GLOSSARY_OPEN =
        "<div class=\"yomitan-glossary memento-glossary\">"
            "<ol>";
    constexpr const char *GLOSSARY_CLOSE =
        "<style>"
            ".memento-glossary {"
                "text-align: left;"
            "}"

            ".memento-glossary img {"
                "display: inline-block;"
            "}"

            ".memento-glossary table {"
                "table-layout: auto;"
                "border-collapse: collapse;"
            "}"

            ".memento-glossary th {"
                "font-weight: bold;"
                "border-style: solid;"
                "padding: 0.25em;"
                "vertical-align: top;"
                "border-width: 1px;"
                "border-color: currentColor;"
            "}"

            ".memento-glossary td {"
                "border-style: solid;"
                "padding: 0.25em;"
                "vertical-align: top;"
                "border-width: 1px;"
                "border-color: currentColor;"
            "}"
        "</style></ol></div>";
    static const QString LIST_ITEM_DICTIONARY = "<li data-dictionary=\"%1\">";

    if (definitions.empty())
    {
        return {};
    }

    GlossaryData data;
    QSet<int64_t> dictionariesWithStyles;

    QString basepath =
        DirectoryUtils::getDictionaryResourceDir() + QDir::separator();

    data.glossary += GLOSSARY_OPEN;
    data.glossaryBrief += GLOSSARY_OPEN;
    data.glossaryCompact += GLOSSARY_OPEN;

    for (qsizetype i = 0; i < definitions.size(); ++i)
    {
        const TermDefinition *def = definitions[i];

        if (!def->selected())
        {
            continue;
        }

        data.glossary +=
            LIST_ITEM_DICTIONARY.arg(def->dictionaryInfo()->name());
        data.glossaryCompact +=
            LIST_ITEM_DICTIONARY.arg(def->dictionaryInfo()->name());

        data.glossary += "<i>(";
        data.glossaryCompact += "<i>(";
        for (const Tag *tag : def->tags())
        {
            data.glossary += tag->name() + ", ";
            data.glossaryCompact += tag->name() + ", ";
        }
        data.glossary += def->dictionaryInfo()->name();
        data.glossary += ")</i> ";
        data.glossaryCompact += def->dictionaryInfo()->name();
        data.glossaryCompact += ")</i> ";

        data.glossary += "<span>";
        data.glossaryCompact += "<span>";

        QStringList items = GlossaryBuilder::buildGlossary(
            def->glossary(),
            basepath + def->dictionaryInfo()->name(),
            data.files
        );
        for (const QString &item : items)
        {
            data.glossary += item;

            data.glossaryBrief += "<li>";
            data.glossaryBrief += item;
            data.glossaryBrief += "</li>";
        }
        data.glossaryCompact += items.join(" | ");

        data.glossary += "</span></li>";
        data.glossaryCompact += "</span></li>";

        const std::shared_ptr<const DictionaryStyles> styles =
            def->dictionaryInfo()->styles();
        if (styles != nullptr &&
            !dictionariesWithStyles.contains(def->dictionaryInfo()->id()))
        {
            dictionariesWithStyles.insert(def->dictionaryInfo()->id());

            data.glossary += "<style>";
            data.glossary += styles->stylesheet();
            data.glossary += "</style>";

            data.glossaryBrief += "<style>";
            data.glossaryBrief += styles->stylesheet();
            data.glossaryBrief += "</style>";

            data.glossaryCompact += "<style>";
            data.glossaryCompact += styles->stylesheet();
            data.glossaryCompact += "</style>";
        }
    }

    data.glossary += GLOSSARY_CLOSE;
    data.glossaryBrief += GLOSSARY_CLOSE;
    data.glossaryCompact += GLOSSARY_CLOSE;
    return data;
}

/**
 * @brief Build the text for {glossary}, {glossary-brief}, and
 * {glossary-compact}.
 *
 * @param definitions The definitions to build the glossary from.
 * @param args The extender marker arguments.
 * @return The text and files for {glossary}, {glossary-brief}, and
 * {glossary-compact}.
 */
[[nodiscard]]
static GlossaryData getGlossary(
    const QList<TermDefinition *> &definitions,
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

    QList<TermDefinition *> filteredDefinitions;
    std::copy_if(
        std::begin(definitions),
        std::end(definitions),
        std::back_inserter(filteredDefinitions),
        [dicId] (const TermDefinition *td) -> bool
        {
            return td->dictionaryInfo()->id() == dicId;
        }
    );

    return getGlossary(filteredDefinitions);
}

/**
 * @brief Build the {pitch} marker.
 *
 * @param pitches The pitches to use.
 * @return The HTML of the {pitch} marker.
 */
[[nodiscard]]
static QString getPitch(const QList<Pitch *> &pitches)
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

    for (const Pitch *p : pitches)
    {
        const bool multiplePitches = p->positions().size() > 1;

        /* Header */
        if (multipleDicts)
        {
            pitch += "<li><i>" + p->dictionaryInfo()->name() + "</i>";
        }
        if(multiplePitches)
        {
            pitch += "<ol>";
        }

        /* Body */
        for (const uint8_t pos : p->positions())
        {
            if (p->mora().isEmpty())
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
                pitch += p->mora().first();
                if (p->mora().size() > 1)
                {
                    pitch += PITCH_FORMAT
                        .arg(H_STYLE)
                        .arg(p->mora().join("")
                        .remove(0, p->mora().first().size()));
                }
                break;
            case 1:
                pitch += PITCH_FORMAT.arg(HL_STYLE).arg(p->mora().first());
                if (p->mora().size() > 1)
                {
                    pitch += p->mora()
                        .join("")
                        .remove(0, p->mora().first().size());
                }
                break;
            default:
            {
                QString text = p->mora().first();
                pitch += text;

                text.clear();
                for (size_t i = 1; i < pos; ++i)
                {
                    text += p->mora()[i];
                }
                if (!text.isEmpty())
                {
                    pitch += PITCH_FORMAT.arg(HL_STYLE).arg(text);
                }

                text.clear();
                for (int i = pos; i < p->mora().size(); ++i)
                {
                    text += p->mora()[i];
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
 * @brief Get the {pitch-graphs} marker.
 *
 * @param pitches The pitches to use to build the marker.
 * @return The SVGs for the {pitch-graph} marker.
 */
[[nodiscard]]
static QString getPitchGraphs(const QList<Pitch *> &pitches)
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

    for (const Pitch *p : pitches)
    {
        const bool multiplePitches = p->positions().size() > 1;

        /* Header */
        if (multipleDicts)
        {
            pitchGraph += "<li><i>" + p->dictionaryInfo()->name() + "</i>";
        }
        if(multiplePitches)
        {
            pitchGraph += "<ol>";
        }

        /* Body */
        for (const uint8_t pos : p->positions())
        {
            if (p->mora().isEmpty())
            {
                continue;
            }

            if (multiplePitches)
            {
                pitchGraph += "<li>";
            }

            pitchGraph += ImageUtils::generatePitchGraph(
                p->mora().size(), pos, "rgba(0,0,0,0)", "currentColor"
            );

            if (multiplePitches)
            {
                pitchGraph += "</li>";
            }
        }

        /* Trailer */
        if(multiplePitches)
        {
            pitchGraph += "</ol>";
        }
        if (multipleDicts)
        {
            pitchGraph += "</li>";
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
 * @brief Get the marker for {pitch-positions}.
 *
 * @param pitches The pitches to use.
 * @return The text for {pitch-position}.
 */
[[nodiscard]]
static QString getPitchPositions(const QList<Pitch *> &pitches)
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

    for (const Pitch *p : pitches)
    {
        const bool multiplePitches = p->positions().size() > 1;

        /* Header */
        if (multipleDicts)
        {
            pitchPosition += "<li><i>" + p->dictionaryInfo()->name() + "</i>";
        }
        if(multiplePitches)
        {
            pitchPosition += "<ol>";
        }

        /* Body */
        for (const uint8_t pos : p->positions())
        {
            if (p->mora().isEmpty())
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
 * @brief Get the {pitch-categories} for a term.
 *
 * @param term The term to get the categories from.
 * @return The text of the {pitch-categories} marker.
 */
[[nodiscard]]
static QString getPitchCategories(const Term &term)
{
    if (term.pitches().isEmpty())
    {
        return {};
    }

    const bool canBeKifuku = isKifukuApplicable(term.definitions());
    QSet<QString> pitchCategoriesSet{};
    for (const Pitch *p : term.pitches())
    {
        for (const uint8_t pos : p->positions())
        {
            if (p->mora().isEmpty())
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
                if (p->mora().size() == pos)
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
 * @brief Helper method for building the {selection} list.
 *
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
 * @brief Build the {tags} marker.
 *
 * @param tags The tags to use.
 * @return The HTML for the {tags} marker.
 */
[[nodiscard]]
QString getTags(const QList<Tag *> &tags)
{
    if (tags.isEmpty())
    {
        return {};
    }

    QString tagStr = "<ul>";
    for (const Tag *tag : tags)
    {
        tagStr += "<li>";
        tagStr += tag->name();
        if (!tag->notes().isEmpty())
        {
            tagStr += ": ";
            tagStr += tag->notes();
        }
        tagStr += "</li>";
    }
    tagStr += "</ul>";
    return tagStr;
}

/**
 * @brief Build the {tags-brief} marker.
 *
 * @param tags The tags to use.
 * @return The HTML for the {tags-brief} marker.
 */
[[nodiscard]]
QString getTagsBrief(const QList<Tag *> &tags)
{
    if (tags.isEmpty())
    {
        return {};
    }

    QString tagBriefStr = "<ul>";
    for (const Tag *tag : tags)
    {
        tagBriefStr += "<li>";
        tagBriefStr += tag->name();
        tagBriefStr += "</li>";
    }
    tagBriefStr += "</ul>";
    return tagBriefStr;
}

/**
 * @brief Get the {kunyomi} marker of the kanji.
 *
 * @param definitions The definitions to get the kunyomi from.
 * @return The HTML for the {kunyomi} marker.
 */
[[nodiscard]]
static QString getKunyomi(const QList<KanjiDefinition *> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    QString kunyomi;
    for (const KanjiDefinition *def : definitions)
    {
        for (const QString &str : def->kunyomi())
        {
            kunyomi += str;
            kunyomi += ", ";
        }
    }
    kunyomi.chop(2);
    return kunyomi;
}

/**
 * @brief Get the {onyomi} marker of the kanji.
 *
 * @param definitions The definitions to get the onyomi from.
 * @return The HTML for the {onyomi} marker.
 */
[[nodiscard]]
static QString getOnyomi(const QList<KanjiDefinition *> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    QString onyomi;
    for (const KanjiDefinition *def : definitions)
    {
        for (const QString &str : def->onyomi())
        {
            onyomi += str;
            onyomi += ", ";
        }
    }
    onyomi.chop(2);
    return onyomi;
}

/**
 * @brief Get the {stroke-count} marker text.
 *
 * @param definitions The definitions to use to get the marker.
 * @return The HTML of the {stroke-count} marker.
 */
[[nodiscard]]
static QString getStrokeCount(const QList<KanjiDefinition *> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    for (const KanjiDefinition *def : definitions)
    {
        for (const Tag *tag : def->stats())
        {
            if (tag->notes() == "Stroke count")
            {
                return tag->value();
            }
        }
    }
    return {};
}

/**
 * @brief Get the kanji {glossary} marker.
 *
 * @param definitions The definitions to build the glossary from.
 * @return The HTML for the {glossary} marker.
 */
[[nodiscard]]
static QString getGlossary(const QList<KanjiDefinition *> &definitions)
{
    if (definitions.empty())
    {
        return {};
    }

    QString glossary = "<ol>";
    for (const KanjiDefinition *def : definitions)
    {
        if (def->glossary().isEmpty())
        {
            continue;
        }
        glossary += "<li><i>(";
        for (const Tag *tag : def->tags())
        {
            glossary += tag->name();
            glossary += ", ";
        }
        glossary += def->dictionaryInfo()->name();
        glossary += ")</i></li>";
        glossary += "<ol>";
        for (const QString &glos : def->glossary())
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
 * @brief Get the kanji {tags} marker text.
 *
 * @param definitions The definitions to get the tags from.
 * @return The HTML for the kanji {tags} marker.
 */
[[nodiscard]]
static QString getTags(const QList<KanjiDefinition *> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    QString tags;
    for (const KanjiDefinition *def : definitions)
    {
        tags += getTags(def->tags());
    }
    return tags;
}

/**
 * @brief Get the kanji {tags-brief} marker
 *
 * @param definitions The definitions to get the tags from.
 * @return The HTML for the kanji {tags-brief} marker.
 */
[[nodiscard]]
static QString getTagsBrief(const QList<KanjiDefinition *> &definitions)
{
    if (definitions.isEmpty())
    {
        return {};
    }

    QString tagsBrief;
    for (const KanjiDefinition *def : definitions)
    {
        tagsBrief += getTagsBrief(def->tags());
    }
    return tagsBrief;
}

/**
 * @brief Processes kanji specific markers.
 *
 * @param kanji The kanji to build the token with.
 * @param marker The marker to handle.
 * @return A token result containing information for the token.
 */
[[nodiscard]]
static MarkerResult processMarker(
    const Kanji &kanji,
    const Anki::Tokenizer::Marker &marker)
{
    MarkerResult result{};
    result.handled = true;
    if (marker.marker == Anki::Marker::CHARACTER)
    {
        result.text = kanji.character();
        return result;
    }
    else if (marker.marker == Anki::Marker::KUNYOMI)
    {
        result.text = getKunyomi(kanji.definitions());
        return result;
    }
    else if (marker.marker == Anki::Marker::ONYOMI)
    {
        result.text = getOnyomi(kanji.definitions());
        return result;
    }
    else if (marker.marker == Anki::Marker::STROKE_COUNT)
    {
        result.text = getStrokeCount(kanji.definitions());
        return result;
    }
    else if (marker.marker == Anki::Marker::GLOSSARY)
    {
        result.text = getGlossary(kanji.definitions());
        return result;
    }
    else if (marker.marker == Anki::Marker::TAGS)
    {
        result.text = getTags(kanji.definitions());
        return result;
    }
    else if (marker.marker == Anki::Marker::TAGS_BRIEF)
    {
        result.text = getTagsBrief(kanji.definitions());
        return result;
    }
    result.handled = false;
    return result;
}

/**
 * @brief Process term specific markers.
 *
 * @param profile The profile to use.
 * @param term The term to build the token with.
 * @param marker The marker to handle.
 * @param field The field this token belongs to.
 * @param[out] fieldCtx The field context to update.
 * @return A token result containing information for the token.
 */
[[nodiscard]]
static MarkerResult processMarker(
    const AnkiProfile &profile,
    const Term &term,
    const Anki::Tokenizer::Marker &marker,
    const QString &field,
    FieldContext &fieldCtx)
{
    MarkerResult result{};
    result.handled = true;
    if (marker.marker == Anki::Marker::AUDIO)
    {
        fieldCtx.fieldsWithAudio.append(field);
        result.media = true;
        return result;
    }
    else if (marker.marker == Anki::Marker::EXPRESSION)
    {
        result.text = getExpression(term);
        return result;
    }
    else if (marker.marker == Anki::Marker::FURIGANA)
    {
        result.text = getFurigana(term);
        return result;
    }
    else if (marker.marker == Anki::Marker::FURIGANA_PLAIN)
    {
        result.text = getFuriganaPlain(term);
        return result;
    }
    else if (marker.marker == Anki::Marker::READING)
    {
        result.text = getReading(term);
        return result;
    }
    else if (marker.marker == Anki::Marker::GLOSSARY)
    {
        GlossaryData data = getGlossary(term.definitions(), marker.args);
        result.text = data.glossary;
        fieldCtx.files.unite(data.files);
        return result;
    }
    else if (marker.marker == Anki::Marker::GLOSSARY_BRIEF)
    {
        GlossaryData data = getGlossary(term.definitions(), marker.args);
        result.text = data.glossaryBrief;
        fieldCtx.files.unite(data.files);
        return result;
    }
    else if (marker.marker == Anki::Marker::GLOSSARY_COMPACT)
    {
        GlossaryData data = getGlossary(term.definitions(), marker.args);
        result.text = data.glossaryCompact;
        fieldCtx.files.unite(data.files);
        return result;
    }
    else if (marker.marker == Anki::Marker::PITCH ||
        marker.marker == Anki::Marker::PITCH_ALT)
    {
        result.text = getPitch(term.pitches());
        return result;
    }
    else if (marker.marker == Anki::Marker::PITCH_CATEGORIES ||
        marker.marker == Anki::Marker::PITCH_CATEGORIES_ALT)
    {
        result.text = getPitchCategories(term);
        return result;
    }
    else if (marker.marker == Anki::Marker::PITCH_GRAPHS ||
        marker.marker == Anki::Marker::PITCH_GRAPHS_ALT1 ||
        marker.marker == Anki::Marker::PITCH_GRAPHS_ALT2)
    {
        result.text = getPitchGraphs(term.pitches());
        return result;
    }
    else if (marker.marker == Anki::Marker::PITCH_POSITIONS ||
        marker.marker == Anki::Marker::PITCH_POSITIONS_ALT1 ||
        marker.marker == Anki::Marker::PITCH_POSITIONS_ALT2)
    {
        result.text = getPitchPositions(term.pitches());
        return result;
    }
    else if (marker.marker == Anki::Marker::SELECTION)
    {
        result.text = replaceNewLines(getSelection(term.selection()), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::TAGS)
    {
        result.text = getTags(term.tags());
        return result;
    }
    else if (marker.marker == Anki::Marker::TAGS_BRIEF)
    {
        result.text = getTagsBrief(term.tags());
        return result;
    }
    result.handled = false;
    return result;
}

/**
 * @brief Handle common markers for a value.
 *
 * @param profile The profile to build to.
 * @param exp The details of the current expression.
 * @param marker The marker to process in the field.
 * @param field The name of the field.
 * @param[out] fieldCtx The context containing all the media fields.
 * @return A token result containing information for the token.
 */
[[nodiscard]]
static MarkerResult processMarkerCommon(
    const AnkiProfile &profile,
    const Expression &exp,
    const Anki::Tokenizer::Marker &marker,
    const QString &field,
    FieldContext &fieldCtx)
{
    /* If the term never occurs in the corpus of any loaded frequency
     * dictionary, assume it is a very rare word. (The higher the ranking, the
     * rarer the term) */
    constexpr int DEFAULT_FREQ_RANK = 9999999;
    constexpr int DEFAULT_FREQ_OCCURRENCE = 0;

    MarkerResult result{};
    result.handled = true;
    if (marker.marker == Anki::Marker::AUDIO_MEDIA)
    {
        std::optional<AudioMediaParams> params =
            getAudioMediaParams(marker, profile);
        if (!params)
        {
            result.handled = false;
            return result;
        }
        fieldCtx.fieldsWithAudioMedia[*params].append(field),
        result.media = true;
        return result;
    }
    else if (marker.marker == Anki::Marker::AUDIO_CONTEXT)
    {
        std::optional<AudioMediaParams> params =
            getAudioMediaParams(marker, profile);
        if (!params)
        {
            result.handled = false;
            return result;
        }
        params->source = TimingSource::context;
        fieldCtx.fieldsWithAudioMedia[*params].append(field);
        result.media = true;
        return result;
    }
    else if (marker.marker == Anki::Marker::SCREENSHOT)
    {
        std::optional<ScreenshotParams> params = getScreenshotParams(marker);
        if (!params)
        {
            result.handled = false;
            return result;
        }
        fieldCtx.fieldsWithScreenshot[*params].append(field),
        result.media = true;
        return result;
    }
    else if (marker.marker == Anki::Marker::SCREENSHOT_VIDEO)
    {
        std::optional<ScreenshotParams> params = getScreenshotParams(marker);
        if (!params)
        {
            result.handled = false;
            return result;
        }
        fieldCtx.fieldsWithScreenshotVideo[*params].append(field),
        result.media = true;
        return result;
    }
    else if (marker.marker == Anki::Marker::VIDEO)
    {
        std::optional<VideoParams> params = getVideoParams(marker, profile);
        if (!params)
        {
            result.handled = false;
            return result;
        }
        fieldCtx.fieldsWithVideo[*params].append(field);
        result.media = true;
        return result;
    }
    else if (marker.marker == Anki::Marker::TITLE)
    {
        result.text = replaceNewLines(exp.title(), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::CLIPBOARD)
    {
        result.text = replaceNewLines(exp.clipboard(), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::CLOZE_BODY)
    {
        result.text = replaceNewLines(exp.clozeBody(), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::CLOZE_PREFIX)
    {
        result.text = replaceNewLines(exp.clozePrefix(), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::CLOZE_SUFFIX)
    {
        result.text = replaceNewLines(exp.clozeSuffix(), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::SUBTITLE ||
        marker.marker == Anki::Marker::SUBTITLE_ALT)
    {
        result.text = replaceNewLines(exp.subtitle(), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::SUBTITLE_SEC ||
        marker.marker == Anki::Marker::SUBTITLE_SEC_ALT)
    {
        result.text = replaceNewLines(exp.subtitle2(), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::CONTEXT)
    {
        result.text = replaceNewLines(exp.context(), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::CONTEXT_SEC)
    {
        result.text = replaceNewLines(exp.context2(), profile);
        return result;
    }
    else if (marker.marker == Anki::Marker::FREQUENCIES)
    {
        result.text = buildFrequencies(exp.frequencies(), marker.args);
        return result;
    }
    else if (marker.marker == Anki::Marker::FREQ_HARMONIC_RANK)
    {
        result.text = positiveIntToQString(
            getFrequencyHarmonic(exp.frequencies()), DEFAULT_FREQ_RANK
        );
        return result;
    }
    else if (marker.marker == Anki::Marker::FREQ_HARMONIC_OCCU)
    {
        result.text = positiveIntToQString(
            getFrequencyHarmonic(exp.frequencies()), DEFAULT_FREQ_OCCURRENCE
        );
        return result;
    }
    else if (marker.marker == Anki::Marker::FREQ_AVERAGE_RANK)
    {
        result.text = positiveIntToQString(
            getFrequencyAverage(exp.frequencies()), DEFAULT_FREQ_RANK
        );
        return result;
    }
    else if (marker.marker == Anki::Marker::FREQ_AVERAGE_OCCU)
    {
        result.text = positiveIntToQString(
            getFrequencyAverage(exp.frequencies()), DEFAULT_FREQ_OCCURRENCE
        );
        return result;
    }
    result.handled = false;
    return result;
}

/* End Marker Functions */
/* Begin Media Functions */

/**
 * @brief Add the {audio} files to the context.
 *
 * @param term The term to use to create audio parameters.
 * @param fieldCtx The field context to get data from.
 * @param[out] ctx The note context to add the audio to.
 * @return true if audio was added to the context,
 * @return false otherwise.
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
    audObj[AnkiConnect::Note::URL] = QString(term.audioUrl())
        .replace(REPLACE_EXPRESSION, getExpression(term))
        .replace(REPLACE_READING, getReading(term));
    audObj[AnkiConnect::Note::FILENAME] = AUDIO_FILENAME_FORMAT_STRING
        .arg(term.audioSourceName())
        .arg(term.reading())
        .arg(term.expression())
        .replace(' ', '_');
    audObj[AnkiConnect::Note::FIELDS] = fieldCtx.fieldsWithAudio;
    audObj[AnkiConnect::Note::SKIPHASH] = term.audioSkipHash();

    QJsonArray audio = ctx.ankiObject[AnkiConnect::Note::AUDIO].toArray();
    audio.append(audObj);
    ctx.ankiObject[AnkiConnect::Note::AUDIO] = audio;

    return true;
}

/**
 * @brief Create the audio media file and add it to the context.
 *
 * @param appCtx The context of the application.
 * @param profile The profile to use when generating the media file.
 * @param exp The expression to use when build the {audio-media}.
 * @param fieldCtx The field context containing fields that include media.
 * @param[out] ctx The context to add the media to.
 * @return true if the media was added to the context,
 * @return false otherwise.
 */
static bool createAudioMedia(
    const ::Context &appCtx,
    const AnkiProfile &profile,
    const Expression &exp,
    const FieldContext &fieldCtx,
    Anki::Note::Context &ctx)
{
    if (fieldCtx.fieldsWithAudioMedia.isEmpty())
    {
        return false;
    }

    bool success = true;
    for (const auto &[params, fields] :
            fieldCtx.fieldsWithAudioMedia.asKeyValueRange())
    {
        success = success &&
            createAudioMediaHelper(appCtx, profile, exp, params, fields, ctx);
    }
    return success;
}

/**
 * @brief Create the {screenshot} image and add it to the context.
 *
 * @param appCtx The context of the application.
 * @param profile The profile to use when generating the context file.
 * @param fieldCtx The field context containing fields that include media.
 * @param[out] ctx The context to add the media to.
 * @return true if the media was added to the context,
 * @return false otherwise.
 */
static bool createScreenshot(
    const ::Context &appCtx,
    const AnkiProfile &profile,
    const FieldContext &fieldCtx,
    Anki::Note::Context &ctx)
{
    if (fieldCtx.fieldsWithScreenshot.isEmpty())
    {
        return false;
    }

    const QString imageExt = getImageFileExtension(profile.screenshotType());

    const bool visibility = appCtx.player()->state()->subtitle()->visible();
    appCtx.player()->controller()->setSubtitleVisibility(true);
    QString path =
        appCtx.player()->controller()->tempScreenshot(true, imageExt);
    appCtx.player()->controller()->setSubtitleVisibility(visibility);

    for (const auto &[params, fields] :
        fieldCtx.fieldsWithScreenshot.asKeyValueRange())
    {
        createScreenshotHelper(path, imageExt, params, fields, ctx);
    }

    QFile(path).remove();
    return true;
}

/**
 * @brief Create the {screenshot-video} image and add it to the context.
 *
 * @param appCtx The context of the application.
 * @param profile The profile to use when generating the context file.
 * @param fieldCtx The field context containing fields that include media.
 * @param[out] ctx The context to add the media to.
 * @return true if the media was added to the context,
 * @return false otherwise.
 */
static bool createScreenshotVideo(
    const ::Context &appCtx,
    const AnkiProfile &profile,
    const FieldContext &fieldCtx,
    Anki::Note::Context &ctx)
{
    if (fieldCtx.fieldsWithScreenshotVideo.isEmpty())
    {
        return false;
    }

    const QString imageExt = getImageFileExtension(profile.screenshotType());

    QString path =
        appCtx.player()->controller()->tempScreenshot(false, imageExt);
    for (const auto &[params, fields] :
        fieldCtx.fieldsWithScreenshotVideo.asKeyValueRange())
    {
        createScreenshotHelper(path, imageExt, params, fields, ctx);
    }

    QFile(path).remove();
    return true;
}

/**
 * @brief Create the video file and add it to the context.
 *
 * @param appCtx The context of the application.
 * @param profile The profile to use when generating the context file.
 * @param exp The expression to use when build the {audio-context}.
 * @param fieldCtx The field context containing fields that include media.
 * @param[out] ctx The context to add the media to.
 * @return true if the media was added to the context,
 * @return false otherwise.
 */
static bool createVideo(
    const ::Context &appCtx,
    const AnkiProfile &profile,
    const Expression &exp,
    const FieldContext &fieldCtx,
    Anki::Note::Context &ctx)
{
    if (fieldCtx.fieldsWithVideo.isEmpty())
    {
        return false;
    }

    bool success = true;
    for (const auto &[params, fields] :
        fieldCtx.fieldsWithVideo.asKeyValueRange())
    {
        success = success &&
            createVideoHelper(appCtx, profile, exp, params, fields, ctx);
    }
    return success;
}

/* End Media Functions */
/* Begin Public Functions */

Anki::Note::Context Anki::Note::build(
    const ::Context &appCtx,
    const AnkiProfile &profile,
    const Term &term,
    bool media)
{
    Anki::Note::Context ctx;
    ctx.setDeck(profile.termDeck());
    ctx.setModel(profile.termModel());
    ctx.setDuplicatePolicy(profile.duplicatePolicy());
    ctx.setTags(profile.tags());

    FieldContext fieldCtx;

    QJsonObject fieldsObj =
        ctx.ankiObject[AnkiConnect::Note::FIELDS].toObject();
    for (const AnkiField &field : profile.termFields()->items())
    {
        QString value = field.value;
        QList<Anki::Tokenizer::Token> tokens = Anki::Tokenizer::tokenize(value);
        for (const Anki::Tokenizer::Token &token : tokens)
        {
            MarkerResult finalResult{};
            for (const Anki::Tokenizer::Marker &marker : token.markers)
            {
                MarkerResult result = processMarkerCommon(
                    profile, term, marker, field.name, fieldCtx
                );
                if (!result.isEmpty())
                {
                    finalResult = std::move(result);
                    break;
                }
                else if (result.handled)
                {
                    finalResult = std::move(result);
                    continue;
                }

                result = processMarker(
                    profile, term, marker, field.name, fieldCtx
                );
                if (!result.isEmpty())
                {
                    finalResult = std::move(result);
                    break;
                }
                else if (result.handled)
                {
                    finalResult = std::move(result);
                    continue;
                }
            }
            if (finalResult.handled)
            {
                value.replace(token.raw, finalResult.text);
            }
        }
        fieldsObj[field.name] = value;
    }
    ctx.setFields(fieldsObj);

    if (media)
    {
        createAudio(term, fieldCtx, ctx);
        createAudioMedia(appCtx, profile, term, fieldCtx, ctx);
        createScreenshot(appCtx, profile, fieldCtx, ctx);
        createScreenshotVideo(appCtx, profile, fieldCtx, ctx);
        createVideo(appCtx, profile, term, fieldCtx, ctx);

        ctx.fileMap.reserve(fieldCtx.files.size());
        for (const GlossaryBuilder::FileInfo &fileInfo : fieldCtx.files)
        {
            ctx.fileMap.emplaceBack(fileInfo);
        }
    }

    return ctx;
}

Anki::Note::Context Anki::Note::build(
    const ::Context &appCtx,
    const AnkiProfile &profile,
    const Kanji &kanji,
    bool media)
{
    Anki::Note::Context ctx;
    ctx.setDeck(profile.kanjiDeck());
    ctx.setModel(profile.kanjiModel());
    ctx.setDuplicatePolicy(profile.duplicatePolicy());
    ctx.setTags(profile.tags());

    FieldContext fieldCtx;

    QJsonObject fieldsObj =
        ctx.ankiObject[AnkiConnect::Note::FIELDS].toObject();
    for (const AnkiField &field : profile.kanjiFields()->items())
    {
        QString value = field.value;
        QList<Anki::Tokenizer::Token> tokens = Anki::Tokenizer::tokenize(value);
        for (const Anki::Tokenizer::Token &token : tokens)
        {
            MarkerResult finalResult{};
            for (const Anki::Tokenizer::Marker &marker : token.markers)
            {
                MarkerResult result = processMarkerCommon(
                    profile, kanji, marker, field.name, fieldCtx
                );
                if (!result.isEmpty())
                {
                    finalResult = std::move(result);
                    break;
                }
                else if (result.handled)
                {
                    finalResult = std::move(result);
                    continue;
                }

                result = processMarker(kanji, marker);
                if (!result.isEmpty())
                {
                    finalResult = std::move(result);
                    break;
                }
                else if (result.handled)
                {
                    finalResult = std::move(result);
                    continue;
                }
            }
            if (finalResult.handled)
            {
                value.replace(token.raw, finalResult.text);
            }
        }
        fieldsObj[field.name] = value;
    }
    ctx.setFields(fieldsObj);

    if (media)
    {
        createAudioMedia(appCtx, profile, kanji, fieldCtx, ctx);
        createScreenshot(appCtx, profile, fieldCtx, ctx);
        createScreenshotVideo(appCtx, profile, fieldCtx, ctx);
        createVideo(appCtx, profile, kanji, fieldCtx, ctx);

        ctx.fileMap.reserve(fieldCtx.files.size());
        for (const GlossaryBuilder::FileInfo &fileInfo : fieldCtx.files)
        {
            ctx.fileMap.emplaceBack(fileInfo);
        }
    }

    return ctx;
}

/* End Public Functions */
