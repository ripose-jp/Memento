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

#include "quick/features.h"

#include "constant/version.h"

Features::Features(QObject *parent) : QObject(parent)
{

}

Features::~Features()
{

}

int Features::versionMajor() const noexcept
{
    return Memento::VERSION_MAJOR;
}

int Features::versionMinor() const noexcept
{
    return Memento::VERSION_MINOR;
}

int Features::versionPatch() const noexcept
{
    return Memento::VERSION_PATCH;
}

QString Features::version() const
{
    return Memento::VERSION;
}

QString Features::versionHash() const
{
    return Memento::VERSION_HASH;
}

bool Features::windows() const noexcept
{
#if defined(Q_OS_WIN)
    return true;
#else
    return false;
#endif
}

bool Features::unix() const noexcept
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    return true;
#else
    return false;
#endif
}

bool Features::macos() const noexcept
{
#if defined(Q_OS_MACOS)
    return true;
#else
    return false;
#endif
}

Features::Platform Features::platform() const noexcept
{
#if defined(Q_OS_WIN)
    return Platform::Windows;
#elif defined(Q_OS_MACOS)
    return Platform::MacOs;
#elif defined(Q_OS_UNIX)
    return Platform::Unix;
#else
    return Platform::Unknown;
#endif
}

bool Features::mecab() const noexcept
{
#ifdef MEMENTO_MECAB_SUPPORT
    return true;
#else
    return false;
#endif
}

bool Features::ocr() const noexcept
{
#ifdef MEMENTO_OCR_SUPPORT
    return true;
#else
    return false;
#endif
}

bool Features::qApplication() const noexcept
{
#ifdef MEMENTO_QAPPLICATION
    return true;
#else
    return false;
#endif
}
