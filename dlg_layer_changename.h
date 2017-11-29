/*
 *  \brief
 *  \details
 *  \author    Reenigne from les Sans Hulotte forum
 *  \date      2015
 *  \bug       No known bugs
 *  \copyright GNU Public License v3
 */

#ifndef DLG_LAYER_CHANGENAME_H
#define DLG_LAYER_CHANGENAME_H

#include <QDialog>

namespace Ui {
class Dlg_layer_changeName;
}

class Dlg_layer_changeName : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_layer_changeName(QString currentLayerName, QWidget *parent = 0);
    ~Dlg_layer_changeName();

    QString getLayerName();

private:
    Ui::Dlg_layer_changeName *ui;
};

#endif // DLG_LAYER_CHANGENAME_H
