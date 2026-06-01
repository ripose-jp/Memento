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

#include "state/context.h"

Context::Context(QObject *parent) : QObject(parent)
{

}

Context::~Context()
{

}

Settings *Context::settings() const noexcept
{
    return m_settings;
}

AnkiConfig *Context::ankiConfig() const noexcept
{
    return m_ankiConfig;
}

AnkiClient *Context::ankiClient() const noexcept
{
    return m_ankiClient;
}

AudioPlayer *Context::audioPlayer() const noexcept
{
    return m_audioPlayer;
}

SubtitleLists *Context::subtitleLists() const noexcept
{
    return m_subtitleLists;
}

DictionaryController *Context::dictionaryController() const noexcept
{
    return m_dictionaryController;
}

FileOpenHandler *Context::fileOpenHandler() const noexcept
{
    return m_fileOpenHandler;
}

MpvPlayer *Context::player() const noexcept
{
    return m_player;
}

void Context::setPlayer(MpvPlayer *player) noexcept
{
    m_player = player;
}
