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
    void setDuration(const int value);
    void setPaused(const bool paused);
    void setFullscreen(const bool value);
    void setVolumeLimit(const int value);
    void setVolume(const int value);
    void setPosition(const int value);

Q_SIGNALS:
    void play();
    void pause();
    void stop();
    void seekForward();
    void seekBackward();
    void skipForward();
    void skipBackward();
    void sliderMoved(const int value);
    void volumeSliderMoved(const int value);
    void fullscreenChanged(const bool value);

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