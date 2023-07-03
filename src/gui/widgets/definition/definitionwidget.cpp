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
#include <QGraphicsDropShadowEffect>
#include <QPushButton>
#include <QScrollBar>
#include <QSettings>

#include "audio/audioplayer.h"
#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/iconfactory.h"

/* Begin Constructor/Destructor */

DefinitionWidget::DefinitionWidget(bool showNavigation, QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::DefinitionWidget),
      m_client(GlobalMediator::getGlobalMediator()->getAnkiClient())
{
    m_ui->setupUi(this);
    m_ui->widgetNav->setVisible(showNavigation);

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
    if (m_child)
    {
        m_child->deleteLater();
        m_child = nullptr;
    }
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
    if (m_child)
    {
        m_child->deleteLater();
        m_child = nullptr;
    }
    QLayoutItem *item;
    while ((item = m_ui->layoutScroll->takeAt(0)) != nullptr)
    {
        TermWidget *term = qobject_cast<TermWidget *>(item->widget());
        if (term)
        {
            term->deleteWhenReady();
        }
        else
        {
            delete item->widget();
        }
        delete item;
    }
    GlobalMediator::getGlobalMediator()->getAudioPlayer()->clearFiles();
}

/* Begin Constructor/Destructor */
/* Begin Initializers */

void DefinitionWidget::initTheme()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);
    const bool stylesheetsEnabled = settings.value(
            Constants::Settings::Interface::STYLESHEETS,
            Constants::Settings::Interface::STYLESHEETS_DEFAULT
        ).toBool();
    if (stylesheetsEnabled)
    {
        setStyleSheet(
            settings.value(
                Constants::Settings::Interface::Style::DEFINITION,
                Constants::Settings::Interface::Style::DEFINITION_DEFAULT
            ).toString()
        );
    }
    else
    {
        setStyleSheet(
            Constants::Settings::Interface::Style::DEFINITION_DEFAULT
        );
    }
    settings.endGroup();

    IconFactory *icons = IconFactory::create();
    m_ui->buttonClose->setIcon(icons->getIcon(IconFactory::Icon::close));
}

void DefinitionWidget::initSearch()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Search::GROUP);
    m_state.resultLimit = settings.value(
        Constants::Settings::Search::LIMIT,
        Constants::Settings::Search::LIMIT_DEFAULT
    ).toInt();
    m_state.glossaryStyle = static_cast<Constants::GlossaryStyle>(
        settings.value(
            Constants::Settings::Search::LIST_GLOSSARY,
            static_cast<int>(Constants::Settings::Search::LIST_GLOSSARY_DEFAULT)
        ).toInt()
    );
    m_state.autoPlayAudio = settings.value(
        Constants::Settings::Search::AUTO_PLAY_AUDIO,
        Constants::Settings::Search::AUTO_PLAY_AUDIO_DEFAULT
    ).toBool();

    QString modifier = settings.value(
        Constants::Settings::Search::MODIFIER,
        Constants::Settings::Search::MODIFIER_DEFAULT
    ).toString();
    if (modifier == Constants::Settings::Search::Modifier::SHIFT)
    {
        m_state.searchModifier = Qt::KeyboardModifier::ShiftModifier;
    }
    else if (modifier == Constants::Settings::Search::Modifier::ALT)
    {
        m_state.searchModifier = Qt::KeyboardModifier::AltModifier;
    }
    else if (modifier == Constants::Settings::Search::Modifier::CTRL)
    {
        m_state.searchModifier = Qt::KeyboardModifier::ControlModifier;
    }
    else if (modifier == Constants::Settings::Search::Modifier::SUPER)
    {
        m_state.searchModifier = Qt::KeyboardModifier::MetaModifier;
    }
    else
    {
        m_state.searchModifier = Qt::KeyboardModifier::ShiftModifier;
    }
    settings.endGroup();
}

void DefinitionWidget::initAudioSources()
{
    QSettings settings;
    m_state.sources.clear();
    int size = settings.beginReadArray(Constants::Settings::AudioSource::GROUP);
    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        DefinitionState::AudioSource src;
        src.type = static_cast<Constants::AudioSourceType>(settings.value(
            Constants::Settings::AudioSource::TYPE,
            static_cast<int>(Constants::Settings::AudioSource::TYPE_DEFAULT)
        ).toInt());
        src.name = settings.value(
            Constants::Settings::AudioSource::NAME,
            Constants::Settings::AudioSource::NAME_DEFAULT
        ).toString();
        src.url = settings.value(
            Constants::Settings::AudioSource::URL,
            Constants::Settings::AudioSource::URL_DEFAULT
        ).toString();
        src.md5 = settings.value(
            Constants::Settings::AudioSource::MD5,
            Constants::Settings::AudioSource::MD5_DEFAULT
        ).toString();
        m_state.sources.emplace_back(src);
    }
    settings.endArray();

    m_state.jsonSourceCount = std::count_if(
        std::begin(m_state.sources), std::end(m_state.sources),
        [] (const DefinitionState::AudioSource &src) -> bool
        {
            return src.type == Constants::AudioSourceType::JSON;
        }
    );
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
    connect(
        m_ui->buttonClose, &QToolButton::clicked,
        this, &DefinitionWidget::hide,
        Qt::QueuedConnection
    );
}

void DefinitionWidget::setTerms(SharedTermList terms, SharedKanji kanji)
{
    clearTerms();

    /* Save the terms in shared pointers */
    if (terms)
    {
        for (SharedTerm term : *terms)
        {
            m_terms << QSharedPointer<const Term>(term);
        }
    }

    /* Save kanji in a shared pointer */
    if (kanji)
    {
        m_kanji = QSharedPointer<const Kanji>(kanji);
    }

    /* Early exit if there is nothing to show */
    if (m_terms.isEmpty() && m_kanji == nullptr)
    {
        return;
    }

    /* Add the terms */
    showTerms(0, m_state.resultLimit);
    m_ui->scrollArea->verticalScrollBar()->setValue(0);
    if (m_state.autoPlayAudio && !m_termWidgets.empty())
    {
        m_termWidgets.front()->playAudio();
    }

    QPushButton *buttonShowMore = nullptr;
    /* Add the show more button */
    if (m_state.resultLimit < m_terms.size() + (m_kanji ? 1 : 0))
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
    m_ui->layoutScroll->addStretch();

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
                    setAddable(0, m_state.resultLimit);
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
    if (m_child)
    {
        m_child->deleteLater();
        m_child = nullptr;
    }
}

void DefinitionWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    Q_EMIT widgetShown();
}

void DefinitionWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_child)
    {
        m_child->deleteLater();
        m_child = nullptr;
    }
}

void DefinitionWidget::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    event->accept();
    if (m_child)
    {
        m_child->deleteLater();
        m_child = nullptr;
    }
}

void DefinitionWidget::showMoreTerms()
{
    QLayoutItem *spacer = m_ui->scrollAreaContents->layout()->takeAt(
        m_ui->scrollAreaContents->layout()->count() - 1
    );
    QLayoutItem *showMoreItem = m_ui->scrollAreaContents->layout()->takeAt(
        m_ui->scrollAreaContents->layout()->count() - 1
    );

    /* Add the terms */
    int start = m_termWidgets.size();
    int end   = start + m_state.resultLimit;
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
    m_ui->layoutScroll->addItem(spacer);
}

/* End Event Handlers */
/* Begin Term Helpers */

void DefinitionWidget::showTerms(const int start, const int end)
{
    setUpdatesEnabled(false);
    int i;
    for (i = start; i < m_terms.size() && i < end; ++i)
    {
        TermWidget *termWidget = new TermWidget(m_terms[i], m_state);
        connect(
            termWidget, &TermWidget::kanjiSearched,
            this, &DefinitionWidget::showKanji
        );
        connect(
            termWidget, &TermWidget::contentSearched,
            this, &DefinitionWidget::showChild
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
        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
        m_ui->scrollAreaContents->layout()->addWidget(line);
    }
    setUpdatesEnabled(true);
}

void DefinitionWidget::setAddable(const int start, const int end)
{
    for (int i = start;
         i * 2 + 1 < m_addable.size() && i < m_terms.size() && i < end;
         ++i)
    {
        m_termWidgets[i]->setAddable(m_addable[i * 2], m_addable[i * 2 + 1]);
    }
}

/* End Term Helpers */
/* Begin Kanji Helpers */

void DefinitionWidget::showKanji(QSharedPointer<const Kanji> kanji)
{
    m_savedScroll = m_ui->scrollArea->verticalScrollBar()->value();
    for (int i = 0; i < m_ui->scrollAreaContents->layout()->count(); ++i)
    {
        QWidget *widget =
            m_ui->scrollAreaContents->layout()->itemAt(i)->widget();
        if (widget)
        {
            widget->hide();
        }
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

    for (int i = 0; i < scrollLayout->count(); ++i)
    {
        QWidget *widget = scrollLayout->itemAt(i)->widget();
        if (widget)
        {
            widget->show();
        }
    }
    QApplication::processEvents();
    m_ui->scrollArea->verticalScrollBar()->setValue(m_savedScroll);
}

/* End Kanji Helpers */
/* Begin Child Handlers */

#define SHADOW_BLUR_RADIUS 100.0
#define CHILD_HEIGHT        400

void DefinitionWidget::showChild(SharedTermList terms, SharedKanji kanji)
{
    if (terms == nullptr && kanji == nullptr)
    {
        if (m_child)
        {
            m_child->deleteLater();
            m_child = nullptr;
        }
        return;
    }
    else if (m_child == nullptr)
    {
        m_child = new DefinitionWidget(true, parentWidget());
    }

    QGraphicsDropShadowEffect *ge = new QGraphicsDropShadowEffect;
    ge->setBlurRadius(SHADOW_BLUR_RADIUS);
    ge->setOffset(0);
    m_child->setGraphicsEffect(ge);

    m_child->setTerms(terms, kanji);
    m_child->raise();
    m_child->resize(size().width(), CHILD_HEIGHT);
    if (!positionChild())
    {
        m_child->deleteLater();
        m_child = nullptr;
        return;
    }
    m_child->show();
}

#undef SHADOW_BLUR_RADIUS
#undef CHILD_HEIGHT

bool DefinitionWidget::positionChild()
{
    return positionChild(QCursor::pos());
}

#define VERTICAL_OFFSET 22

bool DefinitionWidget::positionChild(const QPoint &pos)
{
    const QPoint mousePos = parentWidget()->mapFromGlobal(pos);

    int x = mousePos.x() - (m_child->width() / 2);
    if (x < 0)
    {
        x = 0;
    }
    else if (x > parentWidget()->width() - m_child->width())
    {
        x = parentWidget()->width() - m_child->width();
    }

    int y = mousePos.y() - m_child->height() - VERTICAL_OFFSET;
    if (y < 0)
    {
        y = mousePos.y() + VERTICAL_OFFSET;
    }

    if (y + m_child->height() > parentWidget()->height())
    {
        return false;
    }
    m_child->move(x, y);

    return true;
}

#undef VERTICAL_OFFSET

/* End Child Handlers */
