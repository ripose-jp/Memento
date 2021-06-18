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

#ifndef SUBTITLELISTWIDGET_H
#define SUBTITLELISTWIDGET_H

#include <QWidget>

#include <QMultiHash>
#include <QMultiMap>

class QTableWidgetItem;

namespace Ui
{
    class SubtitleListWidget;
}

class SubtitleListWidget : public QWidget
{
    Q_OBJECT

public:
    SubtitleListWidget(QWidget *parent = nullptr);
    ~SubtitleListWidget();

    QString getContext(const QString &seperator);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
    void setTheme();
    void addSubtitle(const QString &subtitle,
                     const double start,
                     const double end,
                     const double delay);
    void updateTimestamps(const double delay);
    void clearSubtitles();
    void seekToSubtitle(QTableWidgetItem *item);

private:
    Ui::SubtitleListWidget *m_ui;

    QMultiMap<double, QString>  m_seenSubtitles;
    QMultiHash<QString, double> m_subStartTimes;

    QMultiMap<double, QString>  m_seenSubtitlesSec;
    QMultiHash<QString, double> m_subStartTimesSec;

    QString formatTimecode(const int time);
};

#endif // SUBTITLELISTWIDGET_H