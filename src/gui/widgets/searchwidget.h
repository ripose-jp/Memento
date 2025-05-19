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

#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QLineEdit>

#include <memory>

#include <QPointer>
#include <QVBoxLayout>
#include <QWheelEvent>

#include "state/context.h"
#include "gui/widgets/definition/definitionwidget.h"

struct Term;
typedef std::shared_ptr<Term> SharedTerm;
typedef std::shared_ptr<QList<SharedTerm>> SharedTermList;

struct Kanji;
typedef std::shared_ptr<Kanji> SharedKanji;

/**
 * Widget for doing keyboard searches.
 */
class SearchEdit : public QLineEdit
{
    Q_OBJECT

public:
    /**
     * Constructs a SearchEdit.
     * @param modifier              The modifier this SearchEdit uses to trigger
     *                              searches.
     * @param searchWithMiddleMouse True to search with middle mouse, false
     *                              otherwise.
     * @param parent                The parent of this SearchEdit.
     */
    SearchEdit(
        Qt::KeyboardModifier modifier = Qt::KeyboardModifier::ShiftModifier,
        bool searchWithMiddleMouse = false,
        QWidget *parent = nullptr);

    /**
     * Sets the modifier used to trigger searches.
     * @param modifier The modifier to use.
     */
    void setModifier(Qt::KeyboardModifier modifier);

    /**
     * Sets if searches should be triggered by middle mouse.
     * @param enabled true to enable, false to disable.
     */
    void setSearchWithMiddleMouse(bool enabled);

Q_SIGNALS:
    /**
     * Emitted when a user search is triggered.
     * @param text The text of the search.
     * @param i    The index into the text to start the search.
     */
    void searchTriggered(const QString &text, int i) const;

protected:
    /**
     * Handles the triggering of searches.
     * @param event The mouse move event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * Handle the triggering of searches with middle mouse.
     * @param event The mouse move event.
     */
    void mousePressEvent(QMouseEvent *event) override;

private:
    /* The modifier used to trigger searches */
    Qt::KeyboardModifier m_modifier = Qt::KeyboardModifier::ShiftModifier;

    /* True if middle mouse should be used for searches, false otherwise */
    bool m_searchWithMiddleMouse = false;

    /* The last index that caused searchTriggered to fire */
    int m_lastIndex = -1;
};

/**
 * Widget for searching for terms.
 */
class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    SearchWidget(QWidget *parent = nullptr);

    /**
     * Initializes the widget with the given context.
     * @param context The application context.
     */
    void initialize(QPointer<Context> context);

Q_SIGNALS:
    /**
     * Emitted when the widget is hidden.
     */
    void widgetHidden() const;

    /**
     * Emitted when the widget is shown.
     */
    void widgetShown() const;

    /**
     * An internal signal for moving found terms onto the UI thread.
     * @param terms The list of updated terms. Belongs to the recipient.
     * @param kanji The kanji if found. Belongs to the recipient.
     */
    void searchUpdated(SharedTermList terms, SharedKanji kanji) const;

public Q_SLOTS:
    /**
     * Sets the current search.
     * @param text The text of the search to set.
     */
    void setSearch(const QString &text);

private Q_SLOTS:
   /**
     * Handler for updating the search widget when new text is entered.
     * @param text The text to search.
     * @param index The index into the text where the search should begin.
     */
    void updateSearch(QString text, int index);

    /**
     * Handler for updating the search widget when new text is entered.
     * @param text The text to search.
     */
    void updateSearch(const QString &text);

    /**
     * Initializes search settings.
     */
    void initSettings();

protected:
    /**
     * Scrolls to the last item in the subtitle list on hide.
     * @param event The hide event, not used.
     */
    void hideEvent(QHideEvent *event) override
        { QWidget::hideEvent(event); emit widgetHidden(); }

    /**
     * Scrolls to the current selected row on show.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override
        { QWidget::showEvent(event); emit widgetShown(); }

    /**
     * Size hint to prevent the widget from being too large when shown.
     * @return The minimum size.
     */
    QSize sizeHint() const override { return minimumSize(); }

    /**
     * Called when the mouse wheel is moves. Reimplemented to prevent wheel
     * events from being passed to the player.
     * @param event The wheel event.
     */
    void wheelEvent(QWheelEvent *event) override
        { QWidget::wheelEvent(event); event->accept(); }

private:
    /* The application context */
    QPointer<Context> m_context = nullptr;

    /* The parent layout */
    QVBoxLayout *m_layoutParent = nullptr;

    /* The search box */
    SearchEdit *m_searchEdit = nullptr;

    /* The definition widget */
    DefinitionWidget *m_definition = nullptr;
};

#endif // SEARCHWIDGET_H
