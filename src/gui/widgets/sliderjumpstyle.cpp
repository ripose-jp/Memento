#include "sliderjumpstyle.h"

int SliderJumpStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
{
    if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
    {
        return (Qt::LeftButton | Qt::MidButton | Qt::RightButton);
    }
    return QProxyStyle::styleHint(hint, option, widget, returnData);
}
