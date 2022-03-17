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

#include <QWidget>

#include <QSharedPointer>
#include <QWheelEvent>

class DefinitionWidget;
class Dictionary;
class QLineEdit;
class QVBoxLayout;

struct Term;
typedef QSharedPointer<Term> SharedTerm;
typedef QSharedPointer<QList<SharedTerm>> SharedTermList;

struct Kanji;
typedef QSharedPointer<Kanji> SharedKanji;

/**
 * Widget for searching for terms.
 */
class SearchWidget : public QWidget
{
    Q_OBJECT

public:
    SearchWidget(QWidget *parent = nullptr);

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

private Q_SLOTS:
    /**
     * Handler for updating the search widget when new text is entered.
     * @param text
     */
    void updateSearch(QString text);

protected:
    /**
     * Scrolls to the last item in the subtitle list on hide.
     * @param event The hide event, not used.
     */
    void hideEvent(QHideEvent *event) override
        { QWidget::hideEvent(event); Q_EMIT widgetHidden(); }

    /**
     * Scrolls to the current selected row on show.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override
        { QWidget::showEvent(event); Q_EMIT widgetShown(); }

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
    /* The parent layout */
    QVBoxLayout *m_layoutParent;

    /* The search box */
    QLineEdit *m_lineEditSearch;

    /* The definition widget */
    DefinitionWidget *m_definition;

    /* Pointer to the global dictionary */
    Dictionary *m_dictionary;
};

#endif // SEARCHWIDGET_H