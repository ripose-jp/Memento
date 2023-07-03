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

#ifndef AUDIOSOURCESETTINGS_H
#define AUDIOSOURCESETTINGS_H

#include <QWidget>

class QComboBox;
namespace Constants
{
    enum class AudioSourceType;
}

namespace Ui
{
    class AudioSourceSettings;
}

/**
 * Widget for adding remote audio sources.
 */
class AudioSourceSettings : public QWidget
{
    Q_OBJECT

public:
    AudioSourceSettings(QWidget *parent = nullptr);
    ~AudioSourceSettings();

protected:
    /**
     * Restores saved settings on show.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    /**
     * Applies the current configuration to the settings.
     */
    void applyChanges();

    /**
     * Restores saved settings to the UI without applying them.
     */
    void restoreDefaults();

    /**
     * Restores saved settings to the UI without applying them.
     */
    void restoreSaved();

    /**
     * Shows a help dialog.
     */
    void showHelp();

    /**
     * Disables/Enables buttons based on the current selected row.
     */
    void updateButtons();

    /**
     * Moves the currently select row up.
     */
    void moveUp();

    /**
     * Moves the currently selected row down.
     */
    void moveDown();

private:
    /**
     * Helper method for moving a row, step spaces.
     * @param row  The row to move.
     * @param step A signed number of rows to move the row.
     */
    void moveRow(const int row, const int step);

    /**
     * Guarantees the last row is empty and removes empty rows that are not the
     * last row.
     */
    void updateRows();

    /**
     * Verifies that names in the name column are not empty or duplicated.
     * @return Error string. Empty string on no error.
     */
    QString verifyNames() const;

    /**
     * Returns if the item at row and col is empty.
     * @param row The row of the item to check.
     * @param col The column of the item to check.
     */
    inline bool itemEmpty(const int row, const int col) const;

    /**
     * Returns if the entire row is empty.
     * @param row The row to check.
     */
    inline bool rowEmpty(const int row) const;

    /**
     * Creates a combo box for audio source type selection.
     * @param type The type the combo box should be set to.
     * @return A QComboBox with all possible options.
     */
    inline QComboBox *createTypeComboBox(Constants::AudioSourceType type) const;

    /**
     * Creates a combo box for audio source type selection.
     * @param type The type the combo box should be set to.
     * @return A QComboBox with all possible options.
     */
    inline QComboBox *createTypeComboBox(const QString &type) const;

    /* The UI object containing all the widgets. */
    Ui::AudioSourceSettings *m_ui;
};

#endif // AUDIOSOURCESETTINGS_H
