////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2023 Ripose
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

#ifndef HITTESTWIDGET_H
#define HITTESTWIDGET_H

#include <QWidget>

#include <vector>

/**
 * Widget that emits signals when the mouse enters or exits.
 */
class HitTestWidget : public QWidget
{
    Q_OBJECT

public:
    HitTestWidget(QWidget *parent = nullptr);

    virtual ~HitTestWidget() override = default;

    /**
     * The type of mouse event.
     */
    enum class MouseEventFlag
    {
        /* The handler will fire on mouse event */
        Enter = 1 << 0,

        /* The handler will fire on mouse exit */
        Exit = 1 << 1,

        /* The handler will fire on mouse move */
        Move = 1 << 2,
    };

    /**
     * Operator overload to allow MouseEventFlags to be used as flags.
     * @param rhs The right hand side value.
     * @param lhs The left hand size value.
     * @return Compiled values as a MouseEventFlag
     */
    constexpr friend MouseEventFlag operator|(
        MouseEventFlag rhs, MouseEventFlag lhs)
    {
        return static_cast<MouseEventFlag>(
            static_cast<int>(rhs) | static_cast<int>(lhs)
        );
    }

    /**
     * Adds a region handler.
     * @param region The region to check. Coordinates range from 0.0 to 1.0 for
     *               both horizontal and vertical.
     * @param handler The hander function for the region.
     * @param flags   The flags for the handler.
     */
    inline void addRegion(
        const QRectF &region,
        std::function<void()> handler,
        MouseEventFlag flags)
    {
        m_regions.emplace_back(region, handler, flags);
    }

    /**
     * Clears all regions.
     */
    inline void clearRegions()
    {
        m_regions.clear();
    }

protected:
    /**
     * Event that fires when the mouse moves around the widget.
     * @param event The mouse event.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    /**
     * Handles all regions with regard to the given point.
     * @param pos The point to test in widget space.
     */
    void handleRegions(const QPoint &pos);

    /**
     * Handles all regions with regard to the given point.
     * @param pos The point to test in region space.
     */
    void handleRegions(const QPointF &pos);

    /**
     * Maps a QPoint in widget space to a QPointF in region space.
     * @param pos The point in widget space.
     * @return The point in region space.
     */
    inline QPointF mapToRegion(const QPoint &pos) const;

    /**
     * Data structure containing region information.
     */
    class Region
    {
    public:
        Region(
            const QRectF &area,
            std::function<void()> handler,
            MouseEventFlag flags) :
            m_area(area),
            m_handler(handler),
            m_flags(flags)
        {

        }

        /**
         * Checks if the enter flag is set.
         * @return true if the enter flag is set,
         * @return false otherwise
         */
        constexpr bool enterFlag() const
        {
            return (m_flags | MouseEventFlag::Enter) == MouseEventFlag::Enter;
        }

        /**
         * Checks if the exit flag is set.
         * @return true if the exit flag is set,
         * @return false otherwise
         */
        constexpr bool exitFlag() const
        {
            return (m_flags | MouseEventFlag::Exit) == MouseEventFlag::Exit;
        }

        /**
         * Checks if the move flag is set.
         * @return true if the move flag is set,
         * @return false otherwise
         */
        constexpr bool moveFlag() const
        {
            return (m_flags | MouseEventFlag::Move) == MouseEventFlag::Move;
        }

        /**
         * Checks if the widget contains the point.
         * @param pos The point in region space.
         * @return true if the ,
         * @return false otherwise.
         */
        inline bool contains(const QPointF &pos) const
        {
            return m_area.contains(pos);
        }

        /**
         * Checks if the region was entered.
         * @param pos The point in region space.
         * @return true if the region was entered,
         * @return false otherwise.
         */
        inline bool entered(const QPointF &pos) const
        {
            return !m_entered && contains(pos);
        }

        /**
         * Checks if the region was exited.
         * @param pos The point in region space.
         * @return true if the region was exited,
         * @return false otherwise.
         */
        inline bool exited(const QPointF &pos) const
        {
            return m_entered && !contains(pos);
        }

        /**
         * Calls the handler function.
         */
        inline void handle() const
        {
            return m_handler();
        }

        /**
         * Updates state based on position.
         * @param pos The position in region space.
         */
        inline void update(const QPointF &pos)
        {
            m_entered = contains(pos);
        }

    private:
        /* The region for this handler */
        const QRectF m_area;

        /* The handler for this region */
        const std::function<void()> m_handler;

        /* The flags for this region */
        const MouseEventFlag m_flags;

        /* True if the region is entered, false otherwise */
        bool m_entered{false};
    };

    /* The region this widget is handling */
    std::vector<Region> m_regions;
};

#endif // HITTESTWIDGET_H
