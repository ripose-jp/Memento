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

#include <QColor>
#include <QFont>

#include "setting/audiosourcemodel.h"
#include "setting/keybindprofilemodel.h"
#include "setting/keys.h"

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        unsigned int version
        READ version
        NOTIFY versionChanged
    )

    /* Window Settings */

    Q_PROPERTY(
        bool windowSubtitleList
        READ windowSubtitleList
        WRITE setWindowSubtitleList
        NOTIFY windowSubtitleListChanged
    )

    Q_PROPERTY(
        bool windowSearch
        READ windowSearch
        WRITE setWindowSearch
        NOTIFY windowSearchChanged
    )

    /* Internal Settings */

    Q_PROPERTY(
        bool internalAutoUpdateOptInShown
        READ internalAutoUpdateOptInShown
        WRITE setInternalAutoUpdateOptInShown
        NOTIFY internalAutoUpdateOptInShownChanged
    )

    /* Recent Settings */

    Q_PROPERTY(
        QStringList recentFiles
        READ recentFiles
        NOTIFY recentFilesChanged
    )

    /* Subtitle List Settings */

    Q_PROPERTY(
        bool subtitleListIgnoreWhitespace
        READ subtitleListIgnoreWhitespace
        WRITE setSubtitleListIgnoreWhitespace
        NOTIFY subtitleListIgnoreWhitespaceChanged
    )

    Q_PROPERTY(
        bool subtitleListAutoSeek
        READ subtitleListAutoSeek
        WRITE setSubtitleListAutoSeek
        NOTIFY subtitleListAutoSeekChanged
    )

    /* Application Settings */

    Q_PROPERTY(
        bool applicationAutoUpdateCheck
        READ applicationAutoUpdateCheck
        WRITE setApplicationAutoUpdateCheck
        NOTIFY applicationAutoUpdateCheckChanged
    )

    /* Audio Source Settings */

    Q_PROPERTY(
        AudioSourceModel *audioSources
        READ audioSources
        CONSTANT
    )

    /* Behavior Settings */

    Q_PROPERTY(
        bool behaviorAutoFit
        READ behaviorAutoFit
        WRITE setBehaviorAutoFit
        NOTIFY behaviorAutoFitChanged
    )

    Q_PROPERTY(
        int behaviorAutoFitPercent
        READ behaviorAutoFitPercent
        WRITE setBehaviorAutoFitPercent
        NOTIFY behaviorAutoFitPercentChanged
    )

    Q_PROPERTY(
        bool behaviorOscMpvCursor
        READ behaviorOscMpvCursor
        WRITE setBehaviorOscMpvCursor
        NOTIFY behaviorOscMpvCursorChanged
    )

    Q_PROPERTY(
        int behaviorOscDuration
        READ behaviorOscDuration
        WRITE setBehaviorOscDuration
        NOTIFY behaviorOscDurationChanged
    )

    Q_PROPERTY(
        int behaviorOscFadeDuration
        READ behaviorOscFadeDuration
        WRITE setBehaviorOscFadeDuration
        NOTIFY behaviorOscFadeDurationChanged
    )

    Q_PROPERTY(
        int behaviorOscMinMove
        READ behaviorOscMinMove
        WRITE setBehaviorOscMinMove
        NOTIFY behaviorOscMinMoveChanged
    )

    Q_PROPERTY(
        bool behaviorOscPreviewThumbnails
        READ behaviorOscPreviewThumbnails
        WRITE setBehaviorOscPreviewThumbnails
        NOTIFY behaviorOscPreviewThumbnailsChanged
    )

    Q_PROPERTY(
        bool behaviorSubtitlePause
        READ behaviorSubtitlePause
        WRITE setBehaviorSubtitlePause
        NOTIFY behaviorSubtitlePauseChanged
    )

    Q_PROPERTY(
        bool behaviorSubtitleCursorShow
        READ behaviorSubtitleCursorShow
        WRITE setBehaviorSubtitleCursorShow
        NOTIFY behaviorSubtitleCursorShowChanged
    )

    Q_PROPERTY(
        bool behaviorSecondarySubtitleCursorShow
        READ behaviorSecondarySubtitleCursorShow
        WRITE setBehaviorSecondarySubtitleCursorShow
        NOTIFY behaviorSecondarySubtitleCursorShowChanged
    )

    Q_PROPERTY(
        Setting::Directory behaviorFileOpenDirectory
        READ behaviorFileOpenDirectory
        WRITE setBehaviorFileOpenDirectory
        NOTIFY behaviorFileOpenDirectoryChanged
    )

    Q_PROPERTY(
        QString behaviorFileOpenCustom
        READ behaviorFileOpenCustom
        WRITE setBehaviorFileOpenCustom
        NOTIFY behaviorFileOpenCustomChanged
    )

    /* Dictionary Settings */

    Q_PROPERTY(
        QList<int64_t> dictionaryOrder
        READ dictionaryOrder
        WRITE setDictionaryOrder
        NOTIFY dictionaryOrderChanged
    )

    /* Search Settings */

    Q_PROPERTY(
        bool searchMatcherExact
        READ searchMatcherExact
        WRITE setSearchMatcherExact
        NOTIFY searchMatcherExactChanged
    )

    Q_PROPERTY(
        bool searchMatcherDeconj
        READ searchMatcherDeconj
        WRITE setSearchMatcherDeconj
        NOTIFY searchMatcherDeconjChanged
    )

    Q_PROPERTY(
        bool searchMatcherMecabIpadic
        READ searchMatcherMecabIpadic
        WRITE setSearchMatcherMecabIpadic
        NOTIFY searchMatcherMecabIpadicChanged
    )

    Q_PROPERTY(
        bool searchMiddleMouseScan
        READ searchMiddleMouseScan
        WRITE setSearchMiddleMouseScan
        NOTIFY searchMiddleMouseScanChanged
    )

    Q_PROPERTY(
        Setting::SearchMethod searchMethod
        READ searchMethod
        WRITE setSearchMethod
        NOTIFY searchMethodChanged
    )

    Q_PROPERTY(
        int searchDelay
        READ searchDelay
        WRITE setSearchDelay
        NOTIFY searchDelayChanged
    )

    Q_PROPERTY(
        Setting::Modifier searchModifier
        READ searchModifier
        WRITE setSearchModifier
        NOTIFY searchModifierChanged
    )

    Q_PROPERTY(
        bool searchHideMpvSubs
        READ searchHideMpvSubs
        WRITE setSearchHideMpvSubs
        NOTIFY searchHideMpvSubsChanged
    )

    Q_PROPERTY(
        bool searchHideSubs
        READ searchHideSubs
        WRITE setSearchHideSubs
        NOTIFY searchHideSubsChanged
    )

    Q_PROPERTY(
        bool searchPauseOnHover
        READ searchPauseOnHover
        WRITE setSearchPauseOnHover
        NOTIFY searchPauseOnHoverChanged
    )

    Q_PROPERTY(
        bool searchAutoPlayAudio
        READ searchAutoPlayAudio
        WRITE setSearchAutoPlayAudio
        NOTIFY searchAutoPlayAudioChanged
    )

    Q_PROPERTY(
        Setting::GlossaryStyle searchGlossaryStyle
        READ searchGlossaryStyle
        WRITE setSearchGlossaryStyle
        NOTIFY searchGlossaryStyleChanged
    )

    Q_PROPERTY(
        bool searchReplaceNewlines
        READ searchReplaceNewlines
        WRITE setSearchReplaceNewlines
        NOTIFY searchReplaceNewlinesChanged
    )

    Q_PROPERTY(
        QString searchReplaceNewlinesWith
        READ searchReplaceNewlinesWith
        WRITE setSearchReplaceNewlinesWith
        NOTIFY searchReplaceNewlinesWithChanged
    )

    Q_PROPERTY(
        QString searchRemoveRegex
        READ searchRemoveRegex
        WRITE setSearchRemoveRegex
        NOTIFY searchRemoveRegexChanged
    )

    Q_PROPERTY(
        bool showAnkiGlossaryCheckbox
        READ showAnkiGlossaryCheckbox
        WRITE setShowAnkiGlossaryCheckbox
        NOTIFY showAnkiGlossaryCheckboxChanged
    )

    /* Interface Settings */

    Q_PROPERTY(
        bool interfaceSystemIcons
        READ interfaceSystemIcons
        WRITE setInterfaceSystemIcons
        NOTIFY interfaceSystemIconsChanged
    )

    Q_PROPERTY(
        QFont interfaceSubtitleFont
        READ interfaceSubtitleFont
        WRITE setInterfaceSubtitleFont
        NOTIFY interfaceSubtitleFontChanged
    )

    Q_PROPERTY(
        double interfaceSubtitleLineSpacing
        READ interfaceSubtitleLineSpacing
        WRITE setInterfaceSubtitleLineSpacing
        NOTIFY interfaceSubtitleLineSpacingChanged
    )

    Q_PROPERTY(
        double interfaceSubtitleScale
        READ interfaceSubtitleScale
        WRITE setInterfaceSubtitleScale
        NOTIFY interfaceSubtitleScaleChanged
    )

    Q_PROPERTY(
        double interfaceSubtitleOffset
        READ interfaceSubtitleOffset
        WRITE setInterfaceSubtitleOffset
        NOTIFY interfaceSubtitleOffsetChanged
    )

    Q_PROPERTY(
        double interfaceSubtitleStroke
        READ interfaceSubtitleStroke
        WRITE setInterfaceSubtitleStroke
        NOTIFY interfaceSubtitleStrokeChanged
    )

    Q_PROPERTY(
        QColor interfaceSubtitleColor
        READ interfaceSubtitleColor
        WRITE setInterfaceSubtitleColor
        NOTIFY interfaceSubtitleColorChanged
    )

    Q_PROPERTY(
        QColor interfaceSubtitleBackground
        READ interfaceSubtitleBackground
        WRITE setInterfaceSubtitleBackground
        NOTIFY interfaceSubtitleBackgroundChanged
    )

    Q_PROPERTY(
        QColor interfaceSubtitleStrokeColor
        READ interfaceSubtitleStrokeColor
        WRITE setInterfaceSubtitleStrokeColor
        NOTIFY interfaceSubtitleStrokeColorChanged
    )

    Q_PROPERTY(
        bool interfaceSearchWindow
        READ interfaceSearchWindow
        WRITE setInterfaceSearchWindow
        NOTIFY interfaceSearchWindowChanged
    )

    Q_PROPERTY(
        int interfacePopupWidth
        READ interfacePopupWidth
        WRITE setInterfacePopupWidth
        NOTIFY interfacePopupWidthChanged
    )

    Q_PROPERTY(
        int interfacePopupHeight
        READ interfacePopupHeight
        WRITE setInterfacePopupHeight
        NOTIFY interfacePopupHeightChanged
    )

    Q_PROPERTY(
        QFont interfaceSearchExpressionFont
        READ interfaceSearchExpressionFont
        WRITE setInterfaceSearchExpressionFont
        NOTIFY interfaceSearchExpressionFontChanged
    )

    Q_PROPERTY(
        QFont interfaceSearchReadingFont
        READ interfaceSearchReadingFont
        WRITE setInterfaceSearchReadingFont
        NOTIFY interfaceSearchReadingFontChanged
    )

    Q_PROPERTY(
        QFont interfaceSearchConjFont
        READ interfaceSearchConjFont
        WRITE setInterfaceSearchConjFont
        NOTIFY interfaceSearchConjFontChanged
    )

    Q_PROPERTY(
        QFont interfaceSearchTagFont
        READ interfaceSearchTagFont
        WRITE setInterfaceSearchTagFont
        NOTIFY interfaceSearchTagFontChanged
    )

    Q_PROPERTY(
        QFont interfaceSearchGlossaryFont
        READ interfaceSearchGlossaryFont
        WRITE setInterfaceSearchGlossaryFont
        NOTIFY interfaceSearchGlossaryFontChanged
    )

    Q_PROPERTY(
        QFont interfaceSearchKanjiFont
        READ interfaceSearchKanjiFont
        WRITE setInterfaceSearchKanjiFont
        NOTIFY interfaceSearchKanjiFontChanged
    )

    Q_PROPERTY(
        bool interfaceSubtitleListWindow
        READ interfaceSubtitleListWindow
        WRITE setInterfaceSubtitleListWindow
        NOTIFY interfaceSubtitleListWindowChanged
    )

    Q_PROPERTY(
        bool interfaceSubtitleListTimestamps
        READ interfaceSubtitleListTimestamps
        WRITE setInterfaceSubtitleListTimestamps
        NOTIFY interfaceSubtitleListTimestampsChanged
    )

    Q_PROPERTY(
        QColor interfaceSubtitleListBackgroundColor
        READ interfaceSubtitleListBackgroundColor
        WRITE setInterfaceSubtitleListBackgroundColor
        NOTIFY interfaceSubtitleListBackgroundColorChanged
    )

    Q_PROPERTY(
        QColor interfaceSubtitleListTextColor
        READ interfaceSubtitleListTextColor
        WRITE setInterfaceSubtitleListTextColor
        NOTIFY interfaceSubtitleListTextColorChanged
    )

    Q_PROPERTY(
        QColor interfaceSubtitleListHoverColor
        READ interfaceSubtitleListHoverColor
        WRITE setInterfaceSubtitleListHoverColor
        NOTIFY interfaceSubtitleListHoverColorChanged
    )

    Q_PROPERTY(
        QColor interfaceSubtitleListSelectedColor
        READ interfaceSubtitleListSelectedColor
        WRITE setInterfaceSubtitleListSelectedColor
        NOTIFY interfaceSubtitleListSelectedColorChanged
    )

    Q_PROPERTY(
        QColor interfaceSubtitleListSelectedHoverColor
        READ interfaceSubtitleListSelectedHoverColor
        WRITE setInterfaceSubtitleListSelectedHoverColor
        NOTIFY interfaceSubtitleListSelectedHoverColorChanged
    )

    Q_PROPERTY(
        QFont interfaceSubtitleListPrimaryFont
        READ interfaceSubtitleListPrimaryFont
        WRITE setInterfaceSubtitleListPrimaryFont
        NOTIFY interfaceSubtitleListPrimaryFontChanged
    )

    Q_PROPERTY(
        QFont interfaceSubtitleListSecondaryFont
        READ interfaceSubtitleListSecondaryFont
        WRITE setInterfaceSubtitleListSecondaryFont
        NOTIFY interfaceSubtitleListSecondaryFontChanged
    )

    /* Keybind Settings */

    Q_PROPERTY(
        KeybindProfileModel *keybinds
        READ keybinds
        CONSTANT
    )

    /* OCR Settings */

    Q_PROPERTY(
        bool ocrEnabled
        READ ocrEnabled
        WRITE setOcrEnabled
        NOTIFY ocrEnabledChanged
    )

    Q_PROPERTY(
        bool ocrUseGpu
        READ ocrUseGpu
        WRITE setOcrUseGpu
        NOTIFY ocrUseGpuChanged
    )

    Q_PROPERTY(
        QString ocrModel
        READ ocrModel
        WRITE setOcrModel
        NOTIFY ocrModelChanged
    )

public:
    Settings(QObject *parent = nullptr);
    virtual ~Settings();

    /**
     * @brief Loads saved settings from disc.
     */
    Q_INVOKABLE void load();

    /**
     * @brief Writes current settings to disc.
     */
    Q_INVOKABLE void write();

    /**
     * @brief Loads defaults for all settings.
     */
    Q_INVOKABLE void defaults();

    /**
     * @brief Loads window settings from disc.
     */
    Q_INVOKABLE void loadWindowSettings();

    /**
     * @brief Writes window settings to disc.
     */
    Q_INVOKABLE void writeWindowSettings();

    /**
     * @brief Load internal settings from disc.
     */
    Q_INVOKABLE void loadInternalSettings();

    /**
     * @brief Write internal settings to disc.
     */
    Q_INVOKABLE void writeInternalSettings();

    /**
     * @brief Loads recent settings from disc.
     */
    Q_INVOKABLE void loadRecentSettings();

    /**
     * @brief Writes recent settings to disc.
     */
    Q_INVOKABLE void writeRecentSettings();

    /**
     * @brief Loads subtitle list settings from disc.
     */
    Q_INVOKABLE void loadSubtitleListSettings();

    /**
     * @brief Write subtitle list settings to disc.
     */
    Q_INVOKABLE void writeSubtitleListSettings();

    /**
     * @brief Loads subtitle list settings defaults.
     */
    Q_INVOKABLE void defaultSubtitleListSettings();

    /**
     * @brief Load the application settings from disc.
     */
    Q_INVOKABLE void loadApplicationSettings();

    /**
     * @brief Write the application settings to disc.
     */
    Q_INVOKABLE void writeApplicationSettings();

    /**
     * @brief Load the application settings defaults.
     */
    Q_INVOKABLE void defaultApplicationSettings();

    /**
     * @brief Loads audio source settings from disc.
     */
    Q_INVOKABLE void loadAudioSourceSettings();

    /**
     * @brief Writes audio source settings to disc.
     */
    Q_INVOKABLE void writeAudioSourceSettings();

    /**
     * @brief Load audio source settings defaults.
     */
    Q_INVOKABLE void defaultAudioSourceSettings();

    /**
     * @brief Load behavior settings from disc.
     */
    Q_INVOKABLE void loadBehaviorSettings();

    /**
     * @brief Writes behavior settings to disc.
     */
    Q_INVOKABLE void writeBehaviorSettings();

    /**
     * @brief Load behavior settings defaults.
     */
    Q_INVOKABLE void defaultBehaviorSettings();

    /**
     * @brief Load the dictionary settings from disc.
     */
    Q_INVOKABLE void loadDictionarySettings();

    /**
     * @brief Writes dictionary settings to disc.
     */
    Q_INVOKABLE void writeDictionarySettings();

    /**
     * @brief Load dictionary setting defaults.
     */
    Q_INVOKABLE void defaultDictionarySettings();

    /**
     * @brief Load search settings from disc.
     */
    Q_INVOKABLE void loadSearchSettings();

    /**
     * @brief Writes search settings to disc.
     */
    Q_INVOKABLE void writeSearchSettings();

    /**
     * @brief Load search settings defaults.
     */
    Q_INVOKABLE void defaultSearchSettings();

    /**
     * @brief Load interface settings from disc.
     */
    Q_INVOKABLE void loadInterfaceSettings();

    /**
     * @brief Write interface settings to disc.
     */
    Q_INVOKABLE void writeInterfaceSettings();

    /**
     * @brief Load interface settings defaults.
     */
    Q_INVOKABLE void defaultInterfaceSettings();

    /**
     * @brief Load keybind settings from disc.
     */
    Q_INVOKABLE void loadKeybindSettings();

    /**
     * @brief Write keybind settings to disc.
     */
    Q_INVOKABLE void writeKeybindSettings();

    /**
     * Load keybind settings defaults.
     */
    Q_INVOKABLE void defaultKeybindSettings();

    /**
     * @brief Load OCR settings from disc.
     */
    Q_INVOKABLE void loadOcrSettings();

    /**
     * @brief Write OCR settings to disc.
     */
    Q_INVOKABLE void writeOcrSettings();

    /**
     * @brief Load OCR settings defaults.
     */
    Q_INVOKABLE void defaultOcrSettings();

    /**
     * @brief The version of the settings file.
     *
     * @return The version number of the setting file.
     */
    [[nodiscard]]
    unsigned int version() const noexcept;

    /**
     * @brief Sets the version number of the configuration file.
     *
     * @param value The version to set.
     */
    void setVersion(unsigned int value);

    /* Window Settings */

    /**
     * @brief The saved subtitle list open state.
     *
     * @return true if the subtitle list was open,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool windowSubtitleList() const noexcept;

    /**
     * @brief Sets the saved subtitle list open state.
     *
     * @param value true if the subtitle list is open, false otherwise.
     */
    void setWindowSubtitleList(bool value);

    /**
     * @brief Sets the saved window search open state.
     *
     * @return true if the search panel is open,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool windowSearch() const noexcept;

    /**
     * @brief Sets the window search open state.
     *
     * @param value true if the search panel is open, false otherwise.
     */
    void setWindowSearch(bool value);

    /* Internal Settings */

    /**
     * @brief Get if the auto update opt-in message has been shown.
     *
     * @return true if the message has been shown,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool internalAutoUpdateOptInShown() const noexcept;

    /**
     * @brief Set if the auto update opt-in message has been shown.
     *
     * @param value true if the message has been shown, false otherwise.
     */
    void setInternalAutoUpdateOptInShown(bool value);

    /* Recent Settings */

    /**
     * @brief Gets the paths of recent files.
     *
     * @return The list of recent files.
     */
    [[nodiscard]]
    const QStringList &recentFiles() const noexcept;

    /**
     * @brief Adds a recent file to the recent files.
     *
     * @param value The path of the recent file to add.
     */
    Q_INVOKABLE void recentFilesAdd(const QString &value);

    /**
     * @brief Clear all recent files.
     */
    Q_INVOKABLE void recentFilesClear();

    /* Subtitle List Settings */

    /**
     * @brief Gets if the subtitle list should ignore whitespace in searches.
     *
     * @return true if the subtitle list should ignore whitespace,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool subtitleListIgnoreWhitespace() const noexcept;

    /**
     * @brief Sets if the subtitle list should ignore whitespace in searches.
     *
     * @param value true to ignore whitespace in searches, false otherwise.
     */
    void setSubtitleListIgnoreWhitespace(
        bool value = Keys::SubtitleList::IGNORE_WHITESPACE_DEFAULT);

    /**
     * @brief Gets if the subtitle list should automatically seek in searches.
     *
     * @return true if the subtitle list should automatically seek in searches,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool subtitleListAutoSeek() const noexcept;

    /**
     * @brief Sets if the subtitle list should autoseek in searches.
     *
     * @param value true to autoseek, false to not.
     */
    void setSubtitleListAutoSeek(
        bool value = Keys::SubtitleList::AUTO_SEEK_DEFAULT);

    /* Application Settings */

    /**
     * @brief Gets if the application should check for updates on start.
     *
     * @return true to check for updates on start,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool applicationAutoUpdateCheck() const noexcept;

    /**
     * @brief Sets if the application should check for updates on start.
     *
     * @param value true if updates should be checked for on start, false
     * otherwise.
     */
    void setApplicationAutoUpdateCheck(
        bool value = Keys::Application::AUTO_UPDATE_CHECK_DEFAULT);

    /* Audio Source Settings */

    /**
     * @brief Gets the audio sources.
     *
     * @return The audio source model.
     */
    AudioSourceModel *audioSources() const noexcept;

    /* Behavior Settings */

    /**
     * @brief Gets the auto fit behavior setting.
     *
     * @return true if the player should fit to content,
     * @return false if it should do nothing.
     */
    [[nodiscard]]
    bool behaviorAutoFit() const noexcept;

    /**
    * @brief Sets the auto fit behavior setting.
    *
    * @param value true to fit to content, false to do nothing.
    */
    void setBehaviorAutoFit(
        bool value = Keys::Behavior::AUTO_FIT_DEFAULT);

    /**
     * @brief Gets the auto fit percentage.
     *
     * @return The percentage to fit to content.
     */
    [[nodiscard]]
    int behaviorAutoFitPercent() const noexcept;

    /**
     * @brief Sets the auto fit percentage.
     *
     * @param value The percentage to fit to content.
     */
    void setBehaviorAutoFitPercent(
        int value = Keys::Behavior::AUTO_FIT_PERCENT_DEFAULT);

    /**
     * @brief Gets the OSC mpv cursor behavior setting.
     *
     * @return true if the OSC should hide the cursor, false .
     */
    [[nodiscard]]
    bool behaviorOscMpvCursor() const noexcept;

    /**
     * @brief Sets the OSC hide cursor behavior setting.
     *
     * @param value true to hide the cursor when the OSC is visible, false
     * otherwise.
     */
    void setBehaviorOscMpvCursor(
        bool value = Keys::Behavior::OSC_MPV_CURSOR_DEFAULT);

    /**
     * @brief Gets the show OSC duration in milliseconds.
     *
     * @return The duration of the OSC should be shown in milliseconds.
     */
    [[nodiscard]]
    int behaviorOscDuration() const noexcept;

    /**
     * @brief Sets the show OSC duration in milliseconds.
     *
     * @param value The duration of the OSC should be shown in milliseconds.
     */
    void setBehaviorOscDuration(
        int value = Keys::Behavior::OSC_DURATION_DEFAULT);

    /**
     * @brief Gets the OSC fade out duration in milliseconds.
     *
     * @return The duration of the OSC fade out in milliseconds.
     */
    [[nodiscard]]
    int behaviorOscFadeDuration() const noexcept;

    /**
     * @brief Sets the OSC fade out duration in milliseconds.
     *
     * @param value The duration of the OSC fade out in milliseconds.
     */
    void setBehaviorOscFadeDuration(
        int value = Keys::Behavior::OSC_FADE_DEFAULT);

    /**
     * @brief Gets the minimum mouse movement in pixels to show the OSC again.
     *
     * @return The minimum mouse movement in pixels to show the OSC again.
     */
    [[nodiscard]]
    int behaviorOscMinMove() const noexcept;

    /**
     * @brief Sets the minimum mouse movement in pixels to show the OSC again.
     *
     * @param value The minimum mouse movement in pixels to show the OSC again.
     */
    void setBehaviorOscMinMove(
        int value = Keys::Behavior::OSC_MIN_MOVE_DEFAULT);

    /**
     * @brief Gets the preview thumbnails are enabled.
     *
     * @return true if preview thumbnails are enabled,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool behaviorOscPreviewThumbnails() const noexcept;

    /**
     * @brief Sets the preview thumbnails are enabled.
     *
     * @param value true if preview thumbnails are enabled, false otherwise.
     */
    void setBehaviorOscPreviewThumbnails(
        bool value = Keys::Behavior::OSC_PREVIEW_THUMBNAILS_DEFAULT);

    /**
     * @brief Gets the subtitle pause behavior setting.
     *
     * @return true if the player should pause when a subtitle ends,
     * @return false if it should do nothing.
     */
    [[nodiscard]]
    bool behaviorSubtitlePause() const noexcept;

    /**
     * @brief Sets the subtitle pause behavior setting.
     *
     * @param value true to pause when a subtitle ends, false to do
     * nothing.
     */
    void setBehaviorSubtitlePause(
        bool value = Keys::Behavior::SUBTITLE_PAUSE_DEFAULT);

    /**
     * @brief Gets if subtitles should be shown when the cursor approaches them.
     *
     * @return true if subtitles should be shown when the cursor approaches,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool behaviorSubtitleCursorShow() const noexcept;

    /**
     * @brief Sets if subtitles should be shown when the cursor approaches them.
     *
     * @param value true to show subtitles when the cursor approaches, false
     * otherwise.
     */
    void setBehaviorSubtitleCursorShow(
        bool value = Keys::Behavior::SUBTITLE_CURSOR_SHOW_DEFAULT);

    /**
     * @brief Gets if secondary subtitles should be shown when the cursor
     * approaches them.
     *
     * @return true if secondary subtitles should be shown when the cursor
     * approaches,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool behaviorSecondarySubtitleCursorShow() const noexcept;

    /**
     * @brief Sets if secondary subtitles should be shown when the cursor
     * approaches them.
     *
     * @param value true to show secondary subtitles when the cursor approaches,
     * false otherwise.
     */
    void setBehaviorSecondarySubtitleCursorShow(
        bool value = Keys::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW_DEFAULT);

    /**
     * @brief Gets the default directory to open files from.
     *
     * @return The default directory to open files from.
     */
    [[nodiscard]]
    Setting::Directory behaviorFileOpenDirectory() const noexcept;

    /**
     * @brief Sets the default directory to open files from.
     *
     * @param value The default directory to open files from.
     */
    void setBehaviorFileOpenDirectory(
        Setting::Directory value = Keys::Behavior::FILE_OPEN_DIR_DEFAULT);

    /**
     * @brief Gets the custom directory to open files from.
     *
     * @return The custom directory to open files from.
     */
    [[nodiscard]]
    QString behaviorFileOpenCustom() const noexcept;

    /**
     * @brief Sets the custom directory to open files from.
     *
     * @param value The custom directory to open files from.
     */
    void setBehaviorFileOpenCustom(
        const QString &value = Keys::Behavior::FILE_OPEN_CUSTOM_DEFAULT);

    /* Dictionary Settings */

    /**
     * @brief Get the list of dictionary IDs sorted by priority.
     *
     * @return The list of dictionary IDs sorted by priority.
     */
    [[nodiscard]]
    const QList<int64_t> &dictionaryOrder() const noexcept;

    /**
     * @brief Set the list of dictionary IDs sorted by priority.
     *
     * @param value The list of dictionary IDs.
     */
    void setDictionaryOrder(const QList<int64_t> &value = {});

    /* Search Settings */

    /**
     * @brief Gets if the exact matcher is used for search results.
     *
     * @return true if the exact matcher is enabled,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool searchMatcherExact() const noexcept;

    /**
     * @brief Sets if the exact matcher is used for search results.
     *
     * @param value true to enable the exact matcher, false otherwise.
     */
    void setSearchMatcherExact(
        bool value = Keys::Search::Matcher::EXACT_DEFAULT);

    /**
     * @brief Gets if the deconjugation matcher is used for search results.
     *
     * @return true if the deconjugation matcher is enabled,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool searchMatcherDeconj() const noexcept;

    /**
     * @brief Sets if the deconjugation matcher is used for search results.
     *
     * @param value true to enable the deconjugation matcher, false otherwise.
     */
    void setSearchMatcherDeconj(
        bool value = Keys::Search::Matcher::DECONJ_DEFAULT);

    /**
     * @brief Gets if the MeCab IPAdic matcher is used for search results.
     *
     * @return true if the MeCab IPAdic matcher is enabled,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool searchMatcherMecabIpadic() const noexcept;

    /**
     * @brief Sets if the MeCab IPAdic matcher is used for search results.
     *
     * @param value true to enable the MeCab IPAdic matcher, false otherwise.
     */
    void setSearchMatcherMecabIpadic(
        bool value = Keys::Search::Matcher::MECAB_IPADIC_DEFAULT);

    /**
     * @brief Gets if middle mouse scanning can be used to search subtitles.
     *
     * @return true if middle mouse scanning is enabled,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool searchMiddleMouseScan() const noexcept;

    /**
     * @brief Sets if middle mouse scanning can be used to search subtitles.
     *
     * @param value true to enable middle mouse scanning, false otherwise.
     */
    void setSearchMiddleMouseScan(
        bool value = Keys::Search::MIDDLE_MOUSE_SCAN_DEFAULT);

    /**
     * @brief Gets the search method to use for searching subtitles.
     *
     * @return The search method to use for searching subtitles.
     */
    [[nodiscard]]
    Setting::SearchMethod searchMethod() const noexcept;

    /**
     * @brief Sets the search method to use for searching subtitles.
     *
     * @param value The search method to use for searching subtitles.
     */
    void setSearchMethod(
        Setting::SearchMethod value = Keys::Search::METHOD_DEFAULT);

    /**
     * @brief Gets the delay in milliseconds to wait before performing a search.
     *
     * @return The delay in milliseconds to wait before performing a search.
     */
    [[nodiscard]]
    int searchDelay() const noexcept;

    /**
     * @brief Sets the delay in milliseconds to wait before performing a search.
     *
     * @param value The delay in milliseconds to wait before performing a
     * search.
     */
    void setSearchDelay(
        int value = Keys::Search::DELAY_DEFAULT);

    /**
     * @brief Gets the modifier key to use as the search modifier.
     *
     * @return The modifier key to use for the search modifier.
     */
    [[nodiscard]]
    Setting::Modifier searchModifier() const noexcept;

    /**
     * @brief Sets the modifier key to use as the search modifier.
     *
     * @param value The modifier key to use for the search modifier.
     */
    void setSearchModifier(
        Setting::Modifier value = Keys::Search::MODIFIER_DEFAULT);

    /**
     * @brief Hides mpv subtitle when search subtitles are visible.
     *
     * @return true to hide mpv subtitle when search subtitles are visible,
     * @return false to do nothing.
     */
    [[nodiscard]]
    bool searchHideMpvSubs() const noexcept;

    /**
     * @brief Sets whether to hide mpv subtitle when search subtitles are
     * visible.
     *
     * @param value true to hide mpv subtitle when search subtitles are visible,
     * false to do nothing.
     */
    void setSearchHideMpvSubs(
        bool value = Keys::Search::HIDE_MPV_SUBS_DEFAULT);

    /**
     * @brief Hides search subtitles during playback.
     *
     * @return true to hide search during playback,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool searchHideSubs() const noexcept;

    /**
     * @brief Sets whether to hide search subtitles during playback.
     *
     * @param value true to hide search during playback, false otherwise.
     */
    void setSearchHideSubs(
        bool value = Keys::Search::HIDE_SUBS_DEFAULT);

    /**
     * @brief Pauses the video when hovering over search results.
     *
     * @return true to pause the video when hovering over search results,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool searchPauseOnHover() const noexcept;

    /**
     * @brief Sets whether to pause the video when hovering over search results.
     *
     * @param value true to pause the video when hovering over search results,
     * false otherwise.
     */
    void setSearchPauseOnHover(
        bool value = Keys::Search::PAUSE_ON_HOVER_DEFAULT);

    /**
     * @brief Automatically plays the audio of the top result when a search
     * completes.
     *
     * @return true to automatically play the audio a search completes,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool searchAutoPlayAudio() const noexcept;

    /**
     * @brief Sets whether to automatically play the audio of the top result
     * when a search completes.
     *
     * @param value true to automatically play the audio a search completes,
     * false otherwise.
     */
    void setSearchAutoPlayAudio(
        bool value = Keys::Search::AUTO_PLAY_AUDIO_DEFAULT);

    /**
     * @brief The style to display term search results.
     *
     * @return The glossary style to use.
     */
    [[nodiscard]]
    Setting::GlossaryStyle searchGlossaryStyle() const noexcept;

    /**
     * @brief Sets the style to display term search results.
     *
     * @param value The glossary style to use.
     */
    void setSearchGlossaryStyle(
        Setting::GlossaryStyle value = Keys::Search::GLOSSARY_STYLE_DEFAULT);

    /**
     * @brief Gets whether to replace newlines in search results with a
     * different string.
     *
     * @return true to replace newlines in search results with a different
     * string,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool searchReplaceNewlines() const noexcept;

    /**
     * @brief Sets whether to replace newlines in search results with a
     * different string.
     *
     * @param value true to replace newlines in search results with a different
     * string, false otherwise.
     */
    void setSearchReplaceNewlines(
        bool value = Keys::Search::REPLACE_NEWLINES_DEFAULT);

    /**
     * @brief Gets the string to replace newlines with in search results.
     *
     * @return The string to replace newlines with in search results.
     */
    [[nodiscard]]
    QString searchReplaceNewlinesWith() const noexcept;

    /**
     * @brief Sets the string to replace newlines with in search results.
     *
     * @param value The string to replace newlines with in search results.
     */
    void setSearchReplaceNewlinesWith(
        const QString &value = Keys::Search::REPLACE_NEWLINES_WITH_DEFAULT);

    /**
     * @brief Gets the regular expression to filter subtitles with.
     *
     * @return The regular expression to filter subtitles with.
     */
    [[nodiscard]]
    QString searchRemoveRegex() const noexcept;

    /**
     * @brief Sets the regular expression to filter subtitles with.
     *
     * @param value The regular expression to filter subtitles with.
     */
    void setSearchRemoveRegex(
        const QString &value = Keys::Search::REMOVE_REGEX_DEFAULT);

    /**
     * @brief Get if the Anki checkbox should be shown in term glossaries.
     *
     * @return true to show the Anki checkbox, false to hide it.
     */
    [[nodiscard]]
    bool showAnkiGlossaryCheckbox() const noexcept;

    /**
     * @brief Set if the Anki checkbox should be shown in term glossaries.
     *
     * @param value true to show the Anki checkbox, false to hide it.
     */
    void setShowAnkiGlossaryCheckbox(
        bool value = Keys::Search::SHOW_ANKI_GLOSSARY_CHECKBOX_DEFAULT);

    /* Interface Settings */

    /**
     * @brief Gets if system icons should be used.
     *
     * @return true to use system icons,
     * @return false to use bundled icons.
     */
    [[nodiscard]]
    bool interfaceSystemIcons() const noexcept;

    /**
     * @brief Sets if system icons should be used.
     *
     * @param value true to use system icons, false to use bundled icons.
     */
    void setInterfaceSystemIcons(
        bool value = Keys::Interface::SYSTEM_ICONS_DEFAULT);

    /**
     * @brief Gets the subtitle font.
     *
     * @return The subtitle font.
     */
    [[nodiscard]]
    const QFont &interfaceSubtitleFont() const noexcept;

    /**
     * @brief Sets the subtitle font.
     *
     * @param value The subtitle font.
     */
    void setInterfaceSubtitleFont(
        const QFont &value = Keys::Interface::Subtitle::FONT_DEFAULT);

    /**
     * @brief Gets the space between subtitle lines.
     *
     * @return The number of units between subtitle lines.
     */
    [[nodiscard]]
    double interfaceSubtitleLineSpacing() const noexcept;

    /**
     * @brief Sets the space between subtitle lines.
     *
     * @param value The number of units between subtitle lines.
     */
    void setInterfaceSubtitleLineSpacing(
        double value = Keys::Interface::Subtitle::LINE_SPACING_DEFAULT);

    /**
     * @brief Gets the subtitle scale as a percentage of window height.
     *
     * @return The subtitle scale factor as a percentage of window height.
     */
    [[nodiscard]]
    double interfaceSubtitleScale() const noexcept;

    /**
     * @brief Sets the subtitle scale as a percentage of window height.
     *
     * @param value The subtitle scale factor as a percentage of window height.
     */
    void setInterfaceSubtitleScale(
        double value = Keys::Interface::Subtitle::SCALE_DEFAULT);

    /**
     * @brief Gets the subtitle vertical offset as a percentage of
     * window height.
     *
     * @return The subtitle vertical offset as a percentage of window height.
     */
    [[nodiscard]]
    double interfaceSubtitleOffset() const noexcept;

    /**
     * @brief Sets the subtitle vertical offset as a percentage of
     * window height.
     *
     * @param value The subtitle vertical offset as a percentage of window
     * height.
     */
    void setInterfaceSubtitleOffset(
        double value = Keys::Interface::Subtitle::OFFSET_DEFAULT);

    /**
     * @brief Gets the subtitle stroke width.
     *
     * @return The subtitle stroke width.
     */
    [[nodiscard]]
    double interfaceSubtitleStroke() const noexcept;

    /**
     * @brief Sets the subtitle stroke width.
     *
     * @param value The subtitle stroke width.
     */
    void setInterfaceSubtitleStroke(
        double value = Keys::Interface::Subtitle::STROKE_DEFAULT);

    /**
     * @brief Gets the subtitle text color.
     *
     * @return The subtitle text color.
     */
    [[nodiscard]]
    QColor interfaceSubtitleColor() const noexcept;

    /**
     * @brief Sets the subtitle text color.
     *
     * @param value The subtitle text color.
     */
    void setInterfaceSubtitleColor(
        const QColor &value = Keys::Interface::Subtitle::TEXT_COLOR_DEFAULT);

    /**
     * @brief Gets the subtitle background color.
     *
     * @return The subtitle background color.
     */
    [[nodiscard]]
    QColor interfaceSubtitleBackground() const noexcept;

    /**
     * @brief Sets the subtitle background color.
     *
     * @param value The subtitle background color.
     */
    void setInterfaceSubtitleBackground(
        const QColor &value = Keys::Interface::Subtitle::BACKGROUND_COLOR_DEFAULT);

    /**
     * @brief Gets the subtitle stroke color.
     *
     * @return The subtitle stroke color.
     */
    [[nodiscard]]
    QColor interfaceSubtitleStrokeColor() const noexcept;

    /**
     * @brief Sets the subtitle stroke color.
     *
     * @param value The subtitle stroke color.
     */
    void setInterfaceSubtitleStrokeColor(
        const QColor &value = Keys::Interface::Subtitle::STROKE_COLOR_DEFAULT);

    /**
     * @brief Gets the search popup width.
     *
     * @return The search popup width.
     */
    [[nodiscard]]
    int interfacePopupWidth() const noexcept;

    /**
     * @brief Sets the search popup width.
     *
     * @param value The search popup width.
     */
    void setInterfacePopupWidth(
        int value = Keys::Interface::POPUP_WIDTH_DEFAULT);

    /**
     * @brief Gets the search popup height.
     *
     * @return The search popup height.
     */
    [[nodiscard]]
    int interfacePopupHeight() const noexcept;

    /**
     * @brief Sets the search popup height.
     *
     * @param value The search popup height.
     */
    void setInterfacePopupHeight(
        int value = Keys::Interface::POPUP_HEIGHT_DEFAULT);

    /**
     * @brief Gets if the search panel should be shown in a separate window.
     *
     * @return true if the search panel should be shown in a separate window,
     * @return false to show the search panel in the main window.
     */
    [[nodiscard]]
    bool interfaceSearchWindow() const noexcept;

    /**
     * @brief Sets if the search panel should be shown in a separate window.
     *
     * @param value true to show the search panel in a separate window, false
     * to show it in the main window.
     */
    void setInterfaceSearchWindow(
        bool value = Keys::Interface::SEARCH_WINDOW_DEFAULT);

    /**
     * @brief Get the search expression font.
     *
     * @return The search expression font.
     */
    [[nodiscard]]
    const QFont &interfaceSearchExpressionFont() const noexcept;

    /**
     * @brief Set the search expression font.
     *
     * @param value The search expression font.
     */
    void setInterfaceSearchExpressionFont(
        const QFont &value = Keys::Interface::SEARCH_EXPRESSION_FONT_DEFAULT);

    /**
     * @brief Get the search reading font.
     *
     * @return The search reading font.
     */
    [[nodiscard]]
    const QFont &interfaceSearchReadingFont() const noexcept;

    /**
     * @brief Set the search reading font.
     *
     * @param value The search reading font.
     */
    void setInterfaceSearchReadingFont(
        const QFont &value = Keys::Interface::SEARCH_READING_FONT_DEFAULT);

    /**
     * @brief Get the search conjugation explanation font.
     *
     * @return The search conjugation explanation font.
     */
    [[nodiscard]]
    const QFont &interfaceSearchConjFont() const noexcept;

    /**
     * @brief Set the search conjugation explanation font.
     *
     * @param value The search conjugation explanation font.
     */
    void setInterfaceSearchConjFont(
        const QFont &value = Keys::Interface::SEARCH_CONJ_FONT_DEFAULT);

    /**
     * @brief Get the search tag font.
     *
     * @return The search tag font.
     */
    [[nodiscard]]
    const QFont &interfaceSearchTagFont() const noexcept;

    /**
     * @brief Set the search tag font.
     *
     * @param value The search tag font.
     */
    void setInterfaceSearchTagFont(
        const QFont &value = Keys::Interface::SEARCH_TAG_FONT_DEFAULT);

    /**
     * @brief Get the search glossary font.
     *
     * @return The search glossary font.
     */
    [[nodiscard]]
    const QFont &interfaceSearchGlossaryFont() const noexcept;

    /**
     * @brief Set the search glossary font.
     *
     * @param value The search glossary font.
     */
    void setInterfaceSearchGlossaryFont(
        const QFont &value = Keys::Interface::SEARCH_GLOSSARY_FONT_DEFAULT);

    /**
     * @brief Get the search kanji font.
     *
     * @return The search kanji font.
     */
    [[nodiscard]]
    const QFont &interfaceSearchKanjiFont() const noexcept;

    /**
     * @brief Set the search kanji font.
     *
     * @param value The search kanji font.
     */
    void setInterfaceSearchKanjiFont(
        const QFont &value = Keys::Interface::SEARCH_KANJI_FONT_DEFAULT);

    /**
     * @brief Get if the subtitle list should be shown in a separate window.
     *
     * @return true if the subtitle list should be shown in a separate window,
     * @return false to show the subtitle list in the main window.
     */
    [[nodiscard]]
    bool interfaceSubtitleListWindow() const noexcept;

    /**
     * @brief Set if the subtitle list should be shown in a separate window.
     *
     * @param value true to show the subtitle list in a separate window, false
     * to show it in the main window.
     */
    void setInterfaceSubtitleListWindow(
        bool value = Keys::Interface::SubtitleList::WINDOW_DEFAULT);

    /**
     * @brief Get if timestamps should be shown in the subtitle list.
     *
     * @return true if timestamps should be shown in the subtitle list,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool interfaceSubtitleListTimestamps() const noexcept;

    /**
     * @brief Set if timestamps should be shown in the subtitle list.
     *
     * @param value true to show timestamps in the subtitle list, false
     * otherwise.
     */
    void setInterfaceSubtitleListTimestamps(
        bool value = Keys::Interface::SubtitleList::TIMESTAMPS_DEFAULT);

    /**
     * @brief Get the subtitle list background color.
     *
     * @return The background color of the subtitle list.
     */
    [[nodiscard]]
    QColor interfaceSubtitleListBackgroundColor() const noexcept;

    /**
     * @brief Set the subtitle list background color.
     *
     * @param value The background color of the subtitle list.
     */
    void setInterfaceSubtitleListBackgroundColor(
        const QColor &value = Keys::Interface::SubtitleList::BACKGROUND_COLOR_DEFAULT);

    /**
     * @brief Get the subtitle list text color.
     *
     * @return The subtitle list text color.
     */
    [[nodiscard]]
    QColor interfaceSubtitleListTextColor() const noexcept;

    /**
     * @brief Set the subtitle list text color.
     *
     * @param value The subtitle list text color.
     */
    void setInterfaceSubtitleListTextColor(
        const QColor &value = Keys::Interface::SubtitleList::TEXT_COLOR_DEFAULT);

    /**
     * @brief Get the background color of hovered items.
     *
     * @return The background color of hovered items.
     */
    [[nodiscard]]
    QColor interfaceSubtitleListHoverColor() const noexcept;

    /**
     * @brief Set the background color of hovered items.
     *
     * @param value The background color of hovered items.
     */
    void setInterfaceSubtitleListHoverColor(
        const QColor &value = Keys::Interface::SubtitleList::HOVER_COLOR_DEFAULT);

    /**
     * @brief Get the background color of selected items.
     *
     * @return The background color of selected items.
     */
    [[nodiscard]]
    QColor interfaceSubtitleListSelectedColor() const noexcept;

    /**
     * @brief Set the background color of selected items.
     *
     * @param value The background color of selected items.
     */
    void setInterfaceSubtitleListSelectedColor(
        const QColor &value = Keys::Interface::SubtitleList::SELECTED_COLOR_DEFAULT);

    /**
     * @brief Get the background color of selected-hovered items.
     *
     * @return The background color fo selected-hovered items.
     */
    [[nodiscard]]
    QColor interfaceSubtitleListSelectedHoverColor() const noexcept;

    /**
     * @brief Set the background color of selected-hovered items.
     *
     * @param value The background color fo selected-hovered items.
     */
    void setInterfaceSubtitleListSelectedHoverColor(
        const QColor &value = Keys::Interface::SubtitleList::SELECTED_HOVER_COLOR_DEFAULT);

    /**
     * @brief Get the font of the primary subtitles in the subtitle list.
     *
     * @return The font of the primary subtitles.
     */
    [[nodiscard]]
    const QFont &interfaceSubtitleListPrimaryFont() const noexcept;

    /**
     * @brief Set the font of the primary subtitles in the subtitle list.
     *
     * @param value The font of the primary subtitles.
     */
    void setInterfaceSubtitleListPrimaryFont(
        const QFont &value = Keys::Interface::SubtitleList::PRIMARY_FONT_DEFAULT);

    /**
     * @brief Get the font of secondary subtitles in the subtitle list.
     *
     * @return The font of secondary subtitles.
     */
    [[nodiscard]]
    const QFont &interfaceSubtitleListSecondaryFont() const noexcept;

    /**
     * @brief Set the font of secondary subtitles in the subtitle list.
     *
     * @param value The font of secondary subtitles.
     */
    void setInterfaceSubtitleListSecondaryFont(
        const QFont &value = Keys::Interface::SubtitleList::SECONDARY_FONT_DEFAULT);

    /* Keybind Settings */

    /**
     * @brief Get the keybinds available.
     *
     * @return The keybind profile model for the application.
     */
    [[nodiscard]]
    KeybindProfileModel *keybinds() const noexcept;

    /* OCR Settings */

    /**
     * @brief Gets if OCR is enabled.
     *
     * @return true if OCR is enabled,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool ocrEnabled() const noexcept;

    /**
     * @brief Sets if OCR is enabled.
     *
     * @param value true to enable OCR, false otherwise.
     */
    void setOcrEnabled(
        bool value = Keys::Ocr::ENABLED_DEFAULT);

    /**
     * @brief Sets if OCR should be GPU accelerated.
     *
     * @return true to use GPU acceleration
     * @return false otherwise.
     */
    [[nodiscard]]
    bool ocrUseGpu() const noexcept;

    /**
     * @brief Sets if OCR should be GPU accelerated.
     *
     * @param value true to use GPU acceleration, false otherwise.
     */
    void setOcrUseGpu(
        bool value = Keys::Ocr::USE_GPU_DEFAULT);

    /**
     * @brief Gets the OCR model to use.
     *
     * @return The OCR model to use.
     */
    [[nodiscard]]
    QString ocrModel() const noexcept;

    /**
     * @brief Sets the OCR model to use.
     *
     * @param value The OCR model to use.
     */
    void setOcrModel(
        const QString &value = Keys::Ocr::MODEL_DEFAULT);

signals:
    /**
     * @brief Emitted when the version is changed.
     *
     * @param value The new version number.
     */
    void versionChanged(unsigned int value);

    /* Window Settings */

    /**
     * @brief Emitted when the subtitle list open state is changed.
     *
     * @param value true if the subtitle list is open, false otherwise.
     */
    void windowSubtitleListChanged(bool value);

    /**
     * @brief Emitted when the window search state is changed.
     *
     * @param value true if the search panel is open, false otherwise.
     */
    void windowSearchChanged(bool value);

    /* Internal Settings */

    /**
     * @brief Emitted when the auto update opt-in shown state is changed.
     *
     * @param value The new value.
     */
    void internalAutoUpdateOptInShownChanged(bool value);

    /* Recent Settings */

    /**
     * @brief Emitted when the recent files are changed.
     */
    void recentFilesChanged();

    /* Subtitle List Settings */

    /**
     * @brief Emitted when the subtitle list ignore whitespace value is changed.
     *
     * @param value The new value.
     */
    void subtitleListIgnoreWhitespaceChanged(bool value);

    /**
     * @brief Emitted when the subtitle list auto seek setting is changed.
     *
     * @param value The new value.
     */
    void subtitleListAutoSeekChanged(bool value);

    /* Application Settings */

    /**
     * @brief Emitted when the auto update check is changed.
     *
     * @param value The new value.
     */
    void applicationAutoUpdateCheckChanged(bool value);

    /* Behavior Settings */

    /**
     * @brief Emitted when the auto fit behavior setting is changed.
     *
     * @param value The new value.
     */
    void behaviorAutoFitChanged(bool value);

    /**
     * @brief Emitted when the auto fit percentage is changed.
     *
     * @param value The new value.
     */
    void behaviorAutoFitPercentChanged(int value);

    /**
     * @brief Emitted when the OSC hide cursor behavior setting is changed.
     *
     * @param value The new value.
     */
    void behaviorOscMpvCursorChanged(bool value);

    /**
     * @brief Emitted when the OSC duration is changed.
     *
     * @param value The new value.
     */
    void behaviorOscDurationChanged(int value);

    /**
     * @brief Emitted when the OSC fade duration is changed.
     *
     * @param value The new value.
     */
    void behaviorOscFadeDurationChanged(int value);

    /**
     * @brief Emitted when the OSC minimum mouse movement is changed.
     *
     * @param value The new value.
     */
    void behaviorOscMinMoveChanged(int value);

    /**
     * @brief Emitted when the OSC preview thumbnails enabled is changed.
     *
     * @param value The new value.
     */
    void behaviorOscPreviewThumbnailsChanged(bool value);

    /**
     * @brief Emitted when the subtitle pause behavior setting is changed.
     *
     * @param value The new value.
     */
    void behaviorSubtitlePauseChanged(bool value);

    /**
     * @brief Emitted when the subtitle cursor show behavior setting is changed.
     *
     * @param value The new value.
     */
    void behaviorSubtitleCursorShowChanged(bool value);

    /**
     * @brief Emitted when the secondary subtitle cursor show behavior setting
     * is changed.
     *
     * @param value The new value.
     */
    void behaviorSecondarySubtitleCursorShowChanged(bool value);

    /**
     * @brief Emitted when the default file open directory setting is changed.
     *
     * @param value The new value.
     */
    void behaviorFileOpenDirectoryChanged(Setting::Directory value);

    /**
     * @brief Emitted when the custom file open directory setting is changed.
     *
     * @param value The new value.
     */
    void behaviorFileOpenCustomChanged(const QString &value);

    /* Dictionary Settings */

    /**
     * @brief Emitted when the dictionary order is changed.
     */
    void dictionaryOrderChanged();

    /* Search Settings */

    /**
     * @brief Emitted when the exact matcher setting is changed.
     *
     * @param value The new value.
     */
    void searchMatcherExactChanged(bool value);

    /**
     * @brief Emitted when the deconjugation matcher setting is changed.
     *
     * @param value The new value.
     */
    void searchMatcherDeconjChanged(bool value);

    /**
     * @brief Emitted when the MeCab IPAdic matcher setting is changed.
     *
     * @param value The new value.
     */
    void searchMatcherMecabIpadicChanged(bool value);

    /**
     * @brief Emitted when the middle mouse scan setting is changed.
     *
     * @param value The new value.
     */
    void searchMiddleMouseScanChanged(bool value);

    /**
     * @brief Emitted when the search method setting is changed.
     *
     * @param value The new value.
     */
    void searchMethodChanged(Setting::SearchMethod value);

    /**
     * @brief Emitted when the search delay setting is changed.
     *
     * @param value The new value.
     */
    void searchDelayChanged(int value);

    /**
     * @brief Emitted when the search modifier setting is changed.
     *
     * @param value The new value.
     */
    void searchModifierChanged(Setting::Modifier value);

    /**
     * @brief Emitted when the search hide subtitles setting is changed.
     *
     * @param value The new value.
     */
    void searchHideMpvSubsChanged(bool value);

    /**
     * @brief Emitted when the search hide setting is changed.
     *
     * @param value The new value.
     */
    void searchHideSubsChanged(bool value);

    /**
     * @brief Emitted when the search pause on hover setting is changed.
     *
     * @param value The new value.
     */
    void searchPauseOnHoverChanged(bool value);

    /**
     * @brief Emitted when the search auto play audio setting is changed.
     *
     * @param value The new value.
     */
    void searchAutoPlayAudioChanged(bool value);

    /**
     * @brief Emitted when the search glossary style setting is changed.
     *
     * @param value The new value.
     */
    void searchGlossaryStyleChanged(Setting::GlossaryStyle value);

    /**
     * @brief Emitted when the search replace newlines setting is changed.
     *
     * @param value The new value.
     */
    void searchReplaceNewlinesChanged(bool value);

    /**
     * @brief Emitted when the search replace newlines with setting is changed.
     *
     * @param value The new value.
     */
    void searchReplaceNewlinesWithChanged(const QString &value);

    /**
     * @brief Emitted when the search remove regex setting is changed.
     *
     * @param value The new value.
     */
    void searchRemoveRegexChanged(const QString &value);

    /**
     * @brief Emitted when the show Anki glossary checkbox setting is changed.
     *
     * @param value The new value.
     */
    void showAnkiGlossaryCheckboxChanged(bool value);

    /* Interface Settings */

    /**
     * @brief Emitted when the interface system icons setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSystemIconsChanged(bool value);

    /**
     * @brief Emitted when the interface subtitle font setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleFontChanged(const QFont &value);

     /**
     * @brief Emitted when the interface subtitle line spacing is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleLineSpacingChanged(double value);

    /**
     * @brief Emitted when the interface subtitle scale setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleScaleChanged(double value);

    /**
     * @brief Emitted when the interface subtitle offset setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleOffsetChanged(double value);

    /**
     * @brief Emitted when the interface subtitle stroke setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleStrokeChanged(double value);

    /**
     * @brief Emitted when the interface subtitle color setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleColorChanged(const QColor &value);

    /**
     * @brief Emitted when the interface subtitle background setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleBackgroundChanged(const QColor &value);

    /**
     * @brief Emitted when the interface subtitle stroke color setting is
     * changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleStrokeColorChanged(const QColor &value);

    /**
     * @brief Emitted when the interface popup width setting is changed.
     *
     * @param value The new value.
     */
    void interfacePopupWidthChanged(int value);

    /**
     * @brief Emitted when the interface search window setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSearchWindowChanged(bool value);

    /**
     * @brief Emitted when the interface popup height setting is changed.
     *
     * @param value The new value.
     */
    void interfacePopupHeightChanged(int value);

    /**
     * @brief Emitted when the interface search expression font setting is
     * changed.
     *
     * @param value The new value.
     */
    void interfaceSearchExpressionFontChanged(const QFont &value);

    /**
     * @brief Emitted when the interface reading expression font setting is
     * changed.
     *
     * @param value The new value.
     */
    void interfaceSearchReadingFontChanged(const QFont &value);

    /**
     * @brief Emitted when the interface search conjugation explanation font
     * setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSearchConjFontChanged(const QFont &value);

    /**
     * @brief Emitted when the interface search tag font setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSearchTagFontChanged(const QFont &value);

    /**
     * @brief Emitted when the interface search glossary font setting is
     * changed.
     *
     * @param value The new value.
     */
    void interfaceSearchGlossaryFontChanged(const QFont &value);

    /**
     * @brief Emitted when the interface search kanji font setting is changed.
     *
     * @param value The new value.
     */
    void interfaceSearchKanjiFontChanged(const QFont &value);

    /**
     * @brief Emitted when the interface subtitle list window setting is
     * changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleListWindowChanged(bool value);

    /**
     * @brief Emitted when the interface subtitle list timestamps setting is
     * changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleListTimestampsChanged(bool value);

    /**
     * @brief Emitted when the subtitle list background color is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleListBackgroundColorChanged(const QColor &value);

    /**
     * @brief Emitted when the subtitle list text color is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleListTextColorChanged(const QColor &value);

    /**
     * @brief Emitted when the subtitle list hover color is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleListHoverColorChanged(const QColor &value);

    /**
     * @brief Emitted when the subtitle list selected color is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleListSelectedColorChanged(const QColor &value);

    /**
     * @brief Emitted when the subtitle list selected hover color is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleListSelectedHoverColorChanged(const QColor &value);

    /**
     * @brief Emitted when the subtitle list primary font is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleListPrimaryFontChanged(const QFont &value);

    /**
     * @brief Emitted when the subtitle list secondary font is changed.
     *
     * @param value The new value.
     */
    void interfaceSubtitleListSecondaryFontChanged(const QFont &value);

    /* OCR Settings */

    /**
     * @brief Emitted when the OCR enabled setting is changed.
     *
     * @param value The new value.
     */
    void ocrEnabledChanged(bool value);

    /**
     * @brief Emitted when the OCR use GPU setting is changed.
     *
     * @param value The new value.
     */
    void ocrUseGpuChanged(bool value);

    /**
     * @brief Emitted when the OCR model setting is changed.
     *
     * @param value The new value.
     */
    void ocrModelChanged(const QString &value);

private:
    /**
     * @brief Loads the version from disc.
     */
    void loadVersion();

    /**
     * @brief Writes the version to disc.
     */
    void writeVersion();

    /**
     * @brief Update settings to the latest version.
     */
    static void updateSettings();

    /* The current version of the settings file */
    unsigned int m_version{0};

    /**
     * @brief Saved window state.
     */
    struct WindowSettings
    {
        /* true if the subtitle list was open on close */
        bool subtitleList{false};

        /* true if the subtitle search was open on close */
        bool search{false};
    };
    WindowSettings m_window{};

    /**
     * @brief Saved internal application state.
     */
    struct InternalSettings
    {
        /* true if the auto update opt in has been shown, false otherwise */
        bool autoUpdateOptInShown{false};
    };
    InternalSettings m_internal{};

    /**
     * @brief Saved recents.
     */
    struct RecentSettings
    {
        /* Recently opened files */
        QStringList files;
    };
    RecentSettings m_recent{};

    /**
     * @brief Subtitle list last used state.
     */
    struct SubtitleListSettings
    {
        /* true to ignore whitespace in searches */
        bool ignoreWhitespace{Keys::SubtitleList::IGNORE_WHITESPACE_DEFAULT};

        /* true to auto-seek in search */
        bool autoSeek{Keys::SubtitleList::AUTO_SEEK_DEFAULT};
    };
    SubtitleListSettings m_subtitleList{};

    /**
     * @brief Application settings.
     */
    struct ApplicationSettings
    {
        /* true if the application should check for updates on start */
        bool autoUpdateCheck{Keys::Application::AUTO_UPDATE_CHECK_DEFAULT};
    };
    ApplicationSettings m_applicationSettings{};

    /**
     * @brief Audio source settings.
     */
    struct AudioSourceSettings
    {
        AudioSourceModel *sources{nullptr};
    };
    AudioSourceSettings m_audioSource{};

    /**
     * @brief Player behavior settings.
     */
    struct BehaviorSettings
    {
        /* true if the player should auto-fit window size to media */
        bool autoFit{Keys::Behavior::AUTO_FIT_DEFAULT};

        /* Factor to multiply dimensions by when auto-fitting */
        int autoFitPercent{Keys::Behavior::AUTO_FIT_PERCENT_DEFAULT};

        /* true if the cursor visibility should be controlled by MPV */
        bool oscMpvCursor{Keys::Behavior::OSC_MPV_CURSOR_DEFAULT};

        /* Number of milliseconds to wait before hiding the OSC */
        int oscDuration{Keys::Behavior::OSC_DURATION_DEFAULT};

        /* Number of milliseconds it should take to hide the OSC */
        int oscFadeDuration{Keys::Behavior::OSC_FADE_DEFAULT};

        /* Number of pixels the cursor has to move to be registered as active */
        int oscMinMove{Keys::Behavior::OSC_MIN_MOVE_DEFAULT};

        /* true if preview thumbnails should be shown */
        bool oscPreviewThumbnails{
            Keys::Behavior::OSC_PREVIEW_THUMBNAILS_DEFAULT
        };

        /* true if the player should pause at the end of each subtitle */
        bool subtitlePause{Keys::Behavior::SUBTITLE_PAUSE_DEFAULT};

        /* true if subtitles should be hidden until approached by the cursor */
        bool subtitleCursorShow{Keys::Behavior::SUBTITLE_CURSOR_SHOW_DEFAULT};

        /* true if secondary subtitle should be hidden until approached by the
         * cursor */
        bool secondarySubtitleCursorShow{
            Keys::Behavior::SECONDARY_SUBTITLE_CURSOR_SHOW_DEFAULT
        };

        /* Default directory to open when opening a file */
        Setting::Directory fileOpenDirectory{
            Keys::Behavior::FILE_OPEN_DIR_DEFAULT
        };

        /* Custom location for Setting::Directory::DirectoryCustom */
        QString fileOpenCustom{Keys::Behavior::FILE_OPEN_CUSTOM_DEFAULT};
    };
    BehaviorSettings m_behavior{};

    /**
     * @brief Dictionary settings.
     */
    struct DictionarySettings
    {
        /* A list of dictionaries in priority order */
        QList<int64_t> order{};
    };
    DictionarySettings m_dictionary{};

    /**
     * @brief Player search settings.
     */
    struct SearchSettings
    {
        /* true if the exact matcher should be used */
        bool matcherExact{Keys::Search::Matcher::EXACT_DEFAULT};

        /* true if the deconjugation matcher should be used */
        bool matcherDeconj{Keys::Search::Matcher::DECONJ_DEFAULT};

        /* true if the MeCab IPAdic matcher should be used */
        bool matcherMecabIpadic{Keys::Search::Matcher::MECAB_IPADIC_DEFAULT};

        /* true if searches should be possible with middle mouse */
        bool middleMouseScan{Keys::Search::MIDDLE_MOUSE_SCAN_DEFAULT};

        /* The method for initiating subtitle searches */
        Setting::SearchMethod method{Keys::Search::METHOD_DEFAULT};

        /* Number of milliseconds to delay starting a subtitle search */
        int delay{Keys::Search::DELAY_DEFAULT};

        /* Modifier key to use to initiate searches */
        Setting::Modifier modifier{Keys::Search::MODIFIER_DEFAULT};

        /* true to hide mpv subtitles when search is visible */
        bool hideMpvSubs{Keys::Search::HIDE_MPV_SUBS_DEFAULT};

        /* true to hide the subtitle search while playing */
        bool hideSubs{Keys::Search::HIDE_SUBS_DEFAULT};

        /* true to pause the player on subtitle search hover */
        bool pauseOnHover{Keys::Search::PAUSE_ON_HOVER_DEFAULT};

        /* true to automatically play top search result audio */
        bool autoPlayAudio{Keys::Search::AUTO_PLAY_AUDIO_DEFAULT};

        /* Style to display glossaries in terms */
        Setting::GlossaryStyle glossaryStyle{
            Keys::Search::GLOSSARY_STYLE_DEFAULT
        };

        /* true to replace newlines in subtitles */
        bool replaceNewlines{Keys::Search::REPLACE_NEWLINES_DEFAULT};

        /* Text to replace newlines in subtitles with if replaceNewLines is true
         */
        QString replaceNewlinesWith{
            Keys::Search::REPLACE_NEWLINES_WITH_DEFAULT
        };

        /* The regular expression filter subtitle text through */
        QString removeRegex{Keys::Search::REMOVE_REGEX_DEFAULT};

        /* true to show Anki glossary checkboxes, false to hide */
        bool showAnkiGlossaryCheckbox{
            Keys::Search::SHOW_ANKI_GLOSSARY_CHECKBOX_DEFAULT
        };
    };
    SearchSettings m_search{};

    /**
     * @brief The player interface settings.
     */
    struct InterfaceSettings
    {
        /* true to use system icons, false to use bundled icons */
        bool systemIcons{Keys::Interface::SYSTEM_ICONS_DEFAULT};

        /* The font to use in subtitles */
        QFont subtitleFont{Keys::Interface::Subtitle::FONT_DEFAULT};

        /* The number of units between subtitle lines */
        double subtitleLineSpacing{
            Keys::Interface::Subtitle::LINE_SPACING_DEFAULT
        };

        /* Percentage of the height of the window a line of the subtitles should
         * occupy */
        double subtitleScale{Keys::Interface::Subtitle::SCALE_DEFAULT};

        /* Percentage of the height of the window that should appear below
         * subtitles */
        double subtitleOffset{Keys::Interface::Subtitle::OFFSET_DEFAULT};

        /* Stroke size to use on subtitles */
        double subtitleStroke{Keys::Interface::Subtitle::STROKE_DEFAULT};

        /* Color of the subtitle */
        QColor subtitleColor{Keys::Interface::Subtitle::TEXT_COLOR};

        /* Background color of the subtitle */
        QColor subtitleBackground{
            Keys::Interface::Subtitle::BACKGROUND_COLOR_DEFAULT
        };

        /* Color of the text stroke */
        QColor subtitleStrokeColor{
            Keys::Interface::Subtitle::STROKE_COLOR_DEFAULT
        };

        /* The width of the search popup */
        int popupWidth{Keys::Interface::POPUP_WIDTH_DEFAULT};

        /* The height of the search popup */
        int popupHeight{Keys::Interface::POPUP_HEIGHT_DEFAULT};

        /* true if the search function should be opened in a new window */
        bool searchWindow{Keys::Interface::SEARCH_WINDOW_DEFAULT};

        /* The font to use for search expressions */
        QFont searchExpressionFont{
            Keys::Interface::SEARCH_EXPRESSION_FONT_DEFAULT
        };

        /* The font to use for search readings */
        QFont searchReadingFont{Keys::Interface::SEARCH_READING_FONT_DEFAULT};

        /* The font to use for conjugation explanations */
        QFont searchConjFont{Keys::Interface::SEARCH_CONJ_FONT_DEFAULT};

        /* The font to use for tags */
        QFont searchTagFont{Keys::Interface::SEARCH_TAG_FONT_DEFAULT};

        /* The font to use for glossaries */
        QFont searchGlossaryFont{Keys::Interface::SEARCH_GLOSSARY_FONT_DEFAULT};

        /* The font to use for kanji entries */
        QFont searchKanjiFont{Keys::Interface::SEARCH_KANJI_FONT_DEFAULT};

        /* true if the subtitle list should be opened in it's own window */
        bool subtitleListWindow{Keys::Interface::SubtitleList::WINDOW_DEFAULT};

        /* true if the subtitle list should show timestamps */
        bool subtitleListTimestamps{
            Keys::Interface::SubtitleList::TIMESTAMPS_DEFAULT
        };

        /* Background color of the subtitle list */
        QColor subtitleListBackgroundColor{
            Keys::Interface::SubtitleList::BACKGROUND_COLOR_DEFAULT
        };

        /* Color of text in the subtitle list */
        QColor subtitleListTextColor{
            Keys::Interface::SubtitleList::TEXT_COLOR_DEFAULT
        };

        /* Color of hovered item backgrounds when not selected */
        QColor subtitleListHoverColor{
            Keys::Interface::SubtitleList::HOVER_COLOR_DEFAULT
        };

        /* Color of selected item backgrounds without being hovered */
        QColor subtitleListSelectedColor{
            Keys::Interface::SubtitleList::SELECTED_COLOR_DEFAULT
        };

        /* Color of selected items that are hovered */
        QColor subtitleListSelectedHoverColor{
            Keys::Interface::SubtitleList::SELECTED_HOVER_COLOR_DEFAULT
        };

        /* Primary subtitle list text font */
        QFont subtitleListPrimaryFont{
            Keys::Interface::SubtitleList::PRIMARY_FONT_DEFAULT
        };

        /* Secondary subtitle list text font */
        QFont subtitleListSecondaryFont{
            Keys::Interface::SubtitleList::SECONDARY_FONT_DEFAULT
        };
    };
    InterfaceSettings m_interface{};

    /**
     * @brief Keybind settings.
     */
    struct KeybindSettings
    {
        /* The keybind profile model */
        KeybindProfileModel *keybinds{nullptr};
    };
    KeybindSettings m_keybind{};

    /**
     * @brief OCR settings.
     */
    struct OcrSettings
    {
        /* true if OCR is enabled */
        bool enabled{Keys::Ocr::ENABLED_DEFAULT};

        /* true to use the GPU for OCR */
        bool useGpu{Keys::Ocr::USE_GPU_DEFAULT};

        /* The model to use for OCR */
        QString model{Keys::Ocr::MODEL_DEFAULT};
    };
    OcrSettings m_ocr{};
};
