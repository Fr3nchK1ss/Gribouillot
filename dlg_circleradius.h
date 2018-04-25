#ifndef DLG_CIRCLERADIUS_H
#define DLG_CIRCLERADIUS_H

#include <QDialog>

namespace Ui {
class Dlg_circleRadius;
}

class Dlg_circleRadius : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_circleRadius(QWidget *parent, QString scaleUnit);
    ~Dlg_circleRadius();

    qreal getRadiusValue();

private:
    Ui::Dlg_circleRadius *ui;
};

#endif // DLG_CIRCLERADIUS_H
