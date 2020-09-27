#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "playeradapter.h"

#include <QMainWindow>
#include <QKeyEvent>
#include <QMouseEvent>

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

public Q_SLOTS:
    void setFullscreen(bool value);

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::MainWindow *m_ui;
    PlayerAdapter *m_player;
};

#endif // MAINWINDOW_H
