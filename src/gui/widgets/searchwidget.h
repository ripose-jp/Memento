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

#include <QWheelEvent>

class DefinitionWidget;
class Dictionary;
class QLineEdit;
class QVBoxLayout;

struct Term;

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
     * An internal signal for moving found terms onto the UI thread.
     * @param terms The list of updated terms. Belongs to the recipient.
     */
    void termsUpdated(const QList<Term *> *terms) const;

    /**
     * An internal signal for clearing the contents of the definition widget.
     */
    void requestClear() const;

private Q_SLOTS:
    /**
     * Handler for updating the search widget when new text is entered.
     * @param text
     */
    void updateSearch(QString text);

protected:
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