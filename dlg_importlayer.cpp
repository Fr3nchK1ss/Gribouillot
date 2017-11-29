/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include "dlg_importlayer.h"
#include "ui_dlg_importlayer.h"

#include <QFileDialog>

Dlg_importLayer::Dlg_importLayer(QString projectName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_importLayer)
{
    ui->setupUi(this);
    setWindowTitle(tr("Import a layer to project \"")+projectName+"\"");
}

Dlg_importLayer::~Dlg_importLayer()
{
    delete ui;
}


/** Public functions */
QString Dlg_importLayer::getLayerPath()
{
    return ui->layerFileEdit->text();
}


/** Private slots */
void Dlg_importLayer::on_layerFileBtt_clicked()
{
    ui->layerFileEdit->setText(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Select a layer XML file"),
                    QDir::currentPath(),
                    "*.xml"));
}
