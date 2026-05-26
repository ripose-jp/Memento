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

#pragma once

#include <QObject>

#include "anki/ankiclient.h"
#include "anki/ankiconfig.h"
#include "audio/audioplayer.h"
#include "dict/dictionarycontroller.h"
#include "player/mpvplayer.h"
#include "setting/settings.h"
#include "subtitle/subtitlelists.h"
#include "util/utils.h"

/**
 * Holds shared application resources.
 */
class Context : public QObject
{
    Q_OBJECT

public:
    Context(QObject *parent = nullptr);
    virtual ~Context();

    /**
     * @brief Get the global application settings.
     *
     * @return The global application settings.
     */
    [[nodiscard]]
    Settings *settings() const noexcept;

    /**
     * @brief Get the global Anki configuration.
     *
     * @return The global Anki config.
     */
    [[nodiscard]]
    AnkiConfig *ankiConfig() const noexcept;

    /**
     * @brief Get the global Anki client.
     *
     * @return The global Anki client.
     */
    [[nodiscard]]
    AnkiClient *ankiClient() const noexcept;

    /**
     * @brief Get the global audio player.
     *
     * @return The global audio player.
     */
    [[nodiscard]]
    AudioPlayer *audioPlayer() const noexcept;

    /**
     * @brief Get the global subtitle lists.
     *
     * @return The global subtitle lists.
     */
    [[nodiscard]]
    SubtitleLists *subtitleLists() const noexcept;

    /**
     * @brief Get the global dictionary controller.
     *
     * @return The global dictionary controller.
     */
    [[nodiscard]]
    DictionaryController *dictionaryController() const noexcept;

    /**
     * @brief Get the application MpvPlayer instance.
     *
     * @return The MpvPlayer instance.
     */
    [[nodiscard]]
    MpvPlayer *player() const noexcept;

    /**
     * @brief Set the player instance. Does not take ownership.
     *
     * @param player The player instance.
     */
    void setPlayer(MpvPlayer *player) noexcept;

private:
    /* The application settings instance. Has ownership. */
    Settings *m_settings{new Settings(this)};

    /* The application Anki configuration */
    AnkiConfig *m_ankiConfig{
        new AnkiConfig(DirectoryUtils::getAnkiConfig(), this)
    };

    /* The application Anki client */
    AnkiClient *m_ankiClient{new AnkiClient(this, this)};

    /* The application audio player. Has ownership. */
    AudioPlayer *m_audioPlayer{new AudioPlayer(this)};

    /* The application subtitle list. Has ownership. */
    SubtitleLists *m_subtitleLists{new SubtitleLists(this)};

    /* The application dictionary controller */
    DictionaryController *m_dictionaryController{
        new DictionaryController(m_settings, this)
    };

    /* The main application MpvPlayer. Does not have ownership. */
    MpvPlayer *m_player{nullptr};
};
