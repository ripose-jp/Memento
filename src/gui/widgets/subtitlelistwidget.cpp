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

#include "../../util/directoryutils.h"
#include "../../util/globalmediator.h"
#include "../../util/constants.h"
#include "../playeradapter.h"

#include <iterator>
#include <QMultiMap>
#include <QApplication>
#include <QSettings>

SubtitleListWidget::SubtitleListWidget(QWidget *parent)
    : QListWidget(parent),
      m_seenSubtitles(new QMultiMap<double, QString>),
      m_subStartTimes(new QMultiHash<QString, double>)
{
    setTheme();

    connect(this, &QListWidget::itemDoubleClicked, this, &SubtitleListWidget::seekToSubtitle);

    GlobalMediator *mediator = GlobalMediator::getGlobalMediator();
    connect(mediator, &GlobalMediator::interfaceSettingsChanged,    this, &SubtitleListWidget::setTheme);
    connect(mediator, &GlobalMediator::playerSubtitleChanged,       this, &SubtitleListWidget::addSubtitle);
    connect(mediator, &GlobalMediator::playerSubtitleTrackChanged,  this, &SubtitleListWidget::clearSubtitles);
    connect(mediator, &GlobalMediator::playerSubtitlesDisabled,     this, &SubtitleListWidget::clearSubtitles);
    connect(mediator, &GlobalMediator::controlsSubtitleListToggled, this, 
        [=] {
            setVisible(!isVisible());
        }
    );
}

SubtitleListWidget::~SubtitleListWidget()
{
    disconnect();
    delete m_seenSubtitles;
    delete m_subStartTimes;
}

void SubtitleListWidget::setTheme()
{
    QSettings settings;
    settings.beginGroup(SETTINGS_INTERFACE);
    if (settings.value(SETTINGS_INTERFACE_STYLESHEETS, SETTINGS_INTERFACE_STYLESHEETS_DEFAULT).toBool())
    {
        setStyleSheet(settings.value(
                SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE,
                SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT
            ).toString()
        );
    }
    else
    {
        setStyleSheet(SETTINGS_INTERFACE_SUBTITLE_LIST_STYLE_DEFAULT);
    }
    settings.endGroup();
}

QString SubtitleListWidget::getContext(const QString &seperator)
{
    QList<QListWidgetItem *> items = selectedItems();
    QString context;
    for (const QListWidgetItem *item : items)
    {
        context += item->text().replace('\n', seperator) + seperator;
    }
    return context;
}

void SubtitleListWidget::addSubtitle(const QString &subtitle, const double start, const double end, const double delay)
{
    size_t i;
    auto it = m_seenSubtitles->constFind(start);
    if (it == m_seenSubtitles->constEnd() || *it != subtitle)
    {
        auto end = m_seenSubtitles->insert(start, subtitle);
        m_subStartTimes->insert(subtitle, start);

        i = std::distance(m_seenSubtitles->begin(), end);
        insertItem(i, subtitle);
    }
    else
    {
        i = std::distance(m_seenSubtitles->constBegin(), it);
    }

    clearSelection();
    setCurrentRow(i);
}

void SubtitleListWidget::clearSubtitles()
{
    clear();
    m_seenSubtitles->clear();
    m_subStartTimes->clear();
}

void SubtitleListWidget::seekToSubtitle(const QListWidgetItem *item)
{
    PlayerAdapter *player = GlobalMediator::getGlobalMediator()->getPlayerAdapter();
    double pos = m_subStartTimes->value(item->text()) + player->getSubDelay();
    player->seek(pos);
}

void SubtitleListWidget::showEvent(QShowEvent *event)
{
    QApplication::processEvents();
    scrollToItem(item(currentRow()));

    Q_EMIT GlobalMediator::getGlobalMediator()->subtitleListShown();
}

void SubtitleListWidget::hideEvent(QHideEvent *event)
{
    const QList<QListWidgetItem *> &items = selectedItems();
    if (!items.isEmpty())
    {
        QApplication::processEvents();
        scrollToItem(items.last());
    }
    Q_EMIT GlobalMediator::getGlobalMediator()->subtitleListHidden();
}