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

#ifndef KANJIWIDGET_H
#define KANJIWIDGET_H

#include <QWidget>

#include "dict/expression.h"

class QFrame;
class QLabel;
class QToolButton;
class QVBoxLayout;

/**
 * Widget for displaying kanji entries.
 */
class KanjiWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructor for the Kanji Widget.
     * @param kanji    The kanji to display.
     * @param showBack true if the back button should be shown.
     * @param parent   The parent of the widget.
     */
    KanjiWidget(
        QSharedPointer<const Kanji> kanji,
        bool showBack = false,
        QWidget *parent = nullptr);

Q_SIGNALS:
    /**
     * Emitted when the back button is pressed.
     */
    void backPressed() const;

private Q_SLOTS:
    /**
     * Adds the currently displayed kanji to Anki.
     * Shows a dialog on error.
     */
    void addKanji();

    /**
     * Opens the Anki browser and searches for the kanji character in the deck
     * AnkiClient is configured to add kanji cards to.
     */
    void openAnki();

private:
    /**
     * Displays all the information in a KanjiDefinition.
     * @param def    The kanji definition to display.
     * @param layout The layout to add widgets to.
     */
    void buildDefinitionLabel(const KanjiDefinition &def, QVBoxLayout *layout);

    /**
     * Adds the key value section of a KanjiDefinition.
     * @param title  The heading of the key-value section.
     * @param pairs  A list of key value pairs. The notes section of the tag is
     *               used as the key.
     * @param layout The layout to add key-values to.
     */
    void addKVSection(const QString &title,
                      const QList<QPair<Tag, QString>> &pairs,
                      QVBoxLayout *layout);

    /**
     * Helper method for creating a horizontal line widget.
     * @return A line widget. Belongs to the caller.
     */
    QFrame *createLine() const;

    /**
     * Helper method for creating properly configured QLabels.
     * @param text      The text of the label.
     * @param bold      true if the label should be bold, false otherwise.
     * @param alignment The alignment of the text.
     * @return A label. Belongs to the caller.
     */
    QLabel *createLabel(
        const QString &text,
        const bool bold = false,
        const Qt::AlignmentFlag alignment = Qt::AlignLeft) const;

    /**
     * Creates a label with the key on the right in bold and the value on the
     * left not in bold.
     * @param key   The key text.
     * @param value The value text.
     * @returns A layout containing the key and value labels. Belongs to the
     *          caller.
     */
    QLayout *createKVLabel(const QString &key, const QString &value) const;

    /* The kanji that this widget is displaying. Has ownership. */
    QSharedPointer<const Kanji> m_kanji;

    /* The button for adding or opening the kanji in Anki. */
    QToolButton *m_buttonAnkiAddOpen;
};

#endif // KANJIWIDGET_H
