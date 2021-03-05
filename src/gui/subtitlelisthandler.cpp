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

#include "subtitlelisthandler.h"

#include <mpv/client.h>

#include "../util/directoryutils.h"

SubtitleListHandler::SubtitleListHandler(QListWidget *list,
                                         PlayerAdapter *player,
                                         QObject *parent)
    : m_list(list), m_player(player), QObject(parent)
{
    m_mpv = mpv_create();
    if (!m_mpv)
    {
        fprintf(stderr, "Could not create mpv context\n");
        exit(EXIT_FAILURE);
    }
    
    mpv_set_option_string(m_mpv, "no-terminal", "yes");
    mpv_set_option_string(m_mpv, "idle", "yes");
    mpv_set_option_string(m_mpv, "keep-open", "yes");
    mpv_set_option_string(m_mpv, "osd-level", "0");
    mpv_set_option_string(m_mpv, "force-window", "no");
    mpv_set_option_string(m_mpv, "config", "yes");
    mpv_set_option_string(m_mpv, "config-dir",
                          DirectoryUtils::getConfigDir().toLatin1());

    if (mpv_initialize(m_mpv) < 0)
    {
        fprintf(stderr, "Could not initialize mpv context\n");
        exit(EXIT_FAILURE);
    }

    /*
    int val = 0;
    mpv_set_property(m_mpv, "osd-level", MPV_FORMAT_INT64, &val);
    val = 1;
    mpv_set_property(m_mpv, "idle", MPV_FORMAT_FLAG, &val);
    val = 1;
    mpv_set_property(m_mpv, "keep-open", MPV_FORMAT_FLAG, &val);
    val = 0;
    mpv_set_property(m_mpv, "force-window", MPV_FORMAT_FLAG, &val);
    */

    connect(m_player, &PlayerAdapter::fileChanged,
        this, &SubtitleListHandler::open);
    connect(m_player, &PlayerAdapter::subtitleTrackChanged,
        this, &SubtitleListHandler::populateList);
}

SubtitleListHandler::~SubtitleListHandler()
{
    mpv_destroy(m_mpv);
}

void SubtitleListHandler::open(const QString &file)
{
    const char *args[3] = {
        "loadfile",
        file.toLatin1().data(),
        NULL
    };

    if (mpv_command(m_mpv, args) < 0)
    {
        fprintf(stderr, "Could not load file %s\n", file.toLatin1().data());
    }
}

void SubtitleListHandler::populateList(int64_t track)
{
    // Set track and seek to start
    if (mpv_set_property(m_mpv, "sid", MPV_FORMAT_INT64, &track) < 0)
        return;
    int paused = 1;
    if (mpv_set_property(m_mpv, "pause", MPV_FORMAT_FLAG, &paused) < 0)
        return;
    double pos = 0.0;
    if (mpv_set_property(m_mpv, "time-pos", MPV_FORMAT_DOUBLE, &pos) < 0)
        return;

    const char *sub_seek_args[3] = {
        "sub-seek",
        "1",
        NULL
    };

    fprintf(stderr, "Seeked to start\n");

    // Get the first subtitle
    char *subtext = NULL;
    if (mpv_get_property(m_mpv, "sub-text", MPV_FORMAT_STRING, &subtext) < 0)
        return;
    fprintf(stderr, "Got first subtitle to start %s\n", subtext);
    if (*subtext == '\0')
    {
        mpv_free(subtext);
        if (mpv_command(m_mpv, sub_seek_args) < 0)
            return;
        if (mpv_get_property(m_mpv, "sub-text", MPV_FORMAT_STRING, &subtext) < 0)
            return;
    }
    double lastpos = -1.0;
    if (mpv_get_property(m_mpv, "time-pos", MPV_FORMAT_DOUBLE, &pos) < 0)
    {
        mpv_free(subtext);
        return;
    }

    fprintf(stderr, "Got first subtitle to start %s\n", subtext);

    // Add all the subtitles to the list
    while (pos != lastpos && *subtext)
    {
        m_list->addItem(subtext);
        m_startTimes.append(pos);

        lastpos = pos;
        mpv_free(subtext);
        if (mpv_command(m_mpv, sub_seek_args) < 0)
        {
            return;
        }
        if (mpv_get_property(m_mpv, "time-pos", MPV_FORMAT_DOUBLE, &pos) < 0)
        {
            return;
        }
        if (mpv_get_property(m_mpv, "sub-text", MPV_FORMAT_STRING, &subtext) < 0)
        {
            return;
        }
    }

    fprintf(stderr, "Finished seeking last: %f end: %f\n", pos, lastpos);

    // Free up any memory that might not be free
    mpv_free(subtext);
}