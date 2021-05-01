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

#include <QMainWindow>

#include "widgets/definitionwidget.h"
#include "widgets/optionswindow.h"
#include "widgets/aboutwindow.h"
#include "widgets/subtitlewidget.h"

#include "../util/globalmediator.h"
#include "playeradapter.h"
#include "../anki/ankiclient.h"

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
#include <QResizeEvent>
#include <QSpacerItem>

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

public Q_SLOTS:
    void setFullscreen(bool value);

private Q_SLOTS:
    void open();
    void repositionSubtitles();
    void updateSubScale(const double inc);
    void moveSubtitles(const double inc);
    void setTracks(QList<const Track *> tracks);
    void setTerms(const QList<Term *> *terms);
    void updateAnkiProfileMenu();
    void setDefinitionWidgetLocation();
    void deleteDefinitionWidget();
    void hideControls();
    void showErrorMessage(const QString title, const QString error) const;
    void showInfoMessage (const QString title, const QString error) const;
    void setTheme();

protected:
    void showEvent(QShowEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow   *m_ui;

    GlobalMediator   *m_mediator;
    PlayerAdapter    *m_player;
    DefinitionWidget *m_definition;
    AnkiClient       *m_ankiClient;
    OptionsWindow    *m_optionsWindow;  
    AboutWindow      *m_aboutWindow;

    struct SubtitleUi
    {
        QVBoxLayout    *layoutPlayerOverlay;
        QHBoxLayout    *layoutSubtitle;
        SubtitleWidget *subtitleWidget;
        QWidget        *spacerWidget;
        double          offsetPercent;
    } m_subtitle;

    QActionGroup *m_actionGroupAudio;
    QList<QPair<QAction *, const Track *>> m_audioTracks;
    QActionGroup *m_actionGroupVideo;
    QList<QPair<QAction *, const Track *>> m_videoTracks;
    QActionGroup *m_actionGroupSubtitle;
    QList<QPair<QAction *, const Track *>> m_subtitleTracks;
    QActionGroup *m_actionGroupSubtitleTwo;
    QList<QPair<QAction *, const Track *>> m_subtitleTwoTracks;

    QActionGroup *m_actionGroupAnkiProfile;
    QList<QAction *> m_ankiProfiles;

    QNetworkAccessManager *m_manager;

    bool m_maximized;

    void clearTracks();
    void clearTrack(
        QList<QPair<QAction *, const Track *>> &tracks, 
        QMenu *menu,
        QActionGroup *actionGroup,
        QAction *actionDisable
    );
    void checkForUpdates();

    inline bool isMouseOverPlayer();
};

#endif // MAINWINDOW_H
