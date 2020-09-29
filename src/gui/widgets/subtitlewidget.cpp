#include "subtitlewidget.h"

SubtitleWidget::SubtitleWidget(QWidget *parent) : QLineEdit(parent)
{
    setStyleSheet("QLineEdit { color : white; background-color : black; }");
    setFrame(false);
}

SubtitleWidget::~SubtitleWidget()
{
    
}

void SubtitleWidget::updateText(const QString &text)
{
    setText(text);
    setAlignment(Qt::AlignCenter);
}

void SubtitleWidget::mouseMoveEvent(QMouseEvent *event)
{
    setSelection(cursorPositionAt(event->pos()), text().length());
}

void SubtitleWidget::leaveEvent(QEvent *event)
{
    deselect();
}