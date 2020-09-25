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
    void seek(int time) override;
    void play() override;
    void pause() override;
    void stop() override;
    void seekForward() override;
    void seekBackward() override;
    void keyPressed(QKeyEvent *event) override;

private:
    MpvWidget *m_mpv;
};

#endif // MPVADAPTER