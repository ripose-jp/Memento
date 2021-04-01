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

#include <QPainter>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    #define FACTORY_CLASS(p) new StyleFactory(p)
#elif __linux__
    #define FACTORY_CLASS(p) new ThemeFactory(p)
#elif __APPLE__
    #if TARGET_OS_MAC
        #define FACTORY_CLASS(p) new StyleFactory(p)
    #else
        #error "Apple OS type no supported"
    #endif
#else
    #error "OS not supported"
#endif

#define PLAY_THEME "media-playback-start"
#define PAUSE_THEME "media-playback-pause"
#define STOP_THEME "media-playback-stop"
#define SEEK_BACKWARD_THEME "media-seek-backward"
#define SEEK_FORWARD_THEME "media-seek-forward"
#define SKIP_BACKWARD_THEME "media-skip-backward"
#define SKIP_FORWARD_THEME "media-skip-forward"
#define FULLSCREEN_THEME "view-fullscreen"
#define RESTORE_THEME "view-restore"


IconFactory *IconFactory::create(const QWidget *parent)
{
    return FACTORY_CLASS(parent);
}

StyleFactory::StyleFactory(const QWidget *parent) : IconFactory(parent)
{
    const QStyle::StandardPixmap pixmaps[skip_backward + 1] = {
        QStyle::SP_MediaPlay,
        QStyle::SP_MediaPause,
        QStyle::SP_MediaStop,
        QStyle::SP_MediaSeekForward,
        QStyle::SP_MediaSeekBackward,
        QStyle::SP_MediaSkipForward,
        QStyle::SP_MediaSkipBackward
    };

    for (size_t i = 0; i < fullscreen; ++i)
    {
        icons[i] = m_parent->style()->standardIcon(pixmaps[i]);
    }
    icons[fullscreen] = buildIcon(":/images/fullscreen.svg");
    icons[restore]    = buildIcon(":/images/restore.svg");
    icons[plus]       = buildIcon(":/images/plus.svg");
    icons[minus]      = buildIcon(":/images/minus.svg");
    icons[back]       = buildIcon(":/images/back.svg");
    icons[up]         = buildIcon(":/images/uparrow.svg");
    icons[down]       = buildIcon(":/images/downarrow.svg");
    icons[hamburger]  = buildIcon(":/images/hamburger.svg");
}

QIcon StyleFactory::getIcon(IconFactory::Icon icon)
{
    return icons[icon];
}

QIcon StyleFactory::buildIcon(const QString &path)
{
    QColor color = m_parent->palette().color(m_parent->foregroundRole());
    QPixmap pixmap(path);

    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.setBrush(color);
    painter.setPen(color);
    painter.drawRect(pixmap.rect());

    return QIcon(pixmap);
}

ThemeFactory::ThemeFactory(const QWidget *parent) : IconFactory(parent)
{
    const QString names[9] = {
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

    StyleFactory styleFactory(m_parent);
    for (size_t i = 0; i < XDG_ICONS; ++i)
    {
        icons[i] = QIcon::hasThemeIcon(names[i]) ? 
            QIcon::fromTheme(names[i]) : 
            styleFactory.getIcon((IconFactory::Icon) i);
    }
    for (size_t i = XDG_ICONS; i < ICON_ENUM_SIZE; ++i)
    {
        icons[i] = styleFactory.getIcon((IconFactory::Icon) i);
    }
}

QIcon ThemeFactory::getIcon(IconFactory::Icon icon)
{
    return icons[icon];
}