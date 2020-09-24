#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>

#define PLAY_ICON "media-playback-start"
#define PAUSE_ICON "media-playback-pause"

namespace Ui
{
    class PlayerControls;
}

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget *parent = 0);
    ~PlayerControls();

public Q_SLOTS:
    void setDuration(int value);
    void setPosition(int value);
    void setPaused(bool paused);

private Q_SLOTS:
    void pauseResume();

Q_SIGNALS:
    void play();
    void pause();
    void seekForward();
    void seekBackward();
    void sliderMoved(int);

private:
    Ui::PlayerControls *m_ui;
    bool m_paused;
};

#endif // PLAYERCONTROLS_H