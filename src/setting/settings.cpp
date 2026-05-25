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

#include "setting/settings.h"

#include <QDir>
#include <QSettings>

#if defined(Q_OS_WIN)
#include "util/utils.h"
#endif // defined(Q_OS_WIN)

Settings::Settings(QObject *parent) : QObject(parent)
{
    m_audioSource.sources = new AudioSourceModel(this);
    m_keybind.keybinds = new KeybindProfileModel(this);

    updateSettings();
    load();
}

Settings::~Settings()
{
    write();
}

void Settings::load()
{
    loadVersion();
    loadWindowSettings();
    loadRecentSettings();
    loadSubtitleListSettings();
    loadApplicationSettings();
    loadAudioSourceSettings();
    loadBehaviorSettings();
    loadDictionarySettings();
    loadSearchSettings();
    loadInterfaceSettings();
    loadKeybindSettings();
    loadOcrSettings();
}

void Settings::write()
{
    writeVersion();
    writeWindowSettings();
    writeRecentSettings();
    writeSubtitleListSettings();
    writeApplicationSettings();
    writeAudioSourceSettings();
    writeBehaviorSettings();
    writeDictionarySettings();
    writeSearchSettings();
    writeInterfaceSettings();
    writeKeybindSettings();
    writeOcrSettings();
}

void Settings::defaults()
{
    defaultSubtitleListSettings();
    defaultApplicationSettings();
    defaultAudioSourceSettings();
    defaultBehaviorSettings();
    defaultDictionarySettings();
    defaultSearchSettings();
    defaultInterfaceSettings();
    defaultKeybindSettings();
    defaultOcrSettings();
}

void Settings::loadVersion()
{
    QSettings s;
    m_version = s.value(Keys::Version::VERSION, 0).toUInt();
}

void Settings::writeVersion()
{
    QSettings s;
    s.setValue(Keys::Version::VERSION, m_version);
}

void Settings::loadWindowSettings()
{
    QSettings s;
    s.beginGroup(Keys::Window::GROUP);

    setWindowSubtitleList(s.value(Keys::Window::SUBTITLE_LIST).toBool());
    setWindowSearch(s.value(Keys::Window::SEARCH).toBool());

    s.endGroup();
}

void Settings::writeWindowSettings()
{
    QSettings s;
    s.beginGroup(Keys::Window::GROUP);

    s.setValue(Keys::Window::SUBTITLE_LIST, windowSubtitleList());
    s.setValue(Keys::Window::SEARCH, windowSearch());

    s.endGroup();
}

void Settings::loadRecentSettings()
{
    QSettings s;
    s.beginGroup(Keys::Recent::GROUP);

    m_recent.files = s.value(Keys::Recent::FILES).toStringList();
    emit recentFilesChanged();

    s.endGroup();
}

void Settings::writeRecentSettings()
{
    QSettings s;
    s.beginGroup(Keys::Recent::GROUP);

    s.setValue(Keys::Recent::FILES, recentFiles());

    s.endGroup();
}

void Settings::loadSubtitleListSettings()
{
    QSettings s;
    s.beginGroup(Keys::SubtitleList::GROUP);

    setSubtitleListIgnoreWhitespace(
        s.value(
            Keys::SubtitleList::IGNORE_WHITESPACE,
            Keys::SubtitleList::IGNORE_WHITESPACE_DEFAULT
        ).toBool()
    );
    setSubtitleListAutoSeek(
        s.value(
            Keys::SubtitleList::AUTO_SEEK,
            Keys::SubtitleList::AUTO_SEEK_DEFAULT
        ).toBool()
    );

    s.endGroup();
}

void Settings::writeSubtitleListSettings()
{
    QSettings s;
    s.beginGroup(Keys::SubtitleList::GROUP);

    s.setValue(
        Keys::SubtitleList::IGNORE_WHITESPACE,
        subtitleListIgnoreWhitespace()
    );
    s.setValue(
        Keys::SubtitleList::AUTO_SEEK,
        subtitleListAutoSeek()
    );

    s.endGroup();
}

void Settings::defaultSubtitleListSettings()
{
    setSubtitleListIgnoreWhitespace();
    setSubtitleListAutoSeek();
}

void Settings::loadApplicationSettings()
{
    QSettings s;
    s.beginGroup(Keys::Application::GROUP);

    setApplicationAutoUpdateCheck(
        s.value(
            Keys::Application::AUTO_UPDATE_CHECK,
            Keys::Application::AUTO_UPDATE_CHECK_DEFAULT
        ).toBool()
    );

    s.endGroup();
}

void Settings::writeApplicationSettings()
{
    QSettings s;
    s.beginGroup(Keys::Application::GROUP);

    s.setValue(
        Keys::Application::AUTO_UPDATE_CHECK,
        applicationAutoUpdateCheck()
    );

    s.endGroup();
}

void Settings::defaultApplicationSettings()
{
    setApplicationAutoUpdateCheck();
}

void Settings::loadAudioSourceSettings()
{
    QSettings s;

    audioSources()->clear();

    int size = s.beginReadArray(Keys::AudioSource::GROUP);
    for (int i = 0; i < size; ++i)
    {
        s.setArrayIndex(i);

        AudioSource src{};
        src.type = static_cast<Setting::AudioSourceType>(
            s.value(
                Keys::AudioSource::TYPE,
                static_cast<int>(Keys::AudioSource::TYPE_DEFAULT)
            ).toInt()
        );
        src.name = s.value(
            Keys::AudioSource::NAME,
            Keys::AudioSource::NAME_DEFAULT
        ).toString();
        src.url = s.value(
            Keys::AudioSource::URL,
            Keys::AudioSource::URL_DEFAULT
        ).toString();
        src.skipHash = s.value(
            Keys::AudioSource::MD5,
            Keys::AudioSource::MD5_DEFAULT
        ).toString();

        audioSources()->appendItem(src);
    }
    s.endArray();

    if (size == 0)
    {
        defaultAudioSourceSettings();
    }
}

void Settings::writeAudioSourceSettings()
{
    QSettings s;

    s.remove(Keys::AudioSource::GROUP);
    s.beginWriteArray(Keys::AudioSource::GROUP);
    for (qsizetype i = 0; i < audioSources()->items().size(); ++i)
    {
        s.setArrayIndex(i);

        const AudioSource &src = audioSources()->items()[i];
        s.setValue(Keys::AudioSource::NAME, src.name);
        s.setValue(Keys::AudioSource::URL, src.url);
        s.setValue(Keys::AudioSource::TYPE, static_cast<int>(src.type));
        s.setValue(Keys::AudioSource::MD5, src.skipHash);
    }
    s.endArray();
}

void Settings::defaultAudioSourceSettings()
{
    audioSources()->clear();
    audioSources()->appendItem(
        Keys::AudioSource::NAME_DEFAULT,
        Keys::AudioSource::URL_DEFAULT,
        Keys::AudioSource::TYPE_DEFAULT,
        Keys::AudioSource::MD5_DEFAULT
    );
}

void Settings::loadBehaviorSettings()
{
    QSettings s;
    s.beginGroup(Keys::Behavior::GROUP);

    setBehaviorAutoFit(
        s.value(
            Keys::Behavior::AUTO_FIT,
            Keys::Behavior::AUTO_FIT_DEFAULT
        ).toBool()
    );
    setBehaviorAutoFitPercent(
        s.value(
            Keys::Behavior::AUTO_FIT_PERCENT,
            Keys::Behavior::AUTO_FIT_PERCENT_DEFAULT
        ).toInt()
    );
    setBehaviorOscMpvCursor(
        s.value(
            Keys::Behavior::OSC_MPV_CURSOR,
            Keys::Behavior::OSC_MPV_CURSOR_DEFAULT
        ).toBool()
    );
    setBehaviorOscDuration(
        s.value(
            Keys::Behavior::OSC_DURATION,
            Keys::Behavior::OSC_DURATION_DEFAULT
        ).toInt()
    );
    setBehaviorOscFadeDuration(
        s.value(
            Keys::Behavior::OSC_FADE,
            Keys::Behavior::OSC_FADE_DEFAULT
        ).toInt()
    );
    setBehaviorOscMinMove(
        s.value(
            Keys::Behavior::OSC_MIN_MOVE,
            Keys::Behavior::OSC_MIN_MOVE_DEFAULT
        ).toInt()
    );
    setBehaviorOscPreviewThumbnails(
        s.value(
            Keys::Behavior::OSC_PREVIEW_THUMBNAILS,
            Keys::Behavior::OSC_PREVIEW_THUMBNAILS_DEFAULT
        ).toBool()
    );
    setBehaviorSubtitlePause(
        s.value(
            Keys::Behavior::SUBTITLE_PAUSE,
            Keys::Behavior::SUBTITLE_PAUSE_DEFAULT
        ).toBool()
    );
    setBehaviorSubtitleCursorShow(
        s.value(
            Keys::Behavior::SUBTITLE_CURSOR_SHOW,
            Keys::Behavior::SUBTITLE_CURSOR_SHOW_DEFAULT
        ).toBool()
    );
    setBehaviorSecondarySubtitleCursorShow(
        s.value(
            Keys::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW,
            Keys::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW_DEFAULT
        ).toBool()
    );
    setBehaviorFileOpenDirectory(
        static_cast<Setting::Directory>(s.value(
            Keys::Behavior::FILE_OPEN_DIR,
            static_cast<int>(Keys::Behavior::FILE_OPEN_DIR_DEFAULT)
        ).toInt())
    );
    setBehaviorFileOpenCustom(
        s.value(
            Keys::Behavior::FILE_OPEN_CUSTOM,
            Keys::Behavior::FILE_OPEN_CUSTOM_DEFAULT
        ).toString()
    );

    s.endGroup();
}

void Settings::writeBehaviorSettings()
{
    QSettings s;
    s.beginGroup(Keys::Behavior::GROUP);

    s.setValue(
        Keys::Behavior::AUTO_FIT,
        behaviorAutoFit()
    );
    s.setValue(
        Keys::Behavior::AUTO_FIT_PERCENT,
        behaviorAutoFitPercent()
    );
    s.setValue(
        Keys::Behavior::OSC_MPV_CURSOR,
        behaviorOscMpvCursor()
    );
    s.setValue(
        Keys::Behavior::OSC_DURATION,
        behaviorOscDuration()
    );
    s.setValue(
        Keys::Behavior::OSC_FADE,
        behaviorOscFadeDuration()
    );
    s.setValue(
        Keys::Behavior::OSC_MIN_MOVE,
        behaviorOscMinMove()
    );
    s.setValue(
        Keys::Behavior::OSC_PREVIEW_THUMBNAILS,
        behaviorOscPreviewThumbnails()
    );
    s.setValue(
        Keys::Behavior::SUBTITLE_PAUSE,
        behaviorSubtitlePause()
    );
    s.setValue(
        Keys::Behavior::SUBTITLE_CURSOR_SHOW,
        behaviorSubtitleCursorShow()
    );
    s.setValue(
        Keys::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW,
        behaviorSecondarySubtitleCursorShow()
    );
    s.setValue(
        Keys::Behavior::FILE_OPEN_DIR,
        static_cast<int>(behaviorFileOpenDirectory())
    );
    s.setValue(
        Keys::Behavior::FILE_OPEN_CUSTOM,
        behaviorFileOpenCustom()
    );

    s.endGroup();
}

void Settings::defaultBehaviorSettings()
{
    setBehaviorAutoFit();
    setBehaviorAutoFitPercent();
    setBehaviorOscMpvCursor();
    setBehaviorOscDuration();
    setBehaviorOscFadeDuration();
    setBehaviorOscMinMove();
    setBehaviorOscPreviewThumbnails();
    setBehaviorSubtitlePause();
    setBehaviorSubtitleCursorShow();
    setBehaviorSecondarySubtitleCursorShow();
    setBehaviorFileOpenDirectory();
    setBehaviorFileOpenCustom();
}

void Settings::loadDictionarySettings()
{
    QSettings s;
    s.beginGroup(Keys::Dictionary::GROUP);

    QVariantList order = s.value(Keys::Dictionary::ORDER).toList();
    QList<int64_t> orderList;
    orderList.reserve(order.size());
    for (const QVariant &value : order)
    {
        orderList.emplaceBack(value.toInt());
    }
    setDictionaryOrder(orderList);

    s.endGroup();
}

void Settings::writeDictionarySettings()
{
    QSettings s;
    s.beginGroup(Keys::Dictionary::GROUP);

    QVariantList order;
    for (int64_t value : dictionaryOrder())
    {
        order.emplaceBack(QVariant::fromValue(value));
    }
    s.setValue(
        Keys::Dictionary::ORDER,
        order
    );

    s.endGroup();
}

void Settings::defaultDictionarySettings()
{
    setDictionaryOrder();
}

void Settings::loadSearchSettings()
{
    QSettings s;
    s.beginGroup(Keys::Search::GROUP);

    setSearchMatcherExact(
        s.value(
            Keys::Search::Matcher::EXACT,
            Keys::Search::Matcher::EXACT_DEFAULT
        ).toBool()
    );
    setSearchMatcherDeconj(
        s.value(
            Keys::Search::Matcher::DECONJ,
            Keys::Search::Matcher::DECONJ_DEFAULT
        ).toBool()
    );
    setSearchMatcherMecabIpadic(
        s.value(
            Keys::Search::Matcher::MECAB_IPADIC,
            Keys::Search::Matcher::MECAB_IPADIC_DEFAULT
        ).toBool()
    );
    setSearchMiddleMouseScan(
        s.value(
            Keys::Search::MIDDLE_MOUSE_SCAN,
            Keys::Search::MIDDLE_MOUSE_SCAN_DEFAULT
        ).toBool()
    );
    setSearchMethod(
        static_cast<Setting::SearchMethod>(s.value(
            Keys::Search::METHOD,
            static_cast<int>(Keys::Search::METHOD_DEFAULT)
        ).toInt())
    );
    setSearchDelay(
        s.value(
            Keys::Search::DELAY,
            Keys::Search::DELAY_DEFAULT
        ).toInt()
    );
    setSearchModifier(
        static_cast<Setting::Modifier>(s.value(
            Keys::Search::MODIFIER,
            static_cast<int>(Keys::Search::MODIFIER_DEFAULT)
        ).toInt())
    );
    setSearchHideMpvSubs(
        s.value(
            Keys::Search::HIDE_MPV_SUBS,
            Keys::Search::HIDE_MPV_SUBS_DEFAULT
        ).toBool()
    );
    setSearchHideSubs(
        s.value(
            Keys::Search::HIDE_SUBS,
            Keys::Search::HIDE_SUBS_DEFAULT
        ).toBool()
    );
    setSearchPauseOnHover(
        s.value(
            Keys::Search::PAUSE_ON_HOVER,
            Keys::Search::PAUSE_ON_HOVER_DEFAULT
        ).toBool()
    );
    setSearchAutoPlayAudio(
        s.value(
            Keys::Search::AUTO_PLAY_AUDIO,
            Keys::Search::AUTO_PLAY_AUDIO_DEFAULT
        ).toBool()
    );
    setSearchGlossaryStyle(
        static_cast<Setting::GlossaryStyle>(s.value(
            Keys::Search::GLOSSARY_STYLE,
            static_cast<int>(Keys::Search::GLOSSARY_STYLE_DEFAULT)
        ).toInt())
    );
    setSearchReplaceNewlines(
        s.value(
            Keys::Search::REPLACE_NEWLINES,
            Keys::Search::REPLACE_NEWLINES_DEFAULT
        ).toBool()
    );
    setSearchReplaceNewlinesWith(
        s.value(
            Keys::Search::REPLACE_NEWLINES_WITH,
            Keys::Search::REPLACE_NEWLINES_WITH_DEFAULT
        ).toString()
    );
    setSearchRemoveRegex(
        s.value(
            Keys::Search::REMOVE_REGEX,
            Keys::Search::REMOVE_REGEX_DEFAULT
        ).toString()
    );

    s.endGroup();
}

void Settings::writeSearchSettings()
{
    QSettings s;
    s.beginGroup(Keys::Search::GROUP);

    s.setValue(
        Keys::Search::Matcher::EXACT,
        searchMatcherExact()
    );
    s.setValue(
        Keys::Search::Matcher::DECONJ,
        searchMatcherDeconj()
    );
    s.setValue(
        Keys::Search::Matcher::MECAB_IPADIC,
        searchMatcherMecabIpadic()
    );
    s.setValue(
        Keys::Search::MIDDLE_MOUSE_SCAN,
        searchMiddleMouseScan()
    );
    s.setValue(
        Keys::Search::METHOD,
        searchMethod()
    );
    s.setValue(
        Keys::Search::DELAY,
        searchDelay()
    );
    s.setValue(
        Keys::Search::MODIFIER,
        static_cast<int>(searchModifier())
    );
    s.setValue(
        Keys::Search::HIDE_MPV_SUBS,
        searchHideMpvSubs()
    );
    s.setValue(
        Keys::Search::HIDE_SUBS,
        searchHideSubs()
    );
    s.setValue(
        Keys::Search::PAUSE_ON_HOVER,
        searchPauseOnHover()
    );
    s.setValue(
        Keys::Search::AUTO_PLAY_AUDIO,
        searchAutoPlayAudio()
    );
    s.setValue(
        Keys::Search::GLOSSARY_STYLE,
        static_cast<int>(searchGlossaryStyle())
    );
    s.setValue(
        Keys::Search::REPLACE_NEWLINES,
        searchReplaceNewlines()
    );
    s.setValue(
        Keys::Search::REPLACE_NEWLINES_WITH,
        searchReplaceNewlinesWith()
    );
    s.setValue(
        Keys::Search::REMOVE_REGEX,
        searchRemoveRegex()
    );

    s.endGroup();
}

void Settings::defaultSearchSettings()
{
    setSearchMatcherExact();
    setSearchMatcherDeconj();
    setSearchMatcherMecabIpadic();
    setSearchMiddleMouseScan();
    setSearchMethod();
    setSearchDelay();
    setSearchModifier();
    setSearchHideMpvSubs();
    setSearchHideSubs();
    setSearchPauseOnHover();
    setSearchAutoPlayAudio();
    setSearchGlossaryStyle();
    setSearchReplaceNewlines();
    setSearchReplaceNewlinesWith();
    setSearchRemoveRegex();
}

void Settings::loadInterfaceSettings()
{
    QSettings s;
    s.beginGroup(Keys::Interface::GROUP);

    setInterfaceSystemIcons(
        s.value(
            Keys::Interface::SYSTEM_ICONS,
            Keys::Interface::SYSTEM_ICONS_DEFAULT
        ).toBool()
    );
    setInterfaceSubtitleFont(
        s.value(
            Keys::Interface::Subtitle::FONT,
            QFont(Keys::Interface::Subtitle::FONT_DEFAULT)
        ).value<QFont>()
    );
    setInterfaceSubtitleLineSpacing(
        s.value(
            Keys::Interface::Subtitle::LINE_SPACING,
            Keys::Interface::Subtitle::LINE_SPACING_DEFAULT
        ).toDouble()
    );
    setInterfaceSubtitleScale(
        s.value(
            Keys::Interface::Subtitle::SCALE,
            Keys::Interface::Subtitle::SCALE_DEFAULT
        ).toDouble()
    );
    setInterfaceSubtitleOffset(
        s.value(
            Keys::Interface::Subtitle::OFFSET,
            Keys::Interface::Subtitle::OFFSET_DEFAULT
        ).toDouble()
    );
    setInterfaceSubtitleStroke(
        s.value(
            Keys::Interface::Subtitle::STROKE,
            Keys::Interface::Subtitle::STROKE_DEFAULT
        ).toDouble()
    );
    setInterfaceSubtitleColor(
        s.value(
            Keys::Interface::Subtitle::TEXT_COLOR,
            Keys::Interface::Subtitle::TEXT_COLOR_DEFAULT
        ).toString()
    );
    setInterfaceSubtitleBackground(
        s.value(
            Keys::Interface::Subtitle::BACKGROUND_COLOR,
            Keys::Interface::Subtitle::BACKGROUND_COLOR_DEFAULT
        ).toString()
    );
    setInterfaceSubtitleStrokeColor(
        s.value(
            Keys::Interface::Subtitle::STROKE_COLOR,
            Keys::Interface::Subtitle::STROKE_COLOR_DEFAULT
        ).toString()
    );
    setInterfaceSearchWindow(
        s.value(
            Keys::Interface::SEARCH_WINDOW,
            Keys::Interface::SEARCH_WINDOW_DEFAULT
        ).toBool()
    );
    setInterfacePopupWidth(
        s.value(
            Keys::Interface::POPUP_WIDTH,
            Keys::Interface::POPUP_WIDTH_DEFAULT
        ).toInt()
    );
    setInterfacePopupHeight(
        s.value(
            Keys::Interface::POPUP_HEIGHT,
            Keys::Interface::POPUP_HEIGHT_DEFAULT
        ).toInt()
    );
    setInterfaceSearchWindow(
        s.value(
            Keys::Interface::SEARCH_WINDOW,
            Keys::Interface::SEARCH_WINDOW_DEFAULT
        ).toBool()
    );
    setInterfaceSearchExpressionFont(
        s.value(
            Keys::Interface::SEARCH_EXPRESSION_FONT,
            QFont(Keys::Interface::SEARCH_EXPRESSION_FONT_DEFAULT)
        ).value<QFont>()
    );
    setInterfaceSearchReadingFont(
        s.value(
            Keys::Interface::SEARCH_READING_FONT,
            QFont(Keys::Interface::SEARCH_READING_FONT_DEFAULT)
        ).value<QFont>()
    );
    setInterfaceSearchConjFont(
        s.value(
            Keys::Interface::SEARCH_CONJ_FONT,
            QFont(Keys::Interface::SEARCH_CONJ_FONT_DEFAULT)
        ).value<QFont>()
    );
    setInterfaceSearchTagFont(
        s.value(
            Keys::Interface::SEARCH_TAG_FONT,
            QFont(Keys::Interface::SEARCH_TAG_FONT_DEFAULT)
        ).value<QFont>()
    );
    setInterfaceSearchGlossaryFont(
        s.value(
            Keys::Interface::SEARCH_GLOSSARY_FONT,
            QFont(Keys::Interface::SEARCH_GLOSSARY_FONT_DEFAULT)
        ).value<QFont>()
    );
    setInterfaceSearchKanjiFont(
        s.value(
            Keys::Interface::SEARCH_KANJI_FONT,
            QFont(Keys::Interface::SEARCH_KANJI_FONT_DEFAULT)
        ).value<QFont>()
    );
    setInterfaceSubtitleListWindow(
        s.value(
            Keys::Interface::SubtitleList::WINDOW,
            Keys::Interface::SubtitleList::WINDOW_DEFAULT
        ).toBool()
    );
    setInterfaceSubtitleListTimestamps(
        s.value(
            Keys::Interface::SubtitleList::TIMESTAMPS,
            Keys::Interface::SubtitleList::TIMESTAMPS_DEFAULT
        ).toBool()
    );
    setInterfaceSubtitleListBackgroundColor(
        s.value(
            Keys::Interface::SubtitleList::BACKGROUND_COLOR,
            Keys::Interface::SubtitleList::BACKGROUND_COLOR_DEFAULT
        ).toString()
    );
    setInterfaceSubtitleListTextColor(
        s.value(
            Keys::Interface::SubtitleList::TEXT_COLOR,
            Keys::Interface::SubtitleList::TEXT_COLOR_DEFAULT
        ).toString()
    );
    setInterfaceSubtitleListHoverColor(
        s.value(
            Keys::Interface::SubtitleList::HOVER_COLOR,
            Keys::Interface::SubtitleList::HOVER_COLOR_DEFAULT
        ).toString()
    );
    setInterfaceSubtitleListSelectedColor(
        s.value(
            Keys::Interface::SubtitleList::SELECTED_COLOR,
            Keys::Interface::SubtitleList::SELECTED_COLOR_DEFAULT
        ).toString()
    );
    setInterfaceSubtitleListSelectedHoverColor(
        s.value(
            Keys::Interface::SubtitleList::SELECTED_HOVER_COLOR,
            Keys::Interface::SubtitleList::SELECTED_HOVER_COLOR_DEFAULT
        ).toString()
    );
    setInterfaceSubtitleListPrimaryFont(
        s.value(
            Keys::Interface::SubtitleList::PRIMARY_FONT,
            QFont(Keys::Interface::SubtitleList::PRIMARY_FONT_DEFAULT)
        ).value<QFont>()
    );
    setInterfaceSubtitleListSecondaryFont(
        s.value(
            Keys::Interface::SubtitleList::SECONDARY_FONT,
            QFont(Keys::Interface::SubtitleList::SECONDARY_FONT_DEFAULT)
        ).value<QFont>()
    );

    s.endGroup();
}

void Settings::writeInterfaceSettings()
{
    QSettings s;
    s.beginGroup(Keys::Interface::GROUP);

    s.setValue(
        Keys::Interface::SYSTEM_ICONS,
        interfaceSystemIcons()
    );
    s.setValue(
        Keys::Interface::Subtitle::FONT,
        interfaceSubtitleFont()
    );
    s.setValue(
        Keys::Interface::Subtitle::LINE_SPACING,
        interfaceSubtitleLineSpacing()
    );
    s.setValue(
        Keys::Interface::Subtitle::SCALE,
        interfaceSubtitleScale()
    );
    s.setValue(
        Keys::Interface::Subtitle::OFFSET,
        interfaceSubtitleOffset()
    );
    s.setValue(
        Keys::Interface::Subtitle::STROKE,
        interfaceSubtitleStroke()
    );
    s.setValue(
        Keys::Interface::Subtitle::TEXT_COLOR,
        interfaceSubtitleColor().name(QColor::HexArgb)
    );
    s.setValue(
        Keys::Interface::Subtitle::BACKGROUND_COLOR,
        interfaceSubtitleBackground().name(QColor::HexArgb)
    );
    s.setValue(
        Keys::Interface::Subtitle::STROKE_COLOR,
        interfaceSubtitleStrokeColor().name(QColor::HexArgb)
    );
    s.setValue(
        Keys::Interface::POPUP_WIDTH,
        interfacePopupWidth()
    );
    s.setValue(
        Keys::Interface::POPUP_HEIGHT,
        interfacePopupHeight()
    );
    s.setValue(
        Keys::Interface::SEARCH_WINDOW,
        interfaceSearchWindow()
    );
    s.setValue(
        Keys::Interface::SEARCH_EXPRESSION_FONT,
        interfaceSearchExpressionFont()
    );
    s.setValue(
        Keys::Interface::SEARCH_READING_FONT,
        interfaceSearchReadingFont()
    );
    s.setValue(
        Keys::Interface::SEARCH_CONJ_FONT,
        interfaceSearchConjFont()
    );
    s.setValue(
        Keys::Interface::SEARCH_TAG_FONT,
        interfaceSearchTagFont()
    );
    s.setValue(
        Keys::Interface::SEARCH_GLOSSARY_FONT,
        interfaceSearchGlossaryFont()
    );
    s.setValue(
        Keys::Interface::SEARCH_KANJI_FONT,
        interfaceSearchKanjiFont()
    );
    s.setValue(
        Keys::Interface::SubtitleList::WINDOW,
        interfaceSubtitleListWindow()
    );
    s.setValue(
        Keys::Interface::SubtitleList::TIMESTAMPS,
        interfaceSubtitleListTimestamps()
    );
    s.setValue(
        Keys::Interface::SubtitleList::BACKGROUND_COLOR,
        interfaceSubtitleListBackgroundColor().name(QColor::HexArgb)
    );
    s.setValue(
        Keys::Interface::SubtitleList::TEXT_COLOR,
        interfaceSubtitleListTextColor().name(QColor::HexArgb)
    );
    s.setValue(
        Keys::Interface::SubtitleList::HOVER_COLOR,
        interfaceSubtitleListHoverColor().name(QColor::HexArgb)
    );
    s.setValue(
        Keys::Interface::SubtitleList::SELECTED_COLOR,
        interfaceSubtitleListSelectedColor().name(QColor::HexArgb)
    );
    s.setValue(
        Keys::Interface::SubtitleList::SELECTED_HOVER_COLOR,
        interfaceSubtitleListSelectedHoverColor().name(QColor::HexArgb)
    );
    s.setValue(
        Keys::Interface::SubtitleList::PRIMARY_FONT,
        interfaceSubtitleListPrimaryFont()
    );
    s.setValue(
        Keys::Interface::SubtitleList::SECONDARY_FONT,
        interfaceSubtitleListSecondaryFont()
    );

    s.endGroup();
}

void Settings::defaultInterfaceSettings()
{
    setInterfaceSystemIcons();
    setInterfaceSubtitleFont();
    setInterfaceSubtitleLineSpacing();
    setInterfaceSubtitleScale();
    setInterfaceSubtitleOffset();
    setInterfaceSubtitleStroke();
    setInterfaceSubtitleColor();
    setInterfaceSubtitleBackground();
    setInterfaceSubtitleStrokeColor();
    setInterfacePopupWidth();
    setInterfacePopupHeight();
    setInterfaceSearchWindow();
    setInterfaceSearchExpressionFont();
    setInterfaceSearchReadingFont();
    setInterfaceSearchConjFont();
    setInterfaceSearchTagFont();
    setInterfaceSearchGlossaryFont();
    setInterfaceSearchKanjiFont();
    setInterfaceSubtitleListWindow();
    setInterfaceSubtitleListTimestamps();
    setInterfaceSubtitleListBackgroundColor();
    setInterfaceSubtitleListTextColor();
    setInterfaceSubtitleListHoverColor();
    setInterfaceSubtitleListSelectedColor();
    setInterfaceSubtitleListSelectedHoverColor();
    setInterfaceSubtitleListPrimaryFont();
    setInterfaceSubtitleListSecondaryFont();
}

void Settings::loadKeybindSettings()
{
    QSettings s;
    s.beginGroup(Keys::Keybind::GROUP);

    QString currentProfileName =
        s.value(Keys::Keybind::CURRENT_PROFILE).toString();

    s.beginGroup(Keys::Keybind::Profile::GROUP);
    QStringList names = s.childGroups();
    QList<KeybindProfile *> profiles;
    profiles.reserve(names.size());
    for (const QString &name : names)
    {
        s.beginGroup(name);

        KeybindProfile *profile = new KeybindProfile(this);
        profile->setName(name);
        if (s.contains(Keys::Keybind::Profile::CARD_ADD))
        {
            profile->setCardAdd(
                s.value(
                    Keys::Keybind::Profile::CARD_ADD
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::CARD_NEXT))
        {
            profile->setCardNext(
                s.value(
                    Keys::Keybind::Profile::CARD_NEXT
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::CARD_PREVIOUS))
        {
            profile->setCardPrevious(
                s.value(
                    Keys::Keybind::Profile::CARD_PREVIOUS
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::OPEN_FILE))
        {
            profile->setOpenFile(
                s.value(
                    Keys::Keybind::Profile::OPEN_FILE
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::OPEN_URL))
        {
            profile->setOpenUrl(
                s.value(
                    Keys::Keybind::Profile::OPEN_URL
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::OSC_VISIBILITY))
        {
            profile->setOscVisibility(
                s.value(
                    Keys::Keybind::Profile::OSC_VISIBILITY
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SHOW_SEARCH))
        {
            profile->setShowSearch(
                s.value(
                    Keys::Keybind::Profile::SHOW_SEARCH
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SHOW_SUBTITLE_LIST))
        {
            profile->setShowSubtitleList(
                s.value(
                    Keys::Keybind::Profile::SHOW_SUBTITLE_LIST
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_AUTO_PAUSE))
        {
            profile->setSubtitleAutoPause(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_AUTO_PAUSE
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_DECREASE_SIZE))
        {
            profile->setSubtitleDecreaseSize(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_DECREASE_SIZE
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_INCREASE_SIZE))
        {
            profile->setSubtitleIncreaseSize(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_INCREASE_SIZE
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_MOVE_DOWN))
        {
            profile->setSubtitleMoveDown(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_MOVE_DOWN
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_MOVE_UP))
        {
            profile->setSubtitleMoveUp(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_MOVE_UP
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_SHOW))
        {
            profile->setSubtitleShow(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_SHOW
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_LIST_COPY_SELECTED))
        {
            profile->setSubtitleListCopySelected(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_LIST_COPY_SELECTED
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_LIST_FIND))
        {
            profile->setSubtitleListFind(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_LIST_FIND
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_LIST_FIND_NEXT))
        {
            profile->setSubtitleListFindNext(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_LIST_FIND_NEXT
                ).toString()
            );
        }
        if (s.contains(Keys::Keybind::Profile::SUBTITLE_LIST_FIND_PREVIOUS))
        {
            profile->setSubtitleListFindPrevious(
                s.value(
                    Keys::Keybind::Profile::SUBTITLE_LIST_FIND_PREVIOUS
                ).toString()
            );
        }
        profiles.emplaceBack(profile);

        s.endGroup();
    }
    s.endGroup();

    s.endGroup();

    if (profiles.isEmpty())
    {
        defaultKeybindSettings();
    }
    else
    {
        keybinds()->setProfiles(std::move(profiles));
        keybinds()->setProfileByName(currentProfileName);
    }
}

void Settings::writeKeybindSettings()
{
    QSettings s;
    s.beginGroup(Keys::Keybind::GROUP);

    if (keybinds()->profile() != nullptr)
    {
        s.setValue(
            Keys::Keybind::CURRENT_PROFILE,
            keybinds()->profile()->name()
        );
    }
    else
    {
        s.remove(Keys::Keybind::CURRENT_PROFILE);
    }

    s.beginGroup(Keys::Keybind::Profile::GROUP);
    s.remove("");
    for (const KeybindProfile *profile : keybinds()->profiles())
    {
        s.beginGroup(profile->name());

        s.setValue(
            Keys::Keybind::Profile::CARD_ADD,
            profile->cardAdd()
        );
        s.setValue(
            Keys::Keybind::Profile::CARD_NEXT,
            profile->cardNext()
        );
        s.setValue(
            Keys::Keybind::Profile::CARD_PREVIOUS,
            profile->cardPrevious()
        );
        s.setValue(
            Keys::Keybind::Profile::OPEN_FILE,
            profile->openFile()
        );
        s.setValue(
            Keys::Keybind::Profile::OPEN_URL,
            profile->openUrl()
        );
        s.setValue(
            Keys::Keybind::Profile::OSC_VISIBILITY,
            profile->oscVisibility()
        );
        s.setValue(
            Keys::Keybind::Profile::SHOW_SEARCH,
            profile->showSearch()
        );
        s.setValue(
            Keys::Keybind::Profile::SHOW_SUBTITLE_LIST,
            profile->showSubtitleList()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_AUTO_PAUSE,
            profile->subtitleAutoPause()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_DECREASE_SIZE,
            profile->subtitleDecreaseSize()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_INCREASE_SIZE,
            profile->subtitleIncreaseSize()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_MOVE_DOWN,
            profile->subtitleMoveDown()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_MOVE_UP,
            profile->subtitleMoveUp()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_SHOW,
            profile->subtitleShow()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_LIST_COPY_SELECTED,
            profile->subtitleListCopySelected()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_LIST_FIND,
            profile->subtitleListFind()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_LIST_FIND_NEXT,
            profile->subtitleListFindNext()
        );
        s.setValue(
            Keys::Keybind::Profile::SUBTITLE_LIST_FIND_PREVIOUS,
            profile->subtitleListFindPrevious()
        );

        s.endGroup();
    }
    s.endGroup();

    s.endGroup();
}

void Settings::defaultKeybindSettings()
{
    QList<KeybindProfile *> profiles = {new KeybindProfile(this)};
    keybinds()->setProfiles(std::move(profiles));
}

void Settings::loadOcrSettings()
{
    QSettings s;
    s.beginGroup(Keys::Ocr::GROUP);

    setOcrEnabled(
        s.value(
            Keys::Ocr::ENABLED,
            Keys::Ocr::ENABLED_DEFAULT
        ).toBool()
    );
    setOcrUseGpu(
        s.value(
            Keys::Ocr::USE_GPU,
            Keys::Ocr::USE_GPU_DEFAULT
        ).toBool()
    );
    setOcrModel(
        s.value(
            Keys::Ocr::MODEL,
            Keys::Ocr::MODEL_DEFAULT
        ).toString()
    );

    s.endGroup();
}

void Settings::writeOcrSettings()
{
    QSettings s;
    s.beginGroup(Keys::Ocr::GROUP);

    s.setValue(
        Keys::Ocr::ENABLED,
        ocrEnabled()
    );
    s.setValue(
        Keys::Ocr::USE_GPU,
        ocrUseGpu()
    );
    s.setValue(
        Keys::Ocr::MODEL,
        ocrModel()
    );

    s.endGroup();
}

void Settings::defaultOcrSettings()
{
    setOcrEnabled();
    setOcrUseGpu();
    setOcrModel();
}

/* Version Settings */

unsigned int Settings::version() const noexcept
{
    return m_version;
}

void Settings::setVersion(unsigned int value)
{
    if (m_version == value)
    {
        return;
    }
    m_version = value;
    emit versionChanged(m_version);
}

/* Window Settings */

bool Settings::windowSubtitleList() const noexcept
{
    return m_window.subtitleList;
}

void Settings::setWindowSubtitleList(bool value)
{
    if (m_window.subtitleList == value)
    {
        return;
    }
    m_window.subtitleList = value;
    emit windowSubtitleListChanged(m_window.subtitleList);
}

bool Settings::windowSearch() const noexcept
{
    return m_window.search;
}

void Settings::setWindowSearch(bool value)
{
    if (m_window.search == value)
    {
        return;
    }
    m_window.search = value;
    emit windowSearchChanged(m_window.search);
}

/* Recent Settings */

const QStringList &Settings::recentFiles() const noexcept
{
    return m_recent.files;
}

void Settings::recentFilesAdd(const QString &value)
{
    constexpr qsizetype MAX_SIZE{10};

    m_recent.files.removeAll(value);
    m_recent.files.prepend(value);
    if (m_recent.files.size() > MAX_SIZE)
    {
        m_recent.files.pop_back();
    }
    emit recentFilesChanged();
}

void Settings::recentFilesClear()
{
    m_recent.files.clear();
    emit recentFilesChanged();
}

/* Subtitle List Settings */

bool Settings::subtitleListIgnoreWhitespace() const noexcept
{
    return m_subtitleList.ignoreWhitespace;
}

void Settings::setSubtitleListIgnoreWhitespace(bool value)
{
    if (m_subtitleList.ignoreWhitespace == value)
    {
        return;
    }
    m_subtitleList.ignoreWhitespace = value;
    emit subtitleListIgnoreWhitespaceChanged(m_subtitleList.ignoreWhitespace);
}

bool Settings::subtitleListAutoSeek() const noexcept
{
    return m_subtitleList.autoSeek;
}

void Settings::setSubtitleListAutoSeek(bool value)
{
    if (m_subtitleList.autoSeek == value)
    {
        return;
    }
    m_subtitleList.autoSeek = value;
    emit subtitleListAutoSeekChanged(m_subtitleList.autoSeek);
}

/* Application Settings */

bool Settings::applicationAutoUpdateCheck() const noexcept
{
    return m_applicationSettings.autoUpdateCheck;
}

void Settings::setApplicationAutoUpdateCheck(bool value)
{
    if (m_applicationSettings.autoUpdateCheck == value)
    {
        return;
    }
    m_applicationSettings.autoUpdateCheck = value;
    emit applicationAutoUpdateCheckChanged(
        m_applicationSettings.autoUpdateCheck
    );
}

/* Audio Source Settings */

AudioSourceModel *Settings::audioSources() const noexcept
{
    return m_audioSource.sources;
}

/* Behavior Settings */

bool Settings::behaviorAutoFit() const noexcept
{
    return m_behavior.autoFit;
}

void Settings::setBehaviorAutoFit(bool value)
{
    if (m_behavior.autoFit == value)
    {
        return;
    }
    m_behavior.autoFit = value;
    emit behaviorAutoFitChanged(m_behavior.autoFit);
}

int Settings::behaviorAutoFitPercent() const noexcept
{
    return m_behavior.autoFitPercent;
}

void Settings::setBehaviorAutoFitPercent(int value)
{
    if (m_behavior.autoFitPercent == value)
    {
        return;
    }
    m_behavior.autoFitPercent = value;
    emit behaviorAutoFitPercentChanged(m_behavior.autoFitPercent);
}

bool Settings::behaviorOscMpvCursor() const noexcept
{
    return m_behavior.oscMpvCursor;
}

void Settings::setBehaviorOscMpvCursor(bool value)
{
    if (m_behavior.oscMpvCursor == value)
    {
        return;
    }
    m_behavior.oscMpvCursor = value;
    emit behaviorOscMpvCursorChanged(m_behavior.oscMpvCursor);
}

int Settings::behaviorOscDuration() const noexcept
{
    return m_behavior.oscDuration;
}

void Settings::setBehaviorOscDuration(int value)
{
    if (m_behavior.oscDuration == value)
    {
        return;
    }
    m_behavior.oscDuration = value;
    emit behaviorOscDurationChanged(m_behavior.oscDuration);
}

int Settings::behaviorOscFadeDuration() const noexcept
{
    return m_behavior.oscFadeDuration;
}

void Settings::setBehaviorOscFadeDuration(int value)
{
    if (m_behavior.oscFadeDuration == value)
    {
        return;
    }
    m_behavior.oscFadeDuration = value;
    emit behaviorOscFadeDurationChanged(m_behavior.oscFadeDuration);
}

int Settings::behaviorOscMinMove() const noexcept
{
    return m_behavior.oscMinMove;
}

void Settings::setBehaviorOscMinMove(int value)
{
    if (m_behavior.oscMinMove == value)
    {
        return;
    }
    m_behavior.oscMinMove = value;
    emit behaviorOscMinMoveChanged(m_behavior.oscMinMove);
}

bool Settings::behaviorOscPreviewThumbnails() const noexcept
{
    return m_behavior.oscPreviewThumbnails;
}

void Settings::setBehaviorOscPreviewThumbnails(bool value)
{
    if (m_behavior.oscPreviewThumbnails == value)
    {
        return;
    }
    m_behavior.oscPreviewThumbnails = value;
    emit behaviorOscPreviewThumbnailsChanged(m_behavior.oscPreviewThumbnails);
}

bool Settings::behaviorSubtitlePause() const noexcept
{
    return m_behavior.subtitlePause;
}

void Settings::setBehaviorSubtitlePause(bool value)
{
    if (m_behavior.subtitlePause == value)
    {
        return;
    }
    m_behavior.subtitlePause = value;
    emit behaviorSubtitlePauseChanged(m_behavior.subtitlePause);
}

bool Settings::behaviorSubtitleCursorShow() const noexcept
{
    return m_behavior.subtitleCursorShow;
}

void Settings::setBehaviorSubtitleCursorShow(bool value)
{
    if (m_behavior.subtitleCursorShow == value)
    {
        return;
    }
    m_behavior.subtitleCursorShow = value;
    emit behaviorSubtitleCursorShowChanged(m_behavior.subtitleCursorShow);
}

bool Settings::behaviorSecondarySubtitleCursorShow() const noexcept
{
    return m_behavior.secondarySubtitleCursorShow;
}

void Settings::setBehaviorSecondarySubtitleCursorShow(bool value)
{
    if (m_behavior.secondarySubtitleCursorShow == value)
    {
        return;
    }
    m_behavior.secondarySubtitleCursorShow = value;
    emit behaviorSecondarySubtitleCursorShowChanged(
        m_behavior.secondarySubtitleCursorShow
    );
}

Setting::Directory Settings::behaviorFileOpenDirectory() const noexcept
{
    return m_behavior.fileOpenDirectory;
}

void Settings::setBehaviorFileOpenDirectory(Setting::Directory value)
{
    if (m_behavior.fileOpenDirectory == value)
    {
        return;
    }
    m_behavior.fileOpenDirectory = value;
    emit behaviorFileOpenDirectoryChanged(m_behavior.fileOpenDirectory);
}

QString Settings::behaviorFileOpenCustom() const noexcept
{
    return m_behavior.fileOpenCustom;
}

void Settings::setBehaviorFileOpenCustom(const QString &value)
{
    if (m_behavior.fileOpenCustom == value)
    {
        return;
    }
    m_behavior.fileOpenCustom = value;
    emit behaviorFileOpenCustomChanged(m_behavior.fileOpenCustom);
}

/* Dictionary Settings */

const QList<int64_t> &Settings::dictionaryOrder() const noexcept
{
    return m_dictionary.order;
}

void Settings::setDictionaryOrder(const QList<int64_t> &order)
{
    if (m_dictionary.order == order)
    {
        return;
    }
    m_dictionary.order = order;
    emit dictionaryOrderChanged();
}

/* Search Settings */

bool Settings::searchMatcherExact() const noexcept
{
    return m_search.matcherExact;
}

void Settings::setSearchMatcherExact(bool value)
{
    if (m_search.matcherExact == value)
    {
        return;
    }
    m_search.matcherExact = value;
    emit searchMatcherExactChanged(m_search.matcherExact);
}

bool Settings::searchMatcherDeconj() const noexcept
{
    return m_search.matcherDeconj;
}

void Settings::setSearchMatcherDeconj(bool value)
{
    if (m_search.matcherDeconj == value)
    {
        return;
    }
    m_search.matcherDeconj = value;
    emit searchMatcherDeconjChanged(m_search.matcherDeconj);
}

bool Settings::searchMatcherMecabIpadic() const noexcept
{
    return m_search.matcherMecabIpadic;
}

void Settings::setSearchMatcherMecabIpadic(bool value)
{
    if (m_search.matcherMecabIpadic == value)
    {
        return;
    }
    m_search.matcherMecabIpadic = value;
    emit searchMatcherMecabIpadicChanged(m_search.matcherMecabIpadic);
}

bool Settings::searchMiddleMouseScan() const noexcept
{
    return m_search.middleMouseScan;
}

void Settings::setSearchMiddleMouseScan(bool value)
{
    if (m_search.middleMouseScan == value)
    {
        return;
    }
    m_search.middleMouseScan = value;
    emit searchMiddleMouseScanChanged(m_search.middleMouseScan);
}

Setting::SearchMethod Settings::searchMethod() const noexcept
{
    return m_search.method;
}

void Settings::setSearchMethod(Setting::SearchMethod value)
{
    if (m_search.method == value)
    {
        return;
    }
    m_search.method = value;
    emit searchMethodChanged(m_search.method);
}

int Settings::searchDelay() const noexcept
{
    return m_search.delay;
}

void Settings::setSearchDelay(int value)
{
    if (m_search.delay == value)
    {
        return;
    }
    m_search.delay = value;
    emit searchDelayChanged(m_search.delay);
}

Setting::Modifier Settings::searchModifier() const noexcept
{
    return m_search.modifier;
}

void Settings::setSearchModifier(Setting::Modifier value)
{
    if (m_search.modifier == value)
    {
        return;
    }
    m_search.modifier = value;
    emit searchModifierChanged(m_search.modifier);
}

bool Settings::searchHideMpvSubs() const noexcept
{
    return m_search.hideMpvSubs;
}

void Settings::setSearchHideMpvSubs(bool value)
{
    if (m_search.hideMpvSubs == value)
    {
        return;
    }
    m_search.hideMpvSubs = value;
    emit searchHideMpvSubsChanged(m_search.hideMpvSubs);
}

bool Settings::searchHideSubs() const noexcept
{
    return m_search.hideSubs;
}

void Settings::setSearchHideSubs(bool value)
{
    if (m_search.hideSubs == value)
    {
        return;
    }
    m_search.hideSubs = value;
    emit searchHideSubsChanged(m_search.hideSubs);
}

bool Settings::searchPauseOnHover() const noexcept
{
    return m_search.pauseOnHover;
}

void Settings::setSearchPauseOnHover(bool value)
{
    if (m_search.pauseOnHover == value)
    {
        return;
    }
    m_search.pauseOnHover = value;
    emit searchPauseOnHoverChanged(m_search.pauseOnHover);
}

bool Settings::searchAutoPlayAudio() const noexcept
{
    return m_search.autoPlayAudio;
}

void Settings::setSearchAutoPlayAudio(bool value)
{
    if (m_search.autoPlayAudio == value)
    {
        return;
    }
    m_search.autoPlayAudio = value;
    emit searchAutoPlayAudioChanged(m_search.autoPlayAudio);
}

Setting::GlossaryStyle Settings::searchGlossaryStyle() const noexcept
{
    return m_search.glossaryStyle;
}

void Settings::setSearchGlossaryStyle(Setting::GlossaryStyle value)
{
    if (m_search.glossaryStyle == value)
    {
        return;
    }
    m_search.glossaryStyle = value;
    emit searchGlossaryStyleChanged(m_search.glossaryStyle);
}

bool Settings::searchReplaceNewlines() const noexcept
{
    return m_search.replaceNewlines;
}

void Settings::setSearchReplaceNewlines(bool value)
{
    if (m_search.replaceNewlines == value)
    {
        return;
    }
    m_search.replaceNewlines = value;
    emit searchReplaceNewlinesChanged(m_search.replaceNewlines);
}

QString Settings::searchReplaceNewlinesWith() const noexcept
{
    return m_search.replaceNewlinesWith;
}

void Settings::setSearchReplaceNewlinesWith(const QString &value)
{
    if (m_search.replaceNewlinesWith == value)
    {
        return;
    }
    m_search.replaceNewlinesWith = value;
    emit searchReplaceNewlinesWithChanged(m_search.replaceNewlinesWith);
}

QString Settings::searchRemoveRegex() const noexcept
{
    return m_search.removeRegex;
}

void Settings::setSearchRemoveRegex(const QString &value)
{
    if (m_search.removeRegex == value)
    {
        return;
    }
    m_search.removeRegex = value;
    emit searchRemoveRegexChanged(m_search.removeRegex);
}

/* Interface Settings */

bool Settings::interfaceSystemIcons() const noexcept
{
    return m_interface.systemIcons;
}

void Settings::setInterfaceSystemIcons(bool value)
{
    if (m_interface.systemIcons == value)
    {
        return;
    }
    m_interface.systemIcons = value;
    emit interfaceSystemIconsChanged(m_interface.systemIcons);
}

const QFont &Settings::interfaceSubtitleFont() const noexcept
{
    return m_interface.subtitleFont;
}

void Settings::setInterfaceSubtitleFont(const QFont &value)
{
    if (m_interface.subtitleFont == value)
    {
        return;
    }
    m_interface.subtitleFont = value;
    emit interfaceSubtitleFontChanged(m_interface.subtitleFont);
}

double Settings::interfaceSubtitleLineSpacing() const noexcept
{
    return m_interface.subtitleLineSpacing;
}

void Settings::setInterfaceSubtitleLineSpacing(double value)
{
    if (m_interface.subtitleLineSpacing == value)
    {
        return;
    }
    m_interface.subtitleLineSpacing = value;
    emit interfaceSubtitleLineSpacingChanged(m_interface.subtitleLineSpacing);
}

double Settings::interfaceSubtitleScale() const noexcept
{
    return m_interface.subtitleScale;
}

void Settings::setInterfaceSubtitleScale(double value)
{
    if (m_interface.subtitleScale == value)
    {
        return;
    }
    m_interface.subtitleScale = value;
    emit interfaceSubtitleScaleChanged(m_interface.subtitleScale);
}

double Settings::interfaceSubtitleOffset() const noexcept
{
    return m_interface.subtitleOffset;
}

void Settings::setInterfaceSubtitleOffset(double value)
{
    if (m_interface.subtitleOffset == value)
    {
        return;
    }
    m_interface.subtitleOffset = value;
    emit interfaceSubtitleOffsetChanged(m_interface.subtitleOffset);
}

double Settings::interfaceSubtitleStroke() const noexcept
{
    return m_interface.subtitleStroke;
}

void Settings::setInterfaceSubtitleStroke(double value)
{
    if (m_interface.subtitleStroke == value)
    {
        return;
    }
    m_interface.subtitleStroke = value;
    emit interfaceSubtitleStrokeChanged(m_interface.subtitleStroke);
}

QColor Settings::interfaceSubtitleColor() const noexcept
{
    return m_interface.subtitleColor;
}

void Settings::setInterfaceSubtitleColor(const QColor &value)
{
    if (m_interface.subtitleColor == value)
    {
        return;
    }
    m_interface.subtitleColor = value;
    emit interfaceSubtitleColorChanged(m_interface.subtitleColor);
}

QColor Settings::interfaceSubtitleBackground() const noexcept
{
    return m_interface.subtitleBackground;
}

void Settings::setInterfaceSubtitleBackground(const QColor &value)
{
    if (m_interface.subtitleBackground == value)
    {
        return;
    }
    m_interface.subtitleBackground = value;
    emit interfaceSubtitleBackgroundChanged(m_interface.subtitleBackground);
}

QColor Settings::interfaceSubtitleStrokeColor() const noexcept
{
    return m_interface.subtitleStrokeColor;
}

void Settings::setInterfaceSubtitleStrokeColor(const QColor &value)
{
    if (m_interface.subtitleStrokeColor == value)
    {
        return;
    }
    m_interface.subtitleStrokeColor = value;
    emit interfaceSubtitleStrokeColorChanged(m_interface.subtitleStrokeColor);
}

int Settings::interfacePopupWidth() const noexcept
{
    return m_interface.popupWidth;
}

void Settings::setInterfacePopupWidth(int value)
{
    if (m_interface.popupWidth == value)
    {
        return;
    }
    m_interface.popupWidth = value;
    emit interfacePopupWidthChanged(m_interface.popupWidth);
}

int Settings::interfacePopupHeight() const noexcept
{
    return m_interface.popupHeight;
}

void Settings::setInterfacePopupHeight(int value)
{
    if (m_interface.popupHeight == value)
    {
        return;
    }
    m_interface.popupHeight = value;
    emit interfacePopupHeightChanged(m_interface.popupHeight);
}

bool Settings::interfaceSearchWindow() const noexcept
{
    return m_interface.searchWindow;
}

void Settings::setInterfaceSearchWindow(bool value)
{
    if (m_interface.searchWindow == value)
    {
        return;
    }
    m_interface.searchWindow = value;
    emit interfaceSearchWindowChanged(m_interface.searchWindow);
}

const QFont &Settings::interfaceSearchExpressionFont() const noexcept
{
    return m_interface.searchExpressionFont;
}

void Settings::setInterfaceSearchExpressionFont(const QFont &value)
{
    if (m_interface.searchExpressionFont == value)
    {
        return;
    }
    m_interface.searchExpressionFont = value;
    emit interfaceSearchExpressionFontChanged(m_interface.searchExpressionFont);
}

const QFont &Settings::interfaceSearchReadingFont() const noexcept
{
    return m_interface.searchReadingFont;
}

void Settings::setInterfaceSearchReadingFont(const QFont &value)
{
    if (m_interface.searchReadingFont == value)
    {
        return;
    }
    m_interface.searchReadingFont = value;
    emit interfaceSearchReadingFontChanged(m_interface.searchReadingFont);
}

const QFont &Settings::interfaceSearchConjFont() const noexcept
{
    return m_interface.searchConjFont;
}

void Settings::setInterfaceSearchConjFont(const QFont &value)
{
    if (m_interface.searchConjFont == value)
    {
        return;
    }
    m_interface.searchConjFont = value;
    emit interfaceSearchConjFontChanged(m_interface.searchConjFont);
}

const QFont &Settings::interfaceSearchTagFont() const noexcept
{
    return m_interface.searchTagFont;
}

void Settings::setInterfaceSearchTagFont(const QFont &value)
{
    if (m_interface.searchTagFont == value)
    {
        return;
    }
    m_interface.searchTagFont = value;
    emit interfaceSearchTagFontChanged(m_interface.searchTagFont);
}

const QFont &Settings::interfaceSearchGlossaryFont() const noexcept
{
    return m_interface.searchGlossaryFont;
}

void Settings::setInterfaceSearchGlossaryFont(const QFont &value)
{
    if (m_interface.searchGlossaryFont == value)
    {
        return;
    }
    m_interface.searchGlossaryFont = value;
    emit interfaceSearchGlossaryFontChanged(m_interface.searchGlossaryFont);
}

const QFont &Settings::interfaceSearchKanjiFont() const noexcept
{
    return m_interface.searchKanjiFont;
}

void Settings::setInterfaceSearchKanjiFont(const QFont &value)
{
    if (m_interface.searchKanjiFont == value)
    {
        return;
    }
    m_interface.searchKanjiFont = value;
    emit interfaceSearchKanjiFontChanged(m_interface.searchKanjiFont);
}

bool Settings::interfaceSubtitleListWindow() const noexcept
{
    return m_interface.subtitleListWindow;
}

void Settings::setInterfaceSubtitleListWindow(bool value)
{
    if (m_interface.subtitleListWindow == value)
    {
        return;
    }
    m_interface.subtitleListWindow = value;
    emit interfaceSubtitleListWindowChanged(m_interface.subtitleListWindow);
}

bool Settings::interfaceSubtitleListTimestamps() const noexcept
{
    return m_interface.subtitleListTimestamps;
}

void Settings::setInterfaceSubtitleListTimestamps(bool value)
{
    if (m_interface.subtitleListTimestamps == value)
    {
        return;
    }
    m_interface.subtitleListTimestamps = value;
    emit interfaceSubtitleListTimestampsChanged(
        m_interface.subtitleListTimestamps
    );
}

QColor Settings::interfaceSubtitleListBackgroundColor() const noexcept
{
    return m_interface.subtitleListBackgroundColor;
}

void Settings::setInterfaceSubtitleListBackgroundColor(const QColor &value)
{
    if (m_interface.subtitleListBackgroundColor == value)
    {
        return;
    }
    m_interface.subtitleListBackgroundColor = value;
    emit interfaceSubtitleListBackgroundColorChanged(
        m_interface.subtitleListBackgroundColor
    );
}

QColor Settings::interfaceSubtitleListTextColor() const noexcept
{
    return m_interface.subtitleListTextColor;
}

void Settings::setInterfaceSubtitleListTextColor(const QColor &value)
{
    if (m_interface.subtitleListTextColor == value)
    {
        return;
    }
    m_interface.subtitleListTextColor = value;
    emit interfaceSubtitleListTextColorChanged(
        m_interface.subtitleListTextColor
    );
}

QColor Settings::interfaceSubtitleListHoverColor() const noexcept
{
    return m_interface.subtitleListHoverColor;
}

void Settings::setInterfaceSubtitleListHoverColor(const QColor &value)
{
    if (m_interface.subtitleListHoverColor == value)
    {
        return;
    }
    m_interface.subtitleListHoverColor = value;
    emit interfaceSubtitleListHoverColorChanged(
        m_interface.subtitleListHoverColor
    );
}

QColor Settings::interfaceSubtitleListSelectedColor() const noexcept
{
    return m_interface.subtitleListSelectedColor;
}

void Settings::setInterfaceSubtitleListSelectedColor(const QColor &value)
{
    if (m_interface.subtitleListSelectedColor == value)
    {
        return;
    }
    m_interface.subtitleListSelectedColor = value;
    emit interfaceSubtitleListSelectedColorChanged(
        m_interface.subtitleListSelectedColor
    );
}

QColor Settings::interfaceSubtitleListSelectedHoverColor() const noexcept
{
    return m_interface.subtitleListSelectedHoverColor;
}

void Settings::setInterfaceSubtitleListSelectedHoverColor(const QColor &value)
{
    if (m_interface.subtitleListSelectedHoverColor == value)
    {
        return;
    }
    m_interface.subtitleListSelectedHoverColor = value;
    emit interfaceSubtitleListSelectedHoverColorChanged(
        m_interface.subtitleListSelectedHoverColor
    );
}

const QFont &Settings::interfaceSubtitleListPrimaryFont() const noexcept
{
    return m_interface.subtitleListPrimaryFont;
}

void Settings::setInterfaceSubtitleListPrimaryFont(const QFont &value)
{
    if (m_interface.subtitleListPrimaryFont == value)
    {
        return;
    }
    m_interface.subtitleListPrimaryFont = value;
    emit interfaceSubtitleListPrimaryFontChanged(
        m_interface.subtitleListPrimaryFont
    );
}

const QFont &Settings::interfaceSubtitleListSecondaryFont() const noexcept
{
    return m_interface.subtitleListSecondaryFont;
}

void Settings::setInterfaceSubtitleListSecondaryFont(const QFont &value)
{
    if (m_interface.subtitleListSecondaryFont == value)
    {
        return;
    }
    m_interface.subtitleListSecondaryFont = value;
    emit interfaceSubtitleListSecondaryFontChanged(
        m_interface.subtitleListSecondaryFont
    );
}

/* Keybind Settings */

KeybindProfileModel *Settings::keybinds() const noexcept
{
    return m_keybind.keybinds;
}

/* OCR Settings */

bool Settings::ocrEnabled() const noexcept
{
    return m_ocr.enabled;
}

void Settings::setOcrEnabled(bool value)
{
    if (m_ocr.enabled == value)
    {
        return;
    }
    m_ocr.enabled = value;
    emit ocrEnabledChanged(m_ocr.enabled);
}

bool Settings::ocrUseGpu() const noexcept
{
    return m_ocr.useGpu;
}

void Settings::setOcrUseGpu(bool value)
{
    if (m_ocr.useGpu == value)
    {
        return;
    }
    m_ocr.useGpu = value;
    emit ocrUseGpuChanged(m_ocr.useGpu);
}

QString Settings::ocrModel() const noexcept
{
    return m_ocr.model;
}

void Settings::setOcrModel(const QString &value)
{
    if (m_ocr.model == value)
    {
        return;
    }
    m_ocr.model = value;
    emit ocrModelChanged(m_ocr.model);
}

/* Update Version */

void Settings::updateSettings()
{
    QSettings settings;
    uint version = settings.value(Keys::Version::VERSION, 0).toUInt();
    if (version == Keys::Version::CURRENT)
    {
        return;
    }
    else if (version > Keys::Version::CURRENT)
    {
        qWarning() << tr(
            "The Memento settings found belong to a newer version.\n"
            "No guarantees can be made that nothing will break or get lost."
        );
    }

    /* Migrate the settings */
    switch(version)
    {
        case 0:
        {
            settings.remove("interface/sublist-style");
            [[fallthrough]];
        }

        case 1:
        {
            /* These paths are hardcoded because DirectoryUtils may change in
             * the future. */
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
            QDir configDir(QString(getenv("HOME")) + "/.config/memento");
            configDir.rename(
                "./dict/dictionaries.sqlite", "./dictionaries.sqlite"
            );

            QDir dictDir(configDir.absolutePath() + "/dict");
            dictDir.removeRecursively();
#elif defined(Q_OS_WIN)
            QDir programDir(DirectoryUtils::getProgramDirectory());
            programDir.rename(
                ".\\config\\dict\\dictionaries.sqlite",
                ".\\config\\dictionaries.sqlite"
            );

            QDir dictDir(
                DirectoryUtils::getProgramDirectory() + "config\\dict"
            );
            dictDir.removeRecursively();

            QString configPath = QDir::toNativeSeparators(
                QStandardPaths::writableLocation(
                    QStandardPaths::AppConfigLocation
                )
            );
            configPath.chop(sizeof("memento") - 1);
            QDir configDir(configPath);
            configDir.removeRecursively();

            programDir.rename(".\\config", configDir.absolutePath());
#endif
            [[fallthrough]];
        }

        case 2:
        {
            bool list = settings.value("search/list-result", true).toBool();
            settings.setValue(
                "list-result",
                static_cast<int>(
                    list ?
                        Setting::GlossaryStyle::GlossaryStyleBullet :
                        Setting::GlossaryStyle::GlossaryStyleLineBreak
                )
            );
            [[fallthrough]];
        }

        case 3:
        {
            settings.remove("behavior/file-open-custom");
            settings.remove("dictionaries");
            settings.remove("interface");
            settings.remove("search/modifier");
        }
    }

    settings.remove(Keys::Window::GROUP);
    settings.setValue(Keys::Version::VERSION, Keys::Version::CURRENT);
}
