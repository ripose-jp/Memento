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

#include <QHash>
#include <QMultiHash>
#include <QMap>

class QTableWidget;
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

    QString getPrimaryContext  (const QString &seperator) const;
    QString getSecondaryContext(const QString &seperator) const;

protected:
    void showEvent  (QShowEvent *event)   override;
    void hideEvent  (QHideEvent *event)   override;
    void resizeEvent(QResizeEvent *event) override;

private Q_SLOTS:
    void setTheme();
    void addPrimarySubtitle  (const QString &subtitle,
                              const double   start,
                              const double   end,
                              const double   delay);
    void addSecondarySubtitle(const QString &subtitle,
                                    double   start,
                              const double   delay);

    void updateTimestamps(const double delay);

    void clearPrimarySubtitles();
    void clearSecondarySubtitles();

    void seekToPrimarySubtitle  (QTableWidgetItem *item) const;
    void seekToSecondarySubtitle(QTableWidgetItem *item) const;

private:
    Ui::SubtitleListWidget *m_ui;

    QMap<double, QTableWidgetItem *>  m_seenPrimarySubs;
    QHash<QTableWidgetItem *, double> m_timesPrimarySubs;

    QMap<double, QTableWidgetItem *>  m_seenSecondarySubs;
    QHash<QTableWidgetItem *, double> m_timesSecondarySubs;
    QMultiHash<QString, double>       m_subSecondaryToTime;

    void hideSecondarySubs();
    void showSecondarySubs();

    QString formatTimecode(const int time);

    QString getContext(const QTableWidget *table, 
                       const QString      &seperator) const;
    
    void addSubtitle(QTableWidget                      *table,
                     QMap<double, QTableWidgetItem *>  &seenSubs,
                     QHash<QTableWidgetItem *, double> &startTimes,
                     const QString                     &subtitle,
                     const double                       start,
                     const double                       delay);

    void updateTimestampsHelper(QTableWidget                           *table,
                                const QMap<double, QTableWidgetItem *> &seenSubs,
                                const double                            delay);

    void clearSubtitles(QTableWidget                      *table,
                        QMap<double, QTableWidgetItem *>  &seenSubs,
                        QHash<QTableWidgetItem *, double> &startTimes);

    void seekToSubtitle(QTableWidgetItem                        *item,
                        const QHash<QTableWidgetItem *, double> &startTimes) const;
};

#endif // SUBTITLELISTWIDGET_H