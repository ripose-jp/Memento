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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "../anki/ankiclient.h"
#include "../util/globalmediator.h"
#include "playeradapter.h"
#include "widgets/aboutwindow.h"
#include "widgets/definition/definitionwidget.h"
#include "widgets/overlay/playeroverlay.h"
#include "widgets/settings/optionswindow.h"

namespace Ui
{
    class MainWindow;
}

/**
 * The MainWindow Memento.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public Q_SLOTS:
    /**
     * Set the fullscreen mode of the window.
     * Remembers if the windows was maximized or not.
     * @param value true if fullscreen, false otherwise
     */
    void setFullscreen(bool value);

protected:
    /**
     * Saves the the values of the MainWindow so they can be restored on the
     * next open. Quits the application.
     * @param event The close event.
     */
    void closeEvent(QCloseEvent *event) override;

    /**
     * Opens all media passed in as command line arguements.
     * Shows a message if no dictionaries are installed.
     * @param event The show event.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Captures all keypress events and sends them to the player.
     * @param event The event sent to the player.
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * Used to close the current DefinitionWidget on a click elsewhere.
     * @param event The mouse event.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * Captures all wheel events and sends them to the player.
     * @param event The event to send to the player.
     */
    void wheelEvent(QWheelEvent *event) override;

    /**
     * Accepts drag events of type text/uri-list.
     * @param event The drag event.
     */
    void dragEnterEvent(QDragEnterEvent *event) override;

    /**
     * Sends dropped URIs over to the player to open.
     * @param event The drop event.
     */
    void dropEvent(QDropEvent *event) override;

    /**
     * Emits a signal when the window focus is changed.
     * @param event The change event.
     */
    void changeEvent(QEvent *event) override;

private Q_SLOTS:
    /**
     * Shows a critical QMessageBox. Used when an error has occurred.
     * This method is used to make sure that errors that occur on threads other
     * than the UI thread can show an error messages.
     * @param title The title of the window.
     * @param error The message to display.
     */
    void showErrorMessage(const QString title, const QString error) const;

    /**
     * Shows an information QMessageBox. Used when an error has occurred.
     * This method is used to make sure that errors that occur on threads other
     * than the UI thread can show an error messages.
     * @param title The title of the window.
     * @param error The message to display.
     */
    void showInfoMessage(const QString title, const QString error) const;

    /**
     * Updates the user interface Memento.
     * MainWindow is repsonsible for
     * - The global palette
     * - The splitter between the player and subtitle list
     * - The position of the subtitle search widget
     */
    void initTheme();

private:
    /**
     * Loads the last saved MainWindow settings. Currently size and maximized
     * state.
     */
    void initWindow();

    /**
     * Intializes DefinitionWidget signals.
     */
    void initDefinitionWidget();

    /**
     * Returns if the cursor is over the player or not.
     * @return true if the cursor is not over the player controls or the
     *         DefinitonWidget, false otherwise.
     */
    inline bool isMouseOverPlayer();

    /* The object containing all of MainWindows widgets. Has ownership. */
    Ui::MainWindow *m_ui;

    /* A saved pointer to the GlobalMediator. Does not have ownership. */
    GlobalMediator *m_mediator;

    /* A saved pointer to the player adapter. Has ownership. */
    PlayerAdapter *m_player;

    /* A saved pointer to the player overlay. Has ownership. */
    PlayerOverlay *m_overlay;

    /* A saved pointer to the current AnkiClient. Has ownership. */
    AnkiClient *m_ankiClient;

    /* A saved pointer to the options window. Has ownership. */
    OptionsWindow *m_optionsWindow;

    /* A saved pointer to the AboutWindow. Has ownership. */
    AboutWindow *m_aboutWindow;

    /* Saved value for determining if the MainWindow is maximized. Used for
     * restoring window state when leaving fullscreen.
     */
    bool m_maximized;
};

#endif // MAINWINDOW_H
