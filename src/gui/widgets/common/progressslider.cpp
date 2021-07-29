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

#include "progressslider.h"

#include <QApplication>
#include <QFont>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <QStyle>

#include "../../../util/globalmediator.h"
#include "sliderjumpstyle.h"

/**
 * Format string for the style sheet of this widget.
 * @param 1 Background color
 * @param 2 Progress color
 */
#define STYLESHEET_FORMAT (QString( \
        "QSlider::groove:horizontal {" \
	        "background: %1;" \
        "}" \
        "QSlider::sub-page:horizontal {" \
	        "background: %2;" \
        "}" \
    ))

/* Begin Constructor/Destructor */

ProgressSlider::ProgressSlider(QWidget *parent) : QSlider(parent)
{
    setMouseTracking(true);
    setOrientation(Qt::Horizontal);
    setMinimum(0);
    setMaximum(0);
    initTheme();

    m_labelTimecode = new QLabel(parent);
    m_labelTimecode->hide();
    m_labelTimecode->setAutoFillBackground(true);
    m_labelTimecode->setMinimumWidth(0);
    m_labelTimecode->setMinimumHeight(0);
    m_labelTimecode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_labelTimecode->setFrameStyle(QFrame::Box | QFrame::Plain);

    connect(
        GlobalMediator::getGlobalMediator(),
        &GlobalMediator::requestThemeRefresh,
        this, &ProgressSlider::initTheme
    );
    connect(
        GlobalMediator::getGlobalMediator(),
        &GlobalMediator::windowFocusChanged,
        this, &ProgressSlider::initStylesheet
    );
}

ProgressSlider::~ProgressSlider()
{
    delete m_labelTimecode;
}

void ProgressSlider::initTheme()
{
    setStyle(new SliderJumpStyle(QApplication::style()));
    initStylesheet();
}

void ProgressSlider::initStylesheet()
{
    setStyleSheet(
        STYLESHEET_FORMAT
            .arg(palette().dark().color().name())
            .arg(palette().highlight().color().name())
    );
} 

/* End Constructor/Destructor */
/* Begin Event Handlers */

void ProgressSlider::showEvent(QShowEvent *event)
{
    QFont font = m_labelTimecode->font();
    font.setPixelSize(height());
    m_labelTimecode->setFont(font);
}

void ProgressSlider::hideEvent(QHideEvent *event)
{
    QSlider::hideEvent(event);
    m_labelTimecode->hide();
}

#define MOUSE_OFFSET 20

void ProgressSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (maximum() == 0)
    {
        QSlider::mouseMoveEvent(event);
        return;
    }

    int pos = QStyle::sliderValueFromPosition(
        minimum(), maximum(), event->x(), width()
    );
    m_labelTimecode->setText(formatTime(pos));
    m_labelTimecode->adjustSize();

    int tipX = event->x() + MOUSE_OFFSET;
    int tipY = (parentWidget()->height() / 2) - (m_labelTimecode->height() / 2);
    if (tipX + m_labelTimecode->width() > width())
    {
        tipX = event->x() - MOUSE_OFFSET - m_labelTimecode->width();
    }

    m_labelTimecode->move(mapToParent(QPoint(tipX, 0)).x(), tipY);

    m_labelTimecode->show();

    QSlider::mouseMoveEvent(event);
}

#undef MOUSE_OFFSET

void ProgressSlider::paintEvent(QPaintEvent* event)
{
    QSlider::paintEvent(event);
    QRect rect = event->rect();
    QPainter p(this);
    p.setPen(palette().window().color());
    for(const double time : m_chapterTimes)
    {
        if (time == 0)
        {
            continue;
        }

        int x = QStyle::sliderPositionFromValue(
            minimum(), maximum(), time, width()
        );
        p.drawLine(x, rect.top(), x, rect.bottom());
    }
}

/* End Event Handlers */
/* Begin Slots */

void ProgressSlider::setChapters(QList<double> chapters)
{
    m_chapterTimes = chapters;
}

/* End Slots */
/* Begin Helpers */

void ProgressSlider::leaveEvent(QEvent *event)
{
    m_labelTimecode->hide();
}

#define SECONDS_IN_MINUTE   60
#define SECONDS_IN_HOUR     3600
#define FILL_SPACES         2
#define BASE_TEN            10
#define FILL_CHAR           '0'

QString ProgressSlider::formatTime(const int64_t time)
{
    int hours   = time / SECONDS_IN_HOUR;
    int minutes = (time % SECONDS_IN_HOUR) / SECONDS_IN_MINUTE;
    int seconds = time % SECONDS_IN_MINUTE;

    QString formatted = QString("%1:%2")
        .arg(minutes, FILL_SPACES, BASE_TEN, QChar(FILL_CHAR))
        .arg(seconds, FILL_SPACES, BASE_TEN, QChar(FILL_CHAR));
    if (hours)
    {
        formatted.prepend(QString("%1:").arg(hours));
    }

    return formatted;
}

#undef SECONDS_IN_MINUTE
#undef SECONDS_IN_HOUR
#undef FILL_SPACES
#undef BASE_TEN
#undef FILL_CHAR

/* End Helpers */
