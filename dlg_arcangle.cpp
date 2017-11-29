/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QtMath>

#include "dlg_arcangle.h"
#include "ui_dlg_arcangle.h"


Dlg_arcAngle::Dlg_arcAngle(qreal spanAngle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_arcAngle)
{
    ui->setupUi(this);
    ui->degSpinBox->setValue(spanAngle);
    ui->radSpinBox->setValue(qDegreesToRadians(ui->degSpinBox->value()));
}

Dlg_arcAngle::~Dlg_arcAngle()
{
    delete ui;
}

qreal Dlg_arcAngle::getAngle()
{
    return ui->degSpinBox->value();
}

void Dlg_arcAngle::on_degSpinBox_editingFinished()
{
    ui->radSpinBox->setValue(qDegreesToRadians(ui->degSpinBox->value()));
}

void Dlg_arcAngle::on_radSpinBox_editingFinished()
{
    ui->degSpinBox->setValue(qRadiansToDegrees(ui->radSpinBox->value()));
}


void Dlg_arcAngle::on_radSpinBox_valueChanged(double radAngle)
{
    qreal maxAngle = 2*M_PI;
    if (radAngle > maxAngle)
        ui->radSpinBox->setValue(maxAngle);
    else if (radAngle < -maxAngle)
        ui->radSpinBox->setValue(-maxAngle);

}
