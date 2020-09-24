#ifndef MPVADAPTER_H
#define MPVADAPTER_H

#include "playeradapter.h"
#include "widgets/mpvwidget.h"

#include <QObject>

class MpvAdapter : public QObject, public PlayerAdapter
{
    Q_OBJECT
    Q_INTERFACES(PlayerAdapter)

public:
    MpvAdapter(MpvWidget *mpv);
    bool isPaused() const override;

public Q_SLOTS:
    void open() override;
    void seek(int time) override;
    void play() override;
    void pause() override;
    void stop() override;
    void seekForward() override;
    void seekBackward() override;

Q_SIGNALS:
    void durationChanged(int value) override;
    void positionChanged(int value) override;
    void stateChanged(bool paused) override;

private:
    MpvWidget *m_mpv;
};

#endif // MPVADAPTER