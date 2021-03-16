////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Ripose
//
// This file is part of Memento.
//
// Memento is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2 of the License.
//
// Memento is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Memento.  If not, see <https://www.gnu.org/licenses/>.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "playeradapter.h"
#include "widgets/definitionwidget.h"
#include "widgets/ankisettings.h"
#include "../anki/ankiclient.h"

#include <QMainWindow>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QActionGroup>
#include <QPair>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QRunnable>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

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
    void threadError(const QString& title, const QString &error) const;
    void threadInfo(const QString& title, const QString &error) const;

public Q_SLOTS:
    void setFullscreen(bool value);

private Q_SLOTS:
    void open();
    void setTracks(QList<const PlayerAdapter::Track *> tracks);
    void updateAnkiProfileMenu();
    void setDefinitionWidgetLocation();
    void hideControls();
    void hidePlayerCursor();
    void showErrorMessage(const QString& title, const QString &error) const;
    void showInfoMessage(const QString& title, const QString &error) const;

protected:
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::MainWindow *m_ui;

    PlayerAdapter *m_player;
    DefinitionWidget *m_definition;

    AnkiSettings *m_ankiSettings;
    AnkiClient *m_ankiClient;

    QActionGroup *m_actionGroupAudio;
    QList<QPair<QAction *, const PlayerAdapter::Track *>> m_audioTracks;
    QActionGroup *m_actionGroupVideo;
    QList<QPair<QAction *, const PlayerAdapter::Track *>> m_videoTracks;
    QActionGroup *m_actionGroupSubtitle;
    QList<QPair<QAction *, const PlayerAdapter::Track *>> m_subtitleTracks;
    QActionGroup *m_actionGroupSubtitleTwo;
    QList<QPair<QAction *, const PlayerAdapter::Track *>> m_subtitleTwoTracks;

    QActionGroup *m_actionGroupAnkiProfile;
    QList<QAction *> m_ankiProfiles;

    QNetworkAccessManager *m_manager;

    bool m_maximized;

    void clearTracks();
    void clearTrack(
        QList<QPair<QAction *, const PlayerAdapter::Track *>> &tracks, 
        QMenu *menu,
        QActionGroup *actionGroup,
        QAction *actionDisable
    );
    void addDictionary();
    void checkForUpdates();

    class DatabaseUpdaterThread : public QRunnable
    {
    public:
        DatabaseUpdaterThread(MainWindow *parent, const QString &path)
            : m_parent(parent), m_path(path) {}
        void run() override;

    private:
        const MainWindow *m_parent;
        const QString m_path;
    };
};

#endif // MAINWINDOW_H
