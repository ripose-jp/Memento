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

#ifndef DICTIONARYSETTINGS_H
#define DICTIONARYSETTINGS_H

#include <QWidget>

#include <QMutex>

namespace Ui
{
    class DictionarySettings;
}

/**
 * Widget used for adding dictionaries and changing their priorities.
 */
class DictionarySettings : public QWidget
{
    Q_OBJECT

public:
    DictionarySettings(QWidget *parent = nullptr);
    ~DictionarySettings();

protected:
    /**
     * Restores the saved settings.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Makes sure the buttons are disabled.
     * @param event The hide event, not used.
     */
    void hideEvent(QHideEvent *event) override;

Q_SIGNALS:
    /**
     * Emitted when the dictionary grabbing thread completes.
     * @param dicts    The list of dictionaries.
     * @param disabled The list of disabled dictionaries.
     */
    void restoreSavedReady(
        const QStringList &dicts,
        const QStringList &disabled) const;

private Q_SLOTS:
    /**
     * Initializes button icons.
     */
    void initIcons();

    /**
     * Restores saved settings and applies them again.
     */
    void restoreSaved();

    /**
     * Handles updating the UI when dictionaries are returned from the worker
     * thread.
     * @param dicts    The list of dictionaries.
     * @param disabled The list of disabled dictionaries.
     */
    void restoreSavedHelper(
        const QStringList &dicts,
        const QStringList &disabled);

    /**
     * Saves the dictionary priorities.
     */
    void applySettings();

    /**
     * Moves the selected dictionary up in the list.
     */
    void moveUp();

    /**
     * Moves the selected dictionary down in the list.
     */
    void moveDown();

    /**
     * Opens a file dialog for adding a dictionary and adds the selected file.
     * Applies current configuration.
     * Shows a dialog on error.
     */
    void addDictionary();

    /**
     * Deletes the currently selected dictionary from the database.
     * Applies current configuration.
     */
    void deleteDictionary();

    /**
     * Enables/Disables all buttons.
     * @param value true to set buttons enabled, false otherwise.
     */
    void setButtonsEnabled(const bool value);

private:
    /* The UI object containing all the widgets. */
    Ui::DictionarySettings *m_ui;

    /* Condition to prevent multiple restoreSaved thread from being fired */
    QMutex m_restoreSavedActive;
};

#endif // DICTIONARYSETTINGS_H
