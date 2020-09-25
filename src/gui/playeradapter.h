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

public Q_SLOTS:
    virtual void open() = 0;
    virtual void seek(int time) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seekForward() = 0;
    virtual void seekBackward() = 0;
    virtual void keyPressed(QKeyEvent *event) = 0;

Q_SIGNALS:
    void durationChanged(int value);
    void positionChanged(int value);
    void stateChanged(bool paused);
};

#endif // PLAYERADAPTER_H