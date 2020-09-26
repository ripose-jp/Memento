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
    void open() override;
    void seek(const int time) override;
    void play() override;
    void pause() override;
    void stop() override;
    void seekForward() override;
    void seekBackward() override;
    void keyPressed(const QKeyEvent *event) override;
    void setFullscreen(const bool value) override;
    void setVolume(const int value) override;
    int getMaxVolume() const override;

private:
    MpvWidget *m_mpv;
};

#endif // MPVADAPTER