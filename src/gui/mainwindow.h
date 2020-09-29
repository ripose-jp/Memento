#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "playeradapter.h"

#include <QMainWindow>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QActionGroup>
#include <QPair>
#include <QDragEnterEvent>
#include <QDropEvent>

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
    void keyPressed(const QKeyEvent *event);
    void wheelMoved(const QWheelEvent *event);

public Q_SLOTS:
    void setFullscreen(bool value);

private Q_SLOTS:
    void open();
    void setTracks(QVector<const PlayerAdapter::Track *> tracks);

protected:
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::MainWindow *m_ui;
    PlayerAdapter *m_player;
    QActionGroup *m_actionGroupAudio;
    QActionGroup *m_actionGroupVideo;
    QActionGroup *m_actionGroupSubtitle;
    QVector<QPair<QAction *, const PlayerAdapter::Track *>> m_audioTracks;
    QVector<QPair<QAction *, const PlayerAdapter::Track *>> m_videoTracks;
    QVector<QPair<QAction *, const PlayerAdapter::Track *>> m_subtitleTracks;

    void clearTracks();
    void clearTrack(QVector<QPair<QAction *, const PlayerAdapter::Track *>> &tracks, QMenu *menu, QActionGroup *actionGroup, QAction *actionDisable);
};

#endif // MAINWINDOW_H
