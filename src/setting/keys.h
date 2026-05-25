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

#include <QFont>
#include <QStandardPaths>

namespace Setting
{
Q_NAMESPACE

enum AudioSourceType
{
    AudioSourceTypeFile = 0,
    AudioSourceTypeJson = 1,
};
Q_ENUM_NS(AudioSourceType)

enum GlossaryStyle
{
    GlossaryStyleBullet = 0,
    GlossaryStyleLineBreak = 1,
    GlossaryStylePipe = 2,
};
Q_ENUM_NS(GlossaryStyle)

enum Directory
{
    DirectoryCurrent = 0,
    DirectoryHome = 1,
    DirectoryMovies = 2,
    DirectoryDocuments = 3,
    DirectoryCustom = 4,
};
Q_ENUM_NS(Directory)

enum SearchMethod
{
    SearchMethodHover = 0,
    SearchMethodModifier = 1,
};
Q_ENUM_NS(SearchMethod)

enum Modifier
{
    ModifierAlt = 0,
    ModifierControl = 1,
    ModifierShift = 2,
    ModifierSuper = 3,
};
Q_ENUM_NS(Modifier)

} // namespace Setting

namespace Keys
{
    namespace Version
    {
        constexpr const char *VERSION = "version";
        constexpr unsigned int CURRENT = 4;
    }

    namespace Window
    {
        constexpr const char *GROUP = "main-window";

        constexpr const char *SUBTITLE_LIST = "subtitle-list";
        constexpr const char *SEARCH = "search";
    }

    namespace Recent
    {
        constexpr const char *GROUP = "recent";

        constexpr const char *FILES = "files";
    }

    namespace SubtitleList
    {
        constexpr const char *GROUP = "subtitle-list";

        constexpr const char *IGNORE_WHITESPACE = "ignore-whitespace";
        constexpr bool IGNORE_WHITESPACE_DEFAULT = false;

        constexpr const char *AUTO_SEEK = "auto-seek";
        constexpr bool AUTO_SEEK_DEFAULT = false;
    }

    namespace AudioSource
    {
        constexpr const char *GROUP = "audio-srcs";

        constexpr const char *NAME = "name";
        constexpr const char *NAME_DEFAULT = "JapanesePod101";

        constexpr const char *URL = "url";
        constexpr const char *URL_DEFAULT = "https://assets.languagepod101.com/dictionary/japanese/audiomp3.php?kanji={expression}&kana={reading}";

        constexpr const char *TYPE = "type";
        constexpr Setting::AudioSourceType TYPE_DEFAULT = Setting::AudioSourceTypeFile;

        constexpr const char *MD5 = "md5";
        constexpr const char *MD5_DEFAULT = "7e2c2f954ef6051373ba916f000168dc";
    }

    namespace Application
    {
        constexpr const char *GROUP = "application";

        constexpr const char *AUTO_UPDATE_CHECK = "auto-update-check";
        constexpr bool AUTO_UPDATE_CHECK_DEFAULT = false;
    }

    namespace Behavior
    {
        constexpr const char *GROUP = "behavior";

        constexpr const char *AUTO_FIT = "autofit";
        constexpr bool AUTO_FIT_DEFAULT = true;

        constexpr const char *AUTO_FIT_PERCENT = "autofit-percent";
        constexpr int AUTO_FIT_PERCENT_DEFAULT = 100;

        constexpr const char *OSC_MPV_CURSOR = "osc-mpv-cursor";
        constexpr bool OSC_MPV_CURSOR_DEFAULT = false;

        constexpr const char *OSC_DURATION = "osc-duration";
        constexpr int OSC_DURATION_DEFAULT = 700;

        constexpr const char *OSC_FADE = "osc-fade";
        constexpr int OSC_FADE_DEFAULT = 250;

        constexpr const char *OSC_MIN_MOVE = "osc-min-move";
        constexpr int OSC_MIN_MOVE_DEFAULT = 0;

        constexpr const char *OSC_PREVIEW_THUMBNAILS = "osc-preview-thumbnails";
        constexpr bool OSC_PREVIEW_THUMBNAILS_DEFAULT = true;

        constexpr const char *SUBTITLE_PAUSE = "subtitle-pause";
        constexpr bool SUBTITLE_PAUSE_DEFAULT = false;

        constexpr const char *SUBTITLE_CURSOR_SHOW = "subtitle-cursor-show";
        constexpr bool SUBTITLE_CURSOR_SHOW_DEFAULT = false;

        constexpr const char *SECONDARY_SUBTITLE_CURSOR_SHOW = "subtitle-cursor-show-secondary";
        constexpr bool SECONDARY_SUBTITLE_CURSOR_SHOW_DEFAULT = false;

        constexpr const char *FILE_OPEN_DIR = "file-open-dir";
        constexpr Setting::Directory FILE_OPEN_DIR_DEFAULT = Setting::DirectoryMovies;

        constexpr const char *FILE_OPEN_CUSTOM = "file-open-custom";
        static const QString FILE_OPEN_CUSTOM_DEFAULT =
            "file://" + QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    }

    namespace Dictionary
    {
        constexpr const char *GROUP = "dictionary";

        constexpr const char *ORDER = "order";
    }

    namespace Search
    {
        constexpr const char *GROUP = "search";

        namespace Matcher
        {
            constexpr const char *EXACT = "exact-matcher";
            constexpr bool EXACT_DEFAULT = true;

            constexpr const char *DECONJ = "deconj-matcher";
            constexpr bool DECONJ_DEFAULT = true;

            constexpr const char *MECAB_IPADIC = "ipadic-matcher";
            constexpr bool MECAB_IPADIC_DEFAULT = true;
        }

        constexpr const char *MIDDLE_MOUSE_SCAN = "middle-mouse-scan";
        constexpr bool MIDDLE_MOUSE_SCAN_DEFAULT = false;

        constexpr const char *METHOD = "method";
        constexpr Setting::SearchMethod METHOD_DEFAULT = Setting::SearchMethodHover;

        constexpr const char *DELAY = "delay";
        constexpr int DELAY_DEFAULT = 250;

        constexpr const char *MODIFIER = "modifier";
        constexpr Setting::Modifier MODIFIER_DEFAULT = Setting::ModifierShift;

        constexpr const char *HIDE_MPV_SUBS = "hide-subs";
        constexpr bool HIDE_MPV_SUBS_DEFAULT = true;

        constexpr const char *HIDE_SUBS = "hide-bar";
        constexpr bool HIDE_SUBS_DEFAULT = false;

        constexpr const char *PAUSE_ON_HOVER = "hover-pause";
        constexpr bool PAUSE_ON_HOVER_DEFAULT = false;

        constexpr const char *AUTO_PLAY_AUDIO = "auto-play-audio";
        constexpr bool AUTO_PLAY_AUDIO_DEFAULT = false;

        constexpr const char *GLOSSARY_STYLE = "list-result";
        constexpr Setting::GlossaryStyle GLOSSARY_STYLE_DEFAULT = Setting::GlossaryStyleBullet;

        constexpr const char *REPLACE_NEWLINES = "replace-lines";
        constexpr bool REPLACE_NEWLINES_DEFAULT = false;

        constexpr const char *REPLACE_NEWLINES_WITH = "replace-with";
        constexpr const char *REPLACE_NEWLINES_WITH_DEFAULT = "";

        constexpr const char *REMOVE_REGEX = "remove-regex";
        constexpr const char *REMOVE_REGEX_DEFAULT = "";
    }

    namespace Interface
    {
        constexpr const char *GROUP = "interface";

        constexpr const char *SYSTEM_ICONS = "system-icons";
        constexpr bool SYSTEM_ICONS_DEFAULT = true;

        namespace Subtitle
        {
            constexpr const char *FONT = "sub-font";
            static const QFont FONT_DEFAULT("Noto Sans CJK JP", 24);

            constexpr const char *LINE_SPACING = "sub-line-spacing";
            constexpr double LINE_SPACING_DEFAULT = 0;

            constexpr const char *SCALE = "sub-scale";
            constexpr double SCALE_DEFAULT = 0.05;

            constexpr const char *OFFSET = "sub-offset";
            constexpr double OFFSET_DEFAULT = 0.045;

            constexpr const char *STROKE = "sub-stroke";
            constexpr double STROKE_DEFAULT = 15.0;

            constexpr const char *TEXT_COLOR = "sub-text-color";
            constexpr const char *TEXT_COLOR_DEFAULT = "#FFFFFFFF";

            constexpr const char *BACKGROUND_COLOR = "sub-bg-color";
            constexpr const char *BACKGROUND_COLOR_DEFAULT = "#00000000";

            constexpr const char *STROKE_COLOR = "sub-stroke-color";
            constexpr const char *STROKE_COLOR_DEFAULT = "#FF000000";
        }

        constexpr const char *POPUP_WIDTH = "sub-popup-width";
        constexpr int POPUP_WIDTH_DEFAULT = 500;

        constexpr const char *POPUP_HEIGHT = "sub-popup-height";
        constexpr int POPUP_HEIGHT_DEFAULT = 400;

        constexpr const char *SEARCH_WINDOW = "search-window";
        constexpr bool SEARCH_WINDOW_DEFAULT = false;

        constexpr const char *SEARCH_EXPRESSION_FONT = "search-expression-font";
        static const QFont SEARCH_EXPRESSION_FONT_DEFAULT(
            "Noto Sans CJK JP",
#if defined(Q_OS_MACOS)
                28
#else
                20
#endif // defined(Q_OS_MACOS)
        );

        constexpr const char *SEARCH_READING_FONT = "search-reading-font";
        const QFont SEARCH_READING_FONT_DEFAULT(
            "Noto Sans CJK JP",
#if defined(Q_OS_MACOS)
                16
#else
                12
#endif // defined(Q_OS_MACOS)
);

        constexpr const char *SEARCH_CONJ_FONT = "search-conj-font";
        const QFont SEARCH_CONJ_FONT_DEFAULT(
            "Noto Sans CJK JP",
#if defined(Q_OS_MACOS)
                16
#else
                12
#endif // defined(Q_OS_MACOS)
        );

        constexpr const char *SEARCH_TAG_FONT = "search-tag-font";
        const QFont SEARCH_TAG_FONT_DEFAULT(
            "Noto Sans CJK JP",
#if defined(Q_OS_MACOS)
                14,
#else
                10,
#endif // defined(Q_OS_MACOS)
            QFont::Bold
        );

        constexpr const char *SEARCH_GLOSSARY_FONT = "search-glossary-font";
        const QFont SEARCH_GLOSSARY_FONT_DEFAULT(
            "Noto Sans CJK JP",
#if defined(Q_OS_MACOS)
                14
#else
                10
#endif // defined(Q_OS_MACOS)
        );

        constexpr const char *SEARCH_KANJI_FONT = "search-kanji-font";
        const QFont SEARCH_KANJI_FONT_DEFAULT(
            "Kanji Stroke Orders",
#if defined(Q_OS_MACOS)
                156
#else
                108
#endif // defined(Q_OS_MACOS)
        );

        namespace SubtitleList
        {
            constexpr const char *WINDOW = "sub-list-window";
            constexpr bool WINDOW_DEFAULT = false;

            constexpr const char *TIMESTAMPS = "sub-list-timestamps";
            constexpr bool TIMESTAMPS_DEFAULT = false;

            constexpr const char *BACKGROUND_COLOR = "sub-list-background-color";
            constexpr const char *BACKGROUND_COLOR_DEFAULT = "#FF000000";

            constexpr const char *TEXT_COLOR = "sub-list-text-color";
            constexpr const char *TEXT_COLOR_DEFAULT = "#FFFFFFFF";

            constexpr const char *HOVER_COLOR = "sub-list-hover-color";
            constexpr const char *HOVER_COLOR_DEFAULT = "#FF1A3B5C";

            constexpr const char *SELECTED_COLOR = "sub-list-selected-color";
            constexpr const char *SELECTED_COLOR_DEFAULT = "#FF295A8A";

            constexpr const char *SELECTED_HOVER_COLOR = "sub-list-selected-hover-color";
            constexpr const char *SELECTED_HOVER_COLOR_DEFAULT = "#FF3F576E";

            constexpr const char *PRIMARY_FONT = "sub-list-primary-font";
            const QFont PRIMARY_FONT_DEFAULT(
                "Noto Sans CJK JP",
#if defined(Q_OS_MACOS)
                20
#else
                16
#endif // defined(Q_OS_MACOS)
            );

            constexpr const char *SECONDARY_FONT = "sub-list-secondary-font";
            const QFont SECONDARY_FONT_DEFAULT(
                "Sans Serif",
#if defined(Q_OS_MACOS)
                20
#else
                16
#endif // defined(Q_OS_MACOS)
            );
        }
    }

    namespace Keybind
    {
        constexpr const char *GROUP = "keybind";

        constexpr const char *CURRENT_PROFILE = "current";

        namespace Profile
        {
            constexpr const char *GROUP = "profiles";

            constexpr const char *CARD_ADD = "card-add";
            constexpr const char *CARD_NEXT = "card-next";
            constexpr const char *CARD_PREVIOUS = "card-prev";
            constexpr const char *OPEN_FILE = "open-file";
            constexpr const char *OPEN_URL = "open-url";
            constexpr const char *OSC_VISIBILITY = "osc-visibility";
            constexpr const char *SHOW_SEARCH = "show-search";
            constexpr const char *SHOW_SUBTITLE_LIST = "show-sub-list";
            constexpr const char *SUBTITLE_AUTO_PAUSE = "sub-auto-pause";
            constexpr const char *SUBTITLE_DECREASE_SIZE = "sub-decrease-size";
            constexpr const char *SUBTITLE_INCREASE_SIZE = "sub-increase-size";
            constexpr const char *SUBTITLE_MOVE_DOWN = "sub-move-down";
            constexpr const char *SUBTITLE_MOVE_UP = "sub-move-up";
            constexpr const char *SUBTITLE_SHOW = "sub-show";
            constexpr const char *SUBTITLE_LIST_COPY_SELECTED = "sub-list-copy-selected";
            constexpr const char *SUBTITLE_LIST_FIND = "sub-list-find";
            constexpr const char *SUBTITLE_LIST_FIND_NEXT = "sub-list-find-next";
            constexpr const char *SUBTITLE_LIST_FIND_PREVIOUS = "sub-list-find-prev";
        }
    }

    namespace Ocr
    {
        constexpr const char *GROUP = "ocr";

        constexpr const char *ENABLED = "enable";
        constexpr bool ENABLED_DEFAULT = true;

        constexpr const char *USE_GPU = "use-gpu";
        constexpr bool USE_GPU_DEFAULT = true;

        constexpr const char *MODEL = "model";
        constexpr const char *MODEL_DEFAULT = "kha-white/manga-ocr-base";
    }
}
