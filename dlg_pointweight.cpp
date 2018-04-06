#include "dlg_pointweight.h"
#include "ui_dlg_pointweight.h"

Dlg_pointWeight::Dlg_pointWeight(QWidget *parent, int weight) :
    QDialog(parent),
    ui(new Ui::Dlg_pointWeight)
{
    ui->setupUi(this);
    ui->spinBox->setValue(weight);
}

Dlg_pointWeight::~Dlg_pointWeight()
{
    delete ui;
}

int Dlg_pointWeight::getWeightValue()
{
    return ui->spinBox->value();
}
