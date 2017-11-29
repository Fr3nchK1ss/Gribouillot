/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */


#include "dlg_centerofmass.h"
#include "ui_dlg_centerofmass.h"

#include <QDebug>
#include <QDialogButtonBox>

Dlg_centerOfMass::Dlg_centerOfMass(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_centerOfMass),
    gridCount(0)
{
    ui->setupUi(this);

    xSum = ySum = wSum = 0;
}

Dlg_centerOfMass::~Dlg_centerOfMass()
{
    delete ui;
}



/**
 * @brief   return the center of mass of the points listed by the dialog.
 */
QPointF Dlg_centerOfMass::getCoMCoord()
{
   return QPointF(xSum/wSum, ySum/wSum);
}


/**
 * @brief   Add the description of a point to the dialog layout and proceed
 *          to compute CoM data.
 */
void Dlg_centerOfMass::addPoint(int x, int y, int weight)
{
    //Only display weighted points in the layout, weight != 0
    if (weight != 0)
    {
        QGridLayout *comLayout = dynamic_cast<QGridLayout*>(layout());
        QString pointString = QString::number(gridCount+1)+
                        ". Point at position (" +
                        QString::number(x) + "; " +
                        QString::number(y) + ") " +
                        "with weight " + QString::number(weight);

        comLayout->addWidget(new QLabel(pointString), gridCount+1, 0);
        gridCount++;

        //CoM computation
        xSum += weight*x;
        ySum += weight*y;
        wSum += weight;
    }

}


/**
 * @brief   Add a standard ButtonBox
 */
void Dlg_centerOfMass::finalizeLayout()
{
    QGridLayout *comLayout = dynamic_cast<QGridLayout*>(layout());
    QDialogButtonBox* buttonBox;

    if ( wSum != 0 )
    {   
        buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);
        connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    }
    else
    {
        ui->comLabel->setText("No weighted points found!");
        buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel);
    }

    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    comLayout->addWidget(new QLabel());
    comLayout->addWidget(buttonBox, gridCount+2, 0);

}

