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

#ifndef INTERFACESETTINGS_H
#define INTERFACESETTINGS_H

#include <QWidget>

class QToolButton;

namespace Ui
{
    class InterfaceSettings;
}

/**
 * Widget for changing the look and feel of the user interface.
 */
class InterfaceSettings : public QWidget
{
    Q_OBJECT

public:
    InterfaceSettings(QWidget *parent = nullptr);
    ~InterfaceSettings();

protected:
    /**
     * Restores saved settings on show.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

private Q_SLOTS:
    /**
     * Restores default settings to the UI. Does not apply settings.
     */
    void restoreDefaults();

    /**
     * Restores saved settings to the UI. Does not apply settings.
     */
    void restoreSaved();

    /**
     * Applies the settings in the UI to the settings.
     */
    void applyChanges();

    /**
     * Shows stylesheet help. Opens the Qt stylesheet reference website.
     */
    void showHelp() const;

private:
    /**
     * Opens a QColorDialog and changes the button to the color chosen.
     * Nothing happens upon the dialog being cancelled by the user.
     * @param button The button to change the color of.
     * @param color  The color to change the value of.
     */
    void askButtonColor(QToolButton *button, QColor &color);

    /**
     * Sets the button to the color provided.
     * @param button The button to change the color of.
     * @param color  The color to set the button to.
     */
    void setButtonColor(QToolButton *button, const QColor &color);

    /* The UI object containing all the widgets. */
    Ui::InterfaceSettings *m_ui;

    /* Color of the subtitle. */
    QColor m_subColor;

    /* Color of the subtitle background. */
    QColor m_bgColor;

    /* Color of the subtitle stroke color */
    QColor m_strokeColor;
};

#endif // INTERFACESETTINGS_H
