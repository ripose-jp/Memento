#ifndef ICONFACTORY_H
#define ICONFACTORY_H

#include <QIcon>
#include <QWidget>
#include <QStyle>
#include <QString>

#define ICON_ENUM_SIZE 9

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
        fullscreen,
        restore
    };

    virtual QIcon getIcon(IconFactory::Icon icon) = 0;
    static IconFactory *create(const QWidget *parent);

protected:
    const QWidget *m_parent;

    IconFactory(const QWidget *parent) : m_parent(parent) {}
};

class StyleFactory : public IconFactory
{
public:
    StyleFactory(const QWidget *parent);
    QIcon getIcon(IconFactory::Icon icon) Q_DECL_OVERRIDE;

private:
    QIcon icons[ICON_ENUM_SIZE];
};

class ThemeFactory : public IconFactory
{
public:
    ThemeFactory(const QWidget *parent);
    QIcon getIcon(IconFactory::Icon icon) Q_DECL_OVERRIDE;

private:
    QIcon icons[ICON_ENUM_SIZE];
};

#endif // ICONFACTORY_H