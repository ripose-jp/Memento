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

#include "../../dict/dictionary.h"

#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QTimer>

class SubtitleWidget : public QLineEdit
{
    Q_OBJECT

public:
    SubtitleWidget(QWidget *parent = 0);
    ~SubtitleWidget();

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void findTerms();
    void postitionChanged(const double value);
    void setSubtitle(QString subtitle,
                     const double start,
                     const double end,
                     const double delay);

private:
    Dictionary *m_dictionary;
    int         m_currentIndex;
    int         m_lastEmittedIndex;
    int         m_lastEmittedSize;
    QTimer     *m_findDelay;

    QString     m_rawText;
    double      m_startTime;
    double      m_endTime;

    void deleteTerms(QList<Term *> *terms);
};

#endif // SUBTITLEWIDGET_H