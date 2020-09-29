#include "subtitlelabel.h"

SubtitleLabel::SubtitleLabel(QWidget *parent) : QLabel(parent)
{
    setStyleSheet("QLabel { color : white; background: black; }");
}

SubtitleLabel::~SubtitleLabel()
{
    
}