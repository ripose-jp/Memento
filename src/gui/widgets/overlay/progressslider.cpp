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
#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <QStyle>
#include <QTextEdit>

/* Begin Constructor/Destructor */

/**
 * Format string for the style sheet of this widget.
 * @param 1 Background color
 * @param 2 Progress color
 */
static const QString STYLESHEET_FORMAT(
    "QSlider::groove:horizontal {"
	    "background: %1;"
    "}"
    "QSlider::sub-page:horizontal {"
	    "background: %2;"
    "}"
);

ProgressSlider::ProgressSlider(QWidget *parent) : QSlider(parent)
{

}

ProgressSlider::~ProgressSlider()
{
    m_labelTimecode->deleteLater();
}

void ProgressSlider::initialize(QPointer<Context> context)
{
    m_context = std::move(context);

    setMouseTracking(true);
    setOrientation(Qt::Horizontal);
    setMinimum(0);
    setMaximum(0);

    m_labelTimecode = new StrokeLabel(parentWidget());
    m_labelTimecode->hide();

    connect(
        m_context,
        &Context::requestThemeRefresh,
        this,
        &ProgressSlider::initTheme,
        Qt::QueuedConnection
    );
    connect(
        m_context,
        &Context::windowFocusChanged,
        this,
        &ProgressSlider::initStylesheet,
        Qt::QueuedConnection
    );

    initTheme();
}

void ProgressSlider::initTheme()
{
    m_labelTimecode->setTextColor(palette().text().color());
    m_labelTimecode->setStrokeColor(palette().window().color());
    initStylesheet();
}

void ProgressSlider::initStylesheet()
{
    setStyleSheet(
        STYLESHEET_FORMAT
            .arg(palette().window().color().darker().name())
            .arg(palette().highlight().color().name())
    );
}

/* End Constructor/Destructor */
/* Begin Event Handlers */

void ProgressSlider::showEvent(QShowEvent *event)
{
    QSlider::showEvent(event);
    QFont font = m_labelTimecode->textFont();
    font.setPixelSize(height());
    m_labelTimecode->setTextFont(font);
}

void ProgressSlider::hideEvent(QHideEvent *event)
{
    QSlider::hideEvent(event);
    m_labelTimecode->hide();
}

void ProgressSlider::mouseMoveEvent(QMouseEvent *event)
{
    constexpr int MOUSE_OFFSET = 20;

    QSlider::mouseMoveEvent(event);

    if (maximum() == 0)
    {
        QSlider::mouseMoveEvent(event);
        return;
    }

    int pos = QStyle::sliderValueFromPosition(
        minimum(), maximum(), event->position().x(), width()
    );
    m_labelTimecode->setText(formatTime(pos));
    m_labelTimecode->adjustSize();

    int tipX = event->position().x() + MOUSE_OFFSET;
    int tipY = (parentWidget()->height() / 2) - (m_labelTimecode->height() / 2);
    if (event->position().x() > width() / 2)
    {
        tipX = event->position().x() - MOUSE_OFFSET - m_labelTimecode->width();
    }

    m_labelTimecode->move(mapToParent(QPoint(tipX, 0)).x(), tipY);

    if (tipX < 0 || tipX + m_labelTimecode->width() > width())
    {
        m_labelTimecode->hide();
    }
    else
    {
        m_labelTimecode->show();
    }
}

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
    QSlider::leaveEvent(event);
    m_labelTimecode->hide();
}

QString ProgressSlider::formatTime(int64_t time)
{

    constexpr int SECONDS_IN_MINUTE = 60;
    constexpr int SECONDS_IN_HOUR = 3600;
    constexpr int FILL_SPACES = 2;
    constexpr int BASE_TEN = 10;
    constexpr char FILL_CHAR = '0';

    if (time < 0)
    {
        time = 0;
    }

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

/* End Helpers */
