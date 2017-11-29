#include "dlg_spiral.h"
#include "ui_dlg_spiral.h"

Dlg_spiral::Dlg_spiral(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dlg_spiral)
{
    ui->setupUi(this);
    ui->label->setText( tr(
      "<font color='blue'>"
      "<p>To draw the 4 centers spiral you can define 4, 3 or only 2 "
      "centers then press [Enter], knowing that:</br>"
      "<ul><li>From 2 centers Gribouillot deduces a square base.</li>"
      "<li>From 3 centers Gribouillot computes a trapezium base.</li>"
      "</ul></br>"
      "You can hold the [Maj] key to draw a base side perpendicular to the previous one, "
      "for example in the case of a rectangle based spiral. "
      "Further construction options are available below.</p>"
      "</font>"));


}

Dlg_spiral::~Dlg_spiral()
{
    delete ui;
}


/**
 * @brief   load saved settings upon initialization
 */
void Dlg_spiral::loadData(bool construct1Item, bool baseDisplay, bool centersOnlyBase)
{

    //Construction method
    if(construct1Item)
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
