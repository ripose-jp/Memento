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

#include <cstdint>

#include <QtSystemDetection>

#if defined(Q_OS_MACOS)
#include <IOKit/pwr_mgt/IOPMLib.h>
#endif

/**
 * @brief Allows from control over screensavers.
 */
class Screensaver : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Creates a new screensaver control object.
     *
     * @param parent The parent of the controller.
     */
    Screensaver(QObject *parent = nullptr);
    virtual ~Screensaver();

    /**
     * @brief Prevents the screensaver from activating.
     *
     * @param reason The reason for inhibition.
     */
    Q_INVOKABLE void inhibit(const QString &reason);

    /**
     * @brief Allows the screensaver to active.
     */
    Q_INVOKABLE void uninhibit();

private:
    /* true if this object is inhibiting screensavers */
    bool m_inhibited{false};

#if defined(Q_OS_MACOS)
    /* The assertion ID controlling the screensaver */
    IOPMAssertionID m_assertionId{};
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_APPLE)
    /* The DBus cookie for inhibit */
    uint32_t m_dbusCookie{0};
#endif
};
