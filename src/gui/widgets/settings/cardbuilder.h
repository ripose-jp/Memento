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

#ifndef CARDBUILDER_H
#define CARDBUILDER_H

#include <QWidget>

#include <QJsonObject>

class QCompleter;

namespace Ui
{
    class CardBuilder;
}

/**
 * Widget for accepting user input describing Anki cards.
 */
class CardBuilder : public QWidget
{
    Q_OBJECT

public:
    CardBuilder(QWidget *parent = nullptr);
    ~CardBuilder();

    /**
     * Sets tags that will be auto
     * @param words  The words to add to the auto completion list.
     */
    void setSuggestions(const QStringList &words);

    /**
     * Populates the Decks spinbox.
     * @param decks List of deck names.
     * @param set   The deck the spinbox should be set to.
     */
    void setDecks(const QStringList &decks, const QString &set = QString());

    /**
     * Populates the Models spinbox.
     * @param models List of model names.
     * @param set    The model to set the spinbox to.
     */
    void setModels(const QStringList &models, const QString &set = QString());

    /**
     * Sets the text of the deck spinbox. The item must exist.
     * @param text The item to set the spinbox to.
     */
    void setDeckCurrentText(const QString &text);

    /**
     * Sets the text of the model spinbox. The item must exist.
     * @param text The item to set the spinbox to.
     */
    void setModelCurrentText(const QString &text);

    /**
     * Populates the fields column of the table with the fields in the list.
     * Everything is the value column is empty.
     * @param fields The list of fields.
     */
    void setFields(const QStringList &fields);

    /**
     * Populates the fields columns with the keys and the value column with the
     * values.
     * @param fields Json object containing fields and values.
     */
    void setFields(const QJsonObject &fields);

    /**
     * Gets the current text in the deck spinbox.
     * @return The current text in the deck spinbox.
     */
    QString getDeckText() const;

    /**
     * Gets the current text in the model spinbox.
     * @return The current text in the model spinbox.
     */
    QString getModelText() const;

    /**
     * Gets the current field-value map in the table.
     * @return A json object with the field column as keys.
     */
    QJsonObject getFields() const;

Q_SIGNALS:
    /**
     * Emitted when the text in the deck spinbox is changed.
     */
    void deckTextChanged(const QString &text);

    /**
     * Emitted when the text in the model spinbox is changed.
     */
    void modelTextChanged(const QString &text);

private:
    /* The UI object containing all the widgets. */
    Ui::CardBuilder *m_ui;

    /* The completer for this widget */
    QCompleter *m_completer;
};

#endif // CARDBUILDER_H
