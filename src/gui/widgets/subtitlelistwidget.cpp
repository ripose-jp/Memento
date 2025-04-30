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

#include "subtitlelistwidget.h"
#include "ui_subtitlelistwidget.h"

#include <iterator>

#include <QApplication>
#include <QClipboard>
#include <QGuiApplication>
#include <QMenu>
#include <QMimeData>
#include <QMultiMap>
#include <QMutexLocker>
#include <QSettings>
#include <QShortcut>
#include <QThreadPool>

#include "util/constants.h"
#include "util/globalmediator.h"
#include "util/iconfactory.h"
#include "util/subtitleparser.h"
#include "util/utils.h"

/* Begin Private Class */

/**
 * A class for managing a temporary file in a QMimeData.
 */
class TempMimeData : public QMimeData
{
public:
    /**
     * Constructs a new QMimeData from the given path.
     * @param path The path copy to clipboard. The file here will be deleted
     *             when this instance is deleted.
     */
    TempMimeData(const QString &path) : QMimeData(), m_path(path)
    {
        setUrls({QUrl::fromLocalFile(path)});
    }

    ~TempMimeData()
    {
        QFile(m_path).remove();
    }

private:
    /* The saved file path */
    const QString m_path;
};

/* End Private Class */
/* Begin Constructor/Destructors */

SubtitleListWidget::SubtitleListWidget(QWidget *parent)
    : QWidget(parent),
      m_ui(new Ui::SubtitleListWidget),
      m_client(GlobalMediator::getGlobalMediator()->getAnkiClient())
{
    m_ui->setupUi(this);
    m_ui->widgetFind->hide();
    m_ui->tabWidget->tabBar()->setDocumentMode(true);
    m_ui->tabWidget->tabBar()->setExpanding(true);

    m_primary.table = m_ui->tablePrim;
    m_secondary.table = m_ui->tableSec;

    m_copyShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_C), this);
    m_copyShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    m_copyAudioShortcut =
        new QShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_C), this);
    m_copyAudioShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    m_findShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
    m_findShortcut->setContext(Qt::WidgetWithChildrenShortcut);

    initTheme();
    initRegex();
    initState();
    hideSecondarySubs();

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();

    /* Signals */
    connect(
        m_ui->tablePrim, &QTableWidget::itemDoubleClicked,
        this,            &SubtitleListWidget::seekToPrimarySubtitle
    );
    connect(
        m_ui->tableSec, &QTableWidget::itemDoubleClicked,
        this,           &SubtitleListWidget::seekToSecondarySubtitle
    );
    connect(
        m_ui->tablePrim, &QTableWidget::customContextMenuRequested,
        this, &SubtitleListWidget::handlePrimaryContextMenu,
        Qt::QueuedConnection
    );
    connect(
        m_ui->tableSec, &QTableWidget::customContextMenuRequested,
        this, &SubtitleListWidget::handleSecondaryContextMenu,
        Qt::QueuedConnection
    );
    connect(
        m_ui->tabWidget, &QTabWidget::currentChanged,
        this,            &SubtitleListWidget::fixTableDimensions,
        Qt::QueuedConnection
    );

    connect(
        mediator, &GlobalMediator::requestThemeRefresh,
        this,     &SubtitleListWidget::initTheme,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::searchSettingsChanged,
        this,     &SubtitleListWidget::initRegex,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSubDelayChanged,
        this,     &SubtitleListWidget::updatePrimaryTimestamps,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSecSubDelayChanged,
        this,     &SubtitleListWidget::updateSecondaryTimestamps,
        Qt::QueuedConnection
    );
    connect(
        this, &SubtitleListWidget::requestRefresh,
        this, &SubtitleListWidget::handleRefresh,
        Qt::QueuedConnection
    );

    connect(
        mediator, &GlobalMediator::playerSubtitleChangedRaw,
        this,     &SubtitleListWidget::updatePrimarySubtitle,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSubtitleTrackChanged,
        this,     &SubtitleListWidget::handlePrimaryTrackChange,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSubtitlesDisabled,
        this,     &SubtitleListWidget::clearPrimarySubtitles,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::searchSettingsChanged,
        this,     &SubtitleListWidget::clearPrimarySubtitles,
        Qt::QueuedConnection
    );

    connect(
        mediator, &GlobalMediator::playerSecSubtitleChanged,
        this,     &SubtitleListWidget::updateSecondarySubtitle,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitleTrackChanged,
        this,     &SubtitleListWidget::handleSecondaryTrackChange,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitlesDisabled,
        this,     &SubtitleListWidget::clearSecondarySubtitles,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitleTrackChanged,
        this,     &SubtitleListWidget::showSecondarySubs,
        Qt::QueuedConnection
    );
    connect(
        mediator, &GlobalMediator::playerSecondSubtitlesDisabled,
        this,     &SubtitleListWidget::hideSecondarySubs,
        Qt::QueuedConnection
    );

    connect(
        mediator, &GlobalMediator::playerFileChanged,
        this,     &SubtitleListWidget::clearCachedSubtitles
    );
    connect(
        mediator, &GlobalMediator::playerTracksChanged,
        this,     &SubtitleListWidget::handleTracklistChange
    );

    connect(
        m_copyShortcut, &QShortcut::activated,
        this, &SubtitleListWidget::copyContext,
        Qt::QueuedConnection
    );
    connect(
        m_copyAudioShortcut, &QShortcut::activated,
        this, &SubtitleListWidget::copyAudioContext,
        Qt::QueuedConnection
    );
    connect(
        m_findShortcut, &QShortcut::activated,
        this, &SubtitleListWidget::findShow,
        Qt::QueuedConnection
    );

    connect(
        m_ui->lineEditSearch, &QLineEdit::textChanged,
        this, &SubtitleListWidget::findText,
        Qt::QueuedConnection
    );
    connect(
        m_ui->checkIgnoreWhitespace,
#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
        &QCheckBox::stateChanged,
#else
        &QCheckBox::checkStateChanged,
#endif
        this, [this] () { findText(m_ui->lineEditSearch->text()); },
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonSearchPrev, &QToolButton::clicked,
        this, &SubtitleListWidget::findPrev,
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonSearchNext, &QToolButton::clicked,
        this, &SubtitleListWidget::findNext,
        Qt::QueuedConnection
    );
    connect(
        m_ui->buttonSearchClose, &QToolButton::clicked,
        m_ui->widgetFind, &QWidget::hide
    );
}

SubtitleListWidget::~SubtitleListWidget()
{
    disconnect();
    clearCachedSubtitles();
    saveState();
    delete m_ui;
}

void SubtitleListWidget::saveState()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::SubtitleList::GROUP);

    settings.setValue(
        Constants::Settings::SubtitleList::IGNORE_WHITESPACE,
        m_ui->checkIgnoreWhitespace->isChecked()
    );
    settings.setValue(
        Constants::Settings::SubtitleList::AUTO_SEEK,
        m_ui->checkAutoSeek->isChecked()
    );

    settings.endGroup();
}

/* End Constructor/Destructors */
/* Begin Initializers */

void SubtitleListWidget::initTheme()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Interface::GROUP);
    const bool customStylesheets = settings.value(
            Constants::Settings::Interface::STYLESHEETS,
            Constants::Settings::Interface::STYLESHEETS_DEFAULT
        ).toBool();
    if (customStylesheets)
    {
        setStyleSheet(settings.value(
                Constants::Settings::Interface::Style::SUBTITLE_LIST,
                Constants::Settings::Interface::Style::SUBTITLE_LIST_DEFAULT
            ).toString()
        );
    }
    else
    {
        setStyleSheet(
            Constants::Settings::Interface::Style::SUBTITLE_LIST_DEFAULT
        );
    }

    const bool showTimestamps = !settings.value(
        Constants::Settings::Interface::Subtitle::LIST_TIMESTAMPS,
        Constants::Settings::Interface::Subtitle::LIST_TIMESTAMPS_DEFAULT
    ).toBool();
    m_ui->tablePrim->setColumnHidden(0, showTimestamps);
    m_ui->tableSec->setColumnHidden (0, showTimestamps);
    settings.endGroup();

    /* Update the FindWidget icons */
    IconFactory *icons = IconFactory::create();
    m_ui->buttonSearchPrev->setIcon(icons->getIcon(IconFactory::Icon::up));
    m_ui->buttonSearchNext->setIcon(icons->getIcon(IconFactory::Icon::down));
    m_ui->buttonSearchClose->setIcon(icons->getIcon(IconFactory::Icon::close));
}

void SubtitleListWidget::initRegex()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::Search::GROUP);
    m_subRegexLock.lock();
    m_subRegex.setPattern(
        settings.value(
            Constants::Settings::Search::REMOVE_REGEX,
            Constants::Settings::Search::REMOVE_REGEX_DEFAULT
        ).toString()
    );
    m_subRegexLock.unlock();
    settings.endGroup();

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    if (player)
    {
        QList<const Track *> tracks = player->getTracks();
        handleTracklistChange(tracks);
        for (const Track *track : tracks)
        {
            delete track;
        }
    }
}

void SubtitleListWidget::initState()
{
    QSettings settings;
    settings.beginGroup(Constants::Settings::SubtitleList::GROUP);

    m_ui->checkIgnoreWhitespace->setChecked(
        settings.value(
            Constants::Settings::SubtitleList::IGNORE_WHITESPACE,
            Constants::Settings::SubtitleList::IGNORE_WHITESPACE_DEFAULT
        ).toBool()
    );
    m_ui->checkAutoSeek->setChecked(
        settings.value(
            Constants::Settings::SubtitleList::AUTO_SEEK,
            Constants::Settings::SubtitleList::AUTO_SEEK_DEFAULT
        ).toBool()
    );

    settings.endGroup();
}

/* End Initializers */
/* Begin Event Handlers */

void SubtitleListWidget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);

    m_ui->tablePrim->scrollToItem(m_ui->tablePrim->currentItem());
    m_ui->tableSec->scrollToItem(m_ui->tableSec->currentItem());
    m_ui->tablePrim->resizeRowsToContents();
    m_ui->tableSec->resizeRowsToContents();

    Q_EMIT widgetShown();
}

void SubtitleListWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);

    QList<QTableWidgetItem *> items = m_ui->tablePrim->selectedItems();
    if (!items.isEmpty())
    {
        m_ui->tablePrim->scrollToItem(items.last());
    }

    items = m_ui->tableSec->selectedItems();
    if (!items.isEmpty())
    {
        m_ui->tableSec->scrollToItem(items.last());
    }

    Q_EMIT widgetHidden();
}

void SubtitleListWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_ui->tablePrim->resizeRowsToContents();
    m_ui->tableSec->resizeRowsToContents();
}

/* End Event Handlers */
/* Begin Adder Methods */

void SubtitleListWidget::handleTracklistChange(
    const QList<const Track *> &tracks)
{
    int64_t primarySid = -1;
    int64_t secondarySid = -1;
    QStringList extTracks;
    QList<int64_t> extSids;
    for (const Track *track : tracks)
    {
        if (track->type == Track::subtitle)
        {
            if (track->selected)
            {
                if (track->mainSelection == 0)
                {
                    primarySid = track->id;
                }
                else
                {
                    secondarySid = track->id;
                }
            }

            if (track->external)
            {
                extTracks << track->externalFilename;
                extSids << track->id;
            }
        }
    }

    QThreadPool::globalInstance()->start([this, extTracks, extSids] {
        SubtitleParser parser;

        m_primary.lock.lock();
        m_secondary.lock.lock();

        for (int i = 0; i < extTracks.size(); ++i)
        {
            if (!m_subtitleMap.contains(extSids[i]))
            {
                m_subtitleMap[extSids[i]] = std::make_shared<
                    std::vector<std::shared_ptr<SubtitleInfo>>>();
                m_subtitleParsed[extSids[i]] = std::make_shared<bool>(false);
            }
            QList<SubtitleInfo> subtitles = parser.parseSubtitles(extTracks[i]);
            std::transform(
                std::begin(subtitles), std::end(subtitles),
                std::back_inserter(*m_subtitleMap[extSids[i]]),
                [] (SubtitleInfo &info)
                {
                    return std::make_shared<SubtitleInfo>(info);
                }
            );
            *m_subtitleParsed[extSids[i]] =
                !m_subtitleMap[extSids[i]]->empty();
        }

        m_secondary.lock.unlock();
        m_primary.lock.unlock();

        Q_EMIT requestRefresh();
    });

    if (primarySid != -1)
    {
        handlePrimaryTrackChange(primarySid);
    }
    if (secondarySid != -1)
    {
        handleSecondaryTrackChange(secondarySid);
    }
}

void SubtitleListWidget::handleRefresh()
{
    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    int64_t sid = player->getSubtitleTrack();
    if (sid > 0)
    {
        handlePrimaryTrackChange(sid);
    }

    sid = player->getSecondarySubtitleTrack();
    if (sid > 0)
    {
        handleSecondaryTrackChange(sid);
    }
}

QString SubtitleListWidget::formatTimecode(const int time)
{
    const int SECONDS_IN_HOUR = 3600;
    const int SECONDS_IN_MINUTE = 60;

    const int hours   = time / SECONDS_IN_HOUR;
    const int minutes = (time % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
    const int seconds = time % SECONDS_IN_MINUTE;

    QString timeStr("%1:%2:%3");
    return timeStr.arg(hours,   2, 10, QLatin1Char('0'))
                  .arg(minutes, 2, 10, QLatin1Char('0'))
                  .arg(seconds, 2, 10, QLatin1Char('0'));
}

QTableWidgetItem *SubtitleListWidget::addTableItem(
    SubtitleList &list,
    const std::shared_ptr<SubtitleInfo> &info,
    double delay,
    bool regex)
{
    QTableWidgetItem *subtitleItem;
    if (regex)
    {
        QString text = QString(info->text).remove(m_subRegex);
        if (text.isEmpty())
        {
            return nullptr;
        }
        subtitleItem = new QTableWidgetItem(text);
    }
    else
    {
        subtitleItem = new QTableWidgetItem(info->text);
    }
    list.itemToSub.insert(subtitleItem, info);
    QStringList lines = info->text.split('\n');
    for (const QString &line : lines)
    {
        list.lineToItem.insert(line, subtitleItem);
    }
    auto endIt = list.startToItem.insert(info->start, subtitleItem);

    /* This is a bit of a hack to get the subtitles to appear in order.
     * If a subtitle starts at the same start time as another, the subtitle
     * that appears later will be closer to seenSubs.begin() than the
     * earlier subtitle. To get around this, we assume the subtitle being
     * added came after all subtitles with the same start time. This isn't
     * going to work 100% of the time, but it's right enough of the time to
     * be worth it.
     */
    size_t i = std::distance(list.startToItem.begin(), endIt);
    for (++endIt;
         endIt != list.startToItem.end() && endIt.key() == info->start;
         ++endIt)
    {
        ++i;
    }

    QTableWidgetItem *timecodeItem =
        new QTableWidgetItem(formatTimecode(info->start + delay));
    timecodeItem->setFlags(Qt::NoItemFlags);

    list.table->insertRow(i);
    list.table->setItem(i, 1, subtitleItem);
    list.table->setItem(i, 0, timecodeItem);
    list.table->resizeRowToContents(i);
    list.modified = true;

    return subtitleItem;
}

void SubtitleListWidget::addSubtitle(
    SubtitleList &list,
    const QString &subtitle,
    const double start,
    const double end,
    const double delay,
    const bool regex)
{
    constexpr double TIME_DELTA = 0.0001;

    /* Check if we have already seen this subtitle. Finds it if we have. */
    auto it = list.startToItem.lowerBound(start - TIME_DELTA);
    while (it != list.startToItem.end() && it.key() - start <= TIME_DELTA)
    {
        if ((*it)->text() == subtitle)
        {
            break;
        }
        ++it;
    }

    QTableWidgetItem *subtitleItem = nullptr;
    if (it == list.startToItem.end() || it.key() - start > TIME_DELTA)
    {
        std::shared_ptr<SubtitleInfo> info = std::make_shared<SubtitleInfo>();
        info->text = subtitle;
        info->start = start;
        info->end = end;
        list.subList->emplace_back(std::move(info));

        if (regex)
        {
            m_subRegexLock.lock();
        }
        subtitleItem = addTableItem(list, list.subList->back(), delay, regex);
        if (regex)
        {
            m_subRegexLock.unlock();
        }
    }
    else
    {
        subtitleItem = *it;
    }

    list.table->clearSelection();
    list.table->setCurrentItem(subtitleItem);
}

template<int SUBTITLE_INDEX>
void SubtitleListWidget::handleTrackChange(SubtitleList &list, int64_t sid)
{
    list.lock.lock();

    clearSubtitles(list);

    if (!m_subtitleMap.contains(sid))
    {
        m_subtitleMap[sid] =
            std::make_shared<std::vector<std::shared_ptr<SubtitleInfo>>>();
        m_subtitleParsed[sid] = std::make_shared<bool>(false);
    }
    list.subList = m_subtitleMap[sid];
    list.subsParsed = m_subtitleParsed[sid];

    double delay = 0;
    if constexpr (SUBTITLE_INDEX == 0)
    {
        delay = GlobalMediator::getGlobalMediator()
            ->getPlayerAdapter()
            ->getSubDelay();
    }
    else if constexpr (SUBTITLE_INDEX == 1)
    {
        delay = GlobalMediator::getGlobalMediator()
            ->getPlayerAdapter()
            ->getSecondarySubDelay();
    }
    m_subRegexLock.lock();
    for (const auto &info : *list.subList)
    {
        addTableItem(list, info, delay, true);
    }
    m_subRegexLock.unlock();

    list.lock.unlock();
}

void SubtitleListWidget::handlePrimaryTrackChange(int64_t sid)
{
    handleTrackChange<0>(m_primary, sid);
}

void SubtitleListWidget::handleSecondaryTrackChange(int64_t sid)
{
    handleTrackChange<1>(m_secondary, sid);
}

void SubtitleListWidget::selectSubtitles(SubtitleList &list,
                                         const QString &subtitle,
                                         double delay)
{
    constexpr double TIME_DELTA = 0.001;

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    double time = player->getTimePos() - delay;

    list.table->clearSelection();

    QList<int> rows;
    QStringList lines = subtitle.split('\n');
    for (const QString &line : lines)
    {
        QList<QTableWidgetItem *> items = list.lineToItem.values(line);
        for (QTableWidgetItem *item : items)
        {
            const auto &info = list.itemToSub[item];
            if (info->start <= time + TIME_DELTA &&
                info->end >= time - TIME_DELTA)
            {
                list.table->scrollToItem(item);
                rows << list.table->row(item);
            }
        }
    }
    std::sort(rows.begin(), rows.end());
    if (!rows.isEmpty())
    {
        list.table->setCurrentCell(rows.first(), 1);
        for (int i = 1; i < rows.size(); ++i)
        {
            list.table->setCurrentCell(rows[i], 1, QItemSelectionModel::Select);
        }
    }
}

void SubtitleListWidget::updatePrimarySubtitle(const QString &subtitle,
                                               double start,
                                               double end,
                                               double delay)
{
    if (!m_primary.lock.try_lock())
    {
        return;
    }
    if (*m_primary.subsParsed)
    {
        selectSubtitles(m_primary, subtitle, delay);
    }
    else
    {
        addSubtitle(m_primary, subtitle, start, end, delay, true);
    }
    m_primary.lock.unlock();
}

void SubtitleListWidget::updateSecondarySubtitle(const QString &subtitle,
                                                 double start,
                                                 double end,
                                                 double delay)
{
    if (!m_secondary.lock.try_lock())
    {
        return;
    }
    if (*m_secondary.subsParsed)
    {
        selectSubtitles(m_secondary, subtitle, delay);
    }
    else
    {
        addSubtitle(m_secondary, subtitle, start, end, delay, false);
    }
    m_secondary.lock.unlock();
}

void SubtitleListWidget::updateTimestampsHelper(SubtitleList &list,
                                                double delay)
{
    size_t i = 0;
    for (auto it = list.startToItem.keyBegin();
         it != list.startToItem.keyEnd();
         ++it)
    {
        const double time = *it + delay < 0 ? 0 : *it + delay;
        QTableWidgetItem *timecodeItem =
            new QTableWidgetItem(formatTimecode(time));
        timecodeItem->setFlags(Qt::NoItemFlags);
        list.table->setItem(i, 0, timecodeItem);

        i++;
    }
    list.table->resizeRowsToContents();
}

void SubtitleListWidget::updatePrimaryTimestamps(const double delay)
{
    updateTimestampsHelper(m_primary, delay);
}

void SubtitleListWidget::updateSecondaryTimestamps(const double delay)
{
    updateTimestampsHelper(m_secondary, delay);
}

/* End Adder Methods */
/* Begin Context Methods */

QString SubtitleListWidget::getContext(const SubtitleList *list,
                                       const QString      &separator) const
{
    QList<QTableWidgetItem *> items = list->table->selectedItems();
    std::sort(items.begin(), items.end(),
        [=] (QTableWidgetItem *lhs, QTableWidgetItem *rhs)
        {
            return list->table->row(lhs) < list->table->row(rhs);
        }
    );
    QString context;
    for (const QTableWidgetItem *item : items)
    {
        context += item->text().replace('\n', separator) + separator;
    }
    return context;
}

QString SubtitleListWidget::getPrimaryContext(const QString &separator) const
{
    return getContext(&m_primary, separator);
}

QString SubtitleListWidget::getSecondaryContext(const QString &separator) const
{
    return getContext(&m_secondary, separator);
}

inline QPair<double, double> SubtitleListWidget::getContextTime(
    const SubtitleList &list) const
{
    double start = 0.0;
    double end = 0.0;

    QList<QTableWidgetItem *> items = list.table->selectedItems();
    if (!items.isEmpty())
    {
        start = list.itemToSub[items.first()]->start;
        end = list.itemToSub[items.last()]->end;
    }
    for (QTableWidgetItem *item : items)
    {
        const auto &info = list.itemToSub[item];
        start = start < info->start ? start : info->start;
        end = end > info->end ? end : info->end;
    }

    return QPair<double, double>(start, end);
}

QPair<double, double> SubtitleListWidget::getPrimaryContextTime() const
{
    return getContextTime(m_primary);
}

/* End Context Methods */
/* Begin Seek Methods */

template<int SUBTITLE_INDEX>
void SubtitleListWidget::seekToSubtitle(QTableWidgetItem *item,
                                        const SubtitleList &list) const
{
    constexpr double SEEK_ERROR = 0.028;

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();

    double delay = 0;
    if constexpr (SUBTITLE_INDEX == 0)
    {
        delay = player->getSubDelay();
    }
    else if constexpr (SUBTITLE_INDEX == 1)
    {
        delay = player->getSecondarySubDelay();
    }

    double pos =
        list.itemToSub[item]->start +
        delay +
        SEEK_ERROR;
    if (pos < 0)
    {
        pos = 0;
    }
    player->seek(pos);
}

void SubtitleListWidget::seekToPrimarySubtitle(QTableWidgetItem *item) const
{
    seekToSubtitle<0>(item, m_primary);
}

void SubtitleListWidget::seekToSecondarySubtitle(QTableWidgetItem *item) const
{
    seekToSubtitle<1>(item, m_secondary);
}

/* End Seek Methods */
/* Begin Context Menu Methods */

template<int SUBTITLE_INDEX>
void SubtitleListWidget::handleContextMenu(
    SubtitleList &list, const QPoint &pos)
{
    QTableWidgetItem *item = list.table->itemAt(pos);
    if (item == nullptr || item->column() != 1)
    {
        return;
    }

    QMenu contextMenu(this);

    QAction actionCopyText("Copy Text", this);
    actionCopyText.setShortcut(m_copyShortcut->key());
    connect(
        &actionCopyText, &QAction::triggered,
        this, &SubtitleListWidget::copyContext
    );
    contextMenu.addAction(&actionCopyText);

    QAction actionCopyAudio("Copy Audio", this);
    actionCopyAudio.setShortcut(m_copyAudioShortcut->key());
    connect(
        &actionCopyAudio, &QAction::triggered,
        this, &SubtitleListWidget::copyAudioContext
    );
    contextMenu.addAction(&actionCopyAudio);

    QAction actionAlign("Align Delay", this);
    connect(
        &actionAlign, &QAction::triggered, this,
        [&list, item] () -> void
        {
            PlayerAdapter *player =
                GlobalMediator::getGlobalMediator()->getPlayerAdapter();
            double currentPos = player->getTimePos();
            double currentStart = list.itemToSub[item]->start;
            double newDelay = currentPos - currentStart;
            if constexpr (SUBTITLE_INDEX == 0)
            {
                player->setSubDelay(newDelay);
            }
            else if constexpr (SUBTITLE_INDEX == 1)
            {
                player->setSecondarySubDelay(newDelay);
            }
        }
    );
    contextMenu.addAction(&actionAlign);

    contextMenu.exec(mapToGlobal(pos));
}

void SubtitleListWidget::handlePrimaryContextMenu(const QPoint &pos)
{
    handleContextMenu<0>(m_primary, pos);
}

void SubtitleListWidget::handleSecondaryContextMenu(const QPoint &pos)
{
    handleContextMenu<1>(m_secondary, pos);
}

/* End Context Menu Methods */
/* Begin Shortcut Handlers */

void SubtitleListWidget::copyContext() const
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (isPrimaryCurrent())
    {
        clipboard->setText(getPrimaryContext("\n"));
    }
    else
    {
        clipboard->setText(getSecondaryContext("\n"));
    }
}

void SubtitleListWidget::copyAudioContext() const
{
    QPair<double, double> times = getContextTime(
        isPrimaryCurrent() ? m_primary : m_secondary
    );
    if (times.first >= times.second)
    {
        return;
    }

    PlayerAdapter *player =
        GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    double delay = isPrimaryCurrent() ?
        player->getSubDelay() : player->getSecondarySubDelay();
    delay -= player->getAudioDelay();
    double start = times.first + delay;
    double end = times.second + delay;
    bool normalize = false;
    double dbLevel = -20.0;

    if (m_client->isEnabled())
    {
        QSharedPointer<const AnkiConfig> config = m_client->getConfig();
        start -= config->audioPadStart;
        end += config->audioPadEnd;
        normalize = config->audioNormalize;
        dbLevel = config->audioDb;
    }

    QThreadPool::globalInstance()->start(
        [=] {
            QString path = player->tempAudioClip(start, end, normalize, dbLevel);
            if (!QFile(path).exists())
            {
                return;
            }
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setMimeData(new TempMimeData(path));
        }
    );
}

/* End Shortcuts Handlers */
/* Begin Clear Methods */

void SubtitleListWidget::clearSubtitles(SubtitleList &list)
{
    list.table->clearContents();
    list.table->setRowCount(0);
    list.subList = nullptr;
    list.subsParsed = nullptr;
    list.startToItem.clear();
    list.itemToSub.clear();
    list.lineToItem.clear();
    list.modified = true;
    list.foundRows.clear();
    list.currentFind = 0;
}

void SubtitleListWidget::clearPrimarySubtitles()
{
    m_primary.lock.lock();
    clearSubtitles(m_primary);
    m_primary.lock.unlock();
}

void SubtitleListWidget::clearSecondarySubtitles()
{
    m_secondary.lock.lock();
    clearSubtitles(m_secondary);
    m_secondary.lock.unlock();
}

void SubtitleListWidget::clearCachedSubtitles()
{
    clearPrimarySubtitles();
    clearSecondarySubtitles();
    m_subtitleMap.clear();
    m_subtitleParsed.clear();
}

/* End Clear Methods */
/* Begin Helper Slots */

void SubtitleListWidget::hideSecondarySubs()
{
    m_ui->tabWidget->setTabVisible(1, false);
    m_ui->tabWidget->tabBar()->hide();
}

void SubtitleListWidget::showSecondarySubs()
{
    if (GlobalMediator::getGlobalMediator()
        ->getPlayerAdapter()->canGetSecondarySubText())
    {
        m_ui->tabWidget->setTabVisible(1, true);
        m_ui->tabWidget->tabBar()->show();
    }
}

void SubtitleListWidget::fixTableDimensions(const int index)
{
    switch(index)
    {
    case 0:
        m_ui->tablePrim->resizeRowsToContents();
        break;
    case 1:
        m_ui->tableSec->resizeRowsToContents();
        break;
    }
}

/* End Helper Slots */
/* Begin Find Widget Slots */

void SubtitleListWidget::findShow()
{
    m_ui->widgetFind->show();
    m_ui->lineEditSearch->setFocus();
    findText(m_ui->lineEditSearch->text());
}

#define MATCH_NONE      "No Matches"
#define MATCH_FORMAT    QString("%1 of %2 Rows")

template<int SUBTITLE_INDEX>
void SubtitleListWidget::findTextHelper(SubtitleList &list, QString text)
{
    const QRegularExpression REGEX_REMOVE_WHITESPACE("\\s*");

    list.modified = false;
    list.foundRows.clear();
    list.currentFind = 0;

    if (m_ui->checkIgnoreWhitespace->isChecked())
    {
        text = text.replace(REGEX_REMOVE_WHITESPACE, "");
    }

    if (text.isEmpty())
    {
        m_ui->labelSearchMatch->setText(MATCH_NONE);
        return;
    }

    for (int i = 0; i < list.table->rowCount(); ++i)
    {
        QTableWidgetItem *item = list.table->item(i, 1);
        QString subtitleText = item->text();
        if (m_ui->checkIgnoreWhitespace->isChecked())
        {
            subtitleText = subtitleText.replace(REGEX_REMOVE_WHITESPACE, "");
        }
        if (subtitleText.contains(text, Qt::CaseInsensitive))
        {
            list.foundRows << i;
        }
    }

    if (list.foundRows.isEmpty())
    {
        m_ui->labelSearchMatch->setText(MATCH_NONE);
    }
    else
    {
        list.currentFind = 0;
        list.table->setCurrentCell(list.foundRows[0], 1);
        m_ui->labelSearchMatch->setText(
            MATCH_FORMAT.arg(1).arg(list.foundRows.size())
        );
        if (m_ui->checkAutoSeek->isChecked())
        {
            seekToSubtitle<SUBTITLE_INDEX>(
                list.table->item(list.foundRows[list.currentFind], 1),
                list
            );
        }
    }
}

void SubtitleListWidget::findText(const QString &text)
{
    SubtitleList &list = isPrimaryCurrent() ? m_primary : m_secondary;
    list.lock.lock();
    if (isPrimaryCurrent())
    {
        findTextHelper<0>(list, text);
    }
    else
    {
        findTextHelper<1>(list, text);
    }
    list.lock.unlock();
}

/**
 * A sanitary macro for doing mod. This is necessary because % in C++ can return
 * negative numbers.
 * @param n       The number to mod.
 * @param modulus The modulus of the operation.
 * @return n mod modulus
 */
static constexpr int mod(int n, int modulus)
{
    return (n % modulus + modulus) % modulus;
}

template<int SUBTITLE_INDEX>
void SubtitleListWidget::findRowHelper(int offset)
{
    SubtitleList &list = isPrimaryCurrent() ? m_primary : m_secondary;

    QMutexLocker locker(&list.lock);

    if (list.modified)
    {
        findTextHelper<SUBTITLE_INDEX>(list, m_ui->lineEditSearch->text());
    }

    if (list.foundRows.isEmpty())
    {
        return;
    }

    list.currentFind = mod(list.currentFind + offset, list.foundRows.size());
    list.table->setCurrentCell(list.foundRows[list.currentFind], 1);
    m_ui->labelSearchMatch->setText(
        MATCH_FORMAT.arg(list.currentFind + 1).arg(list.foundRows.size())
    );
    if (m_ui->checkAutoSeek->isChecked())
    {
        seekToSubtitle<SUBTITLE_INDEX>(
            list.table->item(list.foundRows[list.currentFind], 1),
            list
        );
    }
}

#undef MATCH_NONE
#undef MATCH_FORMAT

void SubtitleListWidget::findPrev()
{
    if (isPrimaryCurrent())
    {
        findRowHelper<0>(-1);
    }
    else
    {
        findRowHelper<1>(-1);
    }
}

void SubtitleListWidget::findNext()
{
    if (isPrimaryCurrent())
    {
        findRowHelper<0>(1);
    }
    else
    {
        findRowHelper<1>(1);
    }
}

bool SubtitleListWidget::isPrimaryCurrent() const
{
    return m_ui->tabWidget->currentWidget() == m_ui->tabPrimary;
}

/* End Find Widget Slots */
