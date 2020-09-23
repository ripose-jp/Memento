#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->m_actionOpen, SIGNAL(triggered()), SLOT(openMedia()));
    /*
    connect(ui->sliderProgress, SIGNAL(sliderMoved(int)), SLOT(seek(int)));
    connect(ui->buttonPlay, SIGNAL(clicked()), SLOT(pauseResume()));
    connect(ui->mpv, SIGNAL(positionChanged(int)), ui->sliderProgress, SLOT(setValue(int)));
    connect(ui->mpv, SIGNAL(durationChanged(int)), this, SLOT(setSliderRange(int)));
    */
}

void MainWindow::openMedia()
{
    QString file = QFileDialog::getOpenFileName(0, "Open a video");
    if (file.isEmpty())
        return;
    ui->m_mpv->command(QStringList() << "loadfile" << file);
}

void MainWindow::seek(int pos)
{
    ui->m_mpv->command(QVariantList() << "seek" << pos << "absolute");
}

void MainWindow::pauseResume()
{
    const bool paused = ui->m_mpv->getProperty("pause").toBool();
    ui->m_mpv->setProperty("pause", !paused);
}

void MainWindow::setSliderRange(int duration)
{
    //ui->sliderProgress->setRange(0, duration);
}

MainWindow::~MainWindow() {
    delete ui;
}