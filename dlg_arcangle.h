/**
 *  @brief
 *  @details
 *  @author    Ludovic A.
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_ARCANGLE_H
#define DLG_ARCANGLE_H

#include <QDialog>

namespace Ui {
class Dlg_arcAngle;
}

class Dlg_arcAngle : public QDialog
{
    Q_OBJECT
public:
    explicit Dlg_arcAngle(qreal spanAngle, QWidget *parent = nullptr);
    ~Dlg_arcAngle();

    qreal getAngle();

private slots:
    void on_degSpinBox_editingFinished();
    void on_radSpinBox_editingFinished();

    void on_radSpinBox_valueChanged(double radAngle);

private:
    Ui::Dlg_arcAngle *ui;
};

#endif // DLG_ARCANGLE_H
