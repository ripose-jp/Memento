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

#include <iostream>
#include <thread>
#include <atomic>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOMessage.h>

/**
 * Code heavily based off of
 * https://ladydebug.com/blog/2020/05/21/programmatically-capture-energy-saver-event-on-mac/
 */
class MacOSPowerEventHandler
{
public:
    MacOSPowerEventHandler() : preventSleep(false)
    {
        eventThread = new std::thread(eventLoop);
    }

    ~MacOSPowerEventHandler()
    {
        delete eventThread;
    }

    void setPreventSleep(const bool value)
    {
        preventSleep = value;
    }

private:
    std::thread       *eventThread;
    io_connect_t       rootPowerDomain;
    std::atomic<bool>  preventSleep;

    void eventLoop()
    {
        IONotificationPortRef notifyPortRef;
        io_object_t           notifierObj;
        rootPowerDomain = IORegisterForSystemPower(nullptr, &notifyPortRef, sleepWakeupCallBack, &notifierObj);
        if (rootPowerDomain == 0)
        {
            std::cerr << "IORegisterForSystemPower failed" << std::endl;
            return;
        }
        CFRunLoopAddSource(CFRunLoopGetCurrent(), IONotificationPortGetRunLoopSource(notifyPortRef), kCFRunLoopCommonModes);
        CFRunLoopRun();
    }

    void sleepWakeupCallBack(void *, io_service_t, natural_t messageType, void *messageArgument)
    {
        if (messageType == kIOMessageCanSystemSleep)
        {
            if (preventSleep)
            {
                IOCancelPowerChange(rootPowerDomain, (long)messageArgument);
            }
            else
            {
                IOAllowPowerChange(rootPowerDomain, (long)messageArgument);
            }
        }
    }
};

#endif // MACOSPOWEREVENTHANDLER_H
