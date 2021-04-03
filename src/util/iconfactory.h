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

#ifndef ICONFACTORY_H
#define ICONFACTORY_H

#include <QIcon>
#include <QWidget>
#include <QStyle>
#include <QString>

#define ICON_ENUM_SIZE 15
#define XDG_ICONS 9

class IconFactory
{
public:
    enum Icon
    {
        play,
        pause,
        stop,
        seek_forward,
        seek_backward,
        skip_forward,
        skip_backward,
        // All bundled icons should be put past this point
        fullscreen,
        restore,
        // All icons without an xdg spec should be put past this point
        plus,
        minus,
        back,
        up,
        down,
        hamburger
    };

    virtual const QIcon &getIcon(IconFactory::Icon icon) const = 0;
    virtual void buildIcons() = 0;

    static IconFactory *create();

protected:
    static inline IconFactory *m_factory = nullptr;

    IconFactory() {}
};

class StyleFactory : public IconFactory
{
public:
    StyleFactory();

    const QIcon &getIcon(IconFactory::Icon icon) const Q_DECL_OVERRIDE;
    void buildIcons() Q_DECL_OVERRIDE;

private:
    QIcon icons[ICON_ENUM_SIZE];

    QIcon buildIcon(const QString &path);
};

class ThemeFactory : public IconFactory
{
public:
    ThemeFactory();

    const QIcon &getIcon(IconFactory::Icon icon) const Q_DECL_OVERRIDE;
    void buildIcons() Q_DECL_OVERRIDE;

private:
    QIcon icons[ICON_ENUM_SIZE];
};

#endif // ICONFACTORY_H