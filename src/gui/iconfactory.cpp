#include "iconfactory.h"

#include <QStyle>
#include <QString>

QIcon IconFactory::getIcon(IconFactory::Icon icon, const QWidget *widget)
{
    QString iconThemeName;
    QStyle::StandardPixmap styleIconEnum;

    switch (icon)
    {
    case IconFactory::Icon::play:
    {
        iconThemeName = QString::fromUtf8(PLAY_THEME);
        styleIconEnum = QStyle::SP_MediaPlay;
        break;
    }
    case IconFactory::Icon::pause:
    {
        iconThemeName = QString::fromUtf8(PAUSE_THEME);
        styleIconEnum = QStyle::SP_MediaPause;
        break;
    }
    case IconFactory::Icon::stop:
    {
        iconThemeName = QString::fromUtf8(STOP_THEME);
        styleIconEnum = QStyle::SP_MediaStop;
        break;
    }
    case IconFactory::Icon::seek_forward:
    {
        iconThemeName = QString::fromUtf8(SEEK_FORWARD_THEME);
        styleIconEnum = QStyle::SP_MediaSeekForward;
        break;
    }
    case IconFactory::Icon::seek_backward:
    {
        iconThemeName = QString::fromUtf8(SEEK_BACKWARD_THEME);
        styleIconEnum = QStyle::SP_MediaSeekBackward;
        break;
    }
    case IconFactory::Icon::skip_forward:
    {
        iconThemeName = QString::fromUtf8(SKIP_FORWARD_THEME);
        styleIconEnum = QStyle::SP_MediaSkipForward;
        break;
    }
    case IconFactory::Icon::skip_backward:
    {
        iconThemeName = QString::fromUtf8(SKIP_BACKWARD_THEME);
        styleIconEnum = QStyle::SP_MediaSkipBackward;
        break;
    }
    case IconFactory::Icon::fullscreen:
    {
        iconThemeName = QString::fromUtf8(FULLSCREEN_THEME);
        styleIconEnum = QStyle::SP_MediaPlay; // TODO, fix icon
        break;
    }
    case IconFactory::Icon::restore:
    {
        iconThemeName = QString::fromUtf8(RESTORE_THEME);
        styleIconEnum = QStyle::SP_MediaPlay; // TODO, fix icon
        break;
    }
    }

    if (QIcon::hasThemeIcon(iconThemeName))
    {
        return QIcon::fromTheme(iconThemeName);
    }
    return widget->style()->standardIcon(styleIconEnum);
}