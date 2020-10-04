#ifndef DEFINITIONWIDGET_H
#define DEFINITIONWIDGET_H

#include <QWidget>

#include "../../lib/kiten/entry.h"

namespace Ui
{
    class DefinitionWidget;
}

class DefinitionWidget : public QWidget
{
    Q_OBJECT

public:
    DefinitionWidget(const Entry *entry, QWidget *parent = 0);
    ~DefinitionWidget();

private:
    Ui::DefinitionWidget *m_ui;
};

#endif // DEFINITIONWIDGET_H