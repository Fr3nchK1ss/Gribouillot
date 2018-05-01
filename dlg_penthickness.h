/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_PENTHICKNESS_H
#define DLG_PENTHICKNESS_H

#include <QDialog>

namespace Ui {
class Dlg_penThickness;
}

class Dlg_penThickness : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_penThickness(QString title, int penWidth, QWidget *parent = nullptr);
    ~Dlg_penThickness();
    int getThicknessValue();

private:
    Ui::Dlg_penThickness *ui;
};

#endif // DLG_PENTHICKNESS_H
