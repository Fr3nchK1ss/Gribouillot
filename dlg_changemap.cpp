#include "dlg_changemap.h"
#include "ui_dlg_changemap.h"

dlg_changeMap::dlg_changeMap(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_changeMap)
{
    ui->setupUi(this);
}

dlg_changeMap::~dlg_changeMap()
{
    delete ui;
}
