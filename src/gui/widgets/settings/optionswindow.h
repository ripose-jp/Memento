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

#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <QWidget>

#include <QMap>

namespace Ui
{
    class OptionsWindow;
}

/**
 * Parent window of all the other settings widgets.
 */
class OptionsWindow : public QWidget
{
    Q_OBJECT

public:
    OptionsWindow(QWidget *parent = nullptr);
    ~OptionsWindow();

protected:
    /**
     * Shows the first setting in alphabetical order on show.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

private:
    /**
     * Adds a settings widget to the window.
     * @param name   The name of the settings widget to add as it will appear in
     *               the list.
     * @param widget The settings widget to add.
     */
    void addOption(const QString &name, QWidget *widget);

    /**
     * Shows the widget that corresponds to the selected settings in the list.
     */
    void showSelectedOption();

    /* The UI object containing all the widgets. */
    Ui::OptionsWindow *m_ui;

    /* The widget currently being shown. */
    QWidget *m_currentWidget;

    /* Maps settings names to settings widgets. */
    QMap<QString, QWidget *> m_widgets;
};

#endif // OPTIONSWINDOW_H
