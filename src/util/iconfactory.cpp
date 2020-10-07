#include "iconfactory.h"

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

IconFactory *IconFactory::create(const QWidget *parent)
{
    return FACTORY_CLASS(parent);
}

QIcon StyleFactory::getIcon(IconFactory::Icon icon)
{
        return m_parent->style()->standardIcon(pixmaps[icon]);
}

QIcon ThemeFactory::getIcon(IconFactory::Icon icon)
{
    if (QIcon::hasThemeIcon(icons[icon]))
    {
            return QIcon::fromTheme(icons[icon]);
    }
    return StyleFactory(m_parent).getIcon(icon);
}