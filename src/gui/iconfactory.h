#ifndef ICONFACTORY_H
#define ICONFACTORY_H

#include <QIcon>
#include <QWidget>

#define PLAY_THEME "media-playback-start"
#define PAUSE_THEME "media-playback-pause"
#define STOP_THEME "media-playback-stop"
#define SEEK_BACKWARD_THEME "media-seek-backward"
#define SEEK_FORWARD_THEME "media-seek-forward"
#define SKIP_BACKWARD_THEME "media-skip-backward"
#define SKIP_FORWARD_THEME "media-skip-forward"
#define FULLSCREEN_THEME "view-fullscreen"
#define RESTORE_THEME "view-restore"

class IconFactory
{
public:
    enum class Icon
    { 
        play, pause, stop, seek_forward, seek_backward,
        skip_forward, skip_backward, fullscreen, restore 
    };
    static QIcon getIcon(IconFactory::Icon icon, const QWidget *widget);

private:
    IconFactory() {}
};

#endif // ICONFACTORY_H