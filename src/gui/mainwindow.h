#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "playeradapter.h"

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

Q_SIGNALS:
    void keyPressed(QKeyEvent *event);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *m_ui;
    PlayerAdapter *m_player;
};

#endif // MAINWINDOW_H
