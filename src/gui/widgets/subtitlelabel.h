#ifndef SUBTITLEWIDGET_H
#define SUBTITLEWIDGET_H

#include <QLabel>

class SubtitleLabel : public QLabel
{
    Q_OBJECT

public:
    SubtitleLabel(QWidget *parent = 0);
    ~SubtitleLabel();
};

#endif // SUBTITLEWIDGET_H