/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_POINTWEIGHT_H
#define DLG_POINTWEIGHT_H

#include <QDialog>

namespace Ui {
class Dlg_pointWeight;
}

class Dlg_pointWeight : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_pointWeight(QWidget *parent = 0, int weight = 0);
    ~Dlg_pointWeight();

    int getWeightValue();

private:
    Ui::Dlg_pointWeight *ui;
};

#endif // DLG_POINTWEIGHT_H
