#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

public Q_SLOTS:
    void openMedia();
    void seek(int pos);
    void pauseResume();

private Q_SLOTS:
    void setSliderRange(int duration);

private:
    Ui::MainWindow *ui;
};

#endif // MainWindow_H
