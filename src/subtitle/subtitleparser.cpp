////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#include "subtitle/subtitleparser.h"

#include <QDebug>
#include <QSet>
#include <QUrl>

#include <limits>
#include <ranges>

/**
 * Information about an SRT subtitle.
 */
struct SrtInfo : public SubtitleEntry
{
    /* The position of this subtitle. */
    int32_t position;
};

std::vector<SubtitleEntry> SubtitleParser::parseSubtitles(QString path) const
{
    std::vector<SubtitleEntry> subtitles;

    QUrl url(path);
    QFile file(url.isLocalFile() ? url.toLocalFile() : path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug(
            "Subtitle Parser: Could not open file %s: %s",
            qUtf8Printable(path),
            qUtf8Printable(file.errorString())
        );
        return subtitles;
    }

    QString lowerPath = path.toLower();
    if (lowerPath.endsWith(".ass"))
    {
        if (!parseASS(file, subtitles))
        {
            return {};
        }
    }
    else if (lowerPath.endsWith(".srt"))
    {
        if (!parseSRT(file, subtitles))
        {
            return {};
        }
    }
    else if (lowerPath.endsWith(".vtt"))
    {
        if (!parseVTT(file, subtitles))
        {
            return {};
        }
    }

    return subtitles;
}

bool SubtitleParser::parseASS(
    QFile &file, std::vector<SubtitleEntry> &out) const
{
    constexpr const char *ASS_HEADER = "[Script Info]";
    constexpr const char *EVENT_HEADER = "[Events]";

    constexpr const char *FORMAT_PREFIX = "Format: ";
    constexpr const char *DIALOGUE_PREFIX = "Dialogue: ";

    constexpr const char *START_FORMAT = "Start";
    constexpr const char *END_FORMAT = "End";
    constexpr const char *TEXT_FORMAT = "Text";

    QTextStream in(&file);
    int lineNumber = 0;

    /* Make sure the file isn't empty */
    if (in.atEnd())
    {
        qDebug() << "ASS Parser: Empty file";
        return false;
    }

    /* Check for the header */
    ++lineNumber;
    QString currentLine = in.readLine();
    if (currentLine.trimmed() != ASS_HEADER)
    {
        qDebug() << "ASS Parser: Missing ASS header";
        qDebug() << "Line Number " << lineNumber;
        qDebug() << currentLine;
        return false;
    }

    /* Skip to the [Events] section */
    while (!in.atEnd())
    {
        ++lineNumber;
        currentLine = in.readLine();
        if (currentLine.trimmed() == EVENT_HEADER)
        {
            break;
        }
    }

    /* There is no [Events] section. This is odd but valid. */
    if (in.atEnd())
    {
        return true;
    }

    /* Get format section */
    ++lineNumber;
    currentLine = in.readLine();
    if (!currentLine.startsWith(FORMAT_PREFIX))
    {
        qDebug() << "ASS Parser: Missing Format line in the [Events] section";
        qDebug() << "Line Number " << lineNumber;
        qDebug() << currentLine;
        return false;
    }
    int startIndex = -1;
    int endIndex = -1;
    int textIndex = -1;
    QStringList format =
        currentLine
            .right(currentLine.size() - sizeof(FORMAT_PREFIX) + 1)
            .split(',');
    for (int i = 0; i < format.size(); ++i)
    {
        if (format[i].trimmed() == START_FORMAT)
        {
            if (startIndex != -1)
            {
                qDebug() << "ASS Parser: Start format redefinition";
                qDebug() << "Line Number " << lineNumber;
                return false;
            }
            startIndex = i;
        }
        else if (format[i].trimmed() == END_FORMAT)
        {
            if (endIndex != -1)
            {
                qDebug() << "ASS Parser: End format redefinition";
                qDebug() << "Line Number " << lineNumber;
                return false;
            }
            endIndex = i;
        }
        else if (format[i].trimmed() == TEXT_FORMAT)
        {
            if (textIndex != -1)
            {
                qDebug() << "ASS Parser: Text format redefinition";
                qDebug() << "Line Number " << lineNumber;
                return false;
            }
            textIndex = i;
        }
    }
    if (startIndex == -1)
    {
        qDebug() << "ASS Parser: Format missing start section";
        qDebug() << "Line Number " << lineNumber;
        return false;
    }
    else if (endIndex == -1)
    {
        qDebug() << "ASS Parser: Format missing end section";
        qDebug() << "Line Number " << lineNumber;
        return false;
    }
    else if (textIndex == -1)
    {
        qDebug() << "ASS Parser: Format missing text section";
        qDebug() << "Line Number " << lineNumber;
        return false;
    }

    /* Get dialogue */
    while (!in.atEnd())
    {
        /* Skip non-dialogue lines */
        ++lineNumber;
        currentLine = in.readLine();
        if (!currentLine.startsWith(DIALOGUE_PREFIX))
        {
            continue;
        }

        /* Split dialogue line */
        QStringList dialogue =
            currentLine
                .right(currentLine.size() - sizeof(DIALOGUE_PREFIX) + 1)
                .split(',');
        if (dialogue.size() < format.size())
        {
            qDebug() << "ASS Parser: Dialogue-Format mismatch";
            qDebug() << "Line Number " << lineNumber;
            return false;
        }

        /* Construct the SubtitleInfo */
        SubtitleEntry entry{};

        /* Get timings */
        bool ok = false;
        entry.start = timecodeToDouble(dialogue[startIndex], &ok);
        if (!ok || entry.start < 0)
        {
            qDebug() << "ASS Parser: Invalid start time";
            qDebug() << "Line Number " << lineNumber;
            qDebug() << dialogue[startIndex];
            return false;
        }
        entry.end = timecodeToDouble(dialogue[endIndex], &ok);
        if (!ok || entry.end < entry.start)
        {
            qDebug() << "ASS Parser: Invalid end time";
            qDebug() << "Line Number " << lineNumber;
            qDebug() << dialogue[endIndex];
            return false;
        }

        /* Get Text */
        entry.text = dialogue[textIndex];
        for (int i = textIndex + 1; i < dialogue.size(); ++i)
        {
            entry.text += ',';
            entry.text += dialogue[i];
        }
        entry.text.remove(m_assFilter);
        entry.text.replace(m_assNewLineReplacer, "\n");

        /* Throw out empty subtitles */
        if (entry.text.trimmed().isEmpty())
        {
            continue;
        }

        /* Add the subtitle */
        out.emplace_back(std::move(entry));
    }

    std::ranges::sort(
        out,
        [] (const SubtitleEntry &lhs, const SubtitleEntry &rhs)
        {
            return lhs.start < rhs.start;
        }
    );
    return true;
}

bool SubtitleParser::parseSRT(
    QFile &file, std::vector<SubtitleEntry> &out) const
{
    constexpr size_t TIMING_START_INDEX = 0;
    constexpr size_t TIMING_ARROW_INDEX = 1;
    constexpr size_t TIMING_END_INDEX = 2;

    constexpr const char *TIMING_ARROW = "-->";

    std::vector<SrtInfo> subs;

    QTextStream in(&file);
    int lineNumber = 0;
    while (!in.atEnd())
    {
        SrtInfo entry{};

        /* Skip all new lines */
        ++lineNumber;
        QString currentLine = in.readLine();
        while (!in.atEnd() && currentLine.isEmpty())
        {
            ++lineNumber;
            currentLine = in.readLine();
        }
        if (in.atEnd())
        {
            break;
        }

        /* Get the position */
        bool ok = false;
        entry.position = currentLine.trimmed().toUInt(&ok);
        if (!ok || entry.position < 0)
        {
            qDebug() << "SRT Parser: Invalid position";
            qDebug() << "Line Number " << lineNumber;
            qDebug() << currentLine;
            return false;
        }

        /* Get the timings */
        if (in.atEnd())
        {
            qDebug() << "SRT Parser: Unexpected file end after position";
            qDebug() << "Line Number " << lineNumber;
            return false;
        }
        ++lineNumber;
        currentLine = in.readLine();
        QStringList timing = currentLine.trimmed().split(' ');
        if (timing.size() != 3)
        {
            qDebug() << "SRT Parser: Invalid timing";
            qDebug() << "Line Number " << lineNumber;
            qDebug() << currentLine;
            return false;
        }
        if (timing[TIMING_ARROW_INDEX] != TIMING_ARROW)
        {
            qDebug() << "SRT Parser: Missing timing arrow";
            qDebug() << "Line Number " << lineNumber;
            qDebug() << currentLine;
            return false;
        }
        entry.start = timecodeToDouble(timing[TIMING_START_INDEX], &ok);
        if (!ok || entry.start < 0.0)
        {
            qDebug() << "SRT Parser: Invalid start time";
            qDebug() << "Line Number " << lineNumber;
            qDebug() << timing[TIMING_START_INDEX];
            return false;
        }
        entry.end = timecodeToDouble(timing[TIMING_END_INDEX], &ok);
        if (!ok || entry.end < entry.start)
        {
            qDebug() << "SRT Parser: Invalid end time";
            qDebug() << "Line Number " << lineNumber;
            qDebug() << timing[TIMING_END_INDEX];
            return false;
        }

        /* Get the lines */
        if (in.atEnd())
        {
            qDebug() << "SRT Parser: Unexpected file end after timings";
            qDebug() << "Line Number " << lineNumber;
            return false;
        }
        while (!in.atEnd())
        {
            if (++lineNumber; (currentLine = in.readLine()).isEmpty())
            {
                break;
            }
            entry.text += currentLine;
            entry.text += '\n';
        }
        entry.text.chop(1);

        /* Filter out SRT formatting */
        entry.text.remove(m_srtFilter);

        /* Don't add if the subtitle is only whitespace */
        if (entry.text.trimmed().isEmpty())
        {
            continue;
        }

        /* Append to the output list */
        subs.emplace_back(std::move(entry));
    }

    std::ranges::sort(
        subs,
        [] (const SrtInfo &lhs, const SrtInfo &rhs)
        {
            if (lhs.start == rhs.start)
            {
                return lhs.position < rhs.position;
            }
            return lhs.start < rhs.start;
        }
    );
    out = {
        std::make_move_iterator(std::begin(subs)),
        std::make_move_iterator(std::end(subs))
    };
    return true;
}

bool SubtitleParser::parseVTT(
    QFile &file, std::vector<SubtitleEntry> &out) const
{
    constexpr size_t TIMING_START_INDEX = 0;
    constexpr size_t TIMING_ARROW_INDEX = 1;
    constexpr size_t TIMING_END_INDEX = 2;

    constexpr const char *VTT_HEADER = "WEBVTT";
    constexpr const char *TIMING_ARROW = "-->";

    int lineNumber = 0;
    QTextStream in(&file);

    /* Exit if the file is empty */
    if (in.atEnd())
    {
        qDebug() << "VTT Parser: Unexpected empty file";
        return false;
    }
    /* Exit if the file is missing the header */
    else if (++lineNumber; !in.readLine().startsWith(VTT_HEADER))
    {
        qDebug() << "VTT Parser: Missing VTT header";
        qDebug() << "Line Number " << lineNumber;
        return false;
    }

    /* Skip past header info */
    while (!in.atEnd())
    {
        if (++lineNumber; in.readLine().trimmed().isEmpty())
        {
            break;
        }
    }

    /* Read subtitles */
    while (!in.atEnd())
    {
        ++lineNumber;
        QString currentLine = in.readLine().trimmed();
        /* Skip empty lines */
        if (currentLine.isEmpty())
        {
            continue;
        }
        /* Skip non-subtitle sections */
        else if (m_vttSections.contains(currentLine.split(' ')[0]))
        {
            while (!in.atEnd())
            {
                ++lineNumber;
                currentLine = in.readLine();
                if (currentLine.trimmed().isEmpty())
                {
                    break;
                }
            }
            continue;
        }

        SubtitleEntry entry{};

        /* Get timings */
        QStringList timings = currentLine.split(' ');
        if (timings.size() < 3 || timings[TIMING_ARROW_INDEX] != TIMING_ARROW)
        {
            if (in.atEnd())
            {
                qDebug() << "VTT Parser: Unexpected file end after cue";
                qDebug() << "Line Number " << lineNumber;
                return false;
            }
            ++lineNumber;
            currentLine = in.readLine();
            timings = currentLine.split(' ');
            if (timings.size() < 3 || timings[TIMING_ARROW_INDEX] != TIMING_ARROW)
            {
                qDebug() << "VTT Parser: Invalid timing line";
                qDebug() << "Line Number " << lineNumber;
                qDebug() << currentLine;
                return false;
            }
        }
        bool ok = false;
        entry.start = timecodeToDouble(timings[TIMING_START_INDEX], &ok);
        if (!ok || entry.start < 0.0)
        {
            qDebug() << "VTT Parser: Invalid start time";
            qDebug() << "Line Number " << lineNumber;
            qDebug() << timings[TIMING_START_INDEX];
            return false;
        }
        entry.end = timecodeToDouble(timings[TIMING_END_INDEX], &ok);
        if (!ok || entry.end < entry.start)
        {
            qDebug() << "VTT Parser: Invalid end time";
            qDebug() << "Line Number " << lineNumber;
            qDebug() << timings[TIMING_END_INDEX];
            return false;
        }

        /* Get the lines */
        if (in.atEnd())
        {
            qDebug() << "VTT Parser: Unexpected file end after timings";
            qDebug() << "Line Number " << lineNumber;
            return false;
        }
        while (!in.atEnd())
        {
            if (++lineNumber; (currentLine = in.readLine()).isEmpty())
            {
                break;
            }
            entry.text += currentLine;
            entry.text += '\n';
        }
        entry.text.chop(1);

        /* Filter out VTT angle brace formatting */
        entry.text.remove(m_angleBraceCleaner);

        /* Don't add if the subtitle is only whitespace */
        if (entry.text.trimmed().isEmpty())
        {
            continue;
        }

        /* Append to the output list */
        out.emplace_back(std::move(entry));
    }

    std::ranges::sort(
        out,
        [] (const SubtitleEntry &lhs, const SubtitleEntry &rhs)
        {
            return lhs.start < rhs.start;
        }
    );
    return true;
}

double SubtitleParser::timecodeToDouble(const QString &timecode, bool *ok) const
{
    constexpr int SECONDS_IN_HOUR = 3600;
    constexpr int SECONDS_IN_MINUTE = 60;
    constexpr double SECONDS_IN_MILLISECOND = 0.001;
    constexpr double SECONDS_IN_HUNDREDTH = 0.01;

    double timeDouble = 0.0;
    int tmp = 0;

    QStringList pieces = timecode.trimmed().split(m_timecodeSplitter);
    std::reverse(pieces.begin(), pieces.end());
    if (pieces.length() != 3 && pieces.length() != 4)
    {
        goto error;
    }

    /* Get sub-second values */
    bool localOk;
    if (pieces[0].size() == 2)
    {
        tmp = pieces[0].toInt(&localOk);
        if (!localOk || tmp < 0 || tmp > 99)
        {
            goto error;
        }
        timeDouble += tmp * SECONDS_IN_HUNDREDTH;
    }
    else if (pieces[0].size() == 3)
    {
        tmp = pieces[0].toInt(&localOk);
        if (!localOk || tmp < 0 || tmp > 999)
        {
            goto error;
        }
        timeDouble += tmp * SECONDS_IN_MILLISECOND;
    }
    else
    {
        goto error;
    }

    /* Get Seconds */
    tmp = pieces[1].toInt(&localOk);
    if (!localOk || tmp < 0 || tmp > 59)
    {
        goto error;
    }
    timeDouble += tmp;

    /* Get Minutes */
    tmp = pieces[2].toInt(&localOk);
    if (!localOk || tmp < 0 || tmp > 59)
    {
        goto error;
    }
    timeDouble += tmp * SECONDS_IN_MINUTE;

    /* Get Hours */
    if (pieces.length() == 4)
    {
        tmp = pieces[3].toInt(&localOk);
        if (!localOk || tmp < 0)
        {
            goto error;
        }
        timeDouble += tmp * SECONDS_IN_HOUR;
    }

    if (ok)
    {
        *ok = true;
    }
    return timeDouble;

error:
    if (ok)
    {
        *ok = false;
    }
    return 0.0;
}
