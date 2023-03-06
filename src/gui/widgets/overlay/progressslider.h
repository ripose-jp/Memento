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

#ifndef PROGRESSSLIDER_H
#define PROGRESSSLIDER_H

#include <QSlider>

class StrokeLabel;

/**
 * A custom slider for displaying video progress.
 */
class ProgressSlider : public QSlider
{
    Q_OBJECT

public:
    ProgressSlider(QWidget *parent = nullptr);
    ~ProgressSlider();

    /**
     * Creates an (HH):MM:SS timecode from the supplied time.
     * @param time The time in seconds.
     * @return (HH):MM:SS timestamp string.
     */
    static QString formatTime(int64_t time);

public Q_SLOTS:
    void setChapters(QList<double> chapters);

protected:
    /**
     * Used to update the font size of the timecode label.
     * @param event The show event, unused.
     */
    void showEvent(QShowEvent *event) override;

    /**
     * Used to hide the timecode label.
     * @param event The hide event.
     */
    void hideEvent(QHideEvent *event) override;

    /**
     * Used to update the timecode label.
     * @param event The mouse move event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * Used to hide the timecode label.
     * @param event The leave event. Unused.
     */
    void leaveEvent(QEvent *event) override;

    /**
     * Paints chapter tick marks on the slider.
     * @param event The paint event.
     */
    void paintEvent(QPaintEvent* event) override;

private Q_SLOTS:
    /**
     * Initializes the look and feel of the widget.
     */
    void initTheme();

    /**
     * Initializes only the stylesheet of the widget.
     */
    void initStylesheet();

private:
    /* The label used for displaying the timecode the mouse is hovering over. */
    StrokeLabel *m_labelTimecode;

    /* Saved chapter times in seconds. */
    QList<double> m_chapterTimes;
};

#endif // PROGRESSSLIDER_H
