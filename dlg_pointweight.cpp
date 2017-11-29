#include "dlg_pointweight.h"
#include "ui_dlg_pointweight.h"

Dlg_pointWeight::Dlg_pointWeight(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_pointWeight)
{
    ui->setupUi(this);
}

Dlg_pointWeight::~Dlg_pointWeight()
{
    delete ui;
}

int Dlg_pointWeight::getWeightValue()
{
    return ui->spinBox->value();
}
