#ifndef PLAYERADAPTER_H
#define PLAYERADAPTER_H

#include <QObject>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QVector>

class PlayerAdapter : public QObject
{
    Q_OBJECT

public:
    using QObject::QObject;
    virtual ~PlayerAdapter() {}
    virtual int getMaxVolume() const = 0;

    struct Track
    {
        enum track_type
        {
            audio,
            video,
            subtitle
        };
        int64_t id;
        track_type type;
        int64_t src_id;
        QString title;
        QString lang;
        bool albumart;
        bool def;
        bool external;
        QString external_filename;
        QString codec;
    } typedef Track;

public Q_SLOTS:
    virtual void open(const QString &file) = 0;
    virtual void open(const QList<QUrl> &files) = 0;
    virtual void seek(const int time) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seekForward() = 0;
    virtual void seekBackward() = 0;
    virtual void skipForward() = 0;
    virtual void skipBackward() = 0;
    virtual void keyPressed(const QKeyEvent *event) = 0;
    virtual void mouseWheelMoved(const QWheelEvent *event) = 0;
    virtual void setFullscreen(const bool value) = 0;
    virtual void setVolume(const int value) = 0;

Q_SIGNALS:
    void durationChanged(const int value);
    void positionChanged(const int value);
    void stateChanged(const bool paused);
    void fullscreenChanged(const bool full);
    void volumeChanged(const int value);
    void tracksChanged(QVector<const PlayerAdapter::Track *> tracks);
    void hideCursor();
    void close();
};

#endif // PLAYERADAPTER_H