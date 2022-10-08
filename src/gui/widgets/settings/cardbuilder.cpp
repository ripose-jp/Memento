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

#include "cardbuilder.h"
#include "ui_cardbuilder.h"

#include <QCompleter>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QStringListModel>
#include <QTextBlock>

/* Begin CompleterEdit Class */

class CompleterEdit : public QPlainTextEdit
{
public:
    CompleterEdit(QCompleter *completer, QWidget *parent = nullptr);
    virtual ~CompleterEdit();

    /**
     * Sets the QCompleter for the widget to use to auto complete.
     * @param completer The completer for this widget to use.
     *                  Does not take ownership. Can be shared. Can be nullptr.
     */
    void setCompleter(QCompleter *completer);

protected:
    /**
     * Returns the ideal size for this widget.
     * @return The ideal size.
     */
    QSize sizeHint() const override;

    /**
     * Called when this widget is focused on. Sets the QCompleter to use this
     * widget.
     * @param event The focus event.
     */
    void focusInEvent(QFocusEvent *event) override;

    /**
     * Called when a key is pressed. Used to forward text to the completer.
     * @param event
     */
    void keyPressEvent(QKeyEvent *event) override;

private Q_SLOTS:
    /**
     * Inserts the completer completion into the current cursor location.
     * @param completion The completion to insert.
     */
    void insertCompletion(const QString &completion);

private:
    /**
     * Returns the first potential completion prefix.
     * The rules for this are the first unclosed { from the text cursor
     * position.
     * @return The first unfinished marker. The empty string if it doesn't
     *         exist.
     */
    QString firstPrefixUnderCursor() const;

    /* The completer this widget is using */
    QCompleter *m_completer = nullptr;
};

/* End CompleterEdit Class */
/* Begin CompleterEdit Implementation */

CompleterEdit::CompleterEdit(QCompleter *completer, QWidget *parent)
    : QPlainTextEdit(parent)
{
    setCompleter(completer);

    setFrameStyle(QFrame::NoFrame);
    setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    setWordWrapMode(QTextOption::WrapMode::NoWrap);
}

CompleterEdit::~CompleterEdit()
{

}

void CompleterEdit::setCompleter(QCompleter *c)
{
    if (m_completer)
    {
        m_completer->disconnect(this);
    }
    m_completer = c;
    if(m_completer)
    {
        connect(
            m_completer, QOverload<const QString &>::of(&QCompleter::activated),
            this, &CompleterEdit::insertCompletion
        );
    }
}

QSize CompleterEdit::sizeHint() const
{
    int height = 0;
    QTextDocument *doc = document();
    QAbstractTextDocumentLayout *layout = doc->documentLayout();
    for (QTextBlock b = doc->begin(); b != doc->end(); b = b.next())
    {
        height += layout->blockBoundingRect(b).height();
    }
    height += doc->documentMargin() * 2;
    height += frameWidth() * 2;
    QSize s = doc->size().toSize();
    s.setHeight(height);
    return s;
}

void CompleterEdit::focusInEvent(QFocusEvent *event)
{
    QPlainTextEdit::focusInEvent(event);
    if (m_completer)
    {
        m_completer->setWidget(this);
    }
}

void CompleterEdit::keyPressEvent(QKeyEvent *e)
{
    if (m_completer == nullptr)
    {
        QPlainTextEdit::keyPressEvent(e);
        return;
    }
    else if (m_completer->popup()->isVisible())
    {
        /* The following keys are forwarded by the completer to the widget */
        switch (e->key())
        {
        /* Let the completer do the default behavior */
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
           break;
       }
    }

    QPlainTextEdit::keyPressEvent(e);

    QString compPrefix = firstPrefixUnderCursor();
    if (compPrefix.isEmpty())
    {
        m_completer->popup()->hide();
        return;
    }
    else if (compPrefix != m_completer->completionPrefix()) {
        m_completer->setCompletionPrefix(compPrefix);
        m_completer->popup()->setCurrentIndex(
            m_completer->completionModel()->index(0, 0)
        );
    }
    QRect rect = cursorRect();
    rect.setWidth(
        m_completer->popup()->sizeHintForColumn(0) +
        m_completer->popup()->verticalScrollBar()->sizeHint().width()
    );
    m_completer->complete(rect);
}

void CompleterEdit::insertCompletion(const QString &completion)
{
    if (m_completer->widget() != this)
    {
        return;
    }
    QTextCursor tc = textCursor();
    int extra = completion.length() - m_completer->completionPrefix().length();
    tc.insertText(completion.right(extra));
    setTextCursor(tc);

    /* Truncates everything up to the next } character if found before a newline
     * or { */
    QString text = toPlainText();
    for (int i = tc.position(); i < text.length(); ++i)
    {
        if (text[i] == '\n' || text[i] == '{')
        {
            return;
        }
        else if (text[i] == '}')
        {
            tc.setPosition(i + 1, QTextCursor::MoveMode::KeepAnchor);
            tc.removeSelectedText();
            return;
        }
    }
}

QString CompleterEdit::firstPrefixUnderCursor() const
{
    if (m_completer == nullptr)
    {
        return QString();
    }
    QString text = toPlainText();
    int end = textCursor().position() - 1;
    int start = -1;
    for (int i = end; i >= 0; --i)
    {
        if (text[i] == '}')
        {
            break;
        }
        else if (text[i] == '{')
        {
            start = i;
            break;
        }
    }
    return start == -1 ? QString() : text.mid(start, end - start + 1);
}

/* End CompleterEdit Implementation */
/* Begin Constructor/Destructors */

CardBuilder::CardBuilder(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::CardBuilder),
      m_completer(new QCompleter(this))
{
    m_ui->setupUi(this);

    m_completer->setCompletionMode(QCompleter::PopupCompletion);
    m_completer->setCaseSensitivity(Qt::CaseSensitive);
    m_completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);

    connect(
        m_ui->comboBoxDeck, &QComboBox::currentTextChanged,
        this,               &CardBuilder::deckTextChanged
    );
    connect(
        m_ui->comboBoxModel, &QComboBox::currentTextChanged,
        this,                &CardBuilder::modelTextChanged
    );
}

CardBuilder::~CardBuilder()
{
    delete m_ui;
}

/* End Constructor/Destructors */
/* Begin Setters */

void CardBuilder::setSuggestions(const QStringList &words)
{
    m_completer->setModel(new QStringListModel(words, m_completer));
}

void CardBuilder::setDecks(const QStringList &decks, const QString &set)
{
    m_ui->comboBoxDeck->clear();
    m_ui->comboBoxDeck->addItems(decks);
    m_ui->comboBoxDeck->setCurrentText(set);
    m_ui->comboBoxDeck->model()->sort(0);
}

void CardBuilder::setModels(const QStringList &models, const QString &set)
{
    m_ui->comboBoxModel->clear();
    m_ui->comboBoxModel->addItems(models);
    m_ui->comboBoxModel->setCurrentText(set);
    m_ui->comboBoxModel->model()->sort(0);
}

void CardBuilder::setDeckCurrentText(const QString &text)
{
    m_ui->comboBoxDeck->setCurrentText(text);
}

void CardBuilder::setModelCurrentText(const QString &text)
{
    m_ui->comboBoxModel->setCurrentText(text);
}

void CardBuilder::setFields(const QStringList &fields)
{
    m_ui->tableFields->setRowCount(fields.size());
    for (int i = 0; i < fields.size(); ++i)
    {
        QTableWidgetItem *item = new QTableWidgetItem(fields[i]);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        m_ui->tableFields->setItem(i, 0, item);
    }
    for (int i = 0; i < fields.size(); ++i)
    {
        CompleterEdit *edit = new CompleterEdit(m_completer);
        connect(
            edit, &CompleterEdit::blockCountChanged,
            this, [=] { m_ui->tableFields->resizeRowToContents(i); }
        );
        m_ui->tableFields->setCellWidget(i, 1, edit);
    }
    m_ui->tableFields->resizeRowsToContents();
}

void CardBuilder::setFields(const QJsonObject &fields)
{
    QStringList keys = fields.keys();
    setFields(keys);
    for (int i = 0; i < fields.size(); ++i)
    {
        const QJsonValue &val = fields[keys[i]];
        if (!val.isUndefined() && !val.isNull())
        {
            CompleterEdit *edit =
                (CompleterEdit *)m_ui->tableFields->cellWidget(i, 1);
            edit->setPlainText(val.toString());
        }
    }
    m_ui->tableFields->resizeRowsToContents();
}

/* End Setters */
/* Begin Getters */

QString CardBuilder::getDeckText() const
{
    return m_ui->comboBoxDeck->currentText();
}

QString CardBuilder::getModelText() const
{
    return m_ui->comboBoxModel->currentText();
}

QJsonObject CardBuilder::getFields() const
{
    QJsonObject fields;
    for (int i = 0; i < m_ui->tableFields->rowCount(); ++i)
    {
        QTableWidgetItem *field = m_ui->tableFields->item(i, 0);
        CompleterEdit *value =
            (CompleterEdit *)m_ui->tableFields->cellWidget(i, 1);
        fields[field->text()] = value->toPlainText();
    }
    return fields;
}

/* Begin Getters */
