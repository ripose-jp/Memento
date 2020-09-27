#include "mpvadapter.h"

#include <QFileDialog>

MpvAdapter::MpvAdapter(MpvWidget *mpv, QObject *parent) : m_mpv(mpv), PlayerAdapter(parent)
{
    connect(m_mpv, &MpvWidget::durationChanged, this, &MpvAdapter::durationChanged);
    connect(m_mpv, &MpvWidget::positionChanged, this, &MpvAdapter::positionChanged);
    connect(m_mpv, &MpvWidget::stateChanged, this, &MpvAdapter::stateChanged);
    connect(m_mpv, &MpvWidget::fullscreenChanged, this, &MpvAdapter::fullscreenChanged);
    connect(m_mpv, &MpvWidget::volumeChanged, this, &MpvAdapter::volumeChanged);
    connect(m_mpv, &MpvWidget::hideCursor, this, &MpvAdapter::hideCursor);
    connect(m_mpv, &MpvWidget::shutdown, this, &MpvAdapter::close);
}

void MpvAdapter::open()
{
    QString file = QFileDialog::getOpenFileName(0, "Open a video");
    if (file.isEmpty())
        return;
    m_mpv->command(QStringList() << "loadfile" << file);
}

void MpvAdapter::seek(const int time)
{
    m_mpv->command(QVariantList() << "seek" << time << "absolute");
}

void MpvAdapter::play()
{
    m_mpv->setProperty("pause", false);
}

void MpvAdapter::pause()
{
    m_mpv->setProperty("pause", true);
}

void MpvAdapter::stop()
{
    m_mpv->command(QVariantList() << "stop");
}

void MpvAdapter::seekForward()
{
    m_mpv->command(QVariantList() << "sub-seek" << 1);
}

void MpvAdapter::seekBackward()
{
    m_mpv->command(QVariantList() << "sub-seek" << -1);
}

void MpvAdapter::keyPressed(const QKeyEvent *event)
{
    QString key = "";
    if (event->modifiers() & Qt::ShiftModifier) {
        key += "Shift+";
    }
    if (event->modifiers() & Qt::ControlModifier) {
        key += "Ctrl+";
    }
    if (event->modifiers() & Qt::AltModifier) {
        key += "Alt+";
    }
    if (event->modifiers() & Qt::MetaModifier) {
        key += "Meta+";
    }
    switch (event->key()) {
        case Qt::Key::Key_Left:
            key += "LEFT";
            break;
        case Qt::Key::Key_Right:
            key += "RIGHT";
            break;
        case Qt::Key::Key_Up:
            key += "UP";
            break;
        case Qt::Key::Key_Down:
            key += "DOWN";
            break;
        case Qt::Key::Key_Enter:
            key += "ENTER";
            break;
        case Qt::Key::Key_Escape:
            key += "ESC";
            break;
        case Qt::Key::Key_PageUp:
            key += "PGUP";
            break;
        case Qt::Key::Key_PageDown:
            key += "PGDWN";
            break;
        case Qt::Key::Key_Backspace:
            key += "BS";
            break;
        default: {
            key += event->text();
        }
    }
    m_mpv->command(QVariantList() << "keypress" << key);
}

void MpvAdapter::setFullscreen(const bool value)
{
    m_mpv->setProperty("fullscreen", value);
}

void MpvAdapter::setVolume(const int value)
{
    m_mpv->setProperty("volume", value);
}

int MpvAdapter::getMaxVolume() const
{
    return m_mpv->getProperty("volume-max").toInt();
}