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

#include "definitionwidget.h"
#include "ui_definitionwidget.h"

#include "../../util/globalmediator.h"
#include "../../util/constants.h"

#include <QFrame>
#include <QScrollBar>
#include <QSettings>
#include <QPushButton>

DefinitionWidget::DefinitionWidget(const QList<Term *> *terms, QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::DefinitionWidget),
      m_client(GlobalMediator::getGlobalMediator()->getAnkiClient()),
      m_terms(terms)
{
    m_ui->setupUi(this);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Build the UI */
    setAutoFillBackground(true);
    setTheme();

    /* Get the term limit */
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);
    size_t limit = settings.value(SETTINGS_SEARCH_LIMIT, DEFAULT_LIMIT).toUInt();
    settings.endGroup();

    /* Add the terms */
    showTerms(0, limit);
    m_ui->scrollArea->verticalScrollBar()->setValue(0);

    QPushButton *buttonShowMore = nullptr;
    /* Add the show more button */
    if (limit < m_terms->size())
    {
        buttonShowMore = new QPushButton;
        buttonShowMore->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        buttonShowMore->setText("Show More");
        buttonShowMore->setEnabled(!m_client->isEnabled());
        m_ui->scrollAreaContents->layout()->addWidget(buttonShowMore);
        connect(buttonShowMore, &QPushButton::clicked, this,
            [=] {
                QLayoutItem *showMoreItem = 
                    m_ui->scrollAreaContents->layout()->takeAt(
                        m_ui->scrollAreaContents->layout()->count() - 1
                    );

                /* Add the terms */
                int start = m_termWidgets.size();
                int end   = start + limit;
                showTerms(start, end);
                setAddable(start, end);

                if (end < m_terms->size())
                {
                    m_ui->scrollAreaContents->layout()->addItem(showMoreItem);
                }
                else
                {
                    QLayoutItem *lineItem = m_ui->scrollAreaContents->layout()->takeAt(
                        m_ui->scrollAreaContents->layout()->count() - 1
                    );
                    delete lineItem->widget();
                    delete lineItem;
                    delete showMoreItem->widget();
                    delete showMoreItem;
                }
            }
        );
    }
    else
    {
        /* Delete the extra line at the end */
        QLayoutItem *item = m_ui->scrollAreaContents->layout()->takeAt(
            m_ui->scrollAreaContents->layout()->count() - 1
        );
        delete item->widget();
        delete item;
    }
    
    /* Check if entries are addable to anki */
    if (m_client->isEnabled())
    {
        AnkiReply *reply = m_client->notesAddable(*terms);
        connect(reply, &AnkiReply::finishedBoolList, this, 
            [=] (const QList<bool> &addable, const QString &error) {
                if (error.isEmpty())
                {
                    m_addable = addable;
                    setAddable(0, limit);
                }
                if (buttonShowMore)
                {
                    buttonShowMore->setEnabled(true);
                }
            }
        );
    }
}

DefinitionWidget::~DefinitionWidget()
{
    for (const Term *term : *m_terms)
    {
        delete term;
    }
    delete m_terms;
    delete m_ui;
}

void DefinitionWidget::setTheme()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    if (settings.value(SETTINGS_INTERFACE_STYLESHEETS, SETTINGS_INTERFACE_STYLESHEETS_DEFAULT).toBool())
    {
        setStyleSheet(
            settings.value(
                SETTINGS_INTERFACE_DEFINITION_STYLE,
                SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT
            ).toString()
        );
    }
    else
    {
        setStyleSheet(SETTINGS_INTERFACE_DEFINITION_STYLE_DEFAULT);
    }
    settings.endGroup();
}

void DefinitionWidget::setAddable(const size_t start, const size_t end)
{
    for (size_t i = start; i < m_addable.size() && i < m_terms->size() && i < end; ++i)
        m_termWidgets[i]->setAddable(m_addable[i]);
}

void DefinitionWidget::showTerms(const size_t start, const size_t end)
{
    setUpdatesEnabled(false);
    for (size_t i = start; i < m_terms->size() && i < end; ++i)
    {
        TermWidget *termWidget = new TermWidget(m_terms->at(i), m_client, this);
        connect(termWidget, &TermWidget::kanjiSearched, this, &DefinitionWidget::showKanji);
        m_termWidgets.append(termWidget);
        m_ui->scrollAreaContents->layout()->addWidget(termWidget);

        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
        m_ui->scrollAreaContents->layout()->addWidget(line);        
    }
    setUpdatesEnabled(true);
}

void DefinitionWidget::showKanji(const Kanji *kanji)
{
    m_savedScroll = m_ui->scrollArea->verticalScrollBar()->value();
    for (size_t i = 0; i < m_ui->scrollAreaContents->layout()->count(); ++i)
    {
        m_ui->scrollAreaContents->layout()->itemAt(i)->widget()->hide();
    }
    KanjiWidget *kanjiWidget = new KanjiWidget(kanji);
    connect(kanjiWidget, &KanjiWidget::backPressed, this, &DefinitionWidget::hideKanji);
    m_ui->scrollAreaContents->layout()->addWidget(kanjiWidget);
    m_ui->scrollArea->verticalScrollBar()->setValue(0);
}

void DefinitionWidget::hideKanji()
{
    QLayout *scrollLayout = m_ui->scrollAreaContents->layout();
    QLayoutItem *kanjiItem = scrollLayout->takeAt(scrollLayout->count() - 1);
    delete kanjiItem->widget();
    delete kanjiItem;

    for (size_t i = 0; i < scrollLayout->count(); ++i)
    {
        scrollLayout->itemAt(i)->widget()->show();
    }
    QApplication::processEvents();
    m_ui->scrollArea->verticalScrollBar()->setValue(m_savedScroll);
}


void DefinitionWidget::hideEvent(QHideEvent *event)
{
    Q_EMIT GlobalMediator::getGlobalMediator()->definitionsHidden();
}

void DefinitionWidget::showEvent(QShowEvent *event)
{
    Q_EMIT GlobalMediator::getGlobalMediator()->definitionsShown();
}