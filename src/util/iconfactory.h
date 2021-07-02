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
#include <QString>
#include <QStyle>
#include <QWidget>

/* Size of the icon enum. Be sure to update when adding icons. */
#define ICON_ENUM_SIZE  17

/* Number of XDG icons decided by linux standards. Likely won't change. */
#define XDG_ICONS       9

/**
 * Parent class of icon factories, needed for polymorphism.
 */
class IconFactory
{
public:
    /* Valid Icons. */
    enum Icon
    {
        play,
        pause,
        stop,
        seek_forward,
        seek_backward,
        skip_forward,
        skip_backward,

        /* All bundled icons should be put past this point. */

        fullscreen,
        restore,

        /* All icons without an xdg spec should be put past this point. */

        plus,
        minus,
        back,
        up,
        down,
        hamburger,
        audio,
        noaudio
    };

    virtual ~IconFactory() { m_factory = nullptr; }

    /**
     * Gets the icon corresponding the Icon enum.
     * @param icon The requested icon.
     * @return The icon corresponding to the enum.
     */
    virtual const QIcon &getIcon(IconFactory::Icon icon) const = 0;

    /**
     * (Re)builds icons. Should be called manually every time there is a palette
     * change.
     */
    virtual void buildIcons() = 0;

    /**
     * Returns an IconFactory. Caller does not have ownership, factory is
     * shared.
     */
    static IconFactory *create();

protected:
    /* Shared IconFactory object. */
    static inline IconFactory *m_factory = nullptr;

    IconFactory() {}
};

/**
 * An IconFactory that sources as many XDG icons as possible before using baked
 * in ones.
 */
class StyleFactory : public IconFactory
{
public:
    StyleFactory();

    const QIcon &getIcon(IconFactory::Icon icon) const override;
    void buildIcons() override;

private:
    QIcon icons[ICON_ENUM_SIZE];

    /**
     * Generates an icon from an image.
     * @param path Path to the icon.
     * @return The icon at path will a fill corresponding to the current
     *         palette.
     */
    QIcon buildIcon(const QString &path);
};


/**
 * An IconFactory that only uses icons included in Memento.
 */
class ThemeFactory : public IconFactory
{
public:
    ThemeFactory();

    const QIcon &getIcon(IconFactory::Icon icon) const override;
    void buildIcons() override;

private:
    QIcon icons[ICON_ENUM_SIZE];
};

#endif // ICONFACTORY_H