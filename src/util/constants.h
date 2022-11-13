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

#define GITHUB_API_LINK                 (QString("https://api.github.com/repos/ripose-jp/memento/releases/latest"))
#define GITHUB_RELEASES                 (QString("https://github.com/ripose-jp/memento/releases"))

/* Versioning */
#define SETTINGS_VERSION                "version"
#define SETTINGS_VERSION_CURRENT        3

/* Window Sizes */
#define SETTINGS_GROUP_WINDOW           "main-window"
#define SETTINGS_GEOMETRY               "geometry"
#define SETTINGS_MAXIMIZE               "maximize"

/* Audio Source Settings */
#define SETTINGS_AUDIO_SRC              "audio-srcs"

enum class AudioSourceType
{
    File = 0,
    JSON = 1,
};

#define SETTINGS_AUDIO_SRC_NAME         "name"
#define SETTINGS_AUDIO_SRC_URL          "url"
#define SETTINGS_AUDIO_SRC_TYPE         "type"
#define SETTINGS_AUDIO_SRC_MD5          "md5"

#define SETTINGS_AUDIO_SRC_NAME_DEFAULT (QString("JapanesePod101"))
#define SETTINGS_AUDIO_SRC_URL_DEFAULT  (QString("http://assets.languagepod101.com/dictionary/japanese/audiomp3.php?kanji={expression}&kana={reading}"))
#define SETTINGS_AUDIO_SRC_TYPE_DEFAULT AudioSourceType::File
#define SETTINGS_AUDIO_SRC_MD5_DEFAULT  (QString("7e2c2f954ef6051373ba916f000168dc"))

/* Behavior Settings */
#define SETTINGS_BEHAVIOR                           "behavior"

#define SETTINGS_BEHAVIOR_AUTOFIT                   "autofit"
#define SETTINGS_BEHAVIOR_AUTOFIT_DEFAULT           true

#define SETTINGS_BEHAVIOR_AUTOFIT_PERCENT           "autofit-percent"
#define SETTINGS_BEHAVIOR_AUTOFIT_PERCENT_DEFAULT   100

#define SETTINGS_BEHAVIOR_OSC_DURATION              "osc-duration"
#define SETTINGS_BEHAVIOR_OSC_DURATION_DEFAULT      700

#define SETTINGS_BEHAVIOR_OSC_FADE                  "osc-fade"
#define SETTINGS_BEHAVIOR_OSC_FADE_DEFAULT          250

#define SETTINGS_BEHAVIOR_OSC_MIN_MOVE              "osc-min-move"
#define SETTINGS_BEHAVIOR_OSC_MIN_MOVE_DEFAULT      0

#define SETTINGS_BEHAVIOR_SUBTITLE_PAUSE            "subtitle-pause"
#define SETTINGS_BEHAVIOR_SUBTITLE_PAUSE_DEFAULT    false

#define SETTINGS_BEHAVIOR_CURSOR_HIDE_OSC          "cursor-hide-osc"
#define SETTINGS_BEHAVIOR_CURSOR_HIDE_OSC_DEFAULT   true

enum class FileOpenDirectory
{
    Current = 0,
    Home = 1,
    Movies = 2,
    Documents = 3,
    Custom = 4
};

#define SETTINGS_BEHAVIOR_FILE_OPEN_DIR             "file-open-dir"
#define SETTINGS_BEHAVIOR_FILE_OPEN_DIR_DEFAULT     (FileOpenDirectory::Current)

#define SETTINGS_BEHAVIOR_FILE_OPEN_CUSTOM          "file-open-custom"
#define SETTINGS_BEHAVIOR_FILE_OPEN_CUSTOM_DEFAULT  (DirectoryUtils::getFileOpenDirectory(FileOpenDirectory::Home))

/* Dictionary Settings */
#define SETTINGS_DICTIONARIES           "dictionaries"

/* Search Settings */
#define SETTINGS_SEARCH                 "search"

#define SEARCH_METHOD_HOVER             "Hover"
#define SEARCH_METHOD_MODIFIER          "Modifier"

#define SEARCH_MODIFIER_ALT             "Alt"
#define SEARCH_MODIFIER_CTRL            "Control"
#define SEARCH_MODIFIER_SHIFT           "Shift"
#define SEARCH_MODIFIER_SUPER           "Super"

#define SETTINGS_SEARCH_LIMIT                       "limit"
#define SETTINGS_SEARCH_LIMIT_DEFAULT               10

#define SETTINGS_SEARCH_METHOD                      "method"
#define SETTINGS_SEARCH_METHOD_DEFAULT              SEARCH_METHOD_HOVER

#define SETTINGS_SEARCH_DELAY                       "delay"
#define SETTINGS_SEARCH_DELAY_DEFAULT               250

#define SETTINGS_SEARCH_MODIFIER                    "modifier"
#define SETTINGS_SEARCH_MODIFIER_DEFAULT            SEARCH_MODIFIER_SHIFT

#define SETTINGS_SEARCH_HIDE_SUBS                   "hide-subs"
#define SETTINGS_SEARCH_HIDE_SUBS_DEFAULT           true

#define SETTINGS_SEARCH_HIDE_BAR                    "hide-bar"
#define SETTINGS_SEARCH_HIDE_BAR_DEFAULT            false

#define SETTINGS_SEARCH_HOVER_PAUSE                 "hover-pause"
#define SETTINGS_SEARCH_HOVER_PAUSE_DEFAULT         false

enum class GlossaryStyle
{
    Bullet = 0,
    LineBreak = 1,
    Pipe = 2,
};

#define SETTINGS_SEARCH_LIST_GLOSSARY               "list-result"
#define SETTINGS_SEARCH_LIST_GLOSSARY_DEFAULT       (GlossaryStyle::Bullet)

#define SETTINGS_SEARCH_REPLACE_LINES               "replace-lines"
#define SETTINGS_SEARCH_REPLACE_LINES_DEFAULT       false

#define SETTINGS_SEARCH_REPLACE_WITH                "replace-with"
#define SETTINGS_SEARCH_REPLACE_WITH_DEFAULT        ""

#define SETTINGS_SEARCH_REMOVE_REGEX                "remove-regex"
#define SETTINGS_SEARCH_REMOVE_REGEX_DEFAULT        ""

/* Interface Settings */
enum class Theme
{
    System       = 0,
    Light        = 1,
    Dark         = 2
};

#define SETTINGS_INTERFACE                                  "interface"

#define SETTINGS_INTERFACE_THEME                            "theme"
#define SETTINGS_INTERFACE_THEME_DEFAULT                    Theme::System

#ifdef APPIMAGE
#define SETTINGS_INTERFACE_SYSTEM_ICONS                     "system-icons"
#define SETTINGS_INTERFACE_SYSTEM_ICONS_DEFAULT             false
#elif defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
#define SETTINGS_INTERFACE_SYSTEM_ICONS                     "system-icons"
#define SETTINGS_INTERFACE_SYSTEM_ICONS_DEFAULT             true
#endif

#define SETTINGS_INTERFACE_STYLESHEETS                      "stylesheets-enabled"
#define SETTINGS_INTERFACE_STYLESHEETS_DEFAULT              false

/* Subtitle Interface */
#define SETTINGS_INTERFACE_SUB_FONT                         "sub-font"
#define SETTINGS_INTERFACE_SUB_FONT_DEFAULT                 "Noto Sans JP"

#define SETTINGS_INTERFACE_SUB_FONT_BOLD                    "sub-font-bold"
#define SETTINGS_INTERFACE_SUB_FONT_BOLD_DEFAULT            false

#define SETTINGS_INTERFACE_SUB_FONT_ITALICS                 "sub-font-italics"
#define SETTINGS_INTERFACE_SUB_FONT_ITALICS_DEFAULT         false

#define SETTINGS_INTERFACE_SUB_SCALE                        "sub-scale"
#define SETTINGS_INTERFACE_SUB_SCALE_DEFAULT                0.05

#define SETTINGS_INTERFACE_SUB_OFFSET                       "sub-offset"
#define SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT               0.045

#define SETTINGS_INTERFACE_SUB_STROKE                       "sub-stroke"
#define SETTINGS_INTERFACE_SUB_STROKE_DEFAULT               11.0

#define SETTINGS_INTERFACE_SUB_TEXT_COLOR                   "sub-text-color"
#define SETTINGS_INTERFACE_SUB_TEXT_COLOR_DEFAULT           "#FFFFFFFF"

#define SETTINGS_INTERFACE_SUB_BG_COLOR                     "sub-bg-color"
#define SETTINGS_INTERFACE_SUB_BG_COLOR_DEFAULT             "#00000000"

#define SETTINGS_INTERFACE_SUB_STROKE_COLOR                 "sub-stroke-color"
#define SETTINGS_INTERFACE_SUB_STROKE_COLOR_DEFAULT         "#FF000000"

#define SETTINGS_INTERFACE_SUB_LIST_WINDOW                  "sub-list-window"
#define SETTINGS_INTERFACE_SUB_LIST_WINDOW_DEFAULT          false

#define SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS              "sub-list-timestamps"
#define SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS_DEFAULT      false

#define SETTINGS_INTERFACE_AUX_SEARCH_WINDOW                "search-window"
#define SETTINGS_INTERFACE_AUX_SEARCH_WINDOW_DEFAULT        false

#if defined(Q_OS_WIN)
#define SETTINGS_INTERFACE_MENUBAR_FULLSCREEN               "menubar-fullscreen"
#define SETTINGS_INTERFACE_MENUBAR_FULLSCREEN_DEFAULT       false
#endif

/* Stylesheets */
#define SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE              "sublist-style"
#if defined(Q_OS_MACOS)
#define SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT      \
    "QTabWidget::pane {\n"\
    "    border-top: 0px;\n"\
    "}\n"\
    "\n"\
    "QTabWidget::tab-bar {\n"\
    "    border: 0px;\n"\
    "    alignment: center;\n"\
    "}\n"\
    "\n"\
    "QTabBar::tab {\n"\
    "    background: black;\n"\
    "}\n"\
    "QTabBar::tab:selected {\n"\
    "    color: white;\n"\
    "}\n"\
    "\n"\
    "QTabBar::tab:!selected {\n"\
    "    color: gray;\n"\
    "}\n"\
    "\n"\
    "QTableWidget {\n"\
    "    background: black;\n"\
    "    color: white;\n"\
    "    font-family: \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", sans-serif;\n"\
    "    font-size: 20pt;\n"\
    "}"
#elif defined(Q_OS_WIN)
#define SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT      \
    "QTabWidget::pane {\n"\
    "    border-top: 0px;\n"\
    "}\n"\
    "\n"\
    "QTabWidget::tab-bar {\n"\
    "    border: 0px;\n"\
    "    alignment: center;\n"\
    "}\n"\
    "\n"\
    "QTabBar::tab {\n"\
    "    background: black;\n"\
    "}\n"\
    "QTabBar::tab:selected {\n"\
    "    color: white;\n"\
    "}\n"\
    "\n"\
    "QTabBar::tab:!selected {\n"\
    "    color: gray;\n"\
    "}\n"\
    "\n"\
    "QTableWidget {\n"\
    "    background: black;\n"\
    "    color: white;\n"\
    "    font-family: \"Meiryo\", \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", sans-serif;\n"\
    "    font-size: 14pt;\n"\
    "}"
#else
#define SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT      \
    "QTabWidget::pane {\n"\
    "    border-top: 0px;\n"\
    "}\n"\
    "\n"\
    "QTabWidget::tab-bar {\n"\
    "    border: 0px;\n"\
    "    alignment: center;\n"\
    "}\n"\
    "\n"\
    "QTabBar::tab {\n"\
    "    background: black;\n"\
    "}\n"\
    "QTabBar::tab:selected {\n"\
    "    color: white;\n"\
    "}\n"\
    "\n"\
    "QTabBar::tab:!selected {\n"\
    "    color: gray;\n"\
    "}\n"\
    "\n"\
    "QTableWidget {\n"\
    "    background: black;\n"\
    "    color: white;\n"\
    "    font-family: \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", sans-serif;\n"\
    "    font-size: 14pt;\n"\
    "}"
#endif

#define SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE            "player-splitter-style"
#define SETTINGS_INTERFACE_PLAYER_SPLITTER_STYLE_DEFAULT    \
    "QSplitter {\n"\
    "    background: black;\n"\
    "}\n"\
    "\n"\
    "QSplitter::handle {\n"\
    "    background: black;\n"\
    "}"

#define SETTINGS_INTERFACE_DEFINITION_STYLE                 "definition-style"
#if defined(Q_OS_MACOS)
#define SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT         \
    "QLabel, QTextEdit {\n"\
    "    font-family: \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", sans-serif;\n"\
    "    font-size: 15pt;\n"\
    "}"
#elif defined(Q_OS_WIN)
#define SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT         \
    "QLabel, QTextEdit {\n"\
    "    font-family: \"Meiryo\", \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", sans-serif;\n"\
    "    font-size: 11pt;\n"\
    "}"
#else
#define SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT         \
    "QLabel, QTextEdit {\n"\
    "    font-family: \"Noto Sans\", \"Noto Sans JP\", \"Noto Sans CJK JP\", sans-serif;\n"\
    "    font-size: 11pt;\n"\
    "}"
#endif

/* OCR Settings */
#define SETTINGS_OCR                    "ocr"

#define SETTINGS_OCR_ENABLE             "enable"
#define SETTINGS_OCR_ENABLE_DEFAULT     true

#define SETTINGS_OCR_ENABLE_GPU         "use-gpu"
#define SETTINGS_OCR_ENABLE_GPU_DEFAULT true

#define SETTINGS_OCR_MODEL              "model"
#define SETTINGS_OCR_MODEL_DEFAULT      "kha-white/manga-ocr-base"

#endif // CONSTANTS_H
