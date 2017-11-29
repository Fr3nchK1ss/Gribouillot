#include "dlg_penthickness.h"
#include "ui_dlg_penthickness.h"

Dlg_penThickness::Dlg_penThickness(QString title, int penWidth, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_penThickness)
{
    ui->setupUi(this);
    ui->spinBox->setValue(penWidth);
    ui->spinBox->setFocus();
    setWindowTitle(title);
}

Dlg_penThickness::~Dlg_penThickness()
{
    delete ui;
}


int Dlg_penThickness::getThicknessValue()
{
    return ui->spinBox->value();
}

