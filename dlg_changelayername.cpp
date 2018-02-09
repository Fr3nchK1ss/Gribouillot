/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include "dlg_changelayername.h"
#include "ui_dlg_changelayername.h"

Dlg_changeLayerName::Dlg_changeLayerName(QString currentLayerName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_changeLayerName)
{
    ui->setupUi(this);
    ui->layerNameEdit->setText(currentLayerName);
}

Dlg_changeLayerName::~Dlg_changeLayerName()
{
    delete ui;
}

QString Dlg_changeLayerName::getLayerName()
{
    return ui->layerNameEdit->text();
}
