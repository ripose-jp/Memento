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

#include <QFrame>
#include <QPushButton>
#include <QScrollBar>
#include <QSettings>

#include "../../../audio/audioplayer.h"
#include "../../../util/constants.h"
#include "../../../util/globalmediator.h"

/* Begin Constructor/Destructor */

DefinitionWidget::DefinitionWidget(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::DefinitionWidget),
      m_client(GlobalMediator::getGlobalMediator()->getAnkiClient())
{
    m_ui->setupUi(this);

    /* Build the UI */
    setCursor(Qt::ArrowCursor);
    setAutoFillBackground(true);

    /* Init Settings */
    initTheme();
    initSearch();
    initAudioSources();
    initSignals();
}

DefinitionWidget::~DefinitionWidget()
{
    clearTerms();
    disconnect();
    delete m_ui;
}

void DefinitionWidget::clearTerms()
{
    ++m_searchId;
    m_savedScroll = 0;
    m_addable.clear();
    m_termWidgets.clear();
    m_terms.clear();
    m_kanji = nullptr;
    QLayoutItem *item;
    while ((item = m_ui->layoutScroll->takeAt(0)) != nullptr)
    {
        item->widget()->deleteLater();
        delete item;
    }
    GlobalMediator::getGlobalMediator()->getAudioPlayer()->clearFiles();
}

/* Begin Constructor/Destructor */
/* Begin Initializers */

void DefinitionWidget::initTheme()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    const bool stylesheetsEnabled = settings.value(
            SETTINGS_INTERFACE_STYLESHEETS,
            SETTINGS_INTERFACE_STYLESHEETS_DEFAULT
        ).toBool();
    if (stylesheetsEnabled)
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

void DefinitionWidget::initSearch()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_SEARCH);
    m_limit = settings.value(
            SETTINGS_SEARCH_LIMIT, DEFAULT_LIMIT
        ).toUInt();
    m_listGlossary = settings.value(
            SETTINGS_SEARCH_LIST_GLOSSARY,
            DEFAULT_LIST_GLOSSARY
        ).toBool();
    settings.endGroup();
}

void DefinitionWidget::initAudioSources()
{
    QSettings settings;
    m_sources.clear();
    m_jsonSources = 0;
    size_t size = settings.beginReadArray(SETTINGS_AUDIO_SRC);
    for (size_t i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        AudioSource src;
        src.type = (AudioSourceType)settings.value(
                SETTINGS_AUDIO_SRC_TYPE,
                (int)SETTINGS_AUDIO_SRC_TYPE_DEFAULT
            ).toInt();
        src.name = settings.value(
                SETTINGS_AUDIO_SRC_NAME, SETTINGS_AUDIO_SRC_NAME_DEFAULT
            ).toString();
        src.url = settings.value(
                SETTINGS_AUDIO_SRC_URL, SETTINGS_AUDIO_SRC_URL_DEFAULT
            ).toString();
        src.md5 = settings.value(
                SETTINGS_AUDIO_SRC_MD5, SETTINGS_AUDIO_SRC_MD5_DEFAULT
            ).toString();
        if (src.type == AudioSourceType::JSON)
        {
            ++m_jsonSources;
        }
        m_sources.append(src);
    }
    settings.endArray();
}

void DefinitionWidget::initSignals()
{
    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    connect(
        mediator, &GlobalMediator::interfaceSettingsChanged,
        this, &DefinitionWidget::initTheme,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::searchSettingsChanged,
        this, &DefinitionWidget::initSearch,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::audioSourceSettingsChanged,
        this, &DefinitionWidget::initAudioSources,
        Qt::QueuedConnection
    );
}

void DefinitionWidget::setTerms(const QList<Term *> *terms, const Kanji *kanji)
{
    clearTerms();

    /* Save the terms in shared pointers */
    if (terms)
    {
        for (Term *term : *terms)
        {
            m_terms << std::shared_ptr<const Term>(term);
        }
        delete terms;
    }

    /* Save kanji in a shared pointer */
    if (kanji)
    {
        m_kanji = std::shared_ptr<const Kanji>(kanji);
    }

    /* Early exit if there is nothing to show */
    if (m_terms.isEmpty() && m_kanji == nullptr)
    {
        return;
    }

    /* Add the terms */
    showTerms(0, m_limit);
    m_ui->scrollArea->verticalScrollBar()->setValue(0);

    QPushButton *buttonShowMore = nullptr;
    /* Add the show more button */
    if (m_limit < m_terms.size() + (m_kanji ? 1 : 0))
    {
        buttonShowMore = new QPushButton;
        buttonShowMore->setSizePolicy(
            QSizePolicy::Preferred, QSizePolicy::Fixed
        );
        buttonShowMore->setText("Show More");
        buttonShowMore->setEnabled(!m_client->isEnabled());
        m_ui->scrollAreaContents->layout()->addWidget(buttonShowMore);
        connect(
            buttonShowMore, &QPushButton::clicked,
            this, &DefinitionWidget::showMoreTerms
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

    /* Check if entries are addable to Anki */
    if (m_client->isEnabled())
    {
        AnkiReply *reply = m_client->notesAddable(m_terms);
        int searchId = m_searchId;
        connect(reply, &AnkiReply::finishedBoolList, this,
            [=] (const QList<bool> &addable, const QString &error) {
                if (error.isEmpty() && searchId == m_searchId)
                {
                    m_addable = addable;
                    setAddable(0, m_limit);
                }
            }
        );
        if (buttonShowMore)
        {
            connect(
                reply, &AnkiReply::finishedBoolList,
                buttonShowMore, [=] { buttonShowMore->setEnabled(true); }
            );
        }
    }

    Q_EMIT widgetShown();
}

/* End Initializers */
/* Begin Event Handlers */

void DefinitionWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    Q_EMIT widgetHidden();
}

void DefinitionWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    Q_EMIT widgetShown();
}

void DefinitionWidget::showMoreTerms()
{
    QLayoutItem *showMoreItem =
        m_ui->scrollAreaContents->layout()->takeAt(
            m_ui->scrollAreaContents->layout()->count() - 1
        );

    /* Add the terms */
    int start = m_termWidgets.size();
    int end   = start + m_limit;
    showTerms(start, end);
    setAddable(start, end);

    if (end < m_terms.size() + (m_kanji ? 1 : 0))
    {
        m_ui->scrollAreaContents->layout()->addItem(showMoreItem);
    }
    else
    {
        QLayoutItem *lineItem =
            m_ui->scrollAreaContents->layout()->takeAt(
                m_ui->scrollAreaContents->layout()->count() - 1
            );
        delete lineItem->widget();
        delete lineItem;
        delete showMoreItem->widget();
        delete showMoreItem;
    }
}

/* End Event Handlers */
/* Begin Term Helpers */

void DefinitionWidget::showTerms(const size_t start, const size_t end)
{
    setUpdatesEnabled(false);
    size_t i;
    for (i = start; i < m_terms.size() && i < end; ++i)
    {
        TermWidget *termWidget = new TermWidget(
              m_terms[i], m_sources, m_jsonSources, m_listGlossary
        );
        connect(
            termWidget, &TermWidget::kanjiSearched,
            this,       &DefinitionWidget::showKanji
        );
        m_termWidgets.append(termWidget);
        m_ui->scrollAreaContents->layout()->addWidget(termWidget);

        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
        m_ui->scrollAreaContents->layout()->addWidget(line);
    }
    if (i < end && m_kanji != nullptr)
    {
        KanjiWidget *kanjiWidget = new KanjiWidget(m_kanji);
        m_ui->scrollAreaContents->layout()->addWidget(kanjiWidget);
        /* An extra line is expected here */
        m_ui->scrollAreaContents->layout()->addWidget(new QFrame);
    }
    setUpdatesEnabled(true);
}

void DefinitionWidget::setAddable(const size_t start, const size_t end)
{
    for (size_t i = start;
         i < m_addable.size() && i < m_terms.size() && i < end;
         ++i)
    {
        m_termWidgets[i]->setAddable(m_addable[i]);
    }
}

/* End Term Helpers */
/* Begin Kanji Helpers */

void DefinitionWidget::showKanji(std::shared_ptr<const Kanji> kanji)
{
    m_savedScroll = m_ui->scrollArea->verticalScrollBar()->value();
    for (size_t i = 0; i < m_ui->scrollAreaContents->layout()->count(); ++i)
    {
        m_ui->scrollAreaContents->layout()->itemAt(i)->widget()->hide();
    }
    KanjiWidget *kanjiWidget = new KanjiWidget(kanji, true);
    connect(
        kanjiWidget, &KanjiWidget::backPressed,
        this,        &DefinitionWidget::hideKanji
    );
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

/* End Kanji Helpers */
