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

#include "termwidget.h"
#include "ui_termwidget.h"

#include <QClipboard>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "audio/audioplayer.h"
#include "dict/dictionary.h"
#include "gui/widgets/definition/definitionwidget.h"
#include "gui/widgets/definition/glossarywidget.h"
#include "gui/widgets/definition/pitchwidget.h"
#include "gui/widgets/definition/tagwidget.h"
#include "gui/widgets/subtitlelistwidget.h"
#include "player/playeradapter.h"
#include "util/constants.h"
#include "util/iconfactory.h"
#include "util/utils.h"

using AudioSource = DefinitionState::AudioSource;

/**
 * Kanji stylesheet format string.
 * @param 1 The color of links. Should be the same as text color.
 */
static const QString KANJI_STYLE_STRING(\
    "<style>"\
        "a {"\
            "color: %1;"\
            "border: 0;"\
            "text-decoration: none;"\
        "}"\
    "</style>"\
);

/**
 * Kanji link format string. Used for making kanji clickable.
 * @param 1 A single kanji such that when clicked, the kanji will be emitted in
 *          in the linkActivated signal.
 */
static const QString KANJI_FORMAT_STRING("<a href=\"%1\">%1</a>");

/* Used for replace expressions with text in URLs */
static constexpr const char *REPLACE_EXPRESSION = "{expression}";

/* Used for replacing reading with text in URLs */
static constexpr const char *REPLACE_READING = "{reading}";

#if defined(Q_OS_MACOS)
static const QString EXPRESSION_STYLE("QLabel { font-size: 30pt; }");
static const QString READING_STYLE("QLabel { font-size: 18pt; }");
static const QString CONJUGATION_STYLE("QLabel { font-size: 18pt; }");
#else
static const QString EXPRESSION_STYLE("QLabel { font-size: 20pt; }");
static const QString READING_STYLE("QLabel { font-size: 12pt; }");
static const QString CONJUGATION_STYLE("QLabel { font-size: 12pt; }");
#endif

/* Begin Constructor/Destructor */

TermWidget::TermWidget(
    Context *context,
    std::shared_ptr<const Term> term,
    const DefinitionState &state,
    QWidget *parent) :
    QWidget(parent),
    m_ui(std::make_unique<Ui::TermWidget>()),
    m_context(std::move(context)),
    m_term(std::move(term)),
    m_state(state),
    m_sources(state.sources),
    m_jsonSources(state.jsonSourceCount)
{
    m_ui->setupUi(this);

    setFocusPolicy(Qt::ClickFocus);

    m_menuAdd = new QMenu("Play Audio Source", m_ui->buttonAddCard);
    m_menuAudio = new QMenu("Add Audio Source", m_ui->buttonAudio);

    m_ui->labelKanji->setStyleSheet(EXPRESSION_STYLE);
    m_ui->labelKana->setStyleSheet(READING_STYLE);

    m_layoutTermTags = new FlowLayout(-1, 6);
    m_layoutFreqTags = new FlowLayout(-1, 6);
    m_layoutPitches  = new QVBoxLayout;
    m_layoutGlossary = new QVBoxLayout;

    m_shortcutAddCard = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_D), this);
    m_shortcutAddCard->setContext(Qt::WidgetWithChildrenShortcut);
    m_shortcutAddCard->setEnabled(false);

    m_ui->layoutGlossaryContainer->addLayout(m_layoutTermTags);
    m_ui->layoutGlossaryContainer->addLayout(m_layoutFreqTags);
    m_ui->layoutGlossaryContainer->addLayout(m_layoutPitches);
    m_ui->layoutGlossaryContainer->addLayout(m_layoutGlossary);

    m_ui->layoutGlossaryContainer->addStretch();

    IconFactory *factory = IconFactory::create();

    m_ui->buttonCollapse->setIcon(factory->getIcon(IconFactory::Icon::down));

    m_ui->buttonAddCard->setIcon(factory->getIcon(IconFactory::Icon::plus));
    m_ui->buttonAddCard->setVisible(false);

    m_ui->buttonKanaKanji->setIcon(factory->getIcon(IconFactory::Icon::kanji));
    m_ui->buttonKanaKanji->setVisible(false);

    m_ui->buttonAnkiOpen->setIcon(
        factory->getIcon(IconFactory::Icon::hamburger)
    );
    m_ui->buttonAnkiOpen->setVisible(false);

    m_ui->buttonAudio->setIcon(factory->getIcon(IconFactory::Icon::audio));
    m_ui->buttonAudio->setVisible(!m_sources.empty());

    initUi(*m_term);

    connect(
        m_ui->buttonCollapse, &QToolButton::clicked,
        this, &TermWidget::toggleGlossaryVisibility
    );
    connect(
        m_ui->labelKanji, &QLabel::linkActivated,
        this, &TermWidget::searchKanji
    );
    connect(
        m_ui->buttonAddCard, &QToolButton::clicked,
        this, QOverload<>::of(&TermWidget::addNote)
    );
    connect(
        m_shortcutAddCard, &QShortcut::activated,
        this, QOverload<>::of(&TermWidget::addNote)
    );
    connect(
        m_ui->buttonAddCard, &QToolButton::customContextMenuRequested,
        this, &TermWidget::showAddableAudioSources
    );
    connect(
        m_ui->buttonKanaKanji, &QToolButton::clicked,
        this, &TermWidget::toggleExpressionKanji
    );
    connect(
        m_ui->buttonAnkiOpen, &QToolButton::clicked,
        this, &TermWidget::searchAnki
    );
    connect(
        m_ui->buttonAudio, &QToolButton::customContextMenuRequested,
        this, &TermWidget::showPlayableAudioSources
    );
    connect(
        m_ui->buttonAudio, &QToolButton::clicked,
        this, QOverload<>::of(&TermWidget::playAudio)
    );
}

TermWidget::~TermWidget()
{

}

void TermWidget::deleteWhenReady()
{
    hide();
    setParent(nullptr);
    if (!m_safeToDelete)
    {
        connect(this, &TermWidget::safeToDelete, this, &QObject::deleteLater);
    }
    else
    {
        deleteLater();
    }
}

/* End Constructor/Destructor */
/* Begin Initializers */

void TermWidget::initUi(const Term &term)
{
    if (term.reading.isEmpty())
    {
        m_ui->labelKana->hide();
    }
    else
    {
        m_ui->labelKana->show();
    }
    m_ui->labelKana->setText(term.reading);
    QString kanjiLabelText = KANJI_STYLE_STRING.arg(
            m_ui->labelKanji->palette().color(
                m_ui->labelKanji->foregroundRole()
            ).name()
        );
    for (const QChar &ch : term.expression)
    {
        kanjiLabelText += CharacterUtils::isKanji(ch) ?
            KANJI_FORMAT_STRING.arg(ch) : ch;
    }
    m_ui->labelKanji->setText(kanjiLabelText);

    if (!term.conjugationExplanation.isEmpty())
    {
        m_labelDeconj = new QLabel;
        m_labelDeconj->setStyleSheet(CONJUGATION_STYLE);
        m_labelDeconj->setWordWrap(true);
        m_labelDeconj->setTextInteractionFlags(Qt::TextSelectableByMouse);
        if (term.reading.isEmpty() || term.conjugationExplanation.size() > 100)
        {
            m_ui->layoutTermWidget->insertWidget(1, m_labelDeconj);
        }
        else
        {
            m_ui->layoutButtonsDeconj->addWidget(m_labelDeconj);
        }
        m_labelDeconj->setText(term.conjugationExplanation);
    }

    for (const Frequency &freq : term.frequencies)
    {
        TagWidget *tag = new TagWidget(freq);
        m_layoutFreqTags->addWidget(tag);
    }

    for (const Pitch &pitch : term.pitches)
    {
        m_layoutPitches->addWidget(new PitchWidget(pitch));
    }

    for (const Tag &termTag : term.tags)
    {
        TagWidget *tag = new TagWidget(termTag);
        m_layoutTermTags->addWidget(tag);
    }

    std::shared_ptr<const AnkiConfig> config =
        m_context->getAnkiClient()->getConfig();
    for (int i = 0; i < term.definitions.size(); ++i)
    {
        GlossaryWidget *g =
            new GlossaryWidget(m_context, m_state, term.definitions[i], i + 1);
        g->setChecked(
            !config->excludeGloss.contains(term.definitions[i].dictionary)
        );
        m_layoutGlossary->addWidget(g);

        connect(
            g, &GlossaryWidget::contentSearched,
            this, &TermWidget::contentSearched
        );
    }
}

std::unique_ptr<Term> TermWidget::initAnkiTerm() const
{
    std::unique_ptr<Term> term = std::make_unique<Term>(*m_term);
    term->definitions.clear();
    for (int i = 0; i < m_layoutGlossary->count(); ++i)
    {
        GlossaryWidget *widget =
            (GlossaryWidget *)m_layoutGlossary->itemAt(i)->widget();
        if (widget->isChecked())
        {
            term->definitions.emplace_back(
                TermDefinition(m_term->definitions[i])
            );
        }
        widget->setCheckable(false);

        if (widget->hasSelection())
        {
            term->selection.emplace_back(widget->selection().trimmed());
        }
    }

    PlayerAdapter *player = m_context->getPlayerAdapter();
    SubtitleListWidget *subList = m_context->getSubtitleListWidget();

    double delay = player->getSubDelay() - player->getAudioDelay();
    term->clipboard = QGuiApplication::clipboard()->text();
    term->title = player->getTitle();
    term->sentence = player->getSubtitle(true);
    term->sentence2 = player->getSecondarySubtitle();
    term->startTime = player->getSubStart() + delay;
    term->endTime = player->getSubEnd() + delay;
    term->context = subList->getPrimaryContext("\n");
    term->context2 = subList->getSecondaryContext("\n");
    QPair<double, double> contextTimes = subList->getPrimaryContextTime();
    term->startTimeContext = contextTimes.first + delay;
    term->endTimeContext = contextTimes.second + delay;

    return term;
}

/* End Initializers */
/* Begin User Input Handlers */

void TermWidget::toggleGlossaryVisibility()
{
    IconFactory *icons = IconFactory::create();
    if (m_ui->glossaryContainer->isVisible())
    {
        m_ui->buttonCollapse->setIcon(icons->getIcon(IconFactory::Icon::up));
        m_ui->glossaryContainer->hide();
    }
    else
    {
        m_ui->buttonCollapse->setIcon(icons->getIcon(IconFactory::Icon::down));
        m_ui->glossaryContainer->show();
    }
}

void TermWidget::toggleExpressionKanji()
{
    m_readingAsExp = !m_readingAsExp;
    IconFactory *icons = IconFactory::create();
    m_ui->buttonKanaKanji->setIcon(icons->getIcon(
        m_readingAsExp ? IconFactory::Icon::kana : IconFactory::Icon::kanji
    ));
}

void TermWidget::addNote()
{
    m_ui->buttonAddCard->setEnabled(false);
    m_shortcutAddCard->setEnabled(false);
    m_ui->buttonKanaKanji->setEnabled(false);
    if (m_ankiTerm == nullptr)
    {
        m_safeToDelete = false;
        m_ankiTerm = initAnkiTerm();
    }

    m_lockJsonSources.lock();
    if (m_jsonSources)
    {
        m_lockJsonSources.unlock();
        loadAudioSources();
        connect(
            this, &TermWidget::audioSourcesLoaded,
            this, QOverload<>::of(&TermWidget::addNote)
        );
        return;
    }
    m_lockJsonSources.unlock();

    const AudioSource *src = getFirstAudioSource();
    addNote(src ? *src : AudioSource());
}

QCoro::Task<void> TermWidget::addNote(const AudioSource &src)
{
    m_ui->buttonAddCard->setEnabled(false);
    m_shortcutAddCard->setEnabled(false);
    m_ui->buttonKanaKanji->setEnabled(false);
    if (m_ankiTerm == nullptr)
    {
        m_safeToDelete = false;
        m_ankiTerm = initAnkiTerm();
    }
    m_ankiTerm->readingAsExpression = m_readingAsExp;
    m_ankiTerm->audioSrcName = src.name;
    m_ankiTerm->audioURL = src.url;
    m_ankiTerm->audioSkipHash = src.md5;

    AnkiReply<int> addNoteResult =
        co_await m_context->getAnkiClient()->addNote(std::move(m_ankiTerm));
    if (!addNoteResult.error.isEmpty())
    {
        emit m_context->showCritical("Error Adding Note", addNoteResult.error);
        co_return;
    }
    m_ui->buttonAnkiOpen->show();
    m_ui->buttonAddCard->hide();
    m_ui->buttonKanaKanji->hide();

    m_safeToDelete = true;
    emit safeToDelete();
}

QCoro::Task<void> TermWidget::searchAnki()
{
    AnkiReply<QList<int>> openBrowseResult =
        co_await m_context->getAnkiClient()->openDuplicates(initAnkiTerm());
    if (!openBrowseResult.error.isEmpty())
    {
        emit m_context->showCritical(
            "Error Opening Anki", openBrowseResult.error
        );
    }
}

void TermWidget::playAudio()
{
    m_ui->buttonAudio->setEnabled(false);

    if (m_sources.empty())
    {
        m_ui->buttonAudio->hide();
        return;
    }

    m_lockJsonSources.lock();
    if (m_jsonSources)
    {
        m_lockJsonSources.unlock();
        loadAudioSources();
        connect(
            this, &TermWidget::audioSourcesLoaded,
            this, QOverload<>::of(&TermWidget::playAudio)
        );
        return;
    }
    m_lockJsonSources.unlock();

    const AudioSource *src = getFirstAudioSource();
    if (src)
    {
        playAudio(*src);
    }
}

void TermWidget::playAudio(const AudioSource &src)
{
    m_ui->buttonAudio->setEnabled(false);
    AudioPlayerReply *reply = m_context->getAudioPlayer()->playAudio(
        QString(src.url)
            .replace(REPLACE_EXPRESSION, m_term->expression)
            .replace(
                REPLACE_READING,
                m_term->reading.isEmpty() ?
                    m_term->expression : m_term->reading
            ),
        src.md5
    );
    m_ui->buttonAudio->setEnabled(reply == nullptr);

    if (reply)
    {
        connect(reply, &AudioPlayerReply::result, this,
            [this] (const bool success)
            {
                if (!success)
                {
                    IconFactory *factory = IconFactory::create();
                    m_ui->buttonAudio->setIcon(
                        factory->getIcon(IconFactory::noaudio)
                    );
                }
                m_ui->buttonAudio->setEnabled(true);
            }
        );
    }
}

void TermWidget::showPlayableAudioSources(const QPoint &pos)
{
    m_lockJsonSources.lock();
    if (m_jsonSources)
    {
        m_menuAudio->clear();
        m_menuAudio->addAction("Loading...");
        loadAudioSources();
    }
    else if (m_menuAudio->actions().isEmpty())
    {
        populateAudioSourceMenu(
            m_menuAudio, [this] (const AudioSource &src) { playAudio(src); }
        );
    }
    m_lockJsonSources.unlock();
    m_menuAudio->exec(m_ui->buttonAudio->mapToGlobal(pos));
}

void TermWidget::showAddableAudioSources(const QPoint &pos)
{
    m_lockJsonSources.lock();
    if (m_jsonSources)
    {
        m_menuAdd->clear();
        m_menuAdd->addAction("Loading...");
        loadAudioSources();
    }
    else if (m_menuAdd->actions().isEmpty())
    {
        populateAudioSourceMenu(
            m_menuAdd, [this] (const AudioSource &src) { return addNote(src); }
        );
    }
    m_lockJsonSources.unlock();
    m_menuAdd->exec(m_ui->buttonAddCard->mapToGlobal(pos));
}

void TermWidget::searchKanji(const QString &ch)
{
    SharedKanji kanji = m_context->getDictionary()->searchKanji(ch);
    if (kanji)
    {
        kanji->title       = m_term->title;
        kanji->sentence    = m_term->sentence;
        kanji->startTime   = m_term->startTime;
        kanji->endTime     = m_term->endTime;
        kanji->sentence2   = m_term->sentence2;
        kanji->clozePrefix = m_term->clozePrefix;
        kanji->clozeBody   = m_term->clozeBody;
        kanji->clozeSuffix = m_term->clozeSuffix;
        emit kanjiSearched(kanji);
    }
}

/* End User Input Handlers */
/* Begin Setters */

void TermWidget::setAddable(bool expression, bool reading)
{
    m_ui->buttonAddCard->setVisible(expression || reading);
    m_shortcutAddCard->setEnabled(expression || reading);
    m_ui->buttonAnkiOpen->setVisible(
        !expression || (!reading && !m_term->reading.isEmpty())
    );
    m_ui->buttonKanaKanji->setVisible(
        (expression || reading) && !m_term->reading.isEmpty()
    );
    m_ui->buttonKanaKanji->setEnabled(expression && reading);
    if (!expression && reading && !m_term->reading.isEmpty())
    {
        toggleExpressionKanji();
    }
    for (int i = 0; i < m_layoutGlossary->count(); ++i)
    {
        GlossaryWidget *widget =
            (GlossaryWidget *)m_layoutGlossary->itemAt(i)->widget();
        widget->setCheckable(expression || reading);
    }
}

/* End Setters */
/* Begin Helpers */

#define JSON_KEY_TYPE               "type"
#define JSON_VALUE_TYPE             "audioSourceList"
#define JSON_KEY_AUDIO_SOURCES      "audioSources"
#define JSON_KEY_AUDIO_SOURCES_NAME "name"
#define JSON_KEY_AUDIO_SOURCES_URL  "url"

/**
 * Processes raw JSON audio sources into a list of audio sources.
 * @param      data The raw json.
 * @param[out] src  The audio source these audio sources belong to.
 */
static inline void processAudioSourceJson(
    const QByteArray &data, AudioSource &src)
{
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
    {
        return;
    }
    QJsonObject obj = doc.object();
    if (obj[JSON_KEY_TYPE].toString() != JSON_VALUE_TYPE)
    {
        return;
    }
    if (!obj[JSON_KEY_AUDIO_SOURCES].isArray())
    {
        return;
    }
    for (const QJsonValue &val : obj[JSON_KEY_AUDIO_SOURCES].toArray())
    {
        if (!val.isObject())
        {
            continue;
        }
        QJsonObject srcObj = val.toObject();
        if (!srcObj[JSON_KEY_AUDIO_SOURCES_NAME].isString() ||
            !srcObj[JSON_KEY_AUDIO_SOURCES_URL].isString())
        {
            continue;
        }

        AudioSource childSrc;
        childSrc.type = Constants::AudioSourceType::File;
        childSrc.name = srcObj[JSON_KEY_AUDIO_SOURCES_NAME].toString();
        childSrc.url = srcObj[JSON_KEY_AUDIO_SOURCES_URL].toString();
        childSrc.md5 = src.md5;
        src.audioSources.emplace_back(childSrc);
    }
}

#undef JSON_KEY_TYPE
#undef JSON_VALUE_TYPE
#undef JSON_KEY_AUDIO_SOURCES
#undef JSON_KEY_AUDIO_SOURCES_NAME
#undef JSON_KEY_AUDIO_SOURCES_URL

#define TRANSFER_TIMEOUT 5000

void TermWidget::loadAudioSources()
{
    if (!m_lockSources.tryLock())
    {
        return;
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    manager->setTransferTimeout(TRANSFER_TIMEOUT);
    for (size_t i = 0; i < m_sources.size(); ++i)
    {
        const AudioSource &src = m_sources[i];
        if (src.type != Constants::AudioSourceType::JSON)
        {
            continue;
        }

        QString url = src.url;
        url.replace(REPLACE_EXPRESSION, m_term->expression)
           .replace(
               REPLACE_READING,
               m_term->reading.isEmpty() ? m_term->expression : m_term->reading
            );
        QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url)));
        connect(reply, &QNetworkReply::finished, this,
            [this, reply, manager, i]
            {
                if (reply->error() == QNetworkReply::NoError)
                {
                    processAudioSourceJson(reply->readAll(), m_sources[i]);
                }

                m_lockJsonSources.lock();
                if (--m_jsonSources == 0)
                {
                    m_lockJsonSources.unlock();
                    populateAudioSourceMenu(
                        m_menuAdd,
                        [this] (const AudioSource &src) { return addNote(src); }
                    );
                    populateAudioSourceMenu(
                        m_menuAudio,
                        [this] (const AudioSource &src) { playAudio(src); }
                    );
                    m_lockSources.unlock();
                    manager->deleteLater();

                    emit audioSourcesLoaded();
                }
                else
                {
                    m_lockJsonSources.unlock();
                }
            }
        );
        connect(reply, &QNetworkReply::finished, reply, &QObject::deleteLater);
    }
}

#undef TRANSFER_TIMEOUT

void TermWidget::populateAudioSourceMenu(
    QMenu *menu,
    std::function<void(const AudioSource &)> handler)
{
    menu->clear();
    for (const AudioSource &src : m_sources)
    {
        if (src.type == Constants::AudioSourceType::File)
        {
            menu->addAction(src.name, this, [=] { handler(src); });
        }
        else if (src.type == Constants::AudioSourceType::JSON)
        {
            for (const AudioSource &childSrc : src.audioSources)
            {
                menu->addAction(
                    childSrc.name, this, [=] { handler(childSrc); }
                );
            }
        }
    }
}

AudioSource *TermWidget::getFirstAudioSource()
{
    AudioSource *ret = nullptr;
    for (AudioSource &src : m_sources)
    {
        if (src.type == Constants::AudioSourceType::File)
        {
            ret = &src;
            break;
        }
        else if (
            src.type == Constants::AudioSourceType::JSON &&
            !src.audioSources.empty()
        )
        {
            ret = &src.audioSources.front();
            break;
        }
    }
    return ret;
}

/* End Helpers */
