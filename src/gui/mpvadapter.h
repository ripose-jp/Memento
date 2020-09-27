#ifndef MPVADAPTER_H
#define MPVADAPTER_H

#include "playeradapter.h"
#include "widgets/mpvwidget.h"

class MpvAdapter : public PlayerAdapter
{
public:
    MpvAdapter(MpvWidget *mpv, QObject *parent = 0);
    virtual ~MpvAdapter() {}

public Q_SLOTS:
    void open() Q_DECL_OVERRIDE;
    void seek(const int time) Q_DECL_OVERRIDE;
    void play() Q_DECL_OVERRIDE;
    void pause() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;
    void seekForward() Q_DECL_OVERRIDE;
    void seekBackward() Q_DECL_OVERRIDE;
    void keyPressed(const QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseWheelMoved(const QWheelEvent *event) Q_DECL_OVERRIDE;
    void setFullscreen(const bool value) Q_DECL_OVERRIDE;
    void setVolume(const int value) Q_DECL_OVERRIDE;
    int getMaxVolume() const Q_DECL_OVERRIDE;

private:
    MpvWidget *m_mpv;
};

#endif // MPVADAPTER