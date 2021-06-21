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

#define VERSION                         (QString("v0.4.7-alpha"))
#define GITHUB_API_LINK                 (QString("https://api.github.com/repos/ripose-jp/memento/releases"))
#define GITHUB_RELEASES                 (QString("https://github.com/ripose-jp/memento/releases"))

/* Versioning */
#define SETTINGS_VERSION                "version"
#define SETTINGS_VERSION_CURRENT        1

/* Window Sizes */
#define SETTINGS_GROUP_WINDOW           "main-window"
#define SETTINGS_GEOMETRY               "geometry"
#define SETTINGS_MAXIMIZE               "maximize"

/* Audio Source Settings */
#define SETTINGS_AUDIO_SRC              "audio-srcs"

#define SETTINGS_AUDIO_SRC_NAME         "name"
#define SETTINGS_AUDIO_SRC_URL          "url"
#define SETTINGS_AUDIO_SRC_MD5          "md5"

#define SETTINGS_AUDIO_SRC_NAME_DEFAULT (QString("JapanesePod101"))
#define SETTINGS_AUDIO_SRC_URL_DEFAULT  (QString("http://assets.languagepod101.com/dictionary/japanese/audiomp3.php?kanji={expression}&kana={reading}"))
#define SETTINGS_AUDIO_SRC_MD5_DEFAULT  (QString("7e2c2f954ef6051373ba916f000168dc"))

/* Dictionary Settings */
#define SETTINGS_DICTIONARIES           "dictionaries"

/* Search Settings */
#define SETTINGS_SEARCH                 "search"
#define SETTINGS_SEARCH_LIMIT           "limit"
#define SETTINGS_SEARCH_METHOD          "method"
#define SETTINGS_SEARCH_DELAY           "delay"
#define SETTINGS_SEARCH_MODIFIER        "modifier"
#define SETTINGS_SEARCH_HIDE_SUBS       "hide-subs"
#define SETTINGS_SEARCH_HIDE_BAR        "hide-bar"
#define SETTINGS_SEARCH_REPLACE_LINES   "replace-lines"
#define SETTINGS_SERACH_REPLACE_WITH    "replace-with"
#define SETTINGS_SEARCH_REMOVE_REGEX    "remove-regex"

#define MODIFIER_ALT                    "Alt"
#define MODIFIER_CTRL                   "Control"
#define MODIFIER_SHIFT                  "Shift"
#define MODIFIER_SUPER                  "Super"

#define SEARCH_METHOD_HOVER             "Hover"
#define SEARCH_METHOD_MODIFIER          "Modifier"

#define DEFAULT_LIMIT                   10
#define DEFAULT_METHOD                  SEARCH_METHOD_HOVER
#define DEFAULT_DELAY                   250
#define DEFAULT_MODIFIER                MODIFIER_SHIFT
#define DEFAULT_HIDE_SUBS               true
#define DEFAULT_HIDE_BAR                false
#define DEFAULT_REPLACE_LINES           false
#define DEFAULT_REPLACE_WITH            ""
#define DEFAULT_REMOVE_REGEX            ""

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

#define SETTINGS_INTERFACE_STYLESHEETS                      "stylesheets-enabled"
#define SETTINGS_INTERFACE_STYLESHEETS_DEFAULT              false

/* Subtitle Interface */
#define SETTINGS_INTERFACE_SUB_FONT                         "sub-font"
#define SETTINGS_INTERFACE_SUB_FONT_DEFAULT                 "Noto Sans CJK JP"

#define SETTINGS_INTERFACE_SUB_FONT_BOLD                    "sub-font-bold"
#define SETTINGS_INTERFACE_SUB_FONT_BOLD_DEFAULT            false

#define SETTINGS_INTERFACE_SUB_FONT_ITALICS                 "sub-font-italics"
#define SETTINGS_INTERFACE_SUB_FONT_ITALICS_DEFAULT         false

#define SETTINGS_INTERFACE_SUB_SCALE                        "sub-scale"
#define SETTINGS_INTERFACE_SUB_SCALE_DEFAULT                0.05

#define SETTINGS_INTERFACE_SUB_OFFSET                       "sub-offset"
#define SETTINGS_INTERFACE_SUB_OFFSET_DEFAULT               0.015

#define SETTINGS_INTERFACE_SUB_STROKE                       "sub-stroke"
#define SETTINGS_INTERFACE_SUB_STROKE_DEFAULT               11.0

#define SETTINGS_INTERFACE_SUB_TEXT_COLOR                   "sub-text-color"
#define SETTINGS_INTERFACE_SUB_TEXT_COLOR_DEFAULT           "#FFFFFFFF"

#define SETTINGS_INTERFACE_SUB_BG_COLOR                     "sub-bg-color"
#define SETTINGS_INTERFACE_SUB_BG_COLOR_DEFAULT             "#00000000"

#define SETTINGS_INTERFACE_SUB_STROKE_COLOR                 "sub-stroke-color"
#define SETTINGS_INTERFACE_SUB_STROKE_COLOR_DEFAULT         "#FF000000"

#define SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS              "sub-list-timestamps"
#define SETTINGS_INTERFACE_SUB_LIST_TIMESTAMPS_DEFAULT      false

/* Stylesheets */
#define SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE              "sublist-style"
#if __APPLE__
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
    "    font-family: \"Noto Sans\", \"Noto Sans CJK JP\", sans-serif;\n"\
    "    font-size: 20pt;\n"\
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
    "    font-family: \"Noto Sans\", \"Noto Sans CJK JP\", sans-serif;\n"\
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
#if __APPLE__
#define SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT         \
    "QLabel {\n"\
    "    font-family: \"Noto Sans\", \"Noto Sans CJK JP\", sans-serif;\n"\
    "    font-size: 15pt;\n"\
    "}"
#else
#define SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT         \
    "QLabel {\n"\
    "    font-family: \"Noto Sans\", \"Noto Sans CJK JP\", sans-serif;\n"\
    "    font-size: 11pt;\n"\
    "}"
#endif

#endif // CONSTANTS_H