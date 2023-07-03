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

#ifndef DEFINITIONWIDGET_H
#define DEFINITIONWIDGET_H

#include <QWidget>

#include <QPointer>
#include <QWheelEvent>

#include "definitionstate.h"
#include "kanjiwidget.h"
#include "termwidget.h"

#include "anki/ankiclient.h"
#include "dict/expression.h"

enum class AudioSourceType;

namespace Ui
{
    class DefinitionWidget;
}

/**
 * Widget used for displaying terms and kanji from the search results.
 */
class DefinitionWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Creates a empty DefinitionWidget.
     * @param showNavigation True if the navbar should be shown. False
     *                       otherwise.
     * @param parent         The parent of the widget. Widget will display over
     *                       its parent or its own window if now parent.
     */
    DefinitionWidget(bool showNavigation = false, QWidget *parent = nullptr);
    ~DefinitionWidget();

Q_SIGNALS:
    /**
     * Emitted when the widget is hidden.
     */
    void widgetHidden() const;

    /**
     * Emitted when the widget is shown.
     */
    void widgetShown() const;

public Q_SLOTS:
    /**
     * Shows the terms in the list.
     * @param terms Pointer to a list of terms. Takes ownership of the terms and
     *              list. Can be nullptr.
     * @param kanji A pointer to the current kanji. Takes ownership. Can be
     *              nullptr.
     */
    void setTerms(SharedTermList terms, SharedKanji kanji);

    /**
     * Clears all terms.
     */
    void clearTerms();

private Q_SLOTS:
    /**
     * Initializes the theme of the Definition Widget and its children.
     */
    void initTheme();

    /**
     * Initializes term limits.
     */
    void initSearch();

    /**
     * Initializes audio sources.
     */
    void initAudioSources();

    /**
     * Initializes settings signals.
     */
    void initSignals();

    /**
     * Shows more terms up to the limit.
     */
    void showMoreTerms();

    /**
     * Sets all terms in m_terms from start to end as addable to Anki based on
     * the values in m_addable.
     * @param start The starting index in m_terms to set addable (inclusive).
     * @param end   The ending index in m_terms to set addable (exclusive).
     */
    void setAddable(const int start, const int end);

    /**
     * Adds the terms in m_terms from start to end to the end of the Definition
     * Widget.
     * @param start The starting index in m_terms of terms to add (inclusive).
     * @param end   The ending index in m_terms of terms to add (exclusive).
     */
    void showTerms(const int start, const int end);

    /**
     * Hides terms and shows a kanji entry.
     * @param kanji The kanji to show.
     */
    void showKanji(QSharedPointer<const Kanji> kanji);

    /**
     * Hides the currently shown kanji entry and brings up current terms.
     */
    void hideKanji();

    /**
     * Shows a child search.
     * @param terms The terms to show. Can be nullptr.
     * @param kanji The kanji to show. Can be nullptr.
     */
    void showChild(SharedTermList terms, SharedKanji kanji);

    /**
     * Positions the child widget around the current cursor position.
     * @return True if the child was successfully repositioned, false otherwise.
     */
    bool positionChild();

    /**
     * Positions the child search at the point below.
     * @param pos The point the child search should be positioned relative to.
     * @return True if the child was successfully repositioned, false otherwise.
     */
    bool positionChild(const QPoint &pos);

protected:
    /**
     * Emits a signal that the Definition Widget has been hidden.
     * @param event The hide event, not used.
     */
    void hideEvent(QHideEvent *event) override;

    /**
     * Emits a signal that the Definition Widget has been shown.
     * @param event The show event, not used.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Destroys all children on resize.
     * @param event The resize event.
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * Hides children when clicked.
     * @param event The click event, not used.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /* Prevents these events from being sent to mpv when widget has focus. */
    void mouseMoveEvent(QMouseEvent *event) override
        { QWidget::mouseMoveEvent(event); event->accept(); }
    void mouseReleaseEvent(QMouseEvent *event) override
        { QWidget::mouseReleaseEvent(event); event->accept(); }
    void mouseDoubleClickEvent(QMouseEvent *event) override
        { QWidget::mouseDoubleClickEvent(event); event->accept(); }

private:
    /* UI object containing all the widgets. */
    Ui::DefinitionWidget *m_ui = nullptr;

    /* The AnkiClient used for communicating with AnkiConnect. */
    AnkiClient *m_client = nullptr;

    /* The shared definition state */
    DefinitionState m_state{};

    /* List of terms. Owned by this widget. */
    QList<QSharedPointer<const Term>> m_terms;

    /* Pointer to the current kanji */
    QSharedPointer<const Kanji> m_kanji;

    /* List of booleans describing if a term can be added to Anki. */
    QList<bool> m_addable;

    /* List of shown term widgets. */
    QList<TermWidget *> m_termWidgets;

    /* The saved scroll location of the term widget. Used for restoring position
     * when looking at a kanji entry.
     */
    int m_savedScroll = 0;

    /* Current search ID. Used to prevent erroneous signals */
    int m_searchId = 0;

    /* The child definition widget */
    QPointer<DefinitionWidget> m_child = nullptr;
};

#endif // DEFINITIONWIDGET_H
