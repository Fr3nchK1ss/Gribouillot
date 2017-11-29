/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include <QDebug>

#include "dlg_setupgps.h"
#include "ui_dlg_setupgps.h"

Dlg_setupGps::Dlg_setupGps(QGraphicsScene *scene, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_setupGps)
{
    ui->setupUi(this);

}

Dlg_setupGps::~Dlg_setupGps()
{
    delete ui;
}


/********** functions used to load/save GPS data *********************/
/**
 * @brief   Return the GPS coordinates of the given point
 */
QString Dlg_setupGps::getFix(QPointF scenePos)
{
    return "no data";

}



/******************* protected functions  *************************/

/*
 * @brief   show the beacons when the dialog is raised by user
 *
void Dlg_setupGps::showEvent(QShowEvent* event)
{
    if(!event->spontaneous())
    {
        for (int i = 0; i < 2; ++i)
            beaconItems[i]->show();
    }
}
*/

/*
 * @brief   Hide the beacons when the dialog is hidden by user
 *
void Dlg_setupGps::hideEvent(QHideEvent* event)
{
    if(!event->spontaneous())
    {
        /**
         * The beacons buttons can not be directly unchecked because they
         * are a part of an exclusive buttonGroup.
         *
        QToolButton* chckdButton = dynamic_cast<QToolButton*>(buttonGroup->checkedButton());
        if (chckdButton != 0)
        {
            buttonGroup->setExclusive(false);
            chckdButton->setChecked(false);
            buttonGroup->setExclusive(true);
        }

        //Hide beacons (they are not deleted)
        for (int i = 0; i < 2; ++i)
            beaconItems[i]->hide();

    }
}

*/

/******************* private functions  *************************/
/**
 * @brief   return a given coordinate 'l' in degrees, arcmins, arcsecs and
 *          cardinal point.
 */
void Dlg_setupGps::getUserFriendlyCoordinate(QString type, int l,
                                int &degree, int &min, int &sec, QString &cardinal)
{
    if ( l > 3600*90 || l < -3600*90)
    {
        degree = min = sec = qQNaN();
    }
    else
    {
        degree = qAbs(l/3600);
        min = qAbs(l%3600/60);
        sec = qAbs(l%3600%60);

        if (type == "latitude")
            cardinal = (l < 0 ? "S" : "N");

        else if (type == "longitude")
            cardinal = (l < 0 ? tr("W") : "E");
    }
}

/**
 * @brief   return a user friendly QString describing the lat or long.
 * @details QString is for example 123° 30′ 00″ E
 */
QString Dlg_setupGps::getUserFriendlyCoordinate(QString type, int l)
{
    int degree;
    int min;
    int sec;
    QString cardinal;

    getUserFriendlyCoordinate(type, l, degree, min, sec, cardinal);

    if ( degree == qQNaN() )
        return QString("?");

    //else
    return QString::number(degree)+"° "
            +QString::number(min)+"' "
            +QString::number(sec)+"'' "
            +cardinal;
}




/******************* private slots *******************/
void Dlg_setupGps::on_disablePshBtt_clicked()
{
    emit disableGPS();
}
/*
void Dlg_setupGps::on_beaconATlBtt_toggled(bool checked)
{
    if (checked)
        beaconItems[0]->grabMouse();
    else
        beaconItems[0]->ungrabMouse();
}

void Dlg_setupGps::on_beaconBTlBtt_toggled(bool checked)
{
    if (checked)
        beaconItems[1]->grabMouse();
    else
        beaconItems[1]->ungrabMouse();

}
*/
