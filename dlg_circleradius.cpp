/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include "dlg_circleradius.h"
#include "ui_dlg_circleradius.h"

Dlg_circleRadius::Dlg_circleRadius(QWidget *parent, QString scaleUnit) :
    QDialog(parent),
    ui(new Ui::Dlg_circleRadius)
{
    ui->setupUi(this);
    ui->dSpinBox->setSuffix(scaleUnit);
}

Dlg_circleRadius::~Dlg_circleRadius()
{
    delete ui;
}

qreal Dlg_circleRadius::getRadiusValue()
{
    return ui->dSpinBox->value();
}
