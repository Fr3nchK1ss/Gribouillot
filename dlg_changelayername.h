/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_CHANGELAYERNAME_H
#define DLG_CHANGELAYERNAME_H

#include <QDialog>

namespace Ui {
class Dlg_changeLayerName;
}

class Dlg_changeLayerName : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_changeLayerName(QString currentLayerName, QWidget *parent = nullptr);
    ~Dlg_changeLayerName();

        QString getLayerName();

private:
    Ui::Dlg_changeLayerName *ui;
};

#endif // DLG_CHANGELAYERNAME_H
