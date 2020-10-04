#include "definitionwidget.h"
#include "ui_definitionwidget.h"

DefinitionWidget::DefinitionWidget(const Entry *entry, QWidget *parent) : QWidget(parent), m_ui(new Ui::DefinitionWidget)
{
    m_ui->setupUi(this);
}

DefinitionWidget::~DefinitionWidget()
{
    delete m_ui;
}