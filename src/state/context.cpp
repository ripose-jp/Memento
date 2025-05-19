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

#include "context.h"

#include "anki/ankiclient.h"
#include "audio/audioplayer.h"
#include "dict/dictionary.h"
#include "gui/widgets/subtitlelistwidget.h"
#include "player/playeradapter.h"

/* Begin Getters */

AnkiClient *Context::getAnkiClient() const
{
    return m_ankiClient;
}

AudioPlayer *Context::getAudioPlayer() const
{
    return m_audioPlayer;
}

Dictionary *Context::getDictionary() const
{
    return m_dictionary;
}

PlayerAdapter *Context::getPlayerAdapter() const
{
    return m_player;
}

QWidget *Context::getPlayerWidget() const
{
    return m_playerWidget;
}

SubtitleListWidget *Context::getSubtitleListWidget() const
{
    return m_subList;
}

/* End Getters */
/* Begin Setters */

void Context::setAnkiClient(AnkiClient *client)
{
    m_ankiClient = client;
}

void Context::setAudioPlayer(AudioPlayer *audioPlayer)
{
    m_audioPlayer = audioPlayer;
}

void Context::setDictionary(Dictionary *dictionary)
{
    m_dictionary = dictionary;
}

void Context::setPlayerAdapter(PlayerAdapter *player)
{
    m_player = player;
}

void Context::setPlayerWidget(QWidget *widget)
{
    m_playerWidget = widget;
}

void Context::setSubtitleList(SubtitleListWidget *subList)
{
    m_subList = subList;
}

/* End Setters */
