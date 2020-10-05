#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>

#define EDICT_FILENAME "edict"
#define EDICT "edict"

class SubtitleWidget : public QLineEdit
{
    Q_OBJECT

public:
    SubtitleWidget(QWidget *parent = 0);
    ~SubtitleWidget();
    void updateText(const QString &text);

protected:
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *event) Q_DECL_OVERRIDE;

private:
    int m_currentIndex;

    void findEntry();
};

#endif // SUBTITLEWIDGET_H