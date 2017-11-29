#include "dlg_circleradius.h"
#include "ui_dlg_circleradius.h"

Dlg_circleRadius::Dlg_circleRadius(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_circleRadius)
{
    ui->setupUi(this);
}

Dlg_circleRadius::~Dlg_circleRadius()
{
    delete ui;
}

qreal Dlg_circleRadius::getRadiusValue()
{
    return ui->dSpinBox->value();
}
