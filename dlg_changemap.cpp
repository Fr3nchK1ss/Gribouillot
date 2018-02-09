/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include "dlg_changemap.h"
#include "ui_dlg_changemap.h"

#include <QFileDialog>

//declaring global functions defined in main.c
QString getSupportedImageFormats();
QString getDefaultImageFilter(QString);

dlg_changeMap::dlg_changeMap(QString mapName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_changeMap)
{
    ui->setupUi(this);
    setWindowTitle(tr("Select the background map"));
    ui->mapFileLbl->setText(mapName+tr(" not found. Select a new file: "));
}

dlg_changeMap::~dlg_changeMap()
{
    delete ui;
}


/** Public functions */
QString dlg_changeMap::getMapPath()
{
    return ui->mapFileEdit->text();
}


/** Private slots */
void dlg_changeMap::on_mapFileBtt_clicked()
{
    QString supportedFormats = getSupportedImageFormats();
    QString defaultFilter = getDefaultImageFilter(supportedFormats);

    ui->mapFileEdit->setText(
                QFileDialog::getOpenFileName(
                    this,
                    tr("Select an image file"),
                    QDir::currentPath(),
                    supportedFormats,
                    &defaultFilter));
}
