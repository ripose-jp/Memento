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

#ifndef PLAYEROVERLAY_H
#define PLAYEROVERLAY_H

#include <QVBoxLayout>

class DefinitionWidget;
class PlayerControls;
class PlayerMenu;
class SubtitleWidget;
struct Term;

/**
 * Widget for overlaying controls over the player.
 */
class PlayerOverlay : public QVBoxLayout
{
    Q_OBJECT

public:
    /**
     * Creates an overlay over the player.
     * @param parent The player widget.
     */
    PlayerOverlay(QWidget *parent);
    ~PlayerOverlay();

public Q_SLOTS:
    /**
     * Hides the overlay if it should be hidden.
     */
    void hideOverlay();

    /**
     * Shows the overlay if it should be shown.
     */
    void showOverlay();

private Q_SLOTS:
    /**
     * Intializes the settings relavent to the overlay.
     */
    void initSettings();

    /**
     * Opens a new DefinitionWidget populated with the terms in the list.
     * @param terms A list of terms to display.
     */
    void setTerms(const QList<Term *> *terms);

    /**
     * Repositions the subtitles over the player. Used for keeping the subtitle
     * widget in the correct place when showing the overlay.
     */
    void repositionSubtitles();

    /**
     * Increases the size of the subtitle widget.
     */
    void increaseSubScale();

    /**
     * Decreases the size of the subtitle widget.
     */
    void decreaseSubScale();

    /**
     * Moves the subtitle widget up.
     */
    void moveSubsUp();

    /**
     * Moves the subtitle widget down.
     */
    void moveSubsDown();

    /**
     * Moves the DefinitionWidget to a position over the player relative to the
     * cursor.
     */
    void setDefinitionWidgetLocation();

    /**
     * Deletes the current DefinitionWidget if it exists.
     */
    void deleteDefinitionWidget();

private:
    /**
     * Returns if the cursor if over the children of the player.
     * @return true if the cursor is over part of the overlay, false otherwise.
     */
    bool underMouse() const;

    /**
     * Changes the subtitle scale. Saved in settings.
     * @param inc A value between -1.0 and 1.0 to increment the current scale
     *            by. If the final value is outside of 0.0 or 1.0, it is
     *            rounded.
     */
    void updateSubScale(const double inc);

    /**
     * Changes the subtitle position. Saved in settings.
     * @param inc A value between -1.0 and 1.0 to increment the position by. If
     *            the final value is outside of 0.0 or 1.0, it is rounded.
     */
    void moveSubtitles(const double inc);

    /* Saved pointer to the player. Does not have ownership. */
    QWidget *m_player;

    /* Saved pointer to the current definition widget. Has ownership. */
    DefinitionWidget *m_definition;

    /* The menu widget */
    PlayerMenu *m_menu;

    /* The subtitle widget */
    SubtitleWidget *m_subtitle;

    /* A generic spacer widget for position the subtitles */
    QWidget *m_spacer;

    /* The widget containing the player controls */
    PlayerControls *m_controls;

    /* Saved setting describing the subtitle offset from the bottom */
    double m_subOffset;
};

#endif // PLAYEROVERLAY_H
