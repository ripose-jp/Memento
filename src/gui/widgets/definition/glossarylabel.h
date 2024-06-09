////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2022 Ripose
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

#ifndef GLOSSARYLABEL_H
#define GLOSSARYLABEL_H

#include <QRunnable>
#include <QTextEdit>

#include "dict/expression.h"
#include "util/constants.h"

class GlossaryLabel : public QTextEdit
{
    Q_OBJECT

public:
    /**
     * Creates a new glossary label.
     * @param modifier The modifier that triggers recursive searches.
     * @param style    The style to display different definitions in.
     * @param parent   The parent of this label.
     */
    GlossaryLabel(
        Qt::KeyboardModifier modifier = Qt::KeyboardModifier::ShiftModifier,
        Constants::GlossaryStyle style = Constants::GlossaryStyle::Bullet,
        QWidget *parent = nullptr);
    virtual ~GlossaryLabel();

    /**
     * Sets the contents of this label.
     * @param definitions The definitions to add to this label.
     * @param basepath    The path where all external resources begin.
     */
    void setContents(const QJsonArray &definitions, QString basepath);

public Q_SLOTS:
    /**
     * Deselects all text.
     */
    void deselectText();

Q_SIGNALS:
    /**
     * Emitted when a search is started.
     * @param terms A list of the terms found. Can be empty.
     * @param kanji The kanji found. Can be nullptr.
     */
    void contentSearched(SharedTermList terms, SharedKanji kanji) const;

protected:
    /**
     * Returns the ideal size for this widget.
     * @return The ideal size.
     */
    QSize sizeHint() const override;

    /**
     * Called when this widget is shown.
     * @param event The show event.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Called when this widget is resized.
     * @param event The resize event.
     */
    void resizeEvent(QResizeEvent *event) override;

    /**
     * Used to search.
     * @param event The mouse move event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * Ignores the click event.
     * @param event The click event.
     */
    void mousePressEvent(QMouseEvent *event) override;

private Q_SLOTS:
    /**
     * Adjust the size of the label.
     */
    void adjustSize();

    /**
     * Handles a finished search.
     * @param terms    The list of found terms. Can be nullptr.
     * @param kanji    The found kanji. Can be nullptr.
     * @param position The index the search started at.
     * @param length   The length of the matched text.
     */
    void handleSearch(
        SharedTermList terms,
        SharedKanji kanji,
        int position,
        int length);

private:
    /**
     * @brief Adds structured style objects.
     * @param      obj The structured style object.
     * @param[out] out The string this style will be appended to.
     */
    void addStructuredStyle(const QJsonObject &obj, QString &out) const;

    /**
     * Adds string structured content.
     * @param      str The string to add.
     * @param[out] out The string this string will be appended to.
     */
    void addStructuredContentHelper(
        const QString &str, QString &out) const;

    /**
     * Adds an array of structured content.
     * @param      arr      The array of structured content.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this content will be appended to.
     */
    void addStructuredContentHelper(
        const QJsonArray &arr, const QString &basepath, QString &out) const;

    /**
     * Adds an object of structured content.
     * @param      obj      The object of structured content.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this content will be appended to.
     */
    void addStructuredContentHelper(
        const QJsonObject &obj, const QString &basepath, QString &out) const;

    /**
     * Parses and outputs structured content to HTML.
     * The root of the structured content add parser.
     * @param      val      The JSON value of the structured content.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this content will be appended to.
     */
    void addStructuredContent(
        const QJsonValue &val, const QString &basepath, QString &out) const;

    /**
     * Displays an image type object.
     * @param      obj      The image object.
     * @param      basepath The base of the image path.
     * @param[out] out      The string this image will be appended to.
     */
    void addImage(
        const QJsonObject &obj,
        const QString &basepath,
        QString &out) const;

    /**
     * Adds a text object to the HTML document.
     * @param      obj The text object.
     * @param[out] out The string the formatted text will be appended to.
     */
    void addText(const QJsonObject &obj, QString &out) const;

    /**
     * Determines if an array contains structured content.
     * @param definitions The array of definitions.
     * @return True if the array contains structured content, false otherwise.
     */
    [[nodiscard]]
    static bool containsStructuredContent(const QJsonArray &definitions);

    /* The modifier that triggers searches */
    Qt::KeyboardModifier m_searchModifier = Qt::KeyboardModifier::ShiftModifier;

    /* The style of this label */
    Constants::GlossaryStyle m_style;

    /* The index that is currently being searched */
    int m_currentIndex = -1;
};

/**
 * A worker thread for searching for terms an kanji.
 */
class DictionaryWorker : public QObject, public QRunnable
{
    Q_OBJECT

public:
    /**
     * Construct a new Dictionary Worker.
     * @param query    The query to search for.
     * @param sentence The sentence containing the query.
     * @param index    The position of the query in the sentence.
     * @param position The position of the query in the entire text.
     */
    DictionaryWorker(
        const QString &query,
        const QString &sentence,
        int index,
        int position
    ) : QObject(nullptr),
        query(query),
        sentence(sentence),
        index(index),
        position(position) {}

    /**
     * Searches the dictionary and emits are signal when finished.
     */
    void run() override;

Q_SIGNALS:
    /**
     * Emitted when a search has been completed.
     * @param terms    A shared pointer to a list of shared terms. Can be
     *                 nullptr.
     * @param kanji    A shared pointer to the found kanji. Can be nullptr.
     * @param position The position of the term in the entire text.
     * @param length   The length of the match.
     */
    void searchDone(
        SharedTermList terms,
        SharedKanji kanji,
        int position,
        int length) const;

private:
    /* The query for this worker */
    const QString query;

    /* The sentence containing the query */
    const QString sentence;

    /* The index of the query in the sentence */
    int index;

    /* The position of the query in the entire text */
    int position;
};

#endif // GLOSSARYLABEL_H
