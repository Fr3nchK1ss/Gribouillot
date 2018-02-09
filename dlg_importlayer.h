/**
 *  @brief
 *  @details
 *  @author    Ludovic A. 
 *  @date      2015 /2016/2017/2018
 *  @bug       No known bugs
 *  @copyright GNU Public License v3
 */

#ifndef DLG_IMPORTLAYER_H
#define DLG_IMPORTLAYER_H

#include <QDialog>

namespace Ui {
class Dlg_importLayer;
}

class Dlg_importLayer : public QDialog
{
    Q_OBJECT

public:
    explicit Dlg_importLayer(QString projectName, QWidget *parent = nullptr);
    ~Dlg_importLayer();

    QString getLayerPath();

private slots:
    void on_layerFileBtt_clicked();

private:
    Ui::Dlg_importLayer *ui;
};

#endif // DLG_IMPORTLAYER_H
