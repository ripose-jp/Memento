////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 Ripose
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

#ifndef OCRSETTINGS_H
#define OCRSETTINGS_H

#include <QWidget>

namespace Ui
{
    class OCRSettings;
}

/**
 * Settings widget for configuring OCR behavior.
 */
class OCRSettings : public QWidget
{
    Q_OBJECT

public:
    OCRSettings(QWidget *parent = nullptr);
    ~OCRSettings();

protected:
    /**
     * Restores saved settings on show.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    /**
     * Restores saved settings. Does not apply them.
     */
    void restoreSaved();

    /**
     * Restores the default settings. Does not apply them.
     */
    void restoreDefaults();

    /**
     * Applies current choices in the UI to the saved settings.
     */
    void applySettings();

    /**
     * Opens a file dialog and sets the text to be the selected model file.
     */
    void setLocalModel();

private:
    /* The UI object containing all the widgets. */
    Ui::OCRSettings *m_ui;
};

#endif // OCRSETTINGS_H
