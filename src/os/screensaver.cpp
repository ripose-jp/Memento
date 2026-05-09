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

#include "os/screensaver.h"

#if defined(Q_OS_WIN)

#include <windows.h>

#elif defined(Q_OS_UNIX) && !defined(Q_OS_APPLE)

#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>

static const char *DBUS_SERVICE{"org.freedesktop.ScreenSaver"};
static const char *DBUS_PATH{"/org/freedesktop/ScreenSaver"};
static const char *DBUS_INTERFACE{"org.freedesktop.ScreenSaver"};
static const char *DBUS_METHOD_INHIBIT{"Inhibit"};
static const char *DBUS_METHOD_UNINHIBIT{"UnInhibit"};
static const char *DBUS_APPLICATION_ID{"io.github.ripose-jp.memento"};

#endif

Screensaver::Screensaver(QObject *parent) : QObject(parent)
{

}

Screensaver::~Screensaver()
{
    uninhibit();
}

void Screensaver::inhibit([[maybe_unused]] const QString &reason)
{
    if (m_inhibited)
    {
        return;
    }

#if defined(Q_OS_WIN)
    SetThreadExecutionState(
        ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED
    );
#elif defined(Q_OS_APPLE)
    QByteArray reasonUtf8 = reason.toUtf8();
    CFStringRef reasonStr = CFStringCreateWithCString(
        NULL, reasonUtf8.constData(), kCFStringEncodingUTF8
    );
    IOPMAssertionCreateWithName(
        kIOPMAssertionTypeNoDisplaySleep,
        kIOPMAssertionLevelOn,
        reasonStr,
        &m_assertionId
    );
    CFRelease(reasonStr);
#elif defined(Q_OS_UNIX)
    QByteArray reasonUtf8 = reason.toUtf8();
    QDBusInterface iface(
        DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE,
        QDBusConnection::sessionBus()
    );
    if (iface.isValid())
    {
        QDBusReply<uint32_t> reply = iface.call(
            DBUS_METHOD_INHIBIT,
            DBUS_APPLICATION_ID,
            reasonUtf8.constData()
        );
        if (reply.isValid())
        {
            m_dbusCookie = reply.value();
        }
    }
#endif
    m_inhibited = true;
}

void Screensaver::uninhibit()
{
    if (!m_inhibited)
    {
        return;
    }

#if defined(Q_OS_WIN)
    SetThreadExecutionState(ES_CONTINUOUS);
#elif defined(Q_OS_MACOS)
    IOPMAssertionRelease(m_assertionId);
#elif defined(Q_OS_UNIX) && !defined(Q_OS_APPLE)
    if (m_dbusCookie != 0)
    {
        QDBusInterface iface(
            DBUS_SERVICE, DBUS_PATH, DBUS_INTERFACE,
            QDBusConnection::sessionBus()
        );
        if (iface.isValid())
        {
            iface.call(DBUS_METHOD_UNINHIBIT, m_dbusCookie);
            m_dbusCookie = 0;
        }
    }
#endif
    m_inhibited = false;
}
