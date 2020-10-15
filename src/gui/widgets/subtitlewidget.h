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

#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include "../../dict/jmdict.h"

#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QRunnable>
#include <QTimer>

class SubtitleWidget : public QLineEdit
{
    Q_OBJECT

public:
    SubtitleWidget(QWidget *parent = 0);
    ~SubtitleWidget();

public Q_SLOTS:
    void updateText(const QString &text);
    void deselectText();

Q_SIGNALS:
    void entriesChanged(const QList<const Entry *> *entry);

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void findEntry();

private:
    JMDict *m_dictionary;
    int m_currentIndex;
    QTimer *m_findDelay;

    class QueryThread : public QRunnable
    {
    public:
        QueryThread(SubtitleWidget *sw, const QString &query, 
                    const QString &currentSubtitle, 
                    const int currentIndex) : m_parent(sw), 
                                              m_query(query), 
                                              m_currentSubtitle(currentSubtitle),
                                              m_currentIndex(currentIndex) {}
        void run();

    private:
        SubtitleWidget *m_parent;
        QString m_query;
        QString m_currentSubtitle;
        const int m_currentIndex;

        void deleteEntries(QList<const Entry *> *entries);
    };
};

#endif // SUBTITLEWIDGET_H