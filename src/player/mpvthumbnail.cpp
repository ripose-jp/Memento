////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
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

#include "player/mpvthumbnail.h"

MpvThumbnail::MpvThumbnail(QQuickItem *parent) : MpvPlayer(parent)
{

}

MpvThumbnail::~MpvThumbnail()
{

}

void MpvThumbnail::initOptions()
{
    mpv_handle *mpv = handle();

    ::mpv_set_option_string(mpv, "vo", "libmpv");
    ::mpv_set_option_string(mpv, "aid", "no");
    ::mpv_set_option_string(mpv, "sid", "no");
    ::mpv_set_option_string(mpv, "secondary-sid", "no");
    ::mpv_set_option_string(mpv, "osc", "no");
    ::mpv_set_option_string(mpv, "pause", "yes");
    ::mpv_set_option_string(mpv, "ytdl", "no");
    ::mpv_set_option_string(mpv, "config", "no");
    ::mpv_set_option_string(mpv, "terminal", "no");
    ::mpv_set_option_string(mpv, "input-default-bindings", "no");
    ::mpv_set_option_string(mpv, "input-vo-keyboard", "no");
    ::mpv_set_option_string(mpv, "load-stats-overlay", "no");
}
