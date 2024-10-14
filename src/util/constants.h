////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QtGlobal>

#include "utils.h"

namespace Constants
{
    namespace Links
    {
        constexpr const char *GITHUB_API = "https://api.github.com/repos/ripose-jp/memento/releases/latest";
        constexpr const char *GITHUB_RELEASES = "https://github.com/ripose-jp/memento/releases";
    }

    enum class AudioSourceType
    {
        File = 0,
        JSON = 1,
    };

    enum class FileOpenDirectory
    {
        Current = 0,
        Home = 1,
        Movies = 2,
        Documents = 3,
        Custom = 4
    };

    enum class GlossaryStyle
    {
        Bullet = 0,
        LineBreak = 1,
        Pipe = 2,
    };

    enum class Theme
    {
        System = 0,
        Light = 1,
        Dark = 2
    };

    namespace Settings
    {
        namespace Version
        {
            constexpr const char *VERSION = "version";
            constexpr unsigned int CURRENT = 3;
        }

        namespace Window
        {
            constexpr const char *GROUP = "main-window";
            constexpr const char *GEOMETRY = "geometry";
            constexpr const char *MAXIMIZE = "maximize";
        }

        namespace AudioSource
        {
            constexpr const char *GROUP = "audio-srcs";

            constexpr const char *NAME = "name";
            constexpr const char *NAME_DEFAULT = "JapanesePod101";

            constexpr const char *URL = "url";
            constexpr const char *URL_DEFAULT = "https://assets.languagepod101.com/dictionary/japanese/audiomp3.php?kanji={expression}&kana={reading}";

            constexpr const char *TYPE = "type";
            constexpr AudioSourceType TYPE_DEFAULT = AudioSourceType::File;

            constexpr const char *MD5 = "md5";
            constexpr const char *MD5_DEFAULT = "7e2c2f954ef6051373ba916f000168dc";
        }

        namespace Behavior
        {
            constexpr const char *GROUP = "behavior";

            constexpr const char *AUTOFIT = "autofit";
            constexpr bool AUTOFIT_DEFAULT = true;

            constexpr const char *AUTOFIT_PERCENT = "autofit-percent";
            constexpr int AUTOFIT_PERCENT_DEFAULT = 100;

            constexpr const char *OSC_CURSOR_HIDE = "cursor-hide-osc";
            constexpr bool OSC_CURSOR_HIDE_DEFAULT = true;

            constexpr const char *OSC_DURATION = "osc-duration";
            constexpr int OSC_DURATION_DEFAULT = 700;

            constexpr const char *OSC_FADE = "osc-fade";
            constexpr int OSC_FADE_DEFAULT = 250;

            constexpr const char *OSC_MIN_MOVE = "osc-min-move";
            constexpr int OSC_MIN_MOVE_DEFAULT = 0;

            constexpr const char *SUBTITLE_PAUSE = "subtitle-pause";
            constexpr bool SUBTITLE_PAUSE_DEFAULT = false;

            constexpr const char *SUBTITLE_CURSOR_SHOW = "subtitle-cursor-show";
            constexpr bool SUBTITLE_CURSOR_SHOW_DEFAULT = false;

            constexpr const char *SECONDARY_SUBTITLE_CURSOR_SHOW = "subtitle-cursor-show-secondary";
            constexpr bool SECONDARY_SUBTITLE_CURSOR_SHOW_DEFAULT = false;

            constexpr const char *FILE_OPEN_DIR = "file-open-dir";
            constexpr FileOpenDirectory FILE_OPEN_DIR_DEFAULT = FileOpenDirectory::Current;

            constexpr const char *FILE_OPEN_CUSTOM = "file-open-custom";
            static const QString FILE_OPEN_CUSTOM_DEFAULT = DirectoryUtils::getFileOpenDirectory(FileOpenDirectory::Home);
        }

        namespace Dictionaries
        {
            constexpr const char *GROUP = "dictionaries";
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

#ifdef MECAB_SUPPORT
                constexpr const char *MECAB_IPADIC = "ipadic-matcher";
                constexpr bool MECAB_IPADIC_DEFAULT = true;
#endif // MECAB_SUPPORT
            }

            namespace Method
            {
                constexpr const char *HOVER = "Hover";
                constexpr const char *MODIFIER = "Modifier";
            }

            namespace Modifier
            {
                constexpr const char *ALT = "Alt";
                constexpr const char *CTRL = "Control";
                constexpr const char *SHIFT = "Shift";
                constexpr const char *SUPER = "Super";
            }

            constexpr const char *LIMIT = "limit";
            constexpr int LIMIT_DEFAULT = 10;

            constexpr const char *METHOD = "method";
            constexpr const char *METHOD_DEFAULT = Method::HOVER;

            constexpr const char *DELAY = "delay";
            constexpr int DELAY_DEFAULT = 250;

            constexpr const char *MODIFIER = "modifier";
            constexpr const char *MODIFIER_DEFAULT = Modifier::SHIFT;

            constexpr const char *HIDE_SUBS = "hide-subs";
            constexpr bool HIDE_SUBS_DEFAULT = true;

            constexpr const char *HIDE_BAR = "hide-bar";
            constexpr bool HIDE_BAR_DEFAULT = false;

            constexpr const char *HOVER_PAUSE = "hover-pause";
            constexpr bool HOVER_PAUSE_DEFAULT = false;

            constexpr const char *AUTO_PLAY_AUDIO = "auto-play-audio";
            constexpr bool AUTO_PLAY_AUDIO_DEFAULT = false;

            constexpr const char *LIST_GLOSSARY = "list-result";
            constexpr GlossaryStyle LIST_GLOSSARY_DEFAULT = GlossaryStyle::Bullet;

            constexpr const char *REPLACE_LINES = "replace-lines";
            constexpr bool REPLACE_LINES_DEFAULT = false;

            constexpr const char *REPLACE_WITH = "replace-with";
            constexpr const char *REPLACE_WITH_DEFAULT = "";

            constexpr const char *REMOVE_REGEX = "remove-regex";
            constexpr const char *REMOVE_REGEX_DEFAULT = "";
        }

        namespace Interface
        {
            constexpr const char *GROUP = "interface";

            constexpr const char *THEME = "theme";
            constexpr Theme THEME_DEFAULT = Theme::System;

            constexpr const char *SYSTEM_ICONS = "system-icons";
#ifdef APPIMAGE
            constexpr bool SYSTEM_ICONS_DEFAULT = false;
#elif defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
            constexpr bool SYSTEM_ICONS_DEFAULT = true;
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
            constexpr const char *DPI_SCALE_OVERRIDE = "dpi-override";
            constexpr const bool DPI_SCALE_OVERRIDE_DEFAULT = false;

            constexpr const char *DPI_SCALE_FACTOR = "dpi-factor";
            constexpr const double DPI_SCALE_FACTOR_DEFAULT = 1.0;
#endif

            constexpr const char *STYLESHEETS = "stylesheets-enabled";
            constexpr bool STYLESHEETS_DEFAULT = false;

            namespace Subtitle
            {
                constexpr const char *FONT = "sub-font";
                constexpr const char *FONT_DEFAULT = "Noto Sans JP";

                constexpr const char *FONT_BOLD = "sub-font-bold";
                constexpr bool FONT_BOLD_DEFAULT = false;

                constexpr const char *FONT_ITALICS = "sub-font-italics";
                constexpr bool FONT_ITALICS_DEFAULT = false;

                constexpr const char *SCALE = "sub-scale";
                constexpr double SCALE_DEFAULT = 0.05;

                constexpr const char *OFFSET = "sub-offset";
                constexpr double OFFSET_DEFAULT = 0.045;

                constexpr const char *STROKE = "sub-stroke";
                constexpr double STROKE_DEFAULT = 11.0;

                constexpr const char *TEXT_COLOR = "sub-text-color";
                constexpr const char *TEXT_COLOR_DEFAULT = "#FFFFFFFF";

                constexpr const char *BACKGROUND_COLOR = "sub-bg-color";
                constexpr const char *BACKGROUND_COLOR_DEFAULT = "#00000000";

                constexpr const char *STROKE_COLOR = "sub-stroke-color";
                constexpr const char *STROKE_COLOR_DEFAULT = "#FF000000";

                constexpr const char *LIST_WINDOW = "sub-list-window";
                constexpr bool LIST_WINDOW_DEFAULT = false;

                constexpr const char *LIST_TIMESTAMPS = "sub-list-timestamps";
                constexpr bool LIST_TIMESTAMPS_DEFAULT = false;

                constexpr const char *SEARCH_WINDOW = "search-window";
                constexpr bool SEARCH_WINDOW_DEFAULT = false;

#if defined(Q_OS_WIN)
                constexpr const char *MENUBAR_FULLSCREEN = "menubar-fullscreen";
                constexpr bool MENUBAR_FULLSCREEN_DEFAULT = false;
#endif
            }

            namespace Style
            {
                constexpr const char *SUBTITLE_LIST = "sublist-style";
                constexpr const char *SUBTITLE_LIST_DEFAULT =
#if defined(Q_OS_MACOS)
"QTabWidget::pane {\n"
"    border-top: 0px;\n"
"}\n"
"\n"
"QTabWidget::tab-bar {\n"
"    border: 0px;\n"
"    alignment: center;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background: black;\n"
"}\n"
"QTabBar::tab:selected {\n"
"    color: white;\n"
"}\n"
"\n"
"QTabBar::tab:!selected {\n"
"    color: gray;\n"
"}\n"
"\n"
"QTableWidget {\n"
"    background: black;\n"
"    color: white;\n"
"    font-family: \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", \"Noto Color Emoji\", sans-serif;\n"
"    font-size: 20pt;\n"
"}";
#elif defined(Q_OS_WIN)
"QTabWidget::pane {\n"
"    border-top: 0px;\n"
"}\n"
"\n"
"QTabWidget::tab-bar {\n"
"    border: 0px;\n"
"    alignment: center;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background: black;\n"
"}\n"
"QTabBar::tab:selected {\n"
"    color: white;\n"
"}\n"
"\n"
"QTabBar::tab:!selected {\n"
"    color: gray;\n"
"}\n"
"\n"
"QTableWidget {\n"
"    background: black;\n"
"    color: white;\n"
"    font-family: \"Meiryo\", \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", \"Noto Color Emoji\", sans-serif;\n"
"    font-size: 14pt;\n"
"}";
#else
"QTabWidget::pane {\n"
"    border-top: 0px;\n"
"}\n"
"\n"
"QTabWidget::tab-bar {\n"
"    border: 0px;\n"
"    alignment: center;\n"
"}\n"
"\n"
"QTabBar::tab {\n"
"    background: black;\n"
"}\n"
"QTabBar::tab:selected {\n"
"    color: white;\n"
"}\n"
"\n"
"QTabBar::tab:!selected {\n"
"    color: gray;\n"
"}\n"
"\n"
"QTableWidget {\n"
"    background: black;\n"
"    color: white;\n"
"    font-family: \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", \"Noto Color Emoji\", sans-serif;\n"
"    font-size: 14pt;\n"
"}";
#endif

                constexpr const char *SPLITTER = "player-splitter-style";
                constexpr const char *SPLITTER_DEFAULT =
"QSplitter {\n"
"    background: black;\n"
"}\n"
"\n"
"QSplitter::handle {\n"
"    background: black;\n"
"}";

                constexpr const char *DEFINITION = "definition-style";
                constexpr const char *DEFINITION_DEFAULT =
#if defined(Q_OS_MACOS)
"QLabel, QTextEdit {\n"
"    font-family: \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", \"Noto Color Emoji\", sans-serif;\n"
"    font-size: 15pt;\n"
"}";
#elif defined(Q_OS_WIN)
"QLabel, QTextEdit {\n"
"    font-family: \"Meiryo\", \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", \"Noto Color Emoji\", sans-serif;\n"
"    font-size: 11pt;\n"
"}";
#else
"QLabel, QTextEdit {\n"
"    font-family: \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", \"Noto Color Emoji\", sans-serif;\n"
"    font-size: 11pt;\n"
"}";
#endif
            }
        }

        namespace OCR
        {
            constexpr const char *GROUP = "ocr";

            constexpr const char *ENABLED = "enable";
            constexpr bool ENABLED_DEFAULT = true;

            constexpr const char *ENABLE_GPU = "use-gpu";
            constexpr bool ENABLE_GPU_DEFAULT = true;

            constexpr const char *MODEL = "model";
            constexpr const char *MODEL_DEFAULT = "kha-white/manga-ocr-base";
        }
    }
}

#endif // CONSTANTS_H
