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

#ifndef MACOSPOWEREVENTHANDLER_H
#define MACOSPOWEREVENTHANDLER_H

#include <atomic>
#include <IOKit/IOMessage.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <iostream>
#include <thread>

/**
 * Prevents sleep on Mac OS X. This class is untested and it should be assumed
 * that it works as intended.
 *
 * Code heavily based off of
 * https://ladydebug.com/blog/2020/05/21/programmatically-capture-energy-saver-event-on-mac/
 */
class MacOSPowerEventHandler
{
public:
    MacOSPowerEventHandler();
    ~MacOSPowerEventHandler();

    /**
     * Sets the CF run loop reference for this event handler.
     * @param ref The ref to set.
     */
    void setRunLoopRef(CFRunLoopRef ref);

    /**
     * Sets the root power domain.
     * Allows the root power domain to be set from the event loop.
     * @param r The root power domain to set.
     */
    void setRootPowerDomain(const io_connect_t &r);

    /**
     * Gets the current root power domain.
     * @return The current root power domain.
     */
    io_connect_t getRootPowerDomain() const;

    /**
     * Determines whether or not sleep should be prevented.
     * @param value true if sleep should be prevented, false otherwise.
     */
    void setPreventSleep(const bool value);

    /**
     * Gets the sleep prevention value.
     * @return true if sleep is being prevent, false otherwise.
     */
    bool getPreventSleep() const;

private:
    std::thread       *eventThread;
    io_connect_t       rootPowerDomain;
    std::atomic<bool>  preventSleep;
    CFRunLoopRef       runLoopRef;
};

#endif // MACOSPOWEREVENTHANDLER_H
