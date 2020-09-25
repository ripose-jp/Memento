#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>

#define SECONDS_IN_MINUTE 60
#define SECONDS_IN_HOUR 3600
#define FILL_SPACES 2
#define BASE_TEN 10
#define FILL_CHAR '0'

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
    void setFullscreen(bool value);
    void setVolumeLimit(int value);
    void setVolume(int value);

Q_SIGNALS:
    void play();
    void pause();
    void seekForward();
    void seekBackward();
    void sliderMoved(int value);
    void volumeSliderMoved(int value);
    void fullscreenChanged(bool value);

private Q_SLOTS:
    void pauseResume();
    void toggleFullscreen();

private:
    Ui::PlayerControls *m_ui;
    bool m_paused;
    bool m_fullscreen;
    QString formatTime(int time);
};

#endif // PLAYERCONTROLS_H