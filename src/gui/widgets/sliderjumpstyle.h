#ifndef SLIDERJUMPSTYLE_H
#define SLIDERJUMPSTYLE_H

#include <QProxyStyle>

class SliderJumpStyle : public QProxyStyle
{
public:
    using QProxyStyle::QProxyStyle;
    int styleHint(QStyle::StyleHint hint, const QStyleOption *option = nullptr, const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const Q_DECL_OVERRIDE;
};

#endif // SLIDERJUMPSTYLE_H