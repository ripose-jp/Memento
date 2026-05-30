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

#pragma once

#include <vector>

#include <QFile>
#include <QList>
#include <QRegularExpression>
#include <QSet>
#include <QString>

#include "subtitle/subtitleentry.h"

/**
 * Object for parsing subtitles of various formats.
 */
class SubtitleParser
{
public:
    /**
     * @brief Parses the subtitles at the given path if possible and returns
     * text, start, and end times for each subtitle.
     *
     * @param path The path to the subtitle.
     * @return A list of SubtitleEntries extracted from the subtitle file.
     */
    std::vector<SubtitleEntry> parseSubtitles(QString path) const;

private:
    /**
     * @brief Parses ASS subtitles.
     *
     * @param file The file containing the raw ass.
     * @param[out] out  The list the resulting SubtitleEntries are saved to.
     * @return true on success, false on error.
     */
    bool parseASS(QFile &file, std::vector<SubtitleEntry> &out) const;

    /**
     * @brief Parses SRT subtitles.
     *
     * @param file The file containing the raw srt.
     * @param[out] out  The list the resulting SubtitleEntries are saved to.
     * @return true on success, false on error.
     */
    bool parseSRT(QFile &file, std::vector<SubtitleEntry> &out) const;

    /**
     * @brief Parses VTT subtitles.
     *
     * @param file The file containing the raw vtt.
     * @param[out] out  The list the resulting SubtitleEntries are saved to.
     * @return true on success, false on error.
     */
    bool parseVTT(QFile &file, std::vector<SubtitleEntry> &out) const;

    /**
     * @brief Converts a timecode of the format HH:MM:SS,MsMsMs
     *
     * @param timecode The timecode string.
     * @param[out] ok Set to true on success, false on error.
     * @return The timecode in seconds.
     */
    double timecodeToDouble(const QString &timecode, bool *ok = nullptr) const;

    /* The regular expression for splitting timecodes */
    const QRegularExpression m_timecodeSplitter{"[:\\.,]"};

    /* Removes ASS styling overrides */
    const QRegularExpression m_assFilter{"{\\\\.*?}"};

    /* Adds newlines to ASS subtitles */
    const QRegularExpression m_assNewLineReplacer{"\\\\n|\\\\N"};

    /* The regular expression for removing SRT formatting */
    const QRegularExpression m_srtFilter{
        "<\\/?[biu]>|"   // Removes <b></b> <i></i> <u></u>
        "{\\/?[biu]}|"   // Removes {b}{/b} {i}{/i} {u}{/u}
        "<\\/?font.*?>|" // Removes <font color=“white”> </font>
        "{\\\\an?\\d}"   // Removes {\a#} {\an#}
    };

    /* Removes everything between angle braces */
    const QRegularExpression m_angleBraceCleaner{"<[^>]*>"};

    /* Checks for all whitespace */
    const QRegularExpression m_whitespaceRegex{"\\s+"};

    /* A set of special VTT sections */
    const QSet<QString> m_vttSections{{
        "NOTE",
        "STYLE",
        "REGION",
    }};
};
