#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QOpenGLWidget>
#include <mpv/client.h>
#include <mpv/render_gl.h>
#include <mpv/qthelper.hpp>

#define CONFIG_PATH "/.config/memento"

class MpvWidget Q_DECL_FINAL: public QOpenGLWidget
{
    Q_OBJECT

public:
    MpvWidget(QWidget *parent = 0);
    ~MpvWidget();
    void command(const QVariant& params);
    void setProperty(const QString& name, const QVariant& value);
    QVariant getProperty(const QString& name) const;
    QSize sizeHint() const { return QSize(480, 270);}
    mpv_handle* get_handle();

Q_SIGNALS:
    void durationChanged(int value);
    void positionChanged(int value);
    void stateChanged(bool paused);
    void shutdown();

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

private Q_SLOTS:
    void on_mpv_events();
    void maybeUpdate();
    
private:
    void handle_mpv_event(mpv_event *event);
    static void on_update(void *ctx);

    mpv_handle *mpv;
    mpv_render_context *mpv_gl;
};

#endif // MPVWIDGET_H