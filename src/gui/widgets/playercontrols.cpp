#include "playercontrols.h"
#include "ui_playercontrols.h"

PlayerControls::PlayerControls(QWidget *parent) : QWidget(parent), ui(new Ui::PlayerControls)
{
    ui->setupUi(this);
}

PlayerControls::~PlayerControls()
{
    delete ui;
}