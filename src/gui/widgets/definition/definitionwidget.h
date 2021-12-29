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

#include <QWheelEvent>

#include "../../../anki/ankiclient.h"
#include "../../../dict/expression.h"
#include "kanjiwidget.h"
#include "termwidget.h"

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
     * @param parent The parent of the widget. Widget will display over its
     *               parent or its own window if now parent.
     */
    DefinitionWidget(QWidget *parent = nullptr);
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
    void setTerms(const QList<Term *> *terms, const Kanji *kanji);

    /**
     * Clears all terms.
     */
    void clearTerms();

private Q_SLOTS:
    /**
     * Intializes the theme of the Definition Widget and its children.
     */
    void initTheme();

    /**
     * Initializes term limits.
     */
    void initSearch();

    /**
     * Intializes audio sources.
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
    void setAddable(const size_t start, const size_t end);

    /**
     * Adds the terms in m_terms from start to end to the end of the Definition
     * Widget.
     * @param start The starting index in m_terms of terms to add (inclusive).
     * @param end   The ending index in m_terms of terms to add (exclusive).
     */
    void showTerms(const size_t start, const size_t end);

    /**
     * Hides terms and shows a kanji entry.
     * @param kanji The kanji to show.
     */
    void showKanji(std::shared_ptr<const Kanji> kanji);

    /**
     * Hides the currently shown kanji entry and brings up current terms.
     */
    void hideKanji();

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

    /* Prevents these events from being sent to mpv when widget has focus. */
    void mouseMoveEvent(QMouseEvent *event) override
        { QWidget::mouseMoveEvent(event); event->accept(); }
    void mouseReleaseEvent(QMouseEvent *event) override
        { QWidget::mouseReleaseEvent(event); event->accept(); }
    void mouseDoubleClickEvent(QMouseEvent *event) override
        { QWidget::mouseDoubleClickEvent(event); event->accept(); }
    void mousePressEvent(QMouseEvent *event) override
        { QWidget::mousePressEvent(event); event->accept(); }

private:
    /* UI object containing all the widgets. */
    Ui::DefinitionWidget *m_ui;

    /* The AnkiClient used for communicating with AnkiConnect. */
    AnkiClient *m_client;

    /* The list of audio sources */
    QList<AudioSource> m_sources;

    /* The maximum number of terms that should be shown on one search */
    size_t m_limit;

    /* List of terms. Owned by this widget. */
    QList<std::shared_ptr<const Term>> m_terms;

    /* Pointer to the current kanji */
    std::shared_ptr<const Kanji> m_kanji;

    /* List of booleans describing if a term can be added to Anki. */
    QList<bool> m_addable;

    /* List of shown term widgets. */
    QList<TermWidget *> m_termWidgets;

    /* The saved scroll location of the term widget. Used for restoring position
     * when looking at a kanji entry.
     */
    int m_savedScroll;

    /* Current search ID. Used to prevent erronious signals */
    int m_searchId = 0;
};

#endif // DEFINITIONWIDGET_H