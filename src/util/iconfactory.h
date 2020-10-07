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
    StyleFactory(const QWidget *parent) : IconFactory(parent) {}

    QIcon getIcon(IconFactory::Icon icon) Q_DECL_OVERRIDE;

private:
    const QStyle::StandardPixmap pixmaps[ICON_ENUM_SIZE] = {
        QStyle::SP_MediaPlay,
        QStyle::SP_MediaPause,
        QStyle::SP_MediaStop,
        QStyle::SP_MediaSeekForward,
        QStyle::SP_MediaSeekBackward,
        QStyle::SP_MediaSkipForward,
        QStyle::SP_MediaSkipBackward,
        QStyle::SP_MediaPlay, // TODO add fullscreen icon
        QStyle::SP_MediaPlay // TODO add restore icon
    };
};

#define PLAY_THEME "media-playback-start"
#define PAUSE_THEME "media-playback-pause"
#define STOP_THEME "media-playback-stop"
#define SEEK_BACKWARD_THEME "media-seek-backward"
#define SEEK_FORWARD_THEME "media-seek-forward"
#define SKIP_BACKWARD_THEME "media-skip-backward"
#define SKIP_FORWARD_THEME "media-skip-forward"
#define FULLSCREEN_THEME "view-fullscreen"
#define RESTORE_THEME "view-restore"

class ThemeFactory : public IconFactory
{
public:
    ThemeFactory(const QWidget *parent) : IconFactory(parent) {}

    QIcon getIcon(IconFactory::Icon icon) Q_DECL_OVERRIDE;

private:
    const QString icons[ICON_ENUM_SIZE] = {
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
};

#endif // ICONFACTORY_H