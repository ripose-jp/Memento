////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2021 Ripose
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

#ifndef ANKISETTINGS_H
#define ANKISETTINGS_H

#include <QWidget>

#include <QMutex>

#include "ankisettingshelp.h"

#include "anki/ankiclient.h"
#include "anki/ankiconfig.h"

namespace Ui
{
    class AnkiSettings;
}

class CardBuilder;

/**
 * Front end for modifying Anki Integration settings.
 */
class AnkiSettings : public QWidget
{
    Q_OBJECT

public:
    AnkiSettings(QWidget *parent = nullptr);
    ~AnkiSettings();

protected:
    /**
     * Restores the current saved config to the UI.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Resets the server to the saved server when closed.
     */
    void hideEvent(QHideEvent *event) override;

private Q_SLOTS:
    /**
     * Updates icons on a theme change.
     */
    void initIcons();

    /**
     * Applies the changes to the AnkiClient.
     */
    void applyChanges();

    /**
     * Restores the default settings to the UI. Does not apply changes.
     */
    void restoreDefaults();

    /**
     * Restores the saved settings.
     */
    void restoreSaved();

    /**
     * Called when the Anki Integration Enabled checkbox is changed.
     * Enables/Disables widgets on the front end.
     * @param state The checkbox state.
     */
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    void enabledStateChanged(int state);
#else
    void enabledStateChanged(Qt::CheckState state);
#endif

    /**
     * Connects to AnkiConnect and updates the decks, models, and fields.
     * @param showErrors If true, shows a dialog box containing errors.
     */
    void connectToClient(const bool showErrors = true);

    /**
     * Shows the fields for the currently selected model in the table.
     * @param cb    The CardBuilder that should be updated.
     * @param model The name of the model to get fields from.
     */
    void updateModelFields(CardBuilder *cb, const QString &model);

    /**
     * Populates the UI with the asked profile.
     * @param text The name of the profile to populate the UI with.
     */
    void changeProfile(const QString &text);

    /**
     * Adds a new profile with all the same settings so long as the name doesn't
     * already exist. Does not apply settings.
     */
    void addProfile();

    /**
     * Removes the current profile so long as it is not the Default profile.
     * Does not apply settings.
     */
    void deleteProfile();

private:
    /**
     * Loads the information from the config into the UI.
     * @param profile The name of the profile.
     * @param config  The struct containing the config information.
     */
    void populateFields(const QString &profile, const AnkiConfig &config);

    /**
     * Saves changes to cached config without applying them to the client.
     * @param profile The profile to save the changes to.
     */
    void applyToConfig(const QString &profile);

    /**
     * Renames a profile in the cache without applying it to the client.
     * @param oldName The current name of the profile.
     * @param newName The new name of the profile.
     */
    void renameProfile(const QString &oldName, const QString &newName);

    /**
     * Converts a duplicate policy in its string representation.
     * @param policy The duplicate policy.
     * @return The string representation of the duplicate policy.
     */
    QString duplicatePolicyToString(AnkiConfig::DuplicatePolicy policy);

    /**
     * Converts a string into its corresponding duplicate policy.
     * @param str The string to convert into a duplicate policy.
     * @returns The enum representation of the duplicate policy string.
     */
    AnkiConfig::DuplicatePolicy stringToDuplicatePolicy(const QString &str);

    /**
     * Converts a file type to its string representation.
     * @param type The enum file type.
     * @return The string representation of type.
     */
    QString fileTypeToString(AnkiConfig::FileType type);

    /**
     * Converts a file type string into its corresponding enum representation.
     * @param str The file type string.
     * @return The corresponding FileType enum representation.
     */
    AnkiConfig::FileType stringToFileType(const QString &str);

    /* The UI object that holds all the widgets. */
    Ui::AnkiSettings *m_ui;

    /* The help window that explains what the card markers are. */
    AnkiSettingsHelp *m_ankiSettingsHelp;

    /* A cached set of configs index by profile name. */
    QHash<QString, QSharedPointer<AnkiConfig>> m_configs;

    /* The name of the current profile visible in the UI. */
    QString m_currentProfile;

    /* A mutex to prevent race conditions when updating CardBuilders. */
    QMutex m_mutexUpdateModelFields;
};

#endif // ANKISETTINGS_H
