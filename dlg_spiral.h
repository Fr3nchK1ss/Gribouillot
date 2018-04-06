#ifndef DLG_SPIRAL_H
#define DLG_SPIRAL_H

#include <QDialog>

#include "gribouillotitem.h"

namespace Ui {
class Dlg_spiral;
}

class Dlg_spiral : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_spiral(QWidget *parent = 0);
    ~Dlg_spiral();

    void loadData(bool constructAs1Item, bool baseDisplay, bool centersOnlyBase);

    bool isOneItem();
    bool isBaseDisplayed();
    bool showBaseCentersOnly();

private:
    Ui::Dlg_spiral *ui;

};

#endif // DLG_SPIRAL_H
