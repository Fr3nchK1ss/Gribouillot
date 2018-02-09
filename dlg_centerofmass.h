/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_CENTEROFMASS_H
#define DLG_CENTEROFMASS_H

#include <QDialog>

namespace Ui {
class Dlg_centerOfMass;
}

class Dlg_centerOfMass : public QDialog
{
    Q_OBJECT
public:
    explicit Dlg_centerOfMass(QWidget *parent = nullptr);
    ~Dlg_centerOfMass();

    QPointF getCoMCoord();
    void addPoint(int x, int y, int weight);
    void finalizeLayout();

private:
    Ui::Dlg_centerOfMass *ui;
    int gridCount;

    qreal xSum, ySum, wSum;

};

#endif // DLG_CENTEROFMASS_H
