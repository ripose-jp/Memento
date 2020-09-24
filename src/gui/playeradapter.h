#ifndef PLAYERADAPTER_H
#define PLAYERADAPTER_H

#include <QObject>

class PlayerAdapter
{
public:
    virtual ~PlayerAdapter() {}

    virtual bool isPaused() const = 0;
    virtual void open() = 0;
    virtual void seek(int time) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seekForward() = 0;
    virtual void seekBackward() = 0;

    // signals
    virtual void durationChanged(int value) = 0;
    virtual void positionChanged(int value) = 0;
    virtual void stateChanged(bool paused) = 0;
};

Q_DECLARE_INTERFACE(PlayerAdapter, "PlayerAdapter")

#endif // PLAYERADAPTER_H