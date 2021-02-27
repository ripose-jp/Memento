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

#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QTimer>

#include <mpv/client.h>
#include <mpv/render_gl.h>

#include "../../util/directoryutils.h"

class MpvWidget Q_DECL_FINAL : public QOpenGLWidget
{
    Q_OBJECT

public:
    MpvWidget(QWidget *parent = 0);
    ~MpvWidget();

    QSize sizeHint() const { return QSize(480, 270); }
    mpv_handle *get_handle() { return mpv; }

Q_SIGNALS:
    void durationChanged(const double value);
    void positionChanged(const double value);
    void volumeChanged(const int value);
    void stateChanged(const bool paused);
    void fullscreenChanged(const bool full);
    void tracklistChanged(const mpv_node *node);
    void titleChanged(const char **name);
    void videoTrackChanged(const int64_t id);
    void audioTrackChanged(const int64_t id);
    void subtitleTrackChanged(const int64_t id);
    void audioDisabled();
    void videoDisabled();
    void subtitleDisabled();
    void subtitleChanged(const QString &subtitle, 
                         const double start, 
                         const double end);
    void hideCursor();
    void shutdown();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void on_mpv_events();
    void maybeUpdate();

private:
    void handle_mpv_event(mpv_event *event);
    static void on_update(void *ctx);
    QString convertToMouseString(const QMouseEvent *event) const;

    mpv_handle *mpv;
    mpv_render_context *mpv_gl;
    QTimer *m_cursorTimer;
#if __linux__
    uint32_t dbus_cookie;
#endif
};

#endif // MPVWIDGET_H