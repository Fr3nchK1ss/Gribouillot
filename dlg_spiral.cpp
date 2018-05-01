/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#include "dlg_spiral.h"
#include "ui_dlg_spiral.h"

Dlg_spiral::Dlg_spiral(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_spiral)
{
    ui->setupUi(this);
    ui->label->setText( tr(
      "<font color='black'>"
      "<p>To draw a 4 centers spiral you can define 4 positions manually. You can "
      "also define only 3 or 2 centers then press [Enter]:</br>"
      "<ul><li>from 2 centers Gribouillot infers a square-shaped base.</li>"
      "<li>From 3 centers Gribouillot infers a trapezium base.</li>"
      "</ul></br>"
      "You can hold the [Maj] key to draw a side perpendicular to the previous one, "
      "for example to draw a rectangle based spiral.</p>"
      "</font>"));

}

Dlg_spiral::~Dlg_spiral()
{
    delete ui;
}


/**
 * @brief   load saved settings upon initialization
 */
void Dlg_spiral::loadData(bool constructAs1Item, bool baseDisplay, bool centersOnlyBase)
{

    //Construction method
    if(constructAs1Item)
        ui->draw1Item->setChecked(true);
    else
        ui->draw4Arcs->setChecked(true);

    //base display
    ui->grpBoxBaseDisplay->setChecked(baseDisplay);

    //draw only centers of base or whole segments
    if(centersOnlyBase)
        ui->displayCenters->setChecked(true);
    else
        ui->displaySegments->setChecked(true);


}


/***************** Public Accessors ********************/
bool Dlg_spiral::isOneItem()
{
    return ui->draw1Item->isChecked();

}

bool Dlg_spiral::isBaseDisplayed()
{
    return ui->grpBoxBaseDisplay->isChecked();

}

bool Dlg_spiral::showBaseCentersOnly()
{
    return ui->displayCenters->isChecked();

}
