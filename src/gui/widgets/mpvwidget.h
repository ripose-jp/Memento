#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QTimer>
#include <mpv/client.h>
#include <mpv/render_gl.h>
#include <mpv/qthelper.hpp>

#define TIMEOUT 2000
#define CONFIG_PATH "/.config/memento"

class MpvWidget Q_DECL_FINAL : public QOpenGLWidget
{
    Q_OBJECT

public:
    MpvWidget(QWidget *parent = 0);
    ~MpvWidget();
    void command(const QVariant &params);
    void setProperty(const QString &name, const QVariant &value);
    QVariant getProperty(const QString &name) const;
    QSize sizeHint() const { return QSize(480, 270); }
    mpv_handle *get_handle();

Q_SIGNALS:
    void durationChanged(int value);
    void positionChanged(int value);
    void volumeChanged(int value);
    void stateChanged(bool paused);
    void fullscreenChanged(bool full);
    void tracklistChanged(mpv_node *node);
    void videoTrackChanged(int64_t id);
    void audioTrackChanged(int64_t id);
    void subtitleTrackChanged(int64_t id);
    void audioDisabled();
    void videoDisabled();
    void subtitleDisabled();
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
};

#endif // MPVWIDGET_H