////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2026 Ripose
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

#pragma once

#include <QObject>

/**
 * @brief Holds text and timing for an mpv subtitle.
 */
class MpvSubtitle : public QObject
{
    Q_OBJECT

    Q_PROPERTY(
        QString text
        READ text
        WRITE setText
        NOTIFY textChanged
    )

    Q_PROPERTY(
        double startTime
        READ startTime
        WRITE setStartTime
        NOTIFY startTimeChanged
    )

    Q_PROPERTY(
        double endTime
        READ endTime
        WRITE setEndTime
        NOTIFY endTimeChanged
    )

    Q_PROPERTY(
        double delay
        READ delay
        WRITE setDelay
        NOTIFY delayChanged
    )

    Q_PROPERTY(
        bool visible
        READ visible
        WRITE setVisible
        NOTIFY visibleChanged
    )

public:
    MpvSubtitle(QObject *parent = nullptr);
    virtual ~MpvSubtitle() = default;

    /**
     * @brief The text of a subtitle.
     *
     * @return The text of a subtitle.
     */
    [[nodiscard]]
    const QString &text() const noexcept;

    /**
     * @brief Sets the text of the subtitle.
     *
     * @param value The text of the subtitle.
     */
    void setText(QString value);

    /**
     * @brief The start time in seconds of the subtitle.
     *
     * @return The start time in seconds of the subtitle.
     */
    [[nodiscard]]
    double startTime() const noexcept;

    /**
     * @brief Sets the start time of the subtitle in seconds.
     *
     * @param value The start time in seconds of the subtitle.
     */
    void setStartTime(double value);

    /**
     * @brief The end time of the subtitle in seconds.
     *
     * @return The end time of the subtitle in seconds.
     */
    [[nodiscard]]
    double endTime() const noexcept;

    /**
     * @brief Sets the end of the subtitle in seconds
     *
     * @param value The end time of the subtitle in seconds.
     */
    void setEndTime(double value);

    /**
     * @brief The subtitle delay in seconds.
     *
     * @return The subtitle delay in seconds.
     */
    [[nodiscard]]
    double delay() const noexcept;

    /**
     * @brief Sets the subtitle delay.
     *
     * @param value The subtitle delay in seconds.
     */
    void setDelay(double value);

    /**
     * @brief Gets if the subtitle is visible.
     *
     * @return true if the subtitle is visible,
     * @return false otherwise.
     */
    [[nodiscard]]
    bool visible() const noexcept;

    /**
     * @brief Sets if the subtitle is visible.
     *
     * @param value true if the subtitle is visible, false otherwise.
     */
    void setVisible(bool value);

signals:
    /**
     * @brief Emitted when the subtitle text is changed.
     *
     * @param value The text of the subtitle.
     */
    void textChanged(const QString &value);

    /**
     * @brief Emitted when the start time of the subtitle changes.
     *
     * @param value The start time of the subtitle in seconds.
     */
    void startTimeChanged(double value);

    /**
     * @brief Emitted when the end time of the subtitle changes.
     *
     * @param value The end time of the subtitle in seconds.
     */
    void endTimeChanged(double value);

    /**
     * @brief Emitted when the subtitle delay changes.
     *
     * @param value The delay of the subtitle in seconds.
     */
    void delayChanged(double value);

    /**
     * @brief Emitted when the subtitle visibility changed.
     *
     * @param value true if the subtitle is visible, false otherwise.
     */
    void visibleChanged(bool value);

private:
    /* The text of the subtitle */
    QString m_text;

    /* The start time of the subtitle in seconds */
    double m_startTime{0};

    /* The end time of the subtitle in seconds */
    double m_endTime{0};

    /* The delay of the subtitle in seconds */
    double m_delay{0};

    /* The visibility of the subtitles */
    bool m_visible{false};
};
