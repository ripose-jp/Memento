#ifndef PLAYERADAPTER_H
#define PLAYERADAPTER_H

#include <QObject>
#include <QKeyEvent>

class PlayerAdapter : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    virtual ~PlayerAdapter() {}
    virtual int getMaxVolume() const = 0;

public Q_SLOTS:
    virtual void open() = 0;
    virtual void seek(int time) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seekForward() = 0;
    virtual void seekBackward() = 0;
    virtual void keyPressed(QKeyEvent *event) = 0;
    virtual void setFullscreen(bool value) = 0;
    virtual void setVolume(int value) = 0;

Q_SIGNALS:
    void durationChanged(int value);
    void positionChanged(int value);
    void stateChanged(bool paused);
    void fullscreenChanged(bool full);
    void volumeChanged(int value);
    void close();
};

#endif // PLAYERADAPTER_H