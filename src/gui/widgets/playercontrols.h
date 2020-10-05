#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>

namespace Ui
{
    class PlayerControls;
}

struct Entry;

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
    void setSubtitle(const QString &subtitle, const int end);

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
    void entryChanged(const Entry *entry);

private Q_SLOTS:
    void pauseResume();
    void toggleFullscreen();

private:
    Ui::PlayerControls *m_ui;
    bool m_paused;
    bool m_fullscreen;
    int m_endtime;

    QString formatTime(int time);
};

#endif // PLAYERCONTROLS_H