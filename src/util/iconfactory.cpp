////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
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

#include "iconfactory.h"

#include <QApplication>
#include <QPainter>
#include <QPalette>

#ifdef APPIMAGE
#define FACTORY_CLASS(p) new StyleFactory(p)
#elif defined(Q_OS_WIN)
#define FACTORY_CLASS(p) new StyleFactory(p)
#elif defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
#define FACTORY_CLASS(p) new ThemeFactory(p)
#elif defined(Q_OS_MACOS)
#define FACTORY_CLASS(p) new StyleFactory(p)
#else
#error "OS not supported"
#endif

#define CLOSE_THEME         "window-close"
#define PLAY_THEME          "media-playback-start"
#define PAUSE_THEME         "media-playback-pause"
#define STOP_THEME          "media-playback-stop"
#define SEEK_BACKWARD_THEME "media-seek-backward"
#define SEEK_FORWARD_THEME  "media-seek-forward"
#define SKIP_BACKWARD_THEME "media-skip-backward"
#define SKIP_FORWARD_THEME  "media-skip-forward"
#define FULLSCREEN_THEME    "view-fullscreen"
#define RESTORE_THEME       "view-restore"

IconFactory *IconFactory::create()
{
    if (m_factory == nullptr)
    {
        m_factory = FACTORY_CLASS();
    }
    return m_factory;
}

#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
IconFactory *IconFactory::recreate(bool useTheme)
#else
IconFactory *IconFactory::recreate(bool)
#endif
{
    delete m_factory;
#if defined(Q_OS_UNIX) && !defined(Q_OS_DARWIN)
    if (useTheme)
    {
        m_factory = new ThemeFactory;
    }
    else
    {
        m_factory = new StyleFactory;
    }
#else
    m_factory = new StyleFactory;
#endif
    return m_factory;
}

void IconFactory::destroy()
{
    delete m_factory;
    m_factory = nullptr;
}

StyleFactory::StyleFactory()
{
    buildIcons();
}

void StyleFactory::buildIcons()
{
    icons[close]         = buildIcon(":/images/close.svg");
    icons[play]          = buildIcon(":images/play.svg");
    icons[pause]         = buildIcon(":images/pause.svg");
    icons[skip_backward] = buildIcon(":images/skipleft.svg");
    icons[skip_forward]  = buildIcon(":images/skipright.svg");
    icons[seek_backward] = buildIcon(":images/seekleft.svg");
    icons[seek_forward]  = buildIcon(":images/seekright.svg");
    icons[fullscreen]    = buildIcon(":/images/fullscreen.svg");
    icons[restore]       = buildIcon(":/images/restore.svg");
    icons[plus]          = buildIcon(":/images/plus.svg");
    icons[minus]         = buildIcon(":/images/minus.svg");
    icons[back]          = buildIcon(":/images/back.svg");
    icons[up]            = buildIcon(":/images/uparrow.svg");
    icons[down]          = buildIcon(":/images/downarrow.svg");
    icons[hamburger]     = buildIcon(":/images/hamburger.svg");
    icons[audio]         = buildIcon(":/images/audio.svg");
    icons[noaudio]       = buildIcon(":/images/noaudio.svg");
    icons[kana]          = buildIcon(":/images/kana.svg");
    icons[kanji]         = buildIcon(":/images/kanji.svg");
    icons[eye]           = buildIcon(":/images/eye.svg");
}

const QIcon &StyleFactory::getIcon(IconFactory::Icon icon) const
{
    return icons[icon];
}

QIcon StyleFactory::buildIcon(const QString &path)
{
    QPixmap pixmap(path);
    QColor color = QApplication::palette().color(
        QPalette::ColorRole::WindowText
    );

    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setBrush(color);
    painter.setPen(color);
    painter.drawRect(pixmap.rect());

    return QIcon(pixmap);
}

ThemeFactory::ThemeFactory()
{
    buildIcons();
}

void ThemeFactory::buildIcons()
{
    static const char *names[] = {
        CLOSE_THEME,
        PLAY_THEME,
        PAUSE_THEME,
        STOP_THEME,
        SEEK_FORWARD_THEME,
        SEEK_BACKWARD_THEME,
        SKIP_FORWARD_THEME,
        SKIP_BACKWARD_THEME,
        FULLSCREEN_THEME,
        RESTORE_THEME
    };

    StyleFactory styleFactory;
    for (size_t i = 0; i < XDG_ICONS; ++i)
    {
        icons[i] = QIcon::hasThemeIcon(names[i])
                 ? QIcon::fromTheme(names[i])
                 : styleFactory.getIcon((IconFactory::Icon) i);
    }
    for (size_t i = XDG_ICONS; i < ICON_ENUM_SIZE; ++i)
    {
        icons[i] = styleFactory.getIcon((IconFactory::Icon) i);
    }
}

const QIcon &ThemeFactory::getIcon(IconFactory::Icon icon) const
{
    return icons[icon];
}
