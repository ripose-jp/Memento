#include "mpvadapter.h"

#include <QFileDialog>

MpvAdapter::MpvAdapter(MpvWidget *mpv, QObject *parent) : m_mpv(mpv), PlayerAdapter(parent)
{
    connect(m_mpv, &MpvWidget::durationChanged, this, &MpvAdapter::durationChanged);
    connect(m_mpv, &MpvWidget::positionChanged, this, &MpvAdapter::positionChanged);
    connect(m_mpv, &MpvWidget::stateChanged, this, &MpvAdapter::stateChanged);
}

void MpvAdapter::open()
{
    QString file = QFileDialog::getOpenFileName(0, "Open a video");
    if (file.isEmpty())
        return;
    m_mpv->command(QStringList() << "loadfile" << file);
}

void MpvAdapter::seek(int time)
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