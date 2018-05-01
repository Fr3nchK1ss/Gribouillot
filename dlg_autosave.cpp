/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include "dlg_autosave.h"
#include "ui_dlg_autosave.h"

Dlg_autosave::Dlg_autosave(QWidget *parent, int oldTimeout) :
    QDialog(parent),
    ui(new Ui::Dlg_autosave)
{
    ui->setupUi(this);
    ui->spinBox->setValue(oldTimeout);
}

Dlg_autosave::~Dlg_autosave()
{
    delete ui;
}

int Dlg_autosave::getTimeoutValue()
{
    return ui->spinBox->value();
}

