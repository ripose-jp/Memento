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

#include <memory>

#include "anki/ankiclient.h"
#include "audio/audioplayer.h"
#include "dict/dictionary.h"
#include "gui/widgets/aboutwindow.h"
#include "gui/widgets/definition/definitionwidget.h"
#include "gui/widgets/overlay/playeroverlay.h"
#include "gui/widgets/settings/optionswindow.h"
#include "player/playeradapter.h"
#include "state/context.h"

#if defined(Q_OS_MACOS)
#include "macos/cocoaeventhandler.h"
#endif

namespace Ui
{
    class MainWindow;
}

/**
 * The MainWindow Memento.
 */
#if defined(Q_OS_MACOS)
class MainWindow : public QMainWindow, public CocoaEventHandlerCallback
#else
class MainWindow : public QMainWindow
#endif
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

#if defined(Q_OS_MACOS)
    /**
     * Call back for the Cocoa event handler. Disables UI updates and pauses
     * the player.
     */
    void beforeTransition() override;

    /**
     * Call back for the Cocoa event handler. Restores updatesEnabled() and the
     * player's pause state.
     */
    void afterTransition() override;
#endif

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
     * Opens all media passed in as command line arguments.
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
     * Updates the user interface Memento.
     * MainWindow is responsible for
     * - The global palette
     * - The splitter between the player and subtitle list
     * - The position of the subtitle search widget
     */
    void initTheme();

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
     * Shows the options window.
     */
    void showOptions() const;

    /**
     * Shows the about window.
     */
    void showAbout() const;

    /**
     * Automatically resizes the screen to the supplied width and height.
     * Will not exceed the dimensions of the screen and only happens if in
     * windowed mode and autofit is enabled.
     * @param width  The width of the video.
     * @param height The height of the video.
     */
    void autoFitMedia(int width, int height);

    /**
     * Toggles subtitle list visibility.
     */
    void toggleSubtitleListVisibility();

    /**
     * Updates the visibility of the subtitle list splitter.
     * This is required to prevent the splitter between the subtitle list and
     * player from being visible when nothing else is.
     */
    void updateSearchSubListSplitter();

private:
    /**
     * Loads the last saved MainWindow settings. Currently size and maximized
     * state.
     */
    void initWindow();

#if defined(Q_OS_WIN)
    /**
     * Sets whether or not 1px border is drawn when in fullscreen.
     */
    void setHasBorderInFullScreen(bool value);
#endif

    /* The object containing all of MainWindows widgets. Has ownership. */
    std::unique_ptr<Ui::MainWindow> m_ui;

    /* A saved pointer to the context */
    Context *m_context = nullptr;

    /* A saved pointer to the current AnkiClient */
    AnkiClient *m_ankiClient = nullptr;

    /* A saved pointer to the current AudioPlayer */
    AudioPlayer *m_audioPlayer = nullptr;

    /* A saved pointer to the current Dictionary */
    Dictionary *m_dictionary = nullptr;

    /* A saved pointer to the player adapter */
    PlayerAdapter *m_player = nullptr;

    /* A saved pointer to the player overlay */
    PlayerOverlay *m_overlay = nullptr;

    /* A saved pointer to the options window */
    std::unique_ptr<OptionsWindow> m_optionsWindow = nullptr;

    /* A saved pointer to the AboutWindow */
    std::unique_ptr<AboutWindow> m_aboutWindow = nullptr;

    /* Saved value for determining if the MainWindow is maximized. Used for
     * restoring window state when leaving fullscreen. */
    bool m_maximized = false;

    /* true if the window has never been shown before, false otherwise */
    bool m_firstShow = true;

#if defined(Q_OS_MACOS)
    /* The Cocoa event handler for preventing updates on screen transitions. */
    std::unique_ptr<CocoaEventHandler> m_cocoaHandler = nullptr;

    /* The old updatesEnabled() value before screen transitions. */
    bool m_oldUpdatesEnabled = true;

    /* The old pause state before screen transitions. */
    bool m_oldPause = false;
#endif
};

#endif // MAINWINDOW_H
