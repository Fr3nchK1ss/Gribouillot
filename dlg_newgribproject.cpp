/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include "dlg_newgribproject.h"
#include "ui_dlg_newgribproject.h"

#include <QApplication>
#include <QByteArray>
#include <QFileDialog>
#include <QImageReader>
#include <QList>
#include <QMessageBox>

//declaring global functions defined in main.c
QString getSupportedImageFormats();
QString getDefaultImageFilter(QString);


Dlg_newGribproject::Dlg_newGribproject(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_newGribproject)
{
    ui->setupUi(this);
    ui->projectDirEdit->setText(QDir::currentPath());
}

Dlg_newGribproject::~Dlg_newGribproject()
{
    delete ui;
}

/****** Public accessors ****/
QString Dlg_newGribproject::getProjectDirPath()
{
    return ui->projectDirEdit->text();
}

QString Dlg_newGribproject::getMapImagePath()
{
    return ui->mapImageEdit->text();
}

bool Dlg_newGribproject::copyMapImageChkBx()
{
    return ui->mapImageChkBx->isChecked();
}

bool Dlg_newGribproject::copyExternalPictures()
{
    return ui->externalPicChckBx->isChecked();
}


/***** Private slots ******/

/**
 * @brief: Set projectDirEdit with the path of the chosen project directory
 */
void Dlg_newGribproject::projectDirPath()
{
    QDir browseFrom = QDir::current();
    browseFrom.cdUp();

    QString projectDir = QFileDialog::getExistingDirectory(
                            this,
                            tr("Select or create a project directory"),
                            browseFrom.path());
    if (projectDir.isEmpty())
        projectDir = QDir::currentPath();

    ui->projectDirEdit->setText(projectDir);

}

/**
 * @brief: Set imageMapEdit with the path of chosen image file
 */
void Dlg_newGribproject::mapImagePath()
{
    QDir browseFrom = QDir::current();
    browseFrom.cdUp();

    QString supportedFormats = getSupportedImageFormats();
    QString defaultFilter = getDefaultImageFilter(supportedFormats);

    ui->mapImageEdit->setText(QFileDialog::getOpenFileName(
                    this,
                    tr("Select an image file"),
                    browseFrom.path(),
                    supportedFormats,
                    &defaultFilter));
}

/**
 * @brief: Reimplement Accept behavior to verify user's input
 */
void Dlg_newGribproject::accept()
{
    if ( ui->mapImageEdit->text().isEmpty() )
    {
        QLabel *msg = new QLabel(tr("You must select a Map image !"));
        msg->setStyleSheet("color : #FF0000");
        ( dynamic_cast<QGridLayout*>(layout()) )->addWidget(msg, 4, 0, 1, -1, Qt::AlignLeft);
    }
    else
        done(QDialog::Accepted);
}
