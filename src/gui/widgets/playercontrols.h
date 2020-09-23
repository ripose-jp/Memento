#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QWidget>

namespace Ui
{
    class PlayerControls;
}

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerControls(QWidget *parent = 0);
    virtual ~PlayerControls();

signals:
    void play();
    void pause();
    void seekForward();
    void seekBackward();

private:
    Ui::PlayerControls *ui;
};

#endif // PLAYERCONTROLS_H