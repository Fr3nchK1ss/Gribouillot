/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QButtonGroup>
#include <QDebug>

#include "maptabwidget.h"
#include "ui_maptabwidget.h"
#include "ui_dlg_setupgps.h"



MapTabWidget::MapTabWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MapTabWidget)
{
    ui->setupUi(this);

    connect (ui->pxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(newPxValue(double)));
    connect (ui->kmSpinBox, SIGNAL(valueChanged(double)), this, SLOT(newKmValue(double)));
    connect (ui->mKmComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(newScaleUnit()));

}

MapTabWidget::~MapTabWidget()
{
    delete ui;
}


/**
 * @brief   reset the User Interface of the map Tab
 */
void MapTabWidget::resetUI()
{
    ui->blackWhiteTlBtt->setChecked(false);
    ui->gpsTlBtt->setChecked(false);
    ui->gpsTlBtt->setIcon(QIcon(QPixmap(":/Resources/Icons/gps-off.png")));
    ui->scaleRulerTlBtt->setChecked(false);

    ui->pxSpinBox->setValue(0);
    ui->kmSpinBox->setValue(0);
    ui->systemScaleLbl->setText(tr("System scale"));
}

/**
 * @brief   compute the kilometer per pixel ratio for measures on map
 */
void MapTabWidget::computeKmPxScale(double km, double px)
{
    QString systemScale;
    QString unit = ui->mKmComboBox->currentText();

    if (px != 0)
        kmPxScale = km/px;

    if (unit == "m")
        kmPxScale = kmPxScale/1000.0;

    //qDebug() << kmPxScale;

    //Show to user
    systemScale = QString::number(kmPxScale*1000, 'f', 1);
    ui->systemScaleLbl->setText(tr("1px = ")+systemScale+tr(" meters (if zoom = 1)"));

    //computeOnmapScale();//Red and white on-map scale
    emit newSystemScale(kmPxScale, unit);
}


qreal MapTabWidget::getKmPxScale()
{
    if (qIsNaN(kmPxScale))
        return 0;
    else
        return kmPxScale;
}


/************************** Public slots ***************************/
/**
 * @brief   unchecked drawing buttons not to conflict with the drawing
 *          buttons of the toolbar
 */
void MapTabWidget::uncheckDrawingButtons()
{
    ui->scaleRulerTlBtt->setChecked(false);
}


/************************** Scale changed ***************************/

/** @brief   scale changed by measuring tape */
void MapTabWidget::newScaleMeasure(double px)
{
    ui->pxSpinBox->setValue(px);
    newPxValue(px);
}

/** @brief   scale changed by user input in pixel spinBox */
void MapTabWidget::newPxValue(double px)
{
    double km = ui->kmSpinBox->value();
    computeKmPxScale(km, px);
}
/** @brief   scale changed by user input in km spinBox */
void MapTabWidget::newKmValue(double km)
{
    double px = ui->pxSpinBox->value();
    computeKmPxScale(km, px);
}
/** @brief   scale changed by user input in meter/kilometer comboBox */
void MapTabWidget::newScaleUnit()
{
    double km = ui->kmSpinBox->value();
    double px = ui->pxSpinBox->value();
    computeKmPxScale(km, px);
}
