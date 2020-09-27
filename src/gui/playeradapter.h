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
    virtual void seek(const int time) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seekForward() = 0;
    virtual void seekBackward() = 0;
    virtual void keyPressed(const QKeyEvent *event) = 0;
    virtual void setFullscreen(const bool value) = 0;
    virtual void setVolume(const int value) = 0;

Q_SIGNALS:
    void durationChanged(const int value);
    void positionChanged(const int value);
    void stateChanged(const bool paused);
    void fullscreenChanged(const bool full);
    void volumeChanged(const int value);
    void hideCursor();
    void close();
};

#endif // PLAYERADAPTER_H